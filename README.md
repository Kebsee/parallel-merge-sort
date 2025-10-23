# Parallel Merge Sort (Group 120)

## Overview



## Manifest

- `Makefile` - build rules for the `test-mergesort` executable
- `mergesort.c` — implementation of `merge`, serial `my_mergesort`, `parallel_mergesort`, and helper functions
- `mergesort.h` — public declarations and global variables used across files
- `test-mergesort.c` — test harness that generates input, runs sorting, checks correctness, and reports timing
- `README.md` — explanation of the project (what you are reading right now)

## Building the project

This project uses `gcc` and a simple Makefile. From the project root run:

```
make
```

This produces the `test-mergesort` executable.

To clean build artifacts:

```
make clean
```

## Features and usage

`test-mergesort` accepts three command-line arguments:

```
./test-mergesort <input size> <cutoff level> <seed>
```

- `<input size>`: number of integers to generate and sort (n >= 2)
- `<cutoff level>`: how many levels of thread spawning to perform (0 = fully serial)
- `<seed>`: random seed for input generation

Examples:

```
# Serial (single-threaded)
./test-mergesort 100000000 0 1234

# Parallel with 2 levels (creates up to 4 worker threads)
./test-mergesort 100000000 2 1234
```

## Testing

The `test-mergesort` program already includes correctness-checking and timing.
Our recommended testing strategy:

1. Start small to verify correctness:

```
make
./test-mergesort 100 1 1234
```

2. Run scalability tests (replace size with large values and varied cutoff):

```
# 1M elements
./test-mergesort 1000000 0 1234
./test-mergesort 1000000 3 1234

# 100M elements
./test-mergesort 100000000 0 1234
./test-mergesort 100000000 3 1234
```

Measure the wall-clock time printed by the program. The assignment requires the parallel version to achieve at least 2x speedup over the serial run (cutoff = 0).

## Known Bugs / Limitations

- The program does simple `malloc`/`free` for thread arguments therefore this allocation overhead could be reduced.

## Reflection and Self-Assessment


## Sources Used

- [POSIX threads explanation (`pthread_create`, `pthread_join`)](https://medium.com/@ayogun/posix-thread-pthread-library-8915d867201f)
- Standard merge sort algorithm from textbooks and [example video](https://www.youtube.com/watch?v=KF2j-9iSf4Q&t=372s)
- Comp Sci 3307 project specification and README.template provided with the starter code


## Authors
- Jake Roberts
- Jonathon Sadler
- Lara Grocke