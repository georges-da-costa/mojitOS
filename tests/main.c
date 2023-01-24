#include "amd_rapl.c"
#include "util.c"
#include "info_reader.c"

int main()
{
    int nb_error = 0;

    nb_error += test_amd_rapl();
    nb_error += test_info_reader();
    nb_error += test_util();

    DEFERRED_ERROR(nb_error);
    return nb_error;
}

