[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/ZBLcpCGt)
# CUDA Labs

This repository contains a set of three labs focused on CUDA parallel programming, split into separate folders, each containing a specific lab exercise.

> **Observation**  
> The test harness redirects any CUDA launch or runtime errors to `*.err` or `*.time` files.  
> If your program “works” but no output appears, open the corresponding `.time` file first.

## Lab Exercise Structure

Each lab is located in a separate folder and contains the necessary files and instructions to complete it. The instructions for each lab exercise are located in a README file in the respective exercise folder.

The labs are designed to be completed in order, as each lab builds upon the previous one. Therefore, it is recommended that you start with the first lab and work your way through them in sequence.

## First: clone on local machine

Below are instructions to run this repository locally. We recommend using the Google Colab method above, and we only offer support to users using the Google Colab execution method.

To get started, clone this repository to your local machine using the following command:

```sh
git clone --recurse-submodules <repository URL>
```

The command `--recurse-submodules` is used to clone the test folder, which is from a separate repository, because of its size.

If you have already cloned the repository without the --recurse-submodules option, you can still clone the submodules by running the following command in the repository's root directory:

```sh
git submodule update --init --recursive
```

Once the repository is cloned, navigate to the first lab folder and read the instructions in the README file. Follow the instructions to complete the lab, and then move on to the next lab in the sequence.

## Running on Google Colab

1. We will use Google Drive for storage. Go to [Drive](https://drive.google.com/drive) and upload this repository folder. Tip: put it in the root of your Drive, in a folder called `cuda-exercises-01`.
2. Access [Google Colab](https://colab.research.google.com/).
3. Go to **File → Upload notebook → Browse** and select `colab-runner.ipynb` from this repository.
4. Run the first cell and authorize Google Drive access.
5. Select the GPU runtime (NVIDIA T4): **Runtime → Change runtime type → T4 GPU → Save**.
6. **Runtime → Run all → Run anyway**.

This will compile and test your code; inspect the cell outputs.  
**Important:** remember to copy your solutions to your repository and commit them!

## Grading and Report

| Component      | Weight |
| -------------- | ------ |
| Implementation | **60 %** |
| Written report | **40 %** |

### Implementation rules

* Your program must produce the expected output and achieve a **≥ 5 × speed‑up** over the provided serial baseline (reasonable exceptions for the smallest input where transfers dominate).  
* **Do not modify** where timing is taken. If the code times the whole program or only the kernel, leave it that way.

### Report requirements

Add a single **`report.pdf`** to the repository root. One report covers **all two labs**.

Include for each lab:

1. **System description** (Colab GPU model, CUDA version; include local specs if relevant).  
2. **Tools** used (compiler flags, profilers, scripts).  
3. **Performance analysis**  
   * Code section that dominated runtime pre‑optimisation.  
   * Achieved speed‑up and absolute execution time.  
   * **Efficiency** = speed‑up ÷ theoretical maximum occupancy for your chosen launch configuration.  
   * **Grid & block‑size experiments** – at least three meaningful configurations (e.g., 128 × 1 vs. 256 × 1 threads per block, differing grid sizes). Present the results in a table or plot and briefly explain the behaviour.

> *We cannot vary “number of threads” like on a CPU test rig because Colab provides exactly one GPU. Instead, analyse how different **block dimensions and grid sizes** affect throughput and occupancy.*

Keep the report to roughly two pages; focus on clear, data‑driven insights.

## Prerequisites

To complete these labs, you will need a basic understanding of the C or C++ programming language and the fundamentals of parallel programming concepts. You will also need a development environment for the language used in the labs and CUDA libraries installed, which is already installed in the Google Colab environment.

## Contributing

If you find any issues with the labs or have suggestions for improvements, feel free to create an issue or pull request on this repository.

## About

This repository is one of the assignments handed out to the students in the course
"MC970 - Introduction to Parallel Programming" offered by the Institute of
Computing at Unicamp.