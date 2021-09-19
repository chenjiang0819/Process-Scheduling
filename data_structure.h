#include <stdio.h>

#ifndef DATA_STRUCTURE
#define DATA_STRUCTURE

/****************************************************************/

typedef struct node node_t;

//nodes for recording the tuples
struct node{
	int time_arrived;
	float process_id;
	float execution_time;
	int execution_counter;
	int process_turnaround;
	float process_overhead;
    char parallelisable;
    int status;
    node_t* free_next;
    node_t* next;
    node_t* parent;
};

//struct of list
typedef struct{
   node_t* head;
   node_t* foot;
} list_t;

/****************************************************************/

//function for making a new empty linked list 
list_t* make_empty_list();

//list in the CPU
list_t* insert_node(list_t* list, node_t* new_node);

//list for free all malloced nodes
list_t* insert_node_for_free(list_t* list, node_t* new_node);

//function to free all malloced nodes
void free_all(list_t* list);

#endif 