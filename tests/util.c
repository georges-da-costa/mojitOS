#include "../src/util.h"
#include "small_test.h"


TFUNCTION(test_modulo_substraction, {
    uint64_t lhs = 0;
    uint64_t rhs = 0;
    uint64_t result = 0;
    uint64_t expected = 0;

    // Test 1:
    // -- Setup
    lhs = 10;
    rhs = 10;
    expected = 0;
    // -- Run
    result = modulo_substraction(lhs, rhs);
    // -- Verification
    TEST_UINT64_T(&result, &expected);

    // Test 2:
    // -- Setup
    lhs = UINT64_MAX;
    rhs = 0;
    expected = UINT64_MAX;
    // -- Run
    result = modulo_substraction(lhs, rhs);
    // -- Verification
    TEST_UINT64_T(&result, &expected);

    // Test 3:
    // -- Setup
    lhs = 0;
    rhs = UINT64_MAX;
    expected = 1;
    // -- Run
    result = modulo_substraction(lhs, rhs);
    // -- Verification
    TEST_UINT64_T(&result, &expected);

    // Test 4:
    // -- Setup
    lhs = 10;
    rhs = 20;
    expected = UINT64_MAX - 9;
    // -- Run
    result = modulo_substraction(lhs, rhs);
    // -- Verification
    TEST_UINT64_T(&result, &expected);

    // Test 5:
    // -- Setup
    lhs = 1000;
    rhs = 1000;
    expected = 0;
    result = modulo_substraction(lhs, rhs);
    TEST_UINT64_T(&result, &expected);
})

TFILE_ENTRY_POINT(test_util, {
    CALL_TFUNCTION(test_modulo_substraction);
})
