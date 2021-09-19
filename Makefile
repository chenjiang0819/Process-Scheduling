allocate: allocate.o cpu_units.o scheduling_mechanism.o data_structure.o
	gcc -o allocate allocate.o cpu_units.o scheduling_mechanism.o data_structure.o -lm -g

allocate.o: allocate.c cpu_units.h scheduling_mechanism.h data_structure.h
	gcc -c  allocate.c cpu_units.h scheduling_mechanism.h data_structure.h -lm -g

cpu_units.o: cpu_units.c cpu_units.h data_structure.h
	gcc -c  cpu_units.c cpu_units.h data_structure.h -lm -g

scheduling_mechanism.o: scheduling_mechanism.c scheduling_mechanism.h
	gcc -c  scheduling_mechanism.c scheduling_mechanism.h -lm -g

data_structure.o: data_structure.c data_structure.h
	gcc -c  data_structure.c data_structure.h -lm -g






