# 設計書: Codexion

| 項目 | 内容 |
|---|---|
| Subject | en.codexion.pdf (v1.5) |
| 作成者 | kamurai |
| 作成日 / 最終更新 | 2026-07-30 / 2026-07-30 |
| ステータス | draft |

---

## 1. 概要と完成条件

- **一言サマリ**: 「食事する哲学者」拡張版のマルチスレッドシミュレータ `codexion`（コーダー=スレッド、ドングル=mutex保護の共有資源）。
- **ゴール**: N人のコーダーが左右2本のドングルを取ってコンパイルを繰り返すシミュレーションを、cooldown・FIFO/EDF調停・10ms精度のburnout監視付きで、デッドロックも starvation も起こさずに実装する。
- **完成条件 (Definition of Done)**:
  - [ ] 不正な引数（負数・非整数・不正スケジューラ）をすべて拒否する
  - [ ] burnout しないパラメータで全員が規定回数コンパイルし、正しい形式・順序のログを出して終了する
  - [ ] burnout するパラメータで、実際の燃え尽き時刻から10ms以内に `burned out` を出力して停止する
  - [ ] `fifo` / `edf` 両方が仕様どおり動作し、調停に自作ヒープを使っている
  - [ ] valgrind でリークゼロ、ThreadSanitizer でデータ競合ゼロ
  - [ ] `-Wall -Wextra -Werror -pthread` でコンパイルでき、norminette がエラーなし
- **スコープ外**: ボーナス規定なし。GUIや統計出力などの独自拡張はしない。

## 2. 要件整理

### 2.1 機能要件（必須）

| ID | 要件 | Subject参照 |
|---|---|---|
| FR-1 | 8つの必須引数のパースと検証 | V |
| FR-2 | コーダー1人=1スレッド（pthread_create / join） | VI |
| FR-3 | ドングルは隣接コーダー間に1本（1人なら1本）、各ドングルの状態を mutex で保護 | VI |
| FR-4 | dongle_cooldown: 解放後、指定ms経過まで再取得不可 | VI |
| FR-5 | 調停: fifo（到着順）/ edf（deadline = last_compile_start + time_to_burnout が最小の者）。自作優先度キュー（ヒープ）必須 | VI |
| FR-6 | monitor スレッドが burnout を検出し、10ms以内にログを出して停止 | VI |
| FR-7 | 指定形式のログ（5種）を混線なく出力（mutexで直列化） | V |
| FR-8 | 終了条件: 誰かの burnout、または全員が number_of_compiles_required 回以上コンパイル | V, VI |

### 2.3 非機能要件

| ID | 要件 | 目標値 |
|---|---|---|
| NFR-1 | burnout ログの遅延 | 実時刻から10ms以内 |
| NFR-2 | メモリリーク / データ競合 | ゼロ（valgrind / TSan で確認） |
| NFR-3 | liveness | パラメータが実現可能なら edf で starvation なし |
| NFR-4 | ビルド | `-Wall -Wextra -Werror -pthread`、不要な再リンクなし |
| NFR-5 | 規約 | Norm 準拠（エラー1つで0点） |

### 2.4 制約・禁止事項

- **グローバル変数禁止**（違反=不合格級）
- 使用可能な外部関数は指定リストのみ（pthread系 / gettimeofday・clock_gettime / usleep / write・printf・fprintf / malloc・free / strcmp・strlen・atoi・memset）
- libft 使用不可。標準ライブラリの優先度キュー相当も不可（ヒープ自作）
- Makefile: NAME, all, clean, fclean, re

## 3. 全体設計

### 3.1 構成図

```mermaid
graph TD
    M[main] --> P[parse_args<br>検証 FR-1]
    P --> I[init_table<br>dongles/coders/mutex確保]
    I --> C1[coder thread ×N<br>compile→debug→refactor]
    I --> MO[monitor thread<br>deadline監視 周期≤2ms]
    C1 -- take/release --> D[(dongle[i]<br>mutex+cond+heap)]
    C1 -- log --> L[print_mtx]
    MO -- burnout検出/停止 --> S[state_mtx<br>stopフラグ]
    M --> J[join全スレッド→destroy/free]
```

### 3.2 データフロー

引数 → `t_config`。`t_table` がドングル配列・コーダー配列・共有mutex群を所有する。各コーダーはループ（2本取得→compile→返却→debug→refactor）を回し、状態変化のたびに直列化されたログを出す。monitor は全コーダーの deadline を周期的に評価し、超過を検出したら stop フラグを立てて burnout ログを出す。main は join 後に全資源を解放する。

### 3.3 ディレクトリ構成

```
codexion/
├── Makefile
├── include/codexion.h
└── src/
    ├── main.c  parse.c  init.c  cleanup.c
    ├── coder.c  dongle.c  heap.c
    ├── monitor.c  log.c  time_utils.c
```

### 3.4 主要コンポーネント一覧

| 名前 | 種別 | 責務 | 依存先 |
|---|---|---|---|
| `t_config` | struct | 検証済み引数の保持 | — |
| `t_dongle` | struct | mutex / cond / holder / available_at（cooldown期限）/ 要求ヒープ | t_heap |
| `t_coder` | struct | id・スレッド・last_compile_start・compile_count・自身のmutex | t_table |
| `t_heap` | struct + 関数群 | 配列ベース二分ヒープ。比較関数差し替えで fifo/edf 両対応（FR-5） | — |
| `t_table` | struct | 全体状態（config・配列・print_mtx・state_mtx・stop・start_time） | 全部 |
| monitor | thread | deadline 監視・burnout 判定・停止指示（FR-6） | t_table |

## 4. 詳細設計

### 4.1 中核アルゴリズム（ドングル取得プロトコル）

```
take_dongle(coder, d):
    lock(d->mtx)
    key = (edf) ? coder->deadline : d->next_seq++     // fifoは到着順
    heap_push(d->q, {key, tie: coder->id})
    while (!stopped() &&
           !(heap_top(d->q).id == coder->id           // 自分の番
             && d->holder == NONE                     // 空いている
             && now_ms() >= d->available_at))         // cooldown明け
        cond_timedwait(d->cond, d->mtx,
                       min(次のcooldown明け, 監視刻み))
    heap_pop(d->q);  d->holder = coder->id
    unlock(d->mtx)
    log("has taken a dongle")

release_dongle(coder, d):
    lock(d->mtx)
    d->holder = NONE
    d->available_at = now_ms() + cooldown
    broadcast(d->cond)                                // 全待機者に再評価させる
    unlock(d->mtx)
```

- **デッドロック回避**: 2本の取得は常に「ドングル番号の小さい方 → 大きい方」の全域順序で行う（Coffman の循環待ち条件を構造的に破る）。1人の場合は2本目が存在せず取得不能 → time_to_burnout 経過で burnout（仕様どおり）。
- **公平性**: 付与順はヒープが一意に決める。EDF のタイブレークは (deadline, coder_id) の辞書式で決定的（Subjectの注記に対応）。
- 計算量: 取得・解放とも O(log N)。

### 4.2 状態管理 / 並行性設計

**ロック一覧と階層**（下位を保持したまま上位を取らない）:

| 階層 | ロック | 保護対象 |
|---|---|---|
| 1 | `dongle[i].mtx`（i昇順のみ） | holder / available_at / ヒープ |
| 2 | `coder[i].mtx` | last_compile_start / compile_count |
| 3 | `state_mtx` | stop フラグ / 完了人数 |
| 4 | `print_mtx` | ログ出力（FR-7） |

**想定競合と対策**:

- monitor の deadline 読取り vs コーダーの更新 → `coder[i].mtx` で保護（`last_compile_start` はコンパイル開始ログの直前に更新）。
- 停止指示とログの混線 → ログ関数内で stop を確認し、burnout 以降は他メッセージを出さない。
- cooldown 待ちの起床精度 → `cond_timedwait` の期限に `available_at` を反映し、broadcast+再評価のループで取り逃しを防ぐ。

**monitor 設計**（NFR-1）: 周期ループ（刻み≤2ms）で全コーダーの `last_compile_start + time_to_burnout` を評価。超過検出→ `state_mtx` で stop 設定 → burnout ログ。周期2msなら検出遅延+出力で10ms以内に収まる。時刻は `gettimeofday` を起点差分のmsに変換（Subject推奨）。`usleep` は500µs単位に分割し、待ち中も stop を確認する。

### 4.3 エラー処理方針

| 異常系 | 検出箇所 | 挙動・メッセージ |
|---|---|---|
| 引数個数 ≠ 8 | parse | usage を stderr に出して exit 1 |
| 非整数・負値・0（コーダー数0等） | parse | `Error: invalid <arg名>`、exit 1 |
| scheduler が fifo/edf 以外 | parse | `Error: scheduler must be fifo or edf`、exit 1 |
| malloc / pthread_* 失敗 | init | 生成済み資源を逆順に解放して exit 1 |

### 4.4 課題固有の設計

- **cooldown 実装**: 専用タイマースレッドは作らず、`available_at` タイムスタンプ + timedwait 期限で表現（スレッド数と競合面を増やさない）。
- **ログのタイムスタンプ**: シミュレーション開始時刻からの相対ms。開始時刻は全スレッド起動前に1回だけ取得し `t_table` に保持。
- **「compiling の前に has taken ×2」**の保証: 2本目の取得ログ→compiling ログを同一関数内で連続して出す（間に他状態ログを挟まない設計）。

## 5. 設計判断の記録

| # | 決定 | 理由 | 検討した代替案 | トレードオフ |
|---|---|---|---|---|
| D-1 | 取得順序の全域順序化（番号昇順）でデッドロック回避 | 正しさを口頭で証明しやすく、EDF調停と独立に成立 | 奇数/偶数で左右反転、執事（許可トークン）方式 | 特定コーダーの初回待ちが僅かに偏るが、調停ヒープが吸収 |
| D-2 | cooldown は available_at 方式 | スレッド追加なし・ロック構造が単純 | cooldown 専用タイマースレッド | timedwait の起床精度に依存（刻みで補償） |
| D-3 | ヒープは1実装+比較関数差し替え | fifo/edf でコード重複なし。タイブレークも比較関数に内包 | キューを2実装 | なし |
| D-4 | monitor は専用1スレッド・周期≤2ms | 10ms要件（NFR-1）を単純な周期設計で満たす | 各コーダーが自己申告 / deadline順イベント駆動 | 常時起床によるCPU消費（刻み幅で調整） |
| D-5 | ログは printf + print_mtx | 要件の直列化を最小構成で満たす | write で自前バッファ | 10ms 遅延が問題化したら write + setvbuf に切替（リスク対策） |

## 6. テスト計画

| ID | 区分 | ケース | 期待結果 | 対応要件 |
|---|---|---|---|---|
| T-1 | 正常系 | `5 800 200 200 200 7 50 fifo` | 全員7回コンパイル、burnout なし | FR-2〜5, FR-8 |
| T-2 | 正常系 | 同条件で `edf` | 同上。付与順が deadline 順 | FR-5 |
| T-3 | burnout | `1 800 200 200 200 5 50 fifo` | 1人はコンパイル不能 → 800ms で burnout | FR-6 |
| T-4 | 精度 | T-3 を100回実行しログ時刻と期限の差を集計 | 最大でも10ms以内 | NFR-1 |
| T-5 | エッジ | タイトな burnout（`4 310 200 100 100 10 0 edf` 等）/ 大きな cooldown | 実現可能なら完走、不可能なら正しい burnout | NFR-3 |
| T-6 | エラー系 | 引数不足 / `-5` / `abc` / `scheduler=xyz` | 明確なエラーで exit 1 | FR-1 |
| T-7 | 品質 | valgrind / `-fsanitize=thread` で T-1〜T-5 | リーク・競合ゼロ | NFR-2 |
| T-8 | ログ整合 | 検証スクリプト: compiling 直前に has taken×2、タイムスタンプ単調、混線なし | すべて成立 | FR-7 |

- **性能検証**: T-4 の分布（平均・最大）を README の性能分析に転記。
- **使用ツール**: valgrind / ThreadSanitizer / norminette / ログ検証用シェルスクリプト（awk）。

## 7. 開発計画とリスク

- **M1（動く最小構成）**: fifo・cooldown なし・monitor なしで哲学者コア（取得順序による回避）を完走させる
- **M2（必須完了）**: cooldown・ヒープ調停・edf・monitor・引数検証・ログ整合
- **M3（仕上げ）**: 10ms精度チューニング、TSan/valgrind 掃討、Norm 整形

| リスク | 影響 | 対策 |
|---|---|---|
| 再現しないタイミングバグ | 評価時に突然死 | TSan 常用+コーダー数大・時間小のストレステストを CI 的に回す |
| burnout 通知が10msを超える | FR-6/NFR-1 未達 | 監視刻みを短縮、printf→write 切替（D-5）、システム負荷下で測定 |
| Norm 制約（関数25行等）で構造が崩れる | 手戻り | 最初から小さい関数に分割し、norminette を毎コミット実行 |

## 8. 参考資料とAI利用記録

- **資料**: Dijkstra の Dining Philosophers 問題解説 / Coffman conditions / pthread man pages（pthread_mutex_lock, pthread_cond_timedwait）/ EDF スケジューリングの解説
- **AI利用**:

| タスク | AIの使い方 | 検証方法 |
|---|---|---|
| 設計書ドラフト作成・並行性設計の壁打ち | ロック階層と調停方式の比較検討 | デッドロック回避の論拠を自分で説明できる状態にし、TSan で裏取り |
| （実装時に追記） | | |
