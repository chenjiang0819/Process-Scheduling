# CPU Process Scheduling Simulator

A handwritten C simulation of preemptive shortest-remaining-time scheduling across one or multiple CPUs, including explicit parallel-process decomposition and an alternative allocation strategy.

Chen Jiang’s University of Melbourne computing coursework.

University: University of Melbourne  
Course: COMP30023 Computer Systems, Assignment 1  
Year: 2021  

## Implementation

Linked CPU queues, least-workload allocation, process/subprocess state, shortest-remaining-time selection, alternative waiting-area allocation, and turnaround/overhead/makespan statistics.

## Technical approach

The original scheduler advances active execution one time unit at a time. Ready work is allocated to CPUs by remaining workload and selected by remaining execution time, with deterministic identifier tie breaks.

Parallel parents retain logical completion state while children carry execution counters; decimal-looking subprocess identifiers are now rendered from exact parent/subprocess integers. Idle periods can advance to the next arrival without changing active execution semantics.

## Run

Run `make`, then `./allocate -f processes.txt -p 2` (optional `-c` selects the original alternative strategy). Run `make test` and `make sanitize`.

## Checks and results

- Apple Clang 21: normal -O2 build and regression suite passed.
- AddressSanitizer + UndefinedBehaviorSanitizer regression suite passed on macOS.
- GCC 14.3 in a network-disabled Linux container: strict -Werror sanitizer checks passed, including LeakSanitizer defaults.
- make test: seven methods passed, including 30 independent SRTF-oracle cases, 100-job cases, both strategies, 1–1,024 processors, idle gaps, simultaneous completion, and repeated execution.
- Strict Clang -O0 -Werror build and regression suite passed.

## 2026 maintenance

- Validated CLI flags and ordered unique process records; rejected malformed, empty, zero-duration, and overflow inputs.
- Corrected idle-gap termination, safe candidate selection, queued-job accounting, serialized fallback state, and child ownership registration.
- Separated parent/subprocess IDs to preserve exact large IDs and suffixes beyond 9.
- Added deterministic fixtures and an independently implemented scheduling oracle; retained both original strategies.

These changes are 2026 AI-assisted maintenance of the original student implementation.

## Usage notes

- The input arrival order must be nondecreasing and process IDs must be unique.
- The simulation uses representable integer time; inputs whose scheduling horizon overflows are rejected.
- The alternative strategy is preserved as a heuristic, not claimed to be globally optimal.

## Original project notes

# Process-Scheduling

This program takes the following command line arguments. The arguments can be passed in any order but they will be passed exactly once.  
-f *filename* will specify the name of the file describing the processes.  
-p processors where processors is one of {1,2,N}, N ≤ 1024.  
-c an optional parameter.  
  
The *filename* contains the processes to be executed and has the following format. Each line of the file corresponds to a process. The first line refers to the first process that needs to be executed, and the last line refers to the last process to be executed. Each line consists of a space-separated tuple (time-arrived, process-id, execution-time, parallelisable). Program will assume that the file will be sorted by time-arrived which is an integer in [0, 2]) indicating seconds; all process-ids will be distinct integers in the domain of [0, 2]) and the first process will always have time-arrived set to 0; execution-time will be an integer in [1, 2]) indicating seconds; parallelisable is either n or p. If it is p, then the corresponding process is parallelisable; if it is n, it is not. Program will ignore n/p when -p is 1. More than one process can arrive at the same time.  
  
Example: ./allocate -f processes.txt -p 1.  
Given processes.txt with the following information:  
0 4 30 n  
3 2 40 n  
5 1 20 n  
20 3 30 n  
  
The program will simulate execution of 4 processes where process 4 arrives at time 0, needs 30 seconds running time to finish; process 2 arrives at time 3 and needs 40 seconds of time to complete. Each line (including the last) will be terminated with a LF (ASCII 0x0a) control character.  
  
**Execution transcript**  
  
For the following events the code will print out a line in the following format:  
• When a (sub)process starts and every time it resumes its execution: <current-time>,RUNNING,pid=<process id>,remaining_time=<T>,cpu=<cpu-id>\n  
where:  
‘current-time’ refers to the time at which CPU is given to a process;  
‘process-id’ refers to the id of the process that is about to be run;  
‘T’ refers to the remaining execution time for this process;  
‘cpu-id’ refers to the processor where the process is scheduled on. It can be 0, 1, 2 . . . , N −1 when
-p N for N ≥ 1;  
Sample output could be:  
20,RUNNING,pid=15,remaining_time=10,cpu=0  
• Every time a process finishes:  
(current-time), FINISHED,pid = (process-id), proc_remaining = (num-proc-left) \n  
where:  
– ‘current-time’ is as above for the RUNNING event;  
– ‘process-id’ refers to the id of the process that has just been completed;  
– ‘num-proc-left’ refers to the number of processes that are waiting to be executed over all processors (i.e., those that have already arrived but not yet completed, including those that have unfinished subprocesses).  
If there is more than one event to be printed at the same time: print FINISHED before RUNNING and print events for smaller CPU ids first.  
  
Example: Consider the last remaining process which has 10 seconds left to completion. Then the following lines may be printed:  
20,RUNNING,pid=15,remaining_time=10,cpu=1  
30,FINISHED,pid=15,proc_remaining=0  
  
  
**Performance statistics**  
  
When the simulation is completed, 3 lines with the following performance statistics about the simulation performance will be printed:  
• Turnaround time: average time (in seconds, rounded up to an integer) between the time when the process completed and when it arrived;  
• Time overhead: maximum and average time overhead when running a process, both rounded to the first two decimal points, where overhead is defined as the turnaround time of the process divided by its total execution time (i.e., the one specified in the process description file).  
• Makespan: the time in seconds when the simulation ended. 
  
Example: For the invocation with arguments -p 1 and the processes file as described above, the simulation would print：  
Turnaround time 62  
Time overhead 2.93 1.9  
Makespan 120
