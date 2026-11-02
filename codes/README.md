*This project has been created as part of the 42 curriculum by kamurai.*

# Codexion

## Description

<details>
<summary>Click to expand</summary>

Codexion is a concurrency simulation written in C with POSIX threads.

### Rules of the subject

This program is based on the following rules defined by the subject.

- **Seats**: one or more coders sit around a circular table. The coders are numbered
  from 1 to `number_of_coders`, and coder 1 sits next to coder `number_of_coders`.
- **Dongles**: there is one dongle between each pair of neighbouring coders, so there
  are as many dongles as coders. When there is only one coder, there is only one dongle.
- **Workflow**: a coder repeats "compile → debug → refactor".
  A coder does only one of these three tasks at a time.
- **Condition to compile**: to compile, a coder must hold two dongles at the same time,
  the left one and the right one. After compiling, the coder puts both dongles back
  on the table.
- **Cooldown**: nobody can take a returned dongle until `dongle_cooldown` milliseconds
  have passed.
- **Arbitration**: when two coders request the same dongle, the dongle chooses who
  gets it according to `scheduler`.
  - `fifo`: the dongle goes to the coder who requested it first.
  - `edf`: the dongle goes to the coder with the earliest burnout deadline
    (`last_compile_start + time_to_burnout`).
- **Burnout**: a coder who does not start compiling within `time_to_burnout`
  milliseconds since the start of their last compile (or since the start of the
  simulation) burns out.
- **No communication**: coders do not talk to each other. A coder does not know
  whether another coder is about to burn out.
- **End**: the simulation stops when one of the following happens.
  - A coder burns out.
  - Every coder has compiled at least `number_of_compiles_required` times.

### Goal of this program

The goal of this program is to share a limited resource (the dongles) between
threads without deadlock, starvation, or data races. To reach this goal, the
program implements:

- one thread per coder, and one monitor thread that detects burnout within 10 ms,
- a cooldown on each dongle after it is returned,
- fair arbitration of dongle requests with a `fifo` or `edf` scheduler,
- serialized logging so that two messages never mix on one line.

### Source files

| File | Role |
|---|---|
| `main.c`, `parse.c` | Read and validate the 8 command-line arguments |
| `init.c`, `simulation.c`, `free.c` | Build the table, create and join the threads, free everything |
| `threads/coder.c`, `threads/actions.c` | Routine of one coder thread (take dongles, compile, debug, refactor) |
| `threads/dongle.c`, `threads/request.c`, `threads/turn.c` | Dongle acquisition, request queue (priority queue), turn decision |
| `threads/monitor.c` | Routine of the monitor thread (burnout and completion check) |
| `threads/print_log.c`, `threads/stop.c`, `threads/time.c` | Logging, stop flag, time helpers |

</details>

## Instructions

<details>
<summary>Click to expand</summary>

### Compilation

```sh
make
```

The Makefile compiles the sources with `cc -Wall -Wextra -Werror -pthread` and
produces the executable `codexion`.

| Rule | Effect |
|---|---|
| `make` / `make all` | Build `codexion` |
| `make clean` | Remove the object files (`.o`) |
| `make fclean` | Remove the object files and `codexion` |
| `make re` | Run `fclean`, then build `codexion` again |

### Execution

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Meaning |
|---|---|
| `number_of_coders` | Number of coders, and also number of dongles (1 or more) |
| `time_to_burnout` | Time (ms) after which a coder who has not started compiling burns out |
| `time_to_compile` | Time (ms) a coder spends compiling while holding two dongles |
| `time_to_debug` | Time (ms) a coder spends debugging |
| `time_to_refactor` | Time (ms) a coder spends refactoring |
| `number_of_compiles_required` | The simulation stops when every coder has compiled this many times |
| `dongle_cooldown` | Time (ms) during which a returned dongle cannot be taken again |
| `scheduler` | Arbitration policy of the dongles: `fifo` or `edf` |

Example:

```sh
./codexion 4 800 200 200 200 3 10 edf
```

All numeric arguments must be non-negative integers. If an argument is missing
or invalid, the program exits with status `1`.

### Output

The subject defines only the following five log messages.

| Event | Exact format |
|---|---|
| A coder took a dongle | `timestamp_in_ms X has taken a dongle` |
| A coder started compiling | `timestamp_in_ms X is compiling` |
| A coder started debugging | `timestamp_in_ms X is debugging` |
| A coder started refactoring | `timestamp_in_ms X is refactoring` |
| A coder burned out | `timestamp_in_ms X burned out` |

- `timestamp_in_ms`: the time elapsed (in ms) from the start of the simulation
  to the moment the log is printed.
- `X`: the number of the coder who prints the log.
- The timestamp, the number, and the message are separated by a single space,
  and each line ends with a newline.

</details>

## Blocking cases handled

<details>
<summary>Click to expand</summary>

### 1. Deadlock (everyone waits for each other and stops)

- Problem: if every coder takes their left dongle at the same time, every coder
  waits for their right dongle forever.
- Solution: odd-numbered coders take their left dongle first, and even-numbered
  coders take their right dongle first.
- Result: two neighbouring coders compete for the same dongle, so a cycle in
  which every coder holds one dongle and waits cannot form.

### 2. Starvation (one coder never gets a turn)

- Problem: if the same coder keeps taking a dongle, the neighbouring coder burns out.
- Solution: each dongle has a queue of requests and hands itself over in a fixed order.
  - fifo: the dongle goes to the coder who requested it first.
  - edf: the dongle goes to the coder whose burnout deadline is closest.
    If two deadlines are equal, the coder who requested first wins.
- Result: a coder who has just compiled cannot overtake the neighbour who is waiting.

### 3. Cooldown (a returned dongle cannot be used right away)

- Problem: nobody sends a signal when a cooldown ends, so a coder sleeping in
  `pthread_cond_wait` cannot wake up.
- Solution: the coder sleeps with `pthread_cond_timedwait`, passing the time at
  which the cooldown ends.
- Result: the coder wakes up at whichever comes first: a signal, or the end of the cooldown.

### 4. Burnout detection (report within 10 ms)

- Solution: the monitor thread checks every coder every 1 ms.
- Reason: with a 1 ms interval, the report stays well within 10 ms even with OS delays.
- A working coder sleeps 500 µs at a time and checks `stop` at each step, so the
  coder notices the end of the simulation quickly.

| Thread | Acceptable delay | Wait interval |
|---|---|---|
| monitor | 10 ms | 1 ms |
| coder while working | about 1 ms | steps of 500 µs |
| coder waiting for a dongle | none (woken by a signal) | `pthread_cond_wait` |
| coder waiting for a cooldown | exactly when the cooldown ends | `pthread_cond_timedwait` |

### 5. Mixed logs (two messages on one line)

- Problem: if two coders print at the same time, two messages can mix on one line.
- Solution: `print_log` locks the output mutex (`log_mutex`) before printing one line.
- Result: no other coder can print until the line is finished. No log is printed
  after `burned out`.

</details>

## Thread synchronization mechanisms

<details>
<summary>Click to expand</summary>

### Threads

- Coder threads: `number_of_coders` threads, each running `run_simulation()`.
- Monitor thread: one thread running `monitor_routine()`.
- Main thread: creates all threads and waits for them with `pthread_join`.

### Locks and what they protect

| Primitive | Owner | Protected data |
|---|---|---|
| `pthread_mutex_t mutex` | each `t_dongle` | `in_use`, `released_at`, `request_queue`, `request_count`, `next_arrival_order` |
| `pthread_cond_t cond` | each `t_dongle` | Wakes coders waiting for this dongle |
| `pthread_mutex_t mutex` | each `t_coder` | `last_compile_start`, `compile_count` |
| `pthread_mutex_t stop_mutex` | `t_table` | `stop` (the end-of-simulation flag) |
| `pthread_mutex_t log_mutex` | `t_table` | Standard output |

`t_config` is written only by the main thread before the threads start, so it
needs no lock.

### pthread_mutex functions

| Function | Role |
|---|---|
| `pthread_mutex_init(&m, attr)` | Initializes the mutex `m` |
| `pthread_mutex_destroy(&m)` | Destroys the mutex `m` |
| `pthread_mutex_lock(&m)` | Locks `m`. If another thread holds it, waits until it is released |
| `pthread_mutex_unlock(&m)` | Releases `m`, which the caller holds |
| `pthread_mutex_trylock(&m)` | Tries to lock `m`. If it cannot, returns `EBUSY` without waiting |
| `pthread_mutex_timedlock(&m, &t)` | Waits to lock `m` until time `t` (not available on macOS) |

`pthread_mutex_trylock` and `pthread_mutex_timedlock` are listed for reference
and are not used in this project.

### How race conditions are prevented

- **Taking a dongle**: a coder checks `in_use` and sets it to `true` while
  holding the dongle's mutex. Two coders therefore can never take the same
  dongle at the same time.
- **Burnout check**: a coder updates `last_compile_start` under its own mutex,
  and the monitor reads it under the same mutex. The monitor therefore never
  reads a half-written value.
- **Stop flag**: every read of `stop` goes through `is_stopped()`, and every
  write goes through `stop_simulation()`. Both functions take `stop_mutex`.

### Communication between coders and the monitor

- **Coder to monitor**: each coder writes `last_compile_start` and
  `compile_count` under its own mutex. The monitor reads them to detect a
  burnout or the end of all compiles.
- **Monitor to coders**: the monitor sets `stop`, then locks every dongle and
  calls `pthread_cond_broadcast` on its `cond`. Coders sleeping on a dongle
  wake up, see `stop`, and leave their loop.
- **No lost wake-up**: a waiting coder checks `stop` while holding the dongle's
  mutex, and `pthread_cond_wait` releases that mutex and starts sleeping in one
  atomic step. The monitor needs the same mutex to broadcast, so the broadcast
  always reaches a coder that has already started sleeping.

</details>

## Resources

<details>
<summary>Click to expand</summary>

For this project, I mainly referred to answers from AI (Claude by Anthropic)
on how to handle threads in C.
AI was also used for the following tasks:

- Writing the Makefile and explaining its rules and compiler options
  (such as `-pthread`).
- Checking the project against the requirements of the subject.
- Reorganizing, translating, and completing this README.

</details>
