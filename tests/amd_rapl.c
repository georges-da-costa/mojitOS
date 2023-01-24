#include "small_test.h"
#include "../src/amd_rapl.c"

int test_raw_to_microjoule()
{
    INIT_TEST_FUNCTION();
    int nb_error = 0;

    uint64_t raw = 0;
    uint64_t unit = 0;
    uint64_t result = 0;
    uint64_t expected = 0;

    // Test 1:
    // -- Setup
    raw = 100;
    unit = 0;
    expected = 100000000;
    // -- Run
    result = raw_to_microjoule(raw, unit);
    // -- Verification
    nb_error += TEST_UINT64_T(&result, &expected);

    // nb_error += TEST 2:
    // -- Setup
    raw = 200;
    unit = 1;
    expected = 100000000;
    // -- Run
    result = raw_to_microjoule(raw, unit);
    // -- Verification
    nb_error += TEST_UINT64_T(&result, &expected);

    // nb_error += TEST 3:
    // -- Setup
    raw = 500;
    unit = 2;
    expected = 125000000;
    // -- Run
    result = raw_to_microjoule(raw, unit);
    // -- Verification
    nb_error += TEST_UINT64_T(&result, &expected);

    // nb_error += TEST 4:
    // -- Setup
    raw = 1000;
    unit = 3;
    expected = 125000000;
    // -- Run
    result = raw_to_microjoule(raw, unit);
    // -- Verification
    nb_error += TEST_UINT64_T(&result, &expected);

    // nb_error += TEST 5:
    // -- Setup
    raw = 10000;
    unit = 4;
    expected = 625000000;
    // -- Run
    result = raw_to_microjoule(raw, unit);
    // -- Verification
    nb_error += TEST_UINT64_T(&result, &expected);

    return nb_error;
}

int test_get_name()
{
    INIT_TEST_FUNCTION();
    int nb_error = 0;

    size_t cpu_id = 0;
    char *result = NULL;
    char expected[100];

    // nb_error += TEST 1:
    // -- Setup
    cpu_id = 0;
    strcpy(expected, "core0");
    // -- Run
    result = get_name(cpu_id);
    // -- Verification
    nb_error += TEST_STR(result, expected);
    free(result);

    // nb_error += TEST 2:
    // -- Setup
    cpu_id = 10000;
    strcpy(expected, "core10000");
    // -- Run
    result = get_name(cpu_id);
    // -- Verification
    nb_error += TEST_STR(result, expected);
    free(result);

    return nb_error;
}

#define NONE 0
#define DUMB_SENSOR(sensor, cpu_id, name)	\
	do {									\
		sensor = (cpu_sensor_t) {			\
			cpu_id,							\
			NONE,							\
			name,							\
			NONE,							\
			NONE,							\
			NONE, 							\
			NONE							\
		}; 									\
	} while(0);

#define DUMB_RAPL(rapl, sensors, nb)		\
	do {									\
		rapl = (_amd_rapl_t) {				\
			sensors,						\
			nb								\
		};									\
	} while(0);

int test_label_amd_rapl()
{
    INIT_TEST_FUNCTION();
    int nb_error = 0;
    cpu_sensor_t sensors[100];
    _amd_rapl_t rapl;
    char *results[100];
    char expecteds[10][100];
    uint64_t nb = 0;

    // Test 1:
    // -- Setup
    nb = 1;
    DUMB_SENSOR(sensors[0], 0, "core0");
    DUMB_RAPL(rapl, sensors, nb);
    strcpy(expecteds[0], "core0");
    // -- Run
    label_amd_rapl(results, (void *) &rapl);
    // -- Verification
    TEST_T_ARRAY(TEST_STR, nb_error, nb, results, expecteds);

    // Test 2:
    // -- Setup
    nb = 4;
    DUMB_SENSOR(sensors[0], 0, "core0");
    DUMB_SENSOR(sensors[1], 1, "core1");
    DUMB_SENSOR(sensors[2], 2, "core2");
    DUMB_SENSOR(sensors[3], 3, "core3");
    DUMB_RAPL(rapl, sensors, nb);
    strcpy(expecteds[0], "core0");
    strcpy(expecteds[1], "core1");
    strcpy(expecteds[2], "core2");
    strcpy(expecteds[3], "core3");
    // -- Run
    label_amd_rapl(results, (void *) &rapl);
    // -- Verification
    TEST_T_ARRAY(TEST_STR, nb_error, nb, results, expecteds);

    // Test 3:
    // -- Setup
    nb = 4;
    DUMB_SENSOR(sensors[0], 0, "core0");
    DUMB_SENSOR(sensors[1], 3, "core3");
    DUMB_SENSOR(sensors[2], 1, "core1");
    DUMB_SENSOR(sensors[3], 2, "core2");
    DUMB_RAPL(rapl, sensors, nb);
    strcpy(expecteds[0], "core0");
    strcpy(expecteds[1], "core3");
    strcpy(expecteds[2], "core1");
    strcpy(expecteds[3], "core2");
    // -- Run
    label_amd_rapl(results, (void *) &rapl);
    // -- Verification
    TEST_T_ARRAY(TEST_STR, nb_error, nb, results, expecteds);

    return nb_error;
}

int test_amd_rapl()
{
    INIT_TEST_FILE();

    int nb_error = 0;
    nb_error += test_raw_to_microjoule();
    nb_error += test_get_name();
    nb_error += test_label_amd_rapl();
    return nb_error;
}

#ifdef __TESTING__AMD__
int main()
{
    test_amd_rapl();
    static const unsigned int time = 10;
    _amd_rapl_t *rapl = NULL;
    unsigned int nb_cpu = init_amd_rapl(NULL, (void **) &rapl);
    uint64_t results[nb_cpu];
    char *labels[nb_cpu];

    label_amd_rapl(labels, (void *) rapl);

    for (unsigned int i = 0; i < rapl->nb; ++i) {
        printf("%s ", labels[i]);
    }
    printf("\n");

    // -- Run

    for (unsigned int i = 0; i < time; ++i) {
        sleep(1);
        get_amd_rapl(results, (void *)rapl);

        for (unsigned int j = 0; j < rapl->nb; ++j) {
            printf("%ld ", results[j]);
        }
        printf("\n");
    }

    clean_amd_rapl(rapl);
    return 0;
}

#endif

