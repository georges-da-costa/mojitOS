#include "util.c"
#include "amd_rapl.c"
#include "info_reader.c"

TMAIN({
    CALL_TFUNCTION(test_util);
    CALL_TFUNCTION(test_amd_rapl);
    CALL_TFUNCTION(test_info_reader);
})
