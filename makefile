all: mojitos

OBJECTS = mojitos.o counters_individual.o counters.h rapl.o rapl.h network.h network.o  load.o infiniband.o

mojitos:$(OBJECTS) counters_option.h
	gcc $(DEBUG) -O3 -Wall -o mojitos $(OBJECTS) -lpowercap

OBJECTS_GRP = $(subst _individual,_group, $(OBJECTS))
mojitos_group: $(OBJECTS_GRP) counters_option.h
	gcc $(DEBUG) -O3 -Wall -o mojitos_group $(OBJECTS_GRP) -lpowercap

counters_%.o: counters_%.c counters.h
	gcc $(DEBUG) -c -O3 -Wall $< -o $@

counters_option.h: counters_option.py
	./counters_option.py > counters_option.h


mojitos.o: mojitos.c counters_option.h
	gcc $(DEBUG) -c -O3 -Wall $< -o $@

debug: DEBUG = -DDEBUG -g

debug: all

%.o : %.c %.h
	gcc $(DEBUG) -c -O3 -Wall $< -o $@

clean:
	\rm -f *~ *.o mojitos_group mojitos counters_option.h
