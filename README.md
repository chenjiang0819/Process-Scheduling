# Process-Scheduling

The program is called allocate and take the following command line arguments. The arguments can be passed in any order but you can assume that they will be passed exactly once.  
-f *filename* will specify the name of the file describing the processes.  
-p processors where processors is one of {1,2,N}, N ≤ 1024.  
-c an optional parameter, when provided, invokes your own scheduler from Section 4.  
  
The *filename* contains the processes to be executed and has the following format. Each line of the file corresponds to a process. The first line refers to the first process that needs to be executed, and the last line refers to the last process to be executed. Each line consists of a space-separated tuple (time-arrived, process-id, execution-time, parallelisable). You can assume that the file will be sorted by time-arrived which is an integer in [0, 2]) indicating seconds; all process-ids will be distinct integers in the domain of [0, 2]) and the first process will always have time-arrived set to 0; execution-time will be an integer in [1, 2]) indicating seconds; parallelisable is either n or p. If it is p, then the corresponding process is
parallelisable; if it is n, it is not. You can ignore n/p when -p is 1. More than one process can arrive at the same time.  
Example: ./allocate -f processes.txt -p 1.  
Given processes.txt with the following information:  
0 4 30 n  
2  
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
<current-time>,FINISHED,pid=<process-id>,proc_remaining=<num-proc-left>\n  
where:  
– ‘<current-time>’ is as above for the RUNNING event;  
– ‘process-id’ refers to the id of the process that has just been completed;  
– ‘num-proc-left’ refers to the number of processes that are waiting to be executed over all
processors (i.e., those that have already arrived but not yet completed, including those that
have unfinished subprocesses).  
If there is more than one event to be printed at the same time: print FINISHED before RUNNING and print
events for smaller CPU ids first.  
Example: Consider the last remaining process which has 10 seconds left to completion. Then the following
lines may be printed:  
20,RUNNING,pid=15,remaining_time=10,cpu=1  
30,FINISHED,pid=15,proc_remaining=0  
  
**Performance statistics**  
When the simulation is completed, 3 lines with the following performance statistics about your simulation
performance will be printed:  
• Turnaround time: average time (in seconds, rounded up to an integer) between the time when the
process completed and when it arrived;  
• Time overhead: maximum and average time overhead when running a process, both rounded to the
first two decimal points, where overhead is defined as the turnaround time of the process divided
by its total execution time (i.e., the one specified in the process description file).  
• Makespan: the time in seconds when your simulation ended.  
Example: For the invocation with arguments -p 1 and the processes file as described above, the simulation would print：  
Turnaround time 62  
Time overhead 2.93 1.9  
Makespan 120  
