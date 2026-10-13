/* Porcess Scheduling:
 *
 * This code is written by Chen Jiang, original handwritten implementation,
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
#include <errno.h>
#include <limits.h>

#include "data_structure.h"
#include "cpu_units.h"
#include "scheduling_mechanism.h"

#define INITIAL 0
#define EQUAL 0

/****************************************************************/

void processor_scheduler(node_t** input_doc, int processor_num, int total_processes, int max_time, int my_algorithm, list_t* free_all_nodes);
void profomance_statistics(node_t** input_doc, double* turnaround_time, double* overhead_time_max, double* overhead_average, int total_processes);
node_t** read_text(char* input, int* max_time, int* total_processes, list_t* free_all_nodes);
void tackle_decimal(double* taget);

/****************************************************************/

static int parse_integer(const char *text, int *value) {
    char *end; errno=0;
    if (!text || !*text || !isdigit((unsigned char)*text)) return 0;
    long n=strtol(text,&end,10);
    if (errno || *end || n<0 || n>INT_MAX) return 0;
    *value=(int)n; return 1;
}

// main function
int main(int argc, char *argv[]){
    char *filename=NULL;
    int processor_num=0, my_algorithm=0, seen_custom=0;
    for (int i=1; i<argc; i++) {
        if (!strcmp(argv[i],"-f") && !filename && i+1<argc) filename=argv[++i];
        else if (!strcmp(argv[i],"-p") && !processor_num && i+1<argc) {
            if (!parse_integer(argv[++i],&processor_num) || processor_num<1 || processor_num>1024) goto usage;
        } else if (!strcmp(argv[i],"-c") && !seen_custom) { my_algorithm=1; seen_custom=1; }
        else goto usage;
    }
    if (!filename || !processor_num) goto usage;
    list_t *free_all_nodes=make_empty_list();
    int max_time=0, total_processes=0;
    node_t **input_doc=read_text(filename,&max_time,&total_processes,free_all_nodes);
    processor_scheduler(input_doc,processor_num,total_processes,max_time,my_algorithm,free_all_nodes);
    free_all(free_all_nodes); free(input_doc); return 0;
usage:
    fprintf(stderr,"Usage: allocate -f FILE -p PROCESSORS(1..1024) [-c]\n");
    return EXIT_FAILURE;
}

/****************************************************************/

//function for reading text
node_t** read_text(char *input, int *max_time, int *total_processes, list_t *free_all_nodes){
    size_t len=0, capacity=16;
    char *buffer=NULL;
    node_t **records=malloc(capacity*sizeof(*records));
    FILE *file=fopen(input,"r");
    long long work=0;
    int last_arrival=0;
    if (!file || !records) goto invalid;
    while (getline(&buffer,&len,file)!=-1) {
        if (strlen(buffer)>256) goto invalid;
        int arrival,id,duration;
        char *a=strtok(buffer," \t\r\n"), *b=strtok(NULL," \t\r\n"),
             *c=strtok(NULL," \t\r\n"), *kind=strtok(NULL," \t\r\n");
        if (!parse_integer(a,&arrival) || !parse_integer(b,&id) || !parse_integer(c,&duration) ||
            !duration || !kind || strlen(kind)!=1 || (kind[0]!='n' && kind[0]!='p') ||
            strtok(NULL," \t\r\n") || arrival<last_arrival || *total_processes==INT_MAX) goto invalid;
        for (int i=0;i<*total_processes;i++) if (records[i]->process_id==id) goto invalid;
        work+=duration;
        if (work+arrival+*total_processes+1>INT_MAX) goto invalid;
        if ((size_t)*total_processes==capacity) {
            capacity*=2; node_t **grown=realloc(records,capacity*sizeof(*records));
            if (!grown) goto invalid;
            records=grown;
        }
        node_t *node=calloc(1,sizeof(*node));
        if (!node) goto invalid;
        node->time_arrived=arrival; node->process_id=id; node->sub_id=-1;
        node->execution_time=duration; node->execution_counter=duration; node->parallelisable=kind[0];
        records[(*total_processes)++]=node; insert_node_for_free(free_all_nodes,node);
        last_arrival=arrival;
    }
    if (ferror(file) || !*total_processes) goto invalid;
    *max_time=(int)(work+last_arrival+*total_processes);
    free(buffer); fclose(file); return records;
invalid:
    fprintf(stderr,"Invalid input: expected unique, ordered process records with positive durations\n");
    if (file) fclose(file);
    free(buffer); free(records); free_all(free_all_nodes); exit(EXIT_FAILURE);
}


//function to implement the schedule in order
void processor_scheduler(node_t** input_doc, int processor_num, int total_processes, int max_time, int my_algorithm, list_t* free_all_nodes){
	int total_time = INITIAL;
	double turnaround_time = INITIAL;
	double overhead_time_max = INITIAL;
	double overhead_average = INITIAL;
 	
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
void profomance_statistics(node_t** input_doc, double* turnaround_time, double* overhead_time_max, double* overhead_average, int total_processes){
	long long total_turnaround = INITIAL;
	double total_overhead = INITIAL;
	double max_overhead = INITIAL;
	//traverse all data for each individual tuple 
    for(int i = INITIAL; i < total_processes; i++){
        total_turnaround = total_turnaround + input_doc[i] -> process_turnaround;
        total_overhead = total_overhead + input_doc[i] -> process_overhead;     
        if(input_doc[i] -> process_overhead > max_overhead){
        	max_overhead = input_doc[i] -> process_overhead;
        }
    }
    //calculate the result
    *turnaround_time = (double)total_turnaround / total_processes;
    *overhead_average = total_overhead / total_processes;
    tackle_decimal(overhead_average);
    *overhead_time_max = max_overhead;
    tackle_decimal(overhead_time_max);
}


//tackle the decimal issue due to different systerm
void tackle_decimal(double *target){
    *target=round(*target*100.0)/100.0;
}

/****************************************************************/





























