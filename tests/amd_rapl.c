/*******************************************************
 Copyright (C) 2023-2023 Georges Da Costa <georges.da-costa@irit.fr>

    This file is part of Mojitos.

    Mojitos is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Mojitos is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MojitO/S.  If not, see <https://www.gnu.org/licenses/>.

*******************************************************/

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
do {                                                   \
    __rapl = (_amd_rapl_t) {                           \
        .sensors = __sensors,                          \
        .sensor_count = __sensors_count                \
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
    for(unsigned int i = 0; i < nb; i++)
    {
        TEST_STR(results[i], expecteds[i]);
    }

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
    for(unsigned int i = 0; i < nb; i++)
    {
        TEST_STR(results[i], expecteds[i]);
    }
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
    for(unsigned int i = 0; i < nb; i++)
    {
        TEST_STR(results[i], expecteds[i]);
    }
})


#define DUMMY_CPUINFO(__sensor, __cpu_id, __package_id, __core_id) \
do {                                                               \
    __sensor = (cpu_sensor_t) {                                    \
        .cpu_id = __cpu_id,                                        \
        .package_id = __package_id,                                \
        .core_id = __core_id,                                      \
        .name = NULL,                                              \
        .fd = NONE,                                                \
        .energy_units = NONE,                                      \
        .core_energy = NONE                                        \
    };                                                             \
} while (0);

TFUNCTION(test_is_duplicate, {
    static const unsigned int nb_core = 4;
    static const unsigned int nb_package = 2;
    static const unsigned int max_cpu = 10;

    unsigned char map[nb_core][nb_package];
    cpu_sensor_t cpu_information[max_cpu];
    unsigned int results[max_cpu];
    unsigned int expecteds[max_cpu];

    // -- Setup
    memset(map, NONE, sizeof(map));
    memset(cpu_information,NONE, sizeof(cpu_sensor_t) * max_cpu);
    DUMMY_CPUINFO(cpu_information[0], 0, 1, 1);
    expecteds[0] = 1;
    expecteds[1] = 0;
    // -- Run
    results[0] = is_duplicate(&cpu_information[0], nb_core, nb_package,map);
    results[1] = is_duplicate(&cpu_information[0], nb_core, nb_package,map);
    // -- Verification
    TEST_BOOL(&results[0], &expecteds[0]);
    TEST_BOOL(&results[1], &expecteds[1]);

    // -- Setup
    memset(map, NONE, sizeof(map));
    memset(cpu_information,NONE, sizeof(cpu_sensor_t) * max_cpu);
    DUMMY_CPUINFO(cpu_information[0], 0, 1, 1);
    DUMMY_CPUINFO(cpu_information[1], 0, 1, 1);
    expecteds[0] = 1;
    expecteds[1] = 0;
    // -- Run
    results[0] = is_duplicate(&cpu_information[0], nb_core, nb_package, map);
    results[1] = is_duplicate(&cpu_information[1], nb_core, nb_package, map);
    // -- Verification
    TEST_BOOL(&results[0], &expecteds[0]);
    TEST_BOOL(&results[1], &expecteds[1]);

    // -- Setup
    memset(map, NONE, sizeof(map));
    memset(cpu_information,NONE, sizeof(cpu_sensor_t) * max_cpu);
    DUMMY_CPUINFO(cpu_information[0], 0, 1, 1);
    DUMMY_CPUINFO(cpu_information[1], 0, 0, 0);
    expecteds[0] = 1;
    expecteds[1] = 1;
    // -- Run
    results[0] = is_duplicate(&cpu_information[0], nb_core, nb_package, map);
    results[1] = is_duplicate(&cpu_information[1], nb_core, nb_package, map);
    // -- Verification
    TEST_BOOL(&results[0], &expecteds[0]);
    TEST_BOOL(&results[1], &expecteds[1]);

    // -- Setup
    memset(map, NONE, sizeof(map));
    memset(cpu_information,NONE, sizeof(cpu_sensor_t) * max_cpu);
    DUMMY_CPUINFO(cpu_information[0], 0, 1, 1);
    DUMMY_CPUINFO(cpu_information[1], 0, 1, 0);
    expecteds[0] = 1;
    expecteds[1] = 1;
    // -- Run
    results[0] = is_duplicate(&cpu_information[0], nb_core, nb_package, map);
    results[1] = is_duplicate(&cpu_information[1], nb_core, nb_package, map);
    // -- Verification
    TEST_BOOL(&results[0], &expecteds[0]);
    TEST_BOOL(&results[1], &expecteds[1]);

    // -- Setup
    memset(map, NONE, sizeof(map));
    memset(cpu_information,NONE, sizeof(cpu_sensor_t) * max_cpu);
    DUMMY_CPUINFO(cpu_information[0], 0, 1, 1);
    DUMMY_CPUINFO(cpu_information[1], 0, 0, 1);
    expecteds[0] = 1;
    expecteds[1] = 1;
    // -- Run
    results[0] = is_duplicate(&cpu_information[0], nb_core, nb_package, map);
    results[1] = is_duplicate(&cpu_information[1], nb_core, nb_package, map);
    // -- Verification
    TEST_BOOL(&results[0], &expecteds[0]);
    TEST_BOOL(&results[1], &expecteds[1]);

    // -- Setup
    memset(map, NONE, sizeof(map));
    memset(cpu_information,NONE, sizeof(cpu_sensor_t) * max_cpu);
    DUMMY_CPUINFO(cpu_information[0], 0, 0, 0);
    DUMMY_CPUINFO(cpu_information[1], 0, 0, 1);
    DUMMY_CPUINFO(cpu_information[2], 0, 1, 0);
    DUMMY_CPUINFO(cpu_information[3], 0, 1, 1);
    DUMMY_CPUINFO(cpu_information[4], 0, 0, 0);
    DUMMY_CPUINFO(cpu_information[5], 0, 0, 1);
    DUMMY_CPUINFO(cpu_information[6], 0, 1, 0);
    DUMMY_CPUINFO(cpu_information[7], 0, 1, 1);
    memset(expecteds, 1, sizeof(unsigned int) * 4);
    memset(&expecteds[4], 0, sizeof(unsigned int) * 4);
    // -- Run
    for (unsigned int i = 0; i < 8; i++)
    {
        results[i] = is_duplicate(&cpu_information[i], nb_core, nb_package, map );
    }
    // -- Verification
    for(unsigned int i = 0; i < 8; i++)
    {
        TEST_BOOL(&results[i], &expecteds[i]);
    }
})


TFILE_ENTRY_POINT(test_amd_rapl, {
    CALL_TFUNCTION(test_raw_to_microjoule);
    CALL_TFUNCTION(test_get_name);
    CALL_TFUNCTION(test_label_amd_rapl);
    CALL_TFUNCTION(test_is_duplicate);
})

#ifdef __TESTING__AMD__
int main()
{
    static const unsigned int time = 10;
    _amd_rapl_t *rapl = NULL;
    unsigned int count_cpu = init_amd_rapl(NULL, (void **) &rapl);
    uint64_t results[count_cpu];
    char *labels[count_cpu];

    label_amd_rapl(labels, (void *) rapl);

    for (unsigned int i = 0; i < rapl->sensor_count; ++i) {
        printf("%s ", labels[i]);
    }
    printf("\n");

    // -- Run

    for (unsigned int i = 0; i < time; ++i) {
        sleep(1);
        get_amd_rapl(results, (void *)rapl);

        for (unsigned int j = 0; j < rapl->sensor_count; ++j) {
            printf("%ld ", results[j]);
        }
        printf("\n");
    }

    clean_amd_rapl(rapl);
    return 0;
}

#endif

