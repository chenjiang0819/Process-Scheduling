#include <stdio.h>
#include "data_structure.h"

#ifndef SCHEDULING_MECHANISM
#define SCHEDULING_MECHANISM

/****************************************************************/

//comparing function for quick sort - small to big
int cmpfunc (const void * input1, const void * input2);

//comparing function for quick sort- big to small
int cmpfunc_task (const void * input1, const void * input2);

//the function for assigning the processes
void assign_prcesses(list_t** cpu, node_t** input_doc, int processor_num, int total_processes, 
	int max_time, int total_time, int* cpu_que, int my_algorithm, list_t* free_all_nodes, node_t** waiting_area);

//function for forking the processes
void forking (int* forked_num, node_t** forked, int processor_num, node_t* assign, int my_algorithm);

//traverse all CPUs to record their status and find the minium workload cpu 
int find_mini_cpu(list_t** cpu, int processor_num, int max_time, int* cpu_n_remain);

//the function do scheduling 
void scheduling (list_t** cpu ,  node_t** input_doc, int processor_num, int total_processes, 
	int max_time, int* total_time, int my_algorithm, list_t* free_all_nodes);

#endif 

