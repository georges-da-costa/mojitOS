#include "util.c"
#include "amd_rapl.c"
#include "info_reader.c"

TFILE_ENTRY_POINT(main, {
    CALL_TFUNCTION(test_util);
    CALL_TFUNCTION(test_amd_rapl);
    CALL_TFUNCTION(test_info_reader);
})
