#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "cpu_units.h"

#define INITIAL 0
#define IMPOSSIBLE_NUM -1
#define FINISH 1

/****************************************************************/

//initialise the CPU
void initial_CPU(int processor_num, list_t** cpu, list_t* free_all_nodes){
	//setup the list for each CPU
	for(int i = 0; i < processor_num ; i++){
		cpu[i] = make_empty_list();
	}
	//put the leaders for each CPU
	for(int i = 0; i < processor_num; i++){
		node_t* new_node = (node_t*)malloc(sizeof(*new_node));
		assert(new_node != NULL);
		new_node -> time_arrived = IMPOSSIBLE_NUM;
        new_node -> process_id =  IMPOSSIBLE_NUM;	        
		new_node -> execution_time = INITIAL;
    	new_node -> execution_counter = INITIAL;
    	new_node -> process_turnaround = INITIAL;
		new_node -> process_overhead = INITIAL;
    	new_node -> parallelisable = 'i';
    	new_node -> status = FINISH;
    	new_node -> next = NULL;
    	new_node -> free_next = NULL;
    	//insert in the list	
		cpu[i] = insert_node(cpu[i], new_node);
		free_all_nodes = insert_node_for_free(free_all_nodes, new_node); 
	}
}


//free the list of CPU
void free_CPU( list_t** cpu, int processor_num){
	for(int i = 0; i < processor_num; i++){
		free(cpu[i]);
	}
	free(cpu);
}