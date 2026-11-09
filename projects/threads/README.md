[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/-Qend-iM)
# Threads Exercises

This repository contains a set of exercises focused on threads programming, each in a separate folder. Read this README carefully to understand your assignment, how it will be graded, and how to write the report. This page also includes tips on some measurement tools.

## Grading

Your final grade for this assignment is composed of the following components:

- **Implementation (60%)**
- **Written report (40%)**

### Report

You must add a single PDF file named `report.pdf` to the root of this repository. Include your name, RA, and a performance analysis of the experiments. You are supposed to write one report covering all the exercises.

The report must contain:
1. **System Description**: The hardware and software you used (e.g., Linux version, compiler, CPU).
2. **Tools**: Which tools you used and how.

Then, for each exercise:
- Identify the most time-consuming part of the code.
- State the speedup you achieved on your system.
- Provide the execution time.
- Calculate the efficiency (speedup divided by number of used cores).
- Include a plot or table of execution times for different numbers of threads and **briefly interpret the results**.

Within each exercise description, you will find a set of questions to help you write your analysis. You can structure your report as you wish, but make sure to address all the points and aim for around 2 pages.

Below, you will find some tips on measuring execution time and profiling your code.

### Implementation

Do **not** change the `stdout` or `stderr` print statements in the exercises. They are used to verify correctness. If you change them, the tests will fail, and you will not receive full credit. We will grade the solution in the `main` branch of your repository.

You can use the test utility `check.py` (located in the root directory) to verify your solution at any time:

```sh
python3 check.py
```

We will consider a solution correct if it passes the performance requirement (> 1.5× speedup) and produces the expected output on a reference machine with 4 cores.

## Exercise Structure

Each exercise lives in its own folder with the necessary files and a `README` that explains how to complete it. Exercises build on each other, so it is recommended to do them in sequence.

## Getting Started

Clone this repository:

```sh
git clone <repository URL>
```

Navigate to the first exercise folder, read its `README`, follow the instructions, then move on to the next exercise.  
Use Google Classroom or the designated support time if you have any questions. Note that we only offer execution assistance for Linux systems.

## Prerequisites

You need a basic understanding of threads programming and the relevant programming language (C or C++). You also need a development environment and any required libraries or tools.

## Tools

Many tools are available for studying the performance of parallel programs. Some measure overall execution time, while others profile function-level time or collect metrics on cache misses, branch mispredictions, etc.
For now, pick a simple one like Hyperifne (describe below) to get used to. You can explore the other tools at your convenience.

Below are short descriptions of some tools you might use. Feel free to use any others you prefer.

### Measuring Time

If you simply want to measure how long a program takes to run, you can use the `time` command:

```sh
time ls
time ls -l
```

Running a command only once may not be enough for good measurements; run it multiple times and average the results. You can also automate it in a script.

An alternative is [Hyperfine](https://github.com/sharkdp/hyperfine), which compares multiple commands:

```sh
hyperfine 'ls' 'ls -l'
```

### Measuring Hot Spots

`gprof` helps you analyze how much time your program spends in different functions.

1. Compile with `-pg`:
   ```sh
   gcc -pg -o hello_threads hello_threads.c
   ```
2. Run your program:
   ```sh
   ./hello_threads 4
   ```
   This generates `gmon.out`.
3. Analyze with `gprof`:
   ```sh
   gprof ./hello_threads gmon.out
   ```
   
The report has two sections:
- **Flat Profile**: shows time per function.
- **Call Graph**: shows which functions call each other.

### Measuring Cache Misses

`perf` is a powerful Linux tool for analyzing performance, including CPU usage and cache misses.

```sh
perf record ./hello_threads 4
perf report
```

- `perf stat ./hello_threads 4` shows a summary of CPU usage.
- `perf record -g ./hello_threads 4 && perf report` displays a call graph.

## Contributing

If you find any issues or have suggestions, feel free to create an issue or pull request. Your feedback is welcome!

## About

This repository is one of the assignments handed out to the students in the course
"MC970 - Introduction to Parallel Programming" offered by the Institute of
Computing at Unicamp.