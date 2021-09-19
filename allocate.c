/* Porcess Scheduling:
 *
 * This code is written by Chen Jiang, student ID: 1127411, 
 * for Assignment 1 of COMP30023 Computer System,
 * in 19 September 2021 at the University of MElbourne
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "data_structure.h"
#include "cpu_units.h"
#include "scheduling_mechanism.h"

#define INITIAL 0
#define EQUAL 0

/****************************************************************/

void processor_scheduler(node_t** input_doc, int processor_num, int total_processes, int max_time, int my_algorithm, list_t* free_all_nodes);
void profomance_statistics(node_t** input_doc, float* turnaround_time, float* overhead_time_max, float* overhead_average, int total_processes);
node_t** read_text(char* input, int* max_time, int* total_processes, list_t* free_all_nodes);
void tackle_decimal(float* taget);

/****************************************************************/

// main function
int main(int argc, char *argv[]){

	//varibles for console input
	char* file_name = "-f";
	char* processor_input = "-p";
	char* algorithm = "-c";
	//varibles for text
	int max_time;
	int total_processes;
	int processor_num = INITIAL;
	node_t** input_doc;
	//switch for my algorithm 
	int my_algorithm = INITIAL;
	//list for free all nodes
	list_t* free_all_nodes = make_empty_list();

	// reading inputs
	if( argc < 5 ){
		    printf("The input is not correct !\n");
		    exit(1);
	}
	for(int i = 0; i < argc; i++){
		if(strcmp(argv[i],file_name) == EQUAL){
			input_doc = read_text(argv[i+1],  &max_time, &total_processes, free_all_nodes);
		}else if(strcmp(argv[i],processor_input) == EQUAL){
			processor_num = atoi(argv[i+1]);
		}else if(strcmp(argv[i],algorithm) == EQUAL){		
			my_algorithm = 1;
		}
	}

	//invoke schedular
	processor_scheduler(input_doc, processor_num, total_processes, max_time, my_algorithm, free_all_nodes);
	//free all malloc
    free_all(free_all_nodes);
    free(input_doc);
	return 0;
}

/****************************************************************/

//function for reading text
node_t** read_text(char* input, int* max_time, int* total_processes, list_t* free_all_nodes){
	int read;
	size_t len = INITIAL;
	char* buffer = NULL;
	int initial = 1;
	node_t** reading_input = (node_t**) malloc (initial * sizeof(*reading_input));
	assert(reading_input != NULL);

	//open file in reading model
	FILE* reading = fopen(input,"r");
    assert(reading != NULL);
    //reading
    *max_time = INITIAL;
    *total_processes = INITIAL;
    while(( read = getline(&buffer, &len, reading)) != EOF){
    	node_t* new_node = (node_t*)malloc(sizeof(*new_node));
    	assert(new_node != NULL);
    	char *temp = strtok(buffer, " ");
		new_node -> time_arrived = atoi(temp);
		temp = strtok(NULL, " ");
	    new_node -> process_id = atof(temp);
	    temp = strtok(NULL, " ");
	    new_node -> execution_time = atoi(temp);
	    new_node -> execution_counter = new_node -> execution_time;
	    new_node -> process_turnaround = INITIAL;
		new_node -> process_overhead = INITIAL;
	    temp = strtok(NULL, " ");
	    new_node -> parallelisable = temp[0];
	    new_node -> status = INITIAL;
	    new_node -> next = NULL;
	    new_node -> free_next = NULL;
	    new_node -> parent = NULL;
	    *max_time = *max_time + new_node -> execution_time;

        reading_input[(*total_processes)++] = new_node;
        free_all_nodes = insert_node_for_free(free_all_nodes, new_node); 
        if( (*total_processes) >= initial){
			initial = initial *2;
			reading_input = realloc(reading_input, initial * sizeof(*reading_input));
			assert(reading_input != NULL);
		}
    }
    // free buffer and close the text
    free(buffer);
    buffer = NULL;
    fclose(reading);
    return reading_input;
};


//function to implement the schedule in order
void processor_scheduler(node_t** input_doc, int processor_num, int total_processes, int max_time, int my_algorithm, list_t* free_all_nodes){
	int total_time = INITIAL;
	float turnaround_time = INITIAL;
	float overhead_time_max = INITIAL;
	float overhead_average = INITIAL;
 	
 	//initialise CPU
 	list_t** cpu = (list_t**) malloc (processor_num * sizeof(*cpu));
	assert(cpu != NULL);
    initial_CPU(processor_num, cpu, free_all_nodes);
    //the scheduling and statistic 
	scheduling (cpu, input_doc, processor_num, total_processes, max_time, &total_time, my_algorithm, free_all_nodes);
	profomance_statistics(input_doc, &turnaround_time, &overhead_time_max, &overhead_average, total_processes);
	printf("Turnaround time %.Lf\n", ceill(turnaround_time));
	printf("Time overhead %.3g %.3g\n",overhead_time_max , overhead_average);
	printf("Makespan %d\n", total_time);
	//free CPU
	free_CPU(cpu, processor_num);
}


//function to performance statistics
void profomance_statistics(node_t** input_doc, float* turnaround_time, float* overhead_time_max, float* overhead_average, int total_processes){
	int total_turnaround = INITIAL;
	float total_overhead = INITIAL;
	float max_overhead = INITIAL;
	//traverse all data for each individual tuple 
    for(int i = INITIAL; i < total_processes; i++){
        total_turnaround = total_turnaround + input_doc[i] -> process_turnaround;
        total_overhead = total_overhead + input_doc[i] -> process_overhead;     
        if(input_doc[i] -> process_overhead > max_overhead){
        	max_overhead = input_doc[i] -> process_overhead;
        }
    }
    //calculate the result
    *turnaround_time = (float)total_turnaround / total_processes;
    *overhead_average = total_overhead / total_processes;
    tackle_decimal(overhead_average);
    *overhead_time_max = max_overhead;
    tackle_decimal(overhead_time_max);
}


//tackle the decimal issue due to different systerm
void tackle_decimal(float* taget){
	float for_decimal = INITIAL;
	int no_decimal = INITIAL;
	no_decimal = (*taget) * 1000;
    if(no_decimal % 10 >= 5){
    	//int will remove the all decimal. We use this to remove the last digit 
    	no_decimal = (no_decimal + 10) / 10;
    	for_decimal = no_decimal * 0.01;
    	*taget = for_decimal;
    }else{
    	//int will remove the all decimal. We use this to remove the last digit 
    	no_decimal = no_decimal / 10;
    	for_decimal = no_decimal * 0.01;
    	*taget = for_decimal;
    }
}

/****************************************************************/





























