# pthread_mutex 関連関数

| 関数 | 役割 |
|---|---|
| `pthread_mutex_init(&m, attr)` | mutex `m` を初期化する |
| `pthread_mutex_destroy(&m)` | mutex `m` を破棄する |
| `pthread_mutex_lock(&m)` | `m` をロックする。他スレッドが保持中なら解放されるまで待つ |
| `pthread_mutex_unlock(&m)` | 保持している `m` を解放する |
| `pthread_mutex_trylock(&m)` | `m` のロックを試みる。取れなければ待たずに `EBUSY` を返す |
| `pthread_mutex_timedlock(&m, &t)` | 時刻 `t` まで `m` のロックを待つ（macOS には無い） |
