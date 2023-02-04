#include "small_test.h"
#include "../src/amd_rapl.c"

TFUNCTION(test_raw_to_microjoule, {
    uint64_t raw = 0;
    uint64_t unit = 0;
    uint64_t result = 0;
    uint64_t expected = 0;

    // Test 1:
    // -- Setup
    raw = 100;
    unit = 1000;
    expected = 390625;
    // -- Run
    result = raw_to_microjoule(raw, unit);
    // -- Verification
    TEST_UINT64_T(&result, &expected);

    // TEST 2:
    // -- Setup
    raw = 200;
    unit = 1;
    expected = 100000000;
    // -- Run
    result = raw_to_microjoule(raw, unit);
    // -- Verification
    TEST_UINT64_T(&result, &expected);

    // TEST 3:
    // -- Setup
    raw = 500;
    unit = 2;
    expected = 125000000;
    // -- Run
    result = raw_to_microjoule(raw, unit);
    // -- Verification
    TEST_UINT64_T(&result, &expected);

    // TEST 4:
    // -- Setup
    raw = 1000;
    unit = 3;
    expected = 125000000;
    // -- Run
    result = raw_to_microjoule(raw, unit);
    // -- Verification
    TEST_UINT64_T(&result, &expected);

    // TEST 5:
    // -- Setup
    raw = 10000;
    unit = 4;
    expected = 625000000;
    // -- Run
    result = raw_to_microjoule(raw, unit);
    // -- Verification
    TEST_UINT64_T(&result, &expected);
})

TFUNCTION(test_get_name, {
    size_t cpu_id = 0;
    char *result = NULL;
    char expected[100];

    // TEST 1:
    // -- Setup
    cpu_id = 0;
    strcpy(expected, "core0");
    // -- Run
    result = get_name(cpu_id);
    // -- Verification
    TEST_STR(result, expected);
    free(result);

    // TEST 2:
    // -- Setup
    cpu_id = 10000;
    strcpy(expected, "core10000");
    // -- Run
    result = get_name(cpu_id);
    // -- Verification
    TEST_STR(result, expected);
    free(result);
})

#define NONE 0
#define DUMMY_SENSOR(__sensor, __cpu_id, __name) \
do {                                             \
  __sensor = (cpu_sensor_t) {                    \
    .cpu_id = __cpu_id,                          \
    .package_id = NONE,                          \
    .core_id = NONE,                             \
    .name = __name,                              \
    .fd = NONE,                                  \
    .energy_units = NONE,                        \
    .core_energy = NONE,                         \
  };                                             \
	} while(0);

#define DUMMY_RAPL(__rapl, __sensors, __sensors_count) \
	do {                                                 \
		__rapl = (_amd_rapl_t) {                           \
			.sensors = __sensors,                            \
			.sensor_count = __sensors_count                 \
		};                                                 \
	} while(0);

TFUNCTION(test_label_amd_rapl, {
    cpu_sensor_t sensors[100];
    _amd_rapl_t rapl;
    char *results[100];
    char expecteds[10][100];
    uint64_t nb = 0;

    // Test 1:
    // -- Setup
    nb = 1;
    DUMMY_SENSOR(sensors[0], 0, "core0");
    DUMMY_RAPL(rapl, sensors, nb);
    strcpy(expecteds[0], "core0");
    // -- Run
    label_amd_rapl(results, (void *) &rapl);
    // -- Verification
    TEST_T_ARRAY(TEST_STR, nb, results, expecteds);

    // Test 2:
    // -- Setup
    nb = 4;
    DUMMY_SENSOR(sensors[0], 0, "core0");
    DUMMY_SENSOR(sensors[1], 1, "core1");
    DUMMY_SENSOR(sensors[2], 2, "core2");
    DUMMY_SENSOR(sensors[3], 3, "core3");
    DUMMY_RAPL(rapl, sensors, nb);
    strcpy(expecteds[0], "core0");
    strcpy(expecteds[1], "core1");
    strcpy(expecteds[2], "core2");
    strcpy(expecteds[3], "core3");
    // -- Run
    label_amd_rapl(results, (void *) &rapl);
    // -- Verification
    TEST_T_ARRAY(TEST_STR, nb, results, expecteds);

    // Test 3:
    // -- Setup
    nb = 4;
    DUMMY_SENSOR(sensors[0], 0, "core0");
    DUMMY_SENSOR(sensors[1], 3, "core3");
    DUMMY_SENSOR(sensors[2], 1, "core1");
    DUMMY_SENSOR(sensors[3], 2, "core2");
    DUMMY_RAPL(rapl, sensors, nb);
    strcpy(expecteds[0], "core0");
    strcpy(expecteds[1], "core3");
    strcpy(expecteds[2], "core1");
    strcpy(expecteds[3], "core2");
    // -- Run
    label_amd_rapl(results, (void *) &rapl);
    // -- Verification
    TEST_T_ARRAY(TEST_STR, nb, results, expecteds);
})

TFUNCTION(test_init_cpu_sensor, {
    static const unsigned int max_cpus = 10;
    unsigned char cpus_map[max_cpus * max_cpus];
    cpu_sensor_t sensor_t1;
    unsigned int result;
    unsigned int expected;

    // Test 1:
    // -- Setup
    memset(cpus_map, 0, max_cpus *max_cpus * sizeof(unsigned char));
    result = 0;
    expected = 1;
    memset(&sensor_t1, 0, sizeof(cpu_sensor_t));
    sensor_t1.cpu_id = 1;
    sensor_t1.core_id = 0;
    sensor_t1.package_id = 0;
    // -- Run
    result = init_cpu_sensor(&sensor_t1, 0, cpus_map, max_cpus);
    // -- Verification
    TEST_UINT64_T(&result, &expected);
})

TFILE_ENTRY_POINT(test_amd_rapl, {
    CALL_TFUNCTION(test_raw_to_microjoule);
    CALL_TFUNCTION(test_get_name);
    CALL_TFUNCTION(test_label_amd_rapl);
    // CALL_TFUNCTION(test_init_cpu_sensor);
})

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

