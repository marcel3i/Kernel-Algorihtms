# Kernel Scheduler

**Kernel Scheduler** is a simulation of various CPU scheduling algorithms typically used within an operating system kernel. This project demonstrates different scheduling strategies including:

- **Shortest Job First (SJF)**
- **Earliest Deadline First (EDF)**
- **First-Come, First-Served (FCFS)**
- **Round Robin (RR)**
- **Priority Scheduling**
- **Priority Round Robin (Priority RR)**

Each scheduler considers job attributes such as name, priority, burst time, start time, and (optionally) a deadline. The simulation calculates key metrics like average waiting time, turnaround time, and response time for performance analysis.

---

## Features

- **Job Structure:**  
  Each job has:
  - `job_name`: A string identifier.
  - `priority`: Numerical priority.
  - `burst`: CPU burst time.
  - `start_time`: Time when the job becomes eligible.
  - `deadline`: Optional deadline for EDF scheduling (set to -1 if unused).

- **Multiple Scheduling Algorithms:**
  - **SJF:** Non-preemptive execution of the shortest job first.
  - **EDF:** Non-preemptive Earliest Deadline First for jobs with deadlines.
  - **FCFS:** Simple first-come, first-served ordering.
  - **RR:** Round Robin scheduling with a configurable time quantum.
  - **Priority:** Non-preemptive scheduling based on job priority.
  - **Priority RR:** A hybrid approach combining priority scheduling with round robin time slicing.

- **Metrics Calculation:**  
  The system computes average waiting time, turnaround time, and response time for each scheduler.

---

## Directory Structure
```
Kernel-Scheduler/
├── include/
│   ├── job.h                # Defines the Job data structure.
│   ├── scheduler.h          # Scheduler function prototypes.
│   ├── list.h               # Linked list structure for job queues.
│   └── cpu.h                # CPU simulation interface.
├── src/
│   ├── cpu.c                # Implements the CPU simulation.
│   ├── list.c               # Implements linked list operations.
│   ├── scheduler_sjf.c      # SJF scheduler implementation.
│   ├── scheduler_edf.c      # EDF scheduler implementation.
│   ├── scheduler_fcfs.c     # FCFS scheduler implementation.
│   ├── scheduler_rr.c       # Round Robin scheduler implementation.
│   ├── scheduler_priority.c # Priority scheduler implementation.
│   ├── scheduler_priority_rr.c  # Priority Round Robin scheduler implementation.
│   └── main.c               # Main driver: reads input and runs the selected scheduler.
├── jobs.txt                 # Sample input file (CSV format).
└── README.md                # This file.
```
---
## Getting Started
- **Prerequisites**
1) C Compiler: You need a C compiler such as GCC. On Windows, using MinGW or MinGW-W64 is recommended. Ensure that the compiler's bin directory is added to your system PATH.
  
- **Installation**
  
1) Clone the Repository:
```
git clone https://github.com/yourusername/Kernel-Scheduler.git
cd Kernel-Scheduler
```
2) Compile the Project:
```
gcc -Iinclude src/*.c -o scheduler
```

## Usage

### Input File Format

The input file (e.g., `jobs.txt`) should be in CSV format:
```bash
job_name,priority,burst,start_time,deadline
```
For example:
```bash 
JobA,5,10,0,-1
JobB,3,5,2,20
JobC,4,8,4,18
```

**Note:** For schedulers that do not use deadlines (e.g., SJF or FCFS), set `deadline` to `-1`.

### Running the Simulation

The program accepts two command-line arguments:

1. The input file (e.g., `jobs.txt`)
2. The scheduler type

Valid scheduler types are:

- `sjf`
- `edf`
- `fcfs`
- `rr`
- `priority`
- `priority_rr`

#### Examples:

```bash
./scheduler jobs.txt sjf
./scheduler jobs.txt edf
./scheduler jobs.txt fcfs
./scheduler jobs.txt rr
./scheduler jobs.txt priority
./scheduler jobs.txt priority_rr

