# Parallel Merge Sort (Group 120)

## Overview

This project implements a parallel merge sort algorithm using POSIX threads (pthreads) in C.  
The program sorts arrays of integers by recursively dividing the sorting task across multiple threads up to a specified cutoff level.  
Beyond the cutoff level, the algorithm falls back to serial merge sort to prevent thread overhead from degrading performance.

---

## Manifest

- `Makefile` — build rules for the `test-mergesort` executable  
- `mergesort.c` — implementation of `merge`, serial `my_mergesort`, `parallel_mergesort`, and helper functions  
- `mergesort.h` — public declarations and global variables used across files  
- `test-mergesort.c` — test harness that generates input, runs sorting, checks correctness, and reports timing  
- `README.md` — documentation and reflection (this file)

---

## Building the Project

This project uses `gcc` and a simple Makefile.  
From the project root, run:

```bash
make
```

This produces the `test-mergesort` executable.

To clean build artifacts:

```bash
make clean
```

---

## Features and Usage

`test-mergesort` accepts three command-line arguments:

```bash
./test-mergesort <input size> <cutoff level> <seed>
```

- `<input size>` — number of integers to generate and sort (n ≥ 2)  
- `<cutoff level>` — number of thread levels to spawn (0 = fully serial)  
- `<seed>` — random seed for input generation

**Examples:**

```bash
# Serial (single-threaded)
./test-mergesort 100000000 0 1234

# Parallel with 2 levels (creates up to 4 worker threads)
./test-mergesort 100000000 2 1234
```

---

## Testing

### Batch Performance Testing and Evaluation

To measure scalability and speedup, we automated all large-scale performance tests using a single Bash script:

```bash
bash tests/run_all.sh
```

This script automatically builds the project (if required) and then executes the `test-mergesort` binary with cutoff levels 0 through 8 running each configuration three times on an input of 100 million elements using a fixed random seed (1234).

Each execution’s full console output, including the “Sorting … took X seconds” timing lines and cutoff headers, is appended to one combined, timestamped log file inside the `logs/` directory.

**Example:**

```
logs/all_runs_20251102_222705.log
```

Each section in the log is clearly labeled with:

- The cutoff level being tested (`>>> Cutoff = 0 … 8`)  
- The repetition number (`Run 1/3`, `Run 2/3`, `Run 3/3`)  
- The full output of each `test-mergesort` run  

This single file provides a complete record of all test iterations, ensuring the performance results are reproducible, traceable, and verifiable.

**Summary:**

- Input size: 100,000,000 elements  
- Cutoff range: 0–8  
- Repetitions per cutoff: 3  
- Output: single combined log in `logs/`  
- Purpose: measure runtime and speedup across thread depths  

This automated batch testing approach provided a reliable and repeatable benchmark for assessing how thread depth (cutoff) affects overall sorting performance and confirmed that our implementation achieved well above the required 2× speedup compared to the serial baseline.

---

## Known Bugs / Limitations

- Thread argument structs are allocated and freed dynamically using `malloc` and `free`. While functionally correct, this introduces minor allocation overhead that could be reduced using memory pooling or argument reuse.  
- No explicit mutex or synchronization primitives were required since threads operate on disjoint array segments, but this limits experimentation with shared-state synchronization.

---

## Reflection and Self-Assessment

The most challenging part was understanding how the recursive parallelism actually worked. It took a while to wrap our heads around how each recursive call could create its own threads and still stay coordinated. We started by getting a normal serial merge sort working first, then added the parallel part once that was stable. Figuring out how thread creation and joining worked at each recursion level, and how the cutoff controlled how many threads were made, was tricky at first. Once we understood that structure, the flow of the parallel version made sense and we were able to get the threading working correctly. It was rewarding to see the transition from a single-threaded version to a functioning parallel merge sort with measurable speedup.

We encountered a significant debugging issue involving a double-free error caused by improper memory ownership between threads. During early testing, the program occasionally crashed or produced heap corruption errors. The cause was traced to a thread routine freeing memory that was not heap-allocated. The function `parallel_mergesort` was sometimes invoked with the address of a local stack variable (for example, `parallel_mergesort(&tmp)`). Inside that function, all arguments created via `buildArgs()` are freed by the thread itself when `a->level > 0`. Because `tmp` was a stack variable (not allocated with `malloc`), the thread attempted to free stack memory, leading to a double-free or heap corruption.

**Fix applied:**

We ensured that `parallel_mergesort` is only ever called with pointers returned by `buildArgs()`, which allocates its argument structure on the heap. All serial fallbacks now call `my_mergesort(...)` directly instead of calling `parallel_mergesort` on stack variables.

Under this design:
- `buildArgs()` allocates each argument with `malloc`.  
- `parallel_mergesort` frees its own argument only when `a->level > 0`.  
- The top-level (`level = 0`) argument, owned by the test harness, is never freed inside the function.  

After applying this fix, the crashes stopped and the program began running reliably across repeated tests.  
This issue taught us the importance of defining clear ownership rules between threads and understanding how memory allocation interacts with concurrency. Once that concept clicked, debugging and reasoning about thread behavior became much easier.

---

## Sources Used

- [POSIX Threads – `pthread_create`, `pthread_join` Overview](https://man7.org/linux/man-pages/man3/pthread_create.3.html)  
- Standard merge sort algorithm from textbooks and online resources  
- COMP SCI 3307 project specification and provided `README.template`

---

## Authors

- Jake Roberts  
- Jonathon Sadler  
- Lara Grocke
