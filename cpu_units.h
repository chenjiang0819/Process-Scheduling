#include <stdio.h>
#include "data_structure.h"

#ifndef CPU_UNITS_H
#define CPU_UNITS_H

/****************************************************************/

//initialise the CPU
void initial_CPU(int processor_num, list_t** cpu, list_t* free_all_nodes);

//free the list of CPU
void free_CPU( list_t** cpu, int processor_num);

#endif 