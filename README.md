# Parallel Merge Sort (Group 120)

## Overview

This project implements a parallel merge sort alogirthm using POSIX threads (pthreads)
in C. The program sorts arrays of integers by recursively dividing the sorting task
across multiple threads up to a specified cutoff level. Beyond the cutoff level, the
algorithm falls back to serial merge sort to prevent thread overhead from degrading
performance.

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

To measure scalability and speedup, we automated all large-scale performance tests using a single Bash script:run_all.sh
This script automatically builds the project (if required) and then executes the test-mergesort binary with cutoff levels 0 through 8, running each configuration three times on an input of 100 million elements using a fixed random seed (1234).
Each execution’s full console output — including the “Sorting … took X seconds” timing lines and cutoff headers — is appended to one combined timestamped log file inside the logs/ directory.
For example:
logs/all_runs_20251102_222705.log
Every block in this log is clearly labeled with:

The cutoff level being tested (>>> Cutoff = 0 … 8)

The repetition number (Run 1/3, Run 2/3, Run 3/3)

The full output of each test-mergesort run

This single file provides a complete record of all test iterations, ensuring the performance results are reproducible, traceable, and verifiable by markers or other group members.
After completing the batch run, we parsed the timing data to determine the average runtime per cutoff and computed the speedup relative to the averaged serial baseline (cutoff = 0). The expected trend, consistent with our results, is a strong performance gain up to around cutoff = 5, after which further thread creation yields diminishing returns due to system overhead and resource saturation.

This automated batch-testing approach provided a reliable, repeatable benchmark for assessing how thread depth (cutoff) affects overall sorting performance and verified that our implementation achieves well above the required 2× speedup compared to the serial baseline.

## Known Bugs / Limitations

- The program does simple `malloc`/`free` for thread arguments therefore this allocation overhead could be reduced.

## Reflection and Self-Assessment

The most challenging part was understanding how the recursive parallelism actually worked. It took a while to wrap our heads around how each recursive call could create its own threads and still stay coordinated. We started by getting a normal serial merge sort working first, then added the parallel part once that was stable. Figuring out how thread creation and joining worked at each recursion level, and how the cutoff controlled how many threads were made, was tricky at first. Once we understood that structure, the flow of the parallel version made sense and we were able to get the threading working correctly. It was rewarding to see the transition from a single-threaded version to a functioning parallel merge sort with measurable speedup.

We encountered a significant debugging issue involving a double-free error caused by improper memory ownership between threads. During early testing, the program occasionally crashed or produced heap corruption errors. The cause was traced to a thread routine freeing memory that was not heap-allocated. The function parallel_mergesort was sometimes invoked with the address of a local stack variable, for example parallel_mergesort(&tmp). Inside that function, all arguments created via buildArgs() are freed by the thread itself when a->level > 0. Because tmp was a stack variable (not allocated with malloc), the thread attempted to free stack memory, leading to a double-free or heap corruption.

To fix this, we ensured that parallel_mergesort is only ever called with pointers returned by buildArgs(), which allocates its argument structure on the heap. All serial fallbacks now call my_mergesort(...) directly instead of calling parallel_mergesort on stack variables. Under this design:

buildArgs() allocates each argument with malloc.

parallel_mergesort frees its own argument only when a->level > 0.

The top-level (level-0) argument, owned by the test harness, is never freed inside the function.

After applying this fix, the crashes stopped and the program began running reliably across repeated tests. This issue taught us the importance of defining clear ownership rules between threads and understanding how memory allocation interacts with concurrency. Once that concept clicked, debugging and reasoning about thread behavior became much easier.


## Sources Used

- [POSIX threads explanation (`pthread_create`, `pthread_join`)](https://medium.com/@ayogun/posix-thread-pthread-library-8915d867201f)
- Standard merge sort algorithm from textbooks and [example video](https://www.youtube.com/watch?v=KF2j-9iSf4Q&t=372s)
- Comp Sci 3307 project specification and README.template provided with the starter code


## Authors
- Jake Roberts
- Jonathon Sadler
- Lara Grocke