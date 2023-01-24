#include "amd_rapl.c"
#include "info_reader.c"

int main()
{
    int nb_error = 0;

    nb_error += test_amd_rapl();
    nb_error += test_info_reader();

    return nb_error;
}

