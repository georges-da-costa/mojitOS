all: mojitos

mojitos_group:mojitos.c counters_group.o counters.h rapl.o rapl.h network.h network.o counters_option.h load.o
	gcc $(DEBUG) -O3 -Wall -o mojitos_group mojitos.c counters_group.o rapl.o network.o load.o -lpowercap

mojitos:mojitos.c counters_individual.o counters.h rapl.o rapl.h network.h network.o counters_option.h load.o
	gcc $(DEBUG) -O3 -Wall -o mojitos mojitos.c counters_individual.o rapl.o network.o load.o -lpowercap

counters_option.h: counters_option.py
	./counters_option.py > counters_option.h

debug: DEBUG = -DDEBUG

debug: all

load.o: load.c load.h
	gcc -O3 -Wall -c load.c

rapl.o: rapl.c rapl.h 
	gcc -O3 -Wall -c rapl.c

network.o: network.c network.h
	gcc -Wall -c network.c

counters_individual.o:counters_individual.c counters.h
	gcc -O3 -Wall -c counters_individual.c

counters_group.o:counters_group.c counters.h
	gcc -O3 -Wall -c counters_group.c

clean:
	\rm -f *~ *.o mojitos_group mojitos counters_option.h
