# pthread_mutex 関連関数

| 関数 | 役割 |
|---|---|
| `pthread_mutex_init(&m, attr)` | mutex `m` を初期化する |
| `pthread_mutex_destroy(&m)` | mutex `m` を破棄する |
| `pthread_mutex_lock(&m)` | `m` をロックする。他スレッドが保持中なら解放されるまで待つ |
| `pthread_mutex_unlock(&m)` | 保持している `m` を解放する |
| `pthread_mutex_trylock(&m)` | `m` のロックを試みる。取れなければ待たずに `EBUSY` を返す |
| `pthread_mutex_timedlock(&m, &t)` | 時刻 `t` まで `m` のロックを待つ（macOS には無い） |

# スレッドごとの待ち時間の設計

方針: 各スレッドの「許容できる遅れ」を先に決め、待ち時間はその 1/5〜1/10 程度にする。
OS のスケジューリングにより `usleep` は指定より長く眠ることがあるため、余裕を持たせる。

| スレッド | 許容できる遅れ（根拠） | 待ち時間 |
|---|---|---|
| monitor | 10ms（burnout 通知の要件） | 1ms |
| 作業中の corder | 約 1ms（ログの時刻精度） | 500µs ずつ刻む |
| 1人のときの corder | 緩い（終了が少し遅れるだけ） | 500µs〜1ms |
| ドングル待ちの corder | 合図で起きるため時間は決めない | `pthread_cond_wait` |
| cooldown 待ちの corder | cooldown 明けの時刻ちょうど | `pthread_cond_timedwait` |

- monitor: 見回りの処理時間と OS の遅延を足しても 10ms に十分な余裕が残る値として 1ms とする。
- 作業中の corder: `usleep(time_to_compile * 1000)` で一度に眠ると寝過ぎが毎回積み重なる。
  500µs ずつ眠り、開始からの経過時間と `stop` を毎回確認する。
- corder 数が多い場合（200人など）: 待ち時間を短くし過ぎると全員の起床で CPU が埋まるため、
  100µs 未満にはしない。

## cooldown 中の待ち方

`pthread_cond_wait` だけでは cooldown 明けに起きられないため、`pthread_cond_timedwait` を使う。

- `pthread_cond_wait` は `signal` / `broadcast` されるまで眠り続ける。
  ドングルの返却時は返却者が合図を送るが、cooldown 明けは時間が経過しただけで誰も合図を送らない。
  そのため、ドングルが使える状態になっても眠ったままとなり、burnout につながる。
- ドングルが使えるようになる時刻は `released_at + dongle_cooldown`。
- `pthread_cond_timedwait` にこの時刻を渡すと、「合図が来る」か「その時刻になる」かの早い方で起きる。
- 起きたら「使えるか」と `stop` を再確認し、まだ使えなければ再び眠る。
- 渡す時刻は `struct timespec`（秒とナノ秒）で、`gettimeofday` と同じ 1970 年基準の絶対時刻で指定する。

`usleep(500)` で眠って確認を繰り返す方法でも動作するが、`pthread_cond_timedwait` の方が
cooldown 明けの瞬間に起きられ、無駄な起床もない。
