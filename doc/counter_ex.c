#include "counter_ex.h"

static int acc;

unsigned int
init_acc(char *a, void **b)
{
	acc = 0;
}

unsigned int
get_acc(uint64_t *results, void *none)
{
	UNUSED(none);
	return a++;
}

void
label_acc(char **labels, void *none)
{
	UNUSED(none);
	labels[0] = "acc";
}
