#include "../src/util.h"
#include "small_test.h"


TFUNCTION(test_modulo_substraction, {
    uint64_t previous = 0;
    uint64_t new = 0;
    uint64_t result = 0;
    uint64_t expected = 0;

    // Test 1:
    // -- Setup
    previous = 10;
    new = 10;
    expected = 0;
    // -- Run
    result = modulo_substraction(previous, new);
    // -- Verification
    TEST_UINT64_T(&result, &expected);

    // Test 2:
    // -- Setup
    previous = UINT64_MAX;
    new = 0;
    expected = 1;
    // -- Run
    result = modulo_substraction(previous, new);
    // -- Verification
    TEST_UINT64_T(&result, &expected);

    // Test 3:
    // -- Setup
    previous = 0;
    new = UINT64_MAX;
    expected = UINT64_MAX;
    // -- Run
    result = modulo_substraction(previous, new);
    // -- Verification
    TEST_UINT64_T(&result, &expected);

    // Test 4:
    // -- Setup
    previous = 20;
    new = 10;
    expected = UINT64_MAX - 9;
    // -- Run
    result = modulo_substraction(previous, new);
    // -- Verification
    TEST_UINT64_T(&result, &expected);
})

TFILE_ENTRY_POINT(test_util, {
    CALL_TFUNCTION(test_modulo_substraction);
})
