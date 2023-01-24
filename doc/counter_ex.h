/*
 * Example of a basic counter: an accumulator
**/

unsigned int init_acc(char *, void **);
unsigned int get_acc(uint64_t *results, void *);
void clean_acc(void*);
void label_acc(char **labels, void *);

struct optparse_long counters_opt = {"accumulator", 'a', OPTPARSE_NONE};
struct captor counters = {
    .usage_arg = NULL,
    .usage_msg = "dumb accumulator\n"
    .init = init_acc,
    .get = get_acc,
    .clean = clean_acc,
    .label = label_acc,
};
