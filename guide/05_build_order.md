# 05. 実装する順番と確認のしかた

一度に全部書くと、どこで間違えたか分からなくなる。
次の順に、1段ずつ「動くことを確かめてから」進めるとよい。

コンパイルは `codes/Makefile` と同じ設定で行う。

```sh
cc -Wall -Wextra -Werror -pthread
```

## 段階ごとの作業

| 段階 | 書くもの | 確認すること |
|---|---|---|
| 1 | `codexion.h` の構造体、`main.c`、`parse.c`、`free.c` の `free_all` | 引数エラーが正しく出る |
| 2 | `init.c`、`free.c` の残り | 構造体を作って片付けられる（メモリリークなし） |
| 3 | `time.c`、`stop.c`、`print_log.c` | 1行のログが正しい形で出る |
| 4 | `simulation.c`、`coder.c` の N = 1 の流れ、`monitor.c` | 1人のとき 800ms で燃え尽きる |
| 5 | `actions.c`、`dongle.c`、`turn.c`、`request.c`（まず `fifo`） | N ≥ 2 で、燃え尽きずに規定回数で止まる |
| 6 | `request.c` の `edf`（`is_higher`） | `edf` でも同じように動く |

## 段階1：引数チェック

```sh
./codexion 4 800 200 200 200 5 0 rr      # Error: scheduler must be fifo or edf
./codexion 0 800 200 200 200 5 0 fifo    # Error: number_of_coders must be > 0
./codexion 4 8a0 200 200 200 5 0 fifo    # Error: argument is not digit
./codexion 4 800                         # Error: wrong number of argments
./codexion 4 800 200 200 200 0 0 fifo    # 何も出さず終了（終了コード 0）
```

## 段階2：構造体の準備と片付け

`run_simulations` の中でスレッドを作らずに、`set_table` → `free_table` だけを呼んで確かめる。

```sh
valgrind --leak-check=full ./codexion 4 800 200 200 200 5 0 fifo
```

`All heap blocks were freed` と出ればよい。

## 段階3：ログ

`print_log` を main から1回呼んで、`0 1 has taken a dongle` の形で出ることを確かめる。

## 段階4：1人のとき

```text
$ ./codexion 1 800 200 200 200 5 0 fifo
0 1 has taken a dongle
800 1 burned out
```

monitor・停止フラグ・ログの3つがそろって初めてこの出力になる。ここが通れば土台は完成である。

## 段階5・6：2人以上

```sh
./codexion 4 800 200 200 200 5 0 fifo
./codexion 5 800 200 200 200 5 0 edf
./codexion 4 800 200 200 200 5 0 fifo | grep -c burned   # 0 になること
```

確認すること：

- 最初に奇数番（1 と 3）がドングルを2本取り、コンパイルを始める。
- 同じ時刻に、隣どうしが2人とも `is compiling` になっていない。
- `burned out` が出たら、そのあとに何も出ていない。
- 全員が規定回数コンパイルしたら、何も出さずに止まる。

燃え尽きが出る例（`time_to_burnout` が短い）も試す。

```sh
./codexion 3 600 200 100 100 2 50 edf    # 600ms 付近で 1人が burned out
```

## データ競合の確認

スレッドが同じ変数を鍵なしで同時に読み書きしていないかは、helgrind で確かめられる。

```sh
valgrind --tool=helgrind ./codexion 4 800 200 200 200 3 0 fifo
```

`ERROR SUMMARY: 0 errors` が目標である。
