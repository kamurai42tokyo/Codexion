*このプロジェクトは、42 のカリキュラムの一環として kamurai が作成した。*

# Codexion

## Description（説明）

<details>
<summary>開いて読む</summary>

Codexion は、C と POSIX スレッドで書いた並行処理のシミュレーションである。

### 課題のルール

このプログラムは、課題が定める次のルールを前提としている。

- **座席**: coder は1人以上で、円卓を囲んで座る。coder の番号は 1 から `number_of_coders` までで、
  coder 1 と coder `number_of_coders` は隣同士である。
- **ドングル**: 隣り合う coder の間にドングルが1本ずつあり、ドングルの本数は coder の人数と同じである。
  coder が1人だけのとき、ドングルは1本だけである。
- **作業の流れ**: coder は「コンパイル → デバッグ → リファクタリング」を繰り返す。
  coder は、3つの作業のうち1つだけを同時に行う。
- **コンパイルの条件**: coder がコンパイルするには、左右のドングル2本を同時に持たなければならない。
  コンパイルを終えた coder は、ドングルを2本とも円卓に返す。
- **クールダウン**: 返されたドングルは、`dongle_cooldown` ミリ秒が経つまで誰も取れない。
- **調停**: 2人の coder が同じドングルを求めたとき、ドングルは `scheduler` に従って渡す相手を決める。
  - `fifo`: 先に申し込んだ coder に渡す。
  - `edf`: 燃え尽きの期限（`last_compile_start + time_to_burnout`）が早い coder に渡す。
- **燃え尽き**: 最後のコンパイル開始（またはシミュレーション開始）から `time_to_burnout` ミリ秒以内に
  コンパイルを始められなかった coder は、燃え尽きる。
- **会話なし**: coder 同士は話さない。coder は、ほかの coder が燃え尽きそうかどうかを知らない。
- **終了**: シミュレーションは、次のどちらかが起きた時点で止まる。
  - coder が1人でも燃え尽きた。
  - 全 coder のコンパイル回数が `number_of_compiles_required` 回以上になった。

### このプログラムの目標

このプログラムの目標は、限られた資源（ドングル）を、デッドロック・飢餓・データ競合なしに
スレッド間で分け合うことである。この目標のために、プログラムは次を実装している。

- coder 1人につき1本のスレッドと、燃え尽きを 10ms 以内に検出する monitor スレッド1本
- 返却後のドングルのクールダウン
- `fifo` または `edf` のスケジューラによる、ドングル申請の公平な調停
- 2つのメッセージが1行に混ざらないようにする、ログ出力の直列化

### ソースファイル

| ファイル | 役割 |
|---|---|
| `main.c`, `parse.c` | コマンドライン引数8つを読み取り、検証する |
| `init.c`, `simulation.c`, `free.c` | 円卓を作り、スレッドを作成・join し、すべてを解放する |
| `threads/coder.c`, `threads/actions.c` | coder スレッド1本の処理（ドングル取得・コンパイル・デバッグ・リファクタ） |
| `threads/dongle.c`, `threads/request.c`, `threads/turn.c` | ドングルの取得、申請の待ち行列（優先度キュー）、順番の判定 |
| `threads/monitor.c` | monitor スレッドの処理（燃え尽きと全員完了の判定） |
| `threads/print_log.c`, `threads/stop.c`, `threads/time.c` | ログ出力、停止フラグ、時刻の補助関数 |

</details>

## Instructions（使い方）

<details>
<summary>開いて読む</summary>

### コンパイル

```sh
make
```

Makefile は `cc -Wall -Wextra -Werror -pthread` でソースをコンパイルし、
実行ファイル `codexion` を作る。

| ルール | 動作 |
|---|---|
| `make` / `make all` | `codexion` を作る |
| `make clean` | オブジェクトファイル（`.o`）を消す |
| `make fclean` | オブジェクトファイルと `codexion` を消す |
| `make re` | `fclean` を実行してから、`codexion` を作り直す |

### 実行

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| 引数 | 意味 |
|---|---|
| `number_of_coders` | coder の人数。ドングルの本数でもある（1以上） |
| `time_to_burnout` | コンパイルを始めていない coder が燃え尽きるまでの時間(ms) |
| `time_to_compile` | coder がドングル2本を持ってコンパイルする時間(ms) |
| `time_to_debug` | coder がデバッグする時間(ms) |
| `time_to_refactor` | coder がリファクタリングする時間(ms) |
| `number_of_compiles_required` | 全 coder のコンパイル回数がこの値に達すると、シミュレーションが止まる |
| `dongle_cooldown` | 返されたドングルを再び取れるようになるまでの時間(ms) |
| `scheduler` | ドングルの調停方式。`fifo` か `edf` |

実行例:

```sh
./codexion 4 800 200 200 200 3 10 edf
```

数値の引数は、すべて0以上の整数でなければならない。引数が足りない場合や不正な場合、
プログラムは終了ステータス `1` で終わる。

### ログの出力

subject が定めるログは次の5種類のみ。

| 種類 | 厳密な内容 |
|---|---|
| ドングルを取った | `timestamp_in_ms X has taken a dongle` |
| コンパイル開始 | `timestamp_in_ms X is compiling` |
| デバッグ開始 | `timestamp_in_ms X is debugging` |
| リファクタリング開始 | `timestamp_in_ms X is refactoring` |
| 燃え尽きた | `timestamp_in_ms X burned out` |

- `timestamp_in_ms`: シミュレーション開始からログを出す瞬間までの経過時間（ミリ秒）。
- `X`: そのログを出す coder の番号。
- 時刻・番号・メッセージは半角スペース1つで区切り、行末は改行とする。

</details>

## Blocking cases handled（対処した並行処理の問題）

<details>
<summary>開いて読む</summary>

### 1. デッドロック（全員が待ち合って止まる）

- 問題: 全員が同時に左のドングルを取ると、全員が右のドングルを待ったまま止まる。
- 対策: 奇数番の coder は左から、偶数番の coder は右からドングルを取る。
- 効果: 隣り合う2人が同じドングルを取り合うため、「全員が1本ずつ持って待つ」輪ができない。

### 2. 飢餓（特定の coder だけ順番が回ってこない）

- 問題: 同じ coder ばかりがドングルを取ると、隣の coder が燃え尽きる。
- 対策: 各ドングルは申込書の待ち行列を持ち、決まった順番でドングルを渡す。
  - fifo: 先に申し込んだ coder に渡す。
  - edf: 燃え尽きの期限が近い coder に渡す。期限が同じなら、先に申し込んだ coder に渡す。
- 効果: コンパイルを終えたばかりの coder は、待っている隣の coder を追い越せない。

### 3. クールダウン（返却直後のドングルは使えない）

- 問題: クールダウン明けには誰も合図を送らないため、`pthread_cond_wait` で眠った coder は起きられない。
- 対策: coder は `pthread_cond_timedwait` に「クールダウンが明ける時刻」を渡して眠る。
- 効果: coder は「合図が来る」か「クールダウンが明ける」かの早い方で起きる。

### 4. 燃え尽きの検出（10ms 以内に知らせる）

- 対策: monitor スレッドは 1ms ごとに全 coder を見回る。
- 理由: 1ms ごとの見回りなら、OS の遅れを足しても 10ms に十分間に合う。
- 作業中の coder は 500µs ずつ眠り、そのたびに `stop` を確認する。このため、coder は終了にすぐ気づける。

| スレッド | 許容できる遅れ | 待ち時間 |
|---|---|---|
| monitor | 10ms | 1ms |
| 作業中の coder | 約 1ms | 500µs ずつ |
| ドングル待ちの coder | 決めない（合図で起きる） | `pthread_cond_wait` |
| cooldown 待ちの coder | cooldown 明けちょうど | `pthread_cond_timedwait` |

### 5. ログの混ざり（2つのメッセージが1行に混ざる）

- 問題: 2人の coder が同時にログを出すと、1行の中に2つのメッセージが混ざる。
- 対策: `print_log` は出力用の鍵（`log_mutex`）をかけてから1行を出す。
- 効果: 1行を出し終わるまで、ほかの coder はログを出せない。`burned out` の後にもログは出ない。

</details>

## Thread synchronization mechanisms（スレッド同期の仕組み）

<details>
<summary>開いて読む</summary>

### スレッド

- coder スレッド: `number_of_coders` 本。各スレッドが `run_simulation()` を実行する。
- monitor スレッド: 1本。`monitor_routine()` を実行する。
- メインスレッド: 全スレッドを作成し、`pthread_join` で終わりを待つ。

### 鍵と、鍵が守るもの

| プリミティブ | 持ち主 | 守るデータ |
|---|---|---|
| `pthread_mutex_t mutex` | 各 `t_dongle` | `in_use`, `released_at`, `request_queue`, `request_count`, `next_arrival_order` |
| `pthread_cond_t cond` | 各 `t_dongle` | このドングルを待つ coder を起こす |
| `pthread_mutex_t mutex` | 各 `t_coder` | `last_compile_start`, `compile_count` |
| `pthread_mutex_t stop_mutex` | `t_table` | `stop`（シミュレーション終了のフラグ） |
| `pthread_mutex_t log_mutex` | `t_table` | 標準出力 |

`t_config` は、スレッド開始前にメインスレッドだけが書き込むため、鍵は不要である。

### pthread_mutex 関連関数

| 関数 | 役割 |
|---|---|
| `pthread_mutex_init(&m, attr)` | mutex `m` を初期化する |
| `pthread_mutex_destroy(&m)` | mutex `m` を破棄する |
| `pthread_mutex_lock(&m)` | `m` をロックする。他スレッドが保持中なら解放されるまで待つ |
| `pthread_mutex_unlock(&m)` | 保持している `m` を解放する |
| `pthread_mutex_trylock(&m)` | `m` のロックを試みる。取れなければ待たずに `EBUSY` を返す |
| `pthread_mutex_timedlock(&m, &t)` | 時刻 `t` まで `m` のロックを待つ（macOS には無い） |

`pthread_mutex_trylock` と `pthread_mutex_timedlock` は参考として載せており、
このプロジェクトでは使っていない。

### データ競合の防ぎ方

- **ドングルの取得**: coder は、ドングルの mutex を持ったまま `in_use` を確認し、`true` にする。
  このため、2人の coder が同じドングルを同時に取ることはない。
- **burnout の判定**: coder は自分の mutex の中で `last_compile_start` を書き換え、
  monitor は同じ mutex の中で `last_compile_start` を読む。
  このため、monitor が書きかけの値を読むことはない。
- **停止フラグ**: `stop` の読み取りはすべて `is_stopped()` を通り、書き込みはすべて
  `stop_simulation()` を通る。どちらの関数も `stop_mutex` を取る。

### coder と monitor の間の通信

- **coder から monitor へ**: 各 coder は、自分の mutex の中で `last_compile_start` と
  `compile_count` を書く。monitor はこの2つを読み、燃え尽きと全員完了を判定する。
- **monitor から coder へ**: monitor は `stop` を立てた後、全ドングルの mutex を取って
  `cond` に `pthread_cond_broadcast` を送る。ドングル待ちで眠っている coder は起きて
  `stop` を確認し、ループを抜ける。
- **合図の取りこぼし防止**: 待つ coder は、ドングルの mutex を持ったまま `stop` を確認する。
  `pthread_cond_wait` は、その mutex の解放と眠りを一度に（不可分に）行う。monitor が broadcast
  するには同じ mutex が要るため、broadcast は必ず、眠り始めた後の coder に届く。

</details>

## Resources（参考資料）

<details>
<summary>開いて読む</summary>

主に、C 言語でのスレッドの扱い方について、AI（Anthropic の Claude）の回答を参考にした。
そのほか、AI を次の作業に使った。

- Makefile の作成と、ルールやコンパイラオプション（`-pthread` など）の説明
- 課題の要件に対するプロジェクトの確認
- この README の整理・翻訳・加筆

</details>
