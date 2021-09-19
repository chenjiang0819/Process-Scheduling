#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "scheduling_mechanism.h"

#define INITIAL 0
#define OFF 0
#define IMPOSSIBLE_NUM -1
#define INCLUDE_ALL 3
#define PARAMETER 2
#define FINISH 1
/****************************************************************/

//function for forking the processes
void forking (int* forked_num, node_t** forked, int processor_num, node_t* assign, int my_algorithm){
	int count = INITIAL;
	float fork_id;

	// splitting the process
	if(my_algorithm == OFF){
		if(assign -> execution_time >= processor_num){
			//splite to number of processor, if execution time >  number of processor
			while(count < processor_num){
				//tackle the sub-process id
				fork_id = count;
				while(fork_id >= 1){
					fork_id = fork_id * 0.1;
				}
				node_t* new_node = (node_t*)malloc(sizeof(*new_node));
	    		assert(new_node != NULL);
				new_node -> time_arrived = assign -> time_arrived;
		        new_node -> process_id =  assign -> process_id + fork_id;	    
				new_node -> execution_time = ceill(assign -> execution_time / processor_num) + 1;		
		    	new_node -> execution_counter = new_node -> execution_time;
		    	new_node -> process_turnaround = INITIAL;
				new_node -> process_overhead = INITIAL;
		    	new_node -> parallelisable = 'f';
		    	new_node -> status = INITIAL;
		    	new_node -> next = NULL;
		    	new_node -> parent = assign;
		    	//record sub-processes
				forked[count++] = new_node;
			}
		}else{
			while(count < assign -> execution_time){
				//splite to execution time, if execution time <  number of processor
				fork_id = count;
				//tackle the sub-process id
				while(fork_id >= 1){
					fork_id = fork_id * 0.1;
				}
				node_t* new_node = (node_t*)malloc(sizeof(*new_node));
	    		assert(new_node != NULL);
				new_node -> time_arrived = assign -> time_arrived;
		        new_node -> process_id =  assign -> process_id + fork_id;
				new_node -> execution_time = ceill(assign -> execution_time / processor_num) + 1;
		    	new_node -> execution_counter = new_node -> execution_time;
		    	new_node -> process_turnaround = INITIAL;
				new_node -> process_overhead = INITIAL;
		    	new_node -> parallelisable = 'f';
		    	new_node -> status = INITIAL;
		    	new_node -> next = NULL;
		    	new_node -> parent = assign;
		    	//record sub-processes
				forked[count++] = new_node;
			}
		}
	}else{
		//since k is calculus problem, i chose k =2 (it works)
		while(count < PARAMETER){
			fork_id = count;
			//tackle the sub-process id
			while(fork_id >= 1){
				fork_id = fork_id * 0.1;
			}
			node_t* new_node = (node_t*)malloc(sizeof(*new_node));
    		assert(new_node != NULL);
			new_node -> time_arrived = assign -> time_arrived;
	        new_node -> process_id =  assign -> process_id + fork_id;	    
			new_node -> execution_time = ceill(assign -> execution_time / PARAMETER) + 1;		
	    	new_node -> execution_counter = new_node -> execution_time;
	    	new_node -> process_turnaround = INITIAL;
			new_node -> process_overhead = INITIAL;
	    	new_node -> parallelisable = 'f';
	    	new_node -> status = INITIAL;
	    	new_node -> next = NULL;
	    	new_node -> free_next = NULL;
	    	new_node -> parent = assign;
	    	//record sub-processes
			forked[count++] = new_node;
		}
	}
	*forked_num = count;
	assign -> status = assign -> status + FINISH - count;
}


//traverse all CPUs to record their status and find the minium workload cpu 
int find_mini_cpu(list_t** cpu, int processor_num, int max_time, int* cpu_n_remain){
	int remain;
	int mini_remain;
	int mini_task_cpu_id = IMPOSSIBLE_NUM;
	node_t* check;
	mini_remain = max_time;

	for(int i = 0; i < processor_num; i++){
		remain = INITIAL;
		check = cpu[i] -> head;
		//traverse all CPUs
		while(check!= NULL){
			remain = remain + check -> execution_counter;		
			check = check -> next;
		}
		if(remain < mini_remain){
			mini_remain = remain;
			mini_task_cpu_id = i;		
		}else if(remain == mini_remain && i < mini_task_cpu_id){
			mini_task_cpu_id = i;			
		}
		cpu_n_remain[i] = remain;
	}
	//return the minimum workload CPU 	
	return mini_task_cpu_id;
}


//comparing function for quick sort - small to big
int cmpfunc (const void * input1, const void * input2){
   return ( (*(node_t**)input1) -> execution_time - (*(node_t**)input2) -> execution_time);
}

//comparing function for quick sort- big to small
int cmpfunc_task (const void * input1, const void * input2){
   return ( (*(node_t**)input2) -> execution_time - (*(node_t**)input1) -> execution_time);
}


//the function for assigning the processes
void assign_prcesses(list_t** cpu, node_t** input_doc, int processor_num, int total_processes, 
	int max_time, int total_time, int* cpu_que, int my_algorithm, list_t* free_all_nodes, node_t** waiting_area){
	int forked_num = INITIAL;
	int mini_task_cpu = IMPOSSIBLE_NUM; 
	int static counter = INITIAL;
	int cpu_n_remain [processor_num]; 
	//to store the sub-processes
	node_t** forked = (node_t**) malloc (processor_num * sizeof(*forked));
	assert(forked != NULL);
	//quick sort the tuples to implement smallest first or my own algorithm 
	if(my_algorithm == 0){
		qsort(input_doc, total_processes, sizeof(node_t*), cmpfunc);
	}else{
		qsort(input_doc, total_processes, sizeof(node_t*), cmpfunc_task);
	}

	//push the processes to selected CPU
	for(int i = 0; i < total_processes; i++){
		//check if the processes have arrived	
		if(input_doc[i] -> time_arrived == total_time){
			//assign processes acording to thier type(parallelisable or not)
			if(input_doc[i] -> parallelisable == 'n' || processor_num == 1){
				if(my_algorithm == OFF){					
					mini_task_cpu = find_mini_cpu(cpu, processor_num, max_time, cpu_n_remain);
					cpu[mini_task_cpu] = insert_node(cpu[mini_task_cpu], input_doc[i]);
					(*cpu_que) ++;
				}else{
					//put the tuples in the waiting area first
					waiting_area[counter ++] = input_doc[i];
				}
			}else if(input_doc[i] -> parallelisable == 'p'){
				if(my_algorithm == OFF){
					forking (&forked_num, forked, processor_num, input_doc[i], my_algorithm);	
					for(int i = 0; i < forked_num; i++){			
						mini_task_cpu = find_mini_cpu(cpu, processor_num, max_time, cpu_n_remain);
						cpu[mini_task_cpu] = insert_node(cpu[mini_task_cpu], forked[i]); 
						free_all_nodes = insert_node_for_free(free_all_nodes, forked[i]);
					}
				}else{
					if(input_doc[i] -> execution_time <= 2 || processor_num <= 2){
						input_doc[i] -> parallelisable == 'n';
						//put the tuples in the waiting area first
						waiting_area[counter ++] = input_doc[i];
						
					}else{
						forking (&forked_num, forked, processor_num, input_doc[i], my_algorithm);	
						for(int i = 0; i < forked_num; i++){
							//put the tuples in the waiting area first			
							waiting_area[counter ++] = forked[i];
							free_all_nodes = insert_node_for_free(free_all_nodes, waiting_area[0]);
		   				}
					}
				}
				(*cpu_que) ++;
			}
		}
    }

 	if(my_algorithm != OFF){
 		//insert the tuples in the selected CPU using my algorithm
	    find_mini_cpu(cpu, processor_num, max_time, cpu_n_remain);    
	    for(int i = 0; i < processor_num; i++){
			if(cpu_n_remain[i] <= 1 && waiting_area[INITIAL] != NULL){
				if(counter > INITIAL){				
					qsort(waiting_area, counter, sizeof(node_t*), cmpfunc_task);
				}
				cpu[i] = insert_node(cpu[i], waiting_area[INITIAL]);	
				//delete the used tuples by swapping it with the NULL pointers		
				if(counter > INITIAL){ 
					waiting_area[INITIAL] = waiting_area[--counter];
				}
				waiting_area[counter] = NULL;
				(*cpu_que) ++;
			}
		}
	}	
	free(forked);
}


//the function do scheduling 
void scheduling (list_t** cpu ,  node_t** input_doc, int processor_num, int total_processes, 
	int max_time, int* total_time, int my_algorithm, list_t* free_all_nodes){
	int minimum;
	int cpu_que = INITIAL;
	float check_change[processor_num];
	node_t* shortest[processor_num];
	node_t* finish_process[processor_num];
	node_t* waiting_area [(total_processes * INCLUDE_ALL)];
	node_t *check; 
	int keep_total = total_processes;

	//initialise all varibles 
	for( int i = 0; i< processor_num; i++){
		check_change[i] = IMPOSSIBLE_NUM;
		shortest[i] = NULL;
	}
	for(int i = 0; i < total_processes * INCLUDE_ALL ; i++){
		waiting_area[i] = NULL;
	}

	//scheduler works one time unit at a time
	while((*total_time) <= max_time){
		//initialise finished list
		for( int i = 0; i< processor_num; i++){
			finish_process[i] = NULL;
		}
		//assigning the processes
		assign_prcesses(cpu ,input_doc, processor_num, keep_total, max_time, *total_time, &cpu_que, my_algorithm, free_all_nodes, waiting_area);
		for(int i = 0; i < processor_num; i++){
			check = cpu[i] -> head;
			minimum = max_time;
			//check if many process has finished 
			while(check != NULL){
				if(check -> execution_counter == 0 && check -> status != FINISH){
					check -> status = FINISH;
					if(check -> parallelisable == 'p' || check -> parallelisable == 'n'){
						total_processes --;
						check -> process_turnaround = (*total_time) - check -> time_arrived;
				    	check -> process_overhead = (float)(check -> process_turnaround) / check -> execution_time;	
				    	finish_process[i] = check;
				    	cpu_que --;	    		
					}else if (check -> parallelisable == 'f'){
						check -> parent -> status ++;
						if(check -> parent -> status == FINISH){			
							total_processes --;
							check -> parent -> process_turnaround = (*total_time) - check  -> parent -> time_arrived;
				    		check -> parent -> process_overhead = (float)(check  -> parent -> process_turnaround) / check -> parent -> execution_time;
				    		finish_process[i] = check -> parent;
				    		cpu_que --;	
						}
					}
				}
				//find the shortest process to run 
				if(minimum > check -> execution_counter && check -> status != FINISH){
					minimum = check -> execution_counter;
					shortest[i] = check;
				}else if(minimum == check -> execution_counter && shortest[i] -> process_id > check -> process_id){
					shortest[i] = check;
				}		
				check = check -> next;
			}
		}

		//print the results
		for(int i = 0; i < processor_num; i++){
			if(finish_process[i] != NULL){
				printf("%d,FINISHED,pid=%g,proc_remaining=%d\n",*total_time, finish_process[i] -> process_id, cpu_que);	
			}
		}
		//if all processes have finished, stop scheduling
		if(total_processes == INITIAL){	   			
    		break;
		}
		for(int i = 0; i < processor_num; i++){
			if(shortest[i] != NULL){
				if(check_change[i] != shortest[i] -> process_id){
					if(shortest[i] -> parallelisable == 'f'){
						printf("%d,RUNNING,pid=%.1f,remaining_time=%d,cpu=%d\n", *total_time, shortest[i] -> process_id, shortest[i] -> execution_counter, i);
					}else{
						printf("%d,RUNNING,pid=%.f,remaining_time=%d,cpu=%d\n", *total_time, shortest[i] -> process_id, shortest[i] -> execution_counter, i);
					}
					check_change[i] = shortest[i] -> process_id;
				}
				//execution the arrangement 
				if(shortest[i] -> status != FINISH && shortest[i] -> execution_counter != 0){
					(shortest[i] -> execution_counter) --;
				}
			}
		}
		(*total_time) ++;
	} 
}