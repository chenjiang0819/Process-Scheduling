#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "data_structure.h"

/****************************************************************/

//function for making a new empty linked list 
list_t* make_empty_list(){
    list_t *list;
	list = (list_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = list->foot = NULL;
	return list;
};

//function for inserting the new nodes at the end of the list, and move the foot point it. 
list_t* insert_node(list_t* list, node_t* new_node){
	assert(new_node != NULL && list != NULL);
	if (list -> foot == NULL){
		list -> head = list -> foot = new_node;
	} else {
		list -> foot -> next = new_node;
		list -> foot = new_node;
	}
    return list;
};


//function for inserting the new nodes at the end of the list, and move the foot point it. 
list_t* insert_node_for_free(list_t* list, node_t* new_node){
	assert(new_node != NULL && list != NULL);
	if (list -> foot == NULL){
		list -> head = list -> foot = new_node;
	} else {
		list -> foot -> free_next = new_node;
		list -> foot = new_node;
	}
	new_node -> free_next = NULL;
    return list;
};


//function to free all malloced nodes
void free_all(list_t* list){
	assert(list != NULL);
	node_t *curr, *prev;
	curr = list -> head;
    while(curr != NULL){         	
        prev = curr;
        curr = curr -> free_next;
        // free the momeries that malloced for the nodes and point it to NULL to prevent the wild pointers
    	free(prev);
    	prev = NULL;
    }
    //free list point it to NULL to prevent the wild pointers
	free(list);
	list = NULL;
};
