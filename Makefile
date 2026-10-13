CC ?= cc
CFLAGS ?= -std=gnu99 -O2 -Wall -Wextra -Wpedantic
SOURCES = allocate.c cpu_units.c data_structure.c scheduling_mechanism.c
HEADERS = cpu_units.h data_structure.h scheduling_mechanism.h

allocate: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -lm -o $@

test: allocate
	mkdir -p .build
	python3 -B -m unittest discover -s tests -v

sanitize:
	mkdir -p .build
	$(CC) -std=gnu99 -O1 -g -Wall -Wextra -Wpedantic -fsanitize=address,undefined -fno-omit-frame-pointer $(SOURCES) -lm -o .build/allocate-sanitize
	PROGRAM=$(CURDIR)/.build/allocate-sanitize python3 -B -m unittest discover -s tests -v

clean:
	rm -f allocate *.o *.gch .build/allocate-sanitize

.PHONY: test sanitize clean
