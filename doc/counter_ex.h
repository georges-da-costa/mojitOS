/*
 * Example of a basic counter: an accumulator
**/

unsigned int init_acc(char *, void **);
unsigned int get_acc(uint64_t *results, void *);
void clean_acc(void *);
void label_acc(char **labels, void *);

Captor rapl = {
    .init = init_acc,
    .get = get_acc,
    .clean = clean_acc,
    .label = label_acc,
    .nb_opt = 1,
};

Optparse rapl_opt[1] = {
    {
        .longname = "accumulator",
        .shortname = 'a',
        .argtype = OPTPARSE_NONE,		/* OPTPARSE_NONE / OPTPARSE_OPTIONAL / OPTPARSE_REQUIRED */
        .usage_arg = NULL,
        .usage_msg = "dumb accumulator",
    },
};
