/*******************************************************
 Copyright (C) 2022-2023 Georges Da Costa <georges.da-costa@irit.fr>

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

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "info_reader.h"
#include "util.h"
#include "amd_rapl.h"

#define BUFFER_SIZE 64

// ---------------------------MSR_REGISTERS
static const uint64_t amd_energy_mask = 0xFFFFFFFF;
static const uint64_t amd_energy_unit_mask = 0x01F00;
static const uint64_t msr_rapl_power_unit = 0xC0010299;
static const uint64_t energy_core_msr = 0xC001029A;
static const uint64_t energy_pkg_msr = 0xC001029B;

// ------------------------------FILE_PATHS
static const char *base_str = "/dev/cpu/%d/msr";

struct cpu_sensor_t {
    size_t cpu_id;
    size_t package_id;
    char *name;

    int fd;
    uint64_t energy_units;
    uint64_t core_energy;
    uint64_t pkg_energy;
};
typedef struct cpu_sensor_t cpu_sensor_t;

struct _amd_rapl_t {
    cpu_sensor_t *sensors;
    unsigned int nb;
};
typedef struct _amd_rapl_t _amd_rapl_t;

// -----------------------------INFO_READER

#ifdef __READ_CPUINFO__

#warning "Must be modified before release"
#define MAX_CPUS 64
#define NB_KEYS 2

static const char *cpuinfo = "/proc/cpuinfo";
static GenericPointer _size_t_allocator(char *s)
{
    size_t value = atoi(s);
    return (GenericPointer) value;
}

static void _set_cpu_id(GenericPointer storage, GenericPointer data)
{
    cpu_sensor_t *cpu = (cpu_sensor_t *) storage;
    cpu->cpu_id = (size_t) data;
}

static void _set_package_id(GenericPointer storage, GenericPointer data)
{
    cpu_sensor_t *cpu = (cpu_sensor_t *) storage;
    cpu->package_id = (size_t) data;
}

static KeyFinder keys[NB_KEYS] = {
    {"processor", ": ", (CopyAllocator *) _size_t_allocator, (Setter *) _set_cpu_id},
    {"physical id", ": ", (CopyAllocator *) _size_t_allocator, (Setter *) _set_package_id}
};
#endif

// --------------------------------READ_MSR

uint64_t read_msr(int fd, uint64_t msr)
{
    uint64_t data;
    if (pread(fd, &data, sizeof data, msr) != sizeof data) {
        fprintf(stderr, "msr(%ld):", msr);
        perror("pread");
        exit(127);
    }
    return data;
}

uint64_t read_unit(int fd)
{
    u_int64_t unit = read_msr(fd, msr_rapl_power_unit);
    return ((unit & amd_energy_unit_mask) >> 8);
}

uint64_t read_raw_core_energy(int fd)
{
    u_int64_t energy = read_msr(fd, energy_core_msr);
    return energy & amd_energy_mask;
}

uint64_t read_raw_pkg_energy(int fd)
{
    u_int64_t energy = read_msr(fd, energy_pkg_msr);
    return energy & amd_energy_mask;
}

// ---------------------------------ENERGY

uint64_t raw_to_microjoule(uint64_t raw, uint64_t unit)
{
    static const uint64_t to_microjoule = 1000000UL;
    // raw * (1 / (unit^2)) -> Joule
    // Joule * 1000000 -> uJoule
    uint64_t microjoule = (raw * to_microjoule) / (1UL << unit);
    return microjoule;
}
uint64_t raw_to_joule(uint64_t raw, uint64_t unit)
{
    // raw * (1 / (unit^2)) -> Joule
    int64_t joule = raw / (1UL << unit);
    return joule;
}

// -----------------------------------DEBUG

#ifdef DEBUG
void debug_print_sensor(cpu_sensor_t *sensor)
{
    //CASSERT(sizeof(cpu_sensor_t) == 56, amd_rapl_c);
    printf("cpu_id : %ld, package_id : %ld, name : %s, fd: %d,  energy_units : %ld, core_energy: %ld, pkg_energy: %ld\n",
           sensor->cpu_id,
           sensor->package_id,
           sensor->name,
           sensor->fd,
           sensor->energy_units,
           sensor->core_energy,
           sensor->pkg_energy
          );
}

void debug_print_amd_rapl(_amd_rapl_t *rapl)
{
    for (unsigned int i = 0; i < rapl->nb; i++) {
        debug_print_sensor(&rapl->sensors[i]);
    }
}

// typedef struct {
//     size_t cpu_id;
//     uint64_t *results;
//     size_t capacity;
// } CpuLogger;
//
// CpuLogger init_logger(size_t cpu_id, size_t capacity)
// {
//
// }
//
// void log_value();


#endif

// ---------------------------AMD_RAPL_UTIL

unsigned int get_nb_cpu()
{
    char filename[BUFFER_SIZE];

    unsigned int n_cpu = 0;
    for (;; n_cpu++) {
        sprintf(filename, base_str, n_cpu);

        int fd = open(filename, O_RDONLY);
        if (fd < 0) {
            break;
        }
        close(fd);
    }
    return n_cpu;
}

char *get_name(size_t cpu_id)
{
    static const char *base_name = "core%ld";
    static const size_t max_lenght = 20;
    char *name = (char *)calloc(max_lenght, sizeof(char));
    snprintf(name, max_lenght, base_name, cpu_id);
    return name;
}

void init_cpu_sensor(cpu_sensor_t *sensor, unsigned int cpu_id)
{
    static char filename[BUFFER_SIZE];
    sprintf(filename, base_str, cpu_id);

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("rdmsr:open");
        exit(127);
    }

    u_int64_t raw_core_energy = read_raw_core_energy(fd);
    u_int64_t raw_pkg_energy = read_raw_pkg_energy(fd);

    sensor->cpu_id = cpu_id;
    sensor->name = get_name(cpu_id);
    sensor->fd = fd;
    sensor->energy_units = read_unit(fd);
    sensor->core_energy = raw_to_microjoule(raw_core_energy, sensor->energy_units);
    sensor->pkg_energy = raw_to_microjoule(raw_pkg_energy, sensor->energy_units);
}

u_int64_t get_core_energy(cpu_sensor_t *sensor)
{
    u_int64_t raw_core_energy = read_raw_core_energy(sensor->fd);
    u_int64_t core_energy = raw_to_microjoule(raw_core_energy, sensor->energy_units);

    u_int64_t energy_consumed = modulo_substraction(sensor->core_energy, core_energy);
    sensor->core_energy = core_energy;
    return energy_consumed;
}

u_int64_t get_pkg_energy(cpu_sensor_t *sensor)
{
    u_int64_t raw_pkg_energy = read_raw_pkg_energy(sensor->fd);
    u_int64_t pkg_energy = raw_to_microjoule(raw_pkg_energy, sensor->energy_units);

    u_int64_t energy_consumed = modulo_substraction(sensor->pkg_energy, pkg_energy);
    sensor->pkg_energy = pkg_energy;
    return energy_consumed;
}

void clean_cpu_sensor(cpu_sensor_t *sensor)
{
    close(sensor->fd);
    free(sensor->name);
}

// ----------------------AMD_RAPL_INTERFACE


void free_amd_rapl(_amd_rapl_t *rapl)
{
    free(rapl->sensors);
    free(rapl);
}


unsigned int init_amd_rapl(char *none, void **ptr)
{
// 	FILE *fcpuinfo = fopen(cpuinfo, "r");
// 	if (fcpuinfo == NULL) {
// 		perror("fopen");
// 		exit(1);
// 	}
//     Parser parser = {
//         (GenericPointer) cpus,
// 		0,
//         MAX_CPUS,
//         sizeof(cpu_sensor_t),
//         keys,
// 		NB_KEYS,
// 		fcpuinfo
//     };
//     parse(&parser);
// 	fclose(fcpuinfo);

// #ifdef DEBUG
// 	printf("nb_cpus: %d\n", parser.nb_stored);
// 	for (unsigned int nb_cpu = 0; nb_cpu < parser.nb_stored; ++nb_cpu) {
// 		debug_print_sensor(&cpus[nb_cpu]);
// 	}
// #endif

    UNUSED(none);
    _amd_rapl_t *rapl = (_amd_rapl_t *) calloc(1, sizeof(_amd_rapl_t));

    unsigned int nb_cpu = get_nb_cpu();
    if (nb_cpu == 0) {
        perror("get_nb_cpu");
        exit(127);
    }

    cpu_sensor_t *cpus = (cpu_sensor_t *) calloc(nb_cpu, sizeof(cpu_sensor_t));

    rapl->nb = nb_cpu;
    rapl->sensors = cpus;

    for (unsigned int i = 0; i < nb_cpu; i++) {
        init_cpu_sensor(&rapl->sensors[i], i);
    }

    *ptr = (void *) rapl;
    return rapl->nb;
}


unsigned int get_amd_rapl(uint64_t *results, void *ptr)
{
    _amd_rapl_t *rapl = (_amd_rapl_t *) ptr;
    for (unsigned int i = 0; i < rapl->nb; i++) {
        // TODO: check if unit is the same
#warning "check if unit is the same"
        results[i] = get_core_energy(&rapl->sensors[i]);
    }
    return rapl->nb;
}

void label_amd_rapl(char **labels, void *ptr)
{
    _amd_rapl_t *rapl = (_amd_rapl_t *) ptr;
    for (unsigned int i = 0; i < rapl->nb; i++) {
        labels[i] = rapl->sensors[i].name;
    }
}

void clean_amd_rapl(void *ptr)
{
    _amd_rapl_t *rapl = (_amd_rapl_t *) ptr;

    for (unsigned int i = 0; i < rapl->nb; ++i) {
        clean_cpu_sensor(&rapl->sensors[i]);
    }
    free(rapl->sensors);
    free(rapl);
}


#ifdef __TESTING_AMD__
#include "small_test.h"

void test_raw_to_microjoule()
{
    printf("==== TEST raw_to_microjoule() ====\n");
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
}

void test_get_name()
{
    printf("==== TEST get_name() ====\n");
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

void test_label_amd_rapl()
{
    printf("==== TEST label_amd_rapl() ====\n");
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
    TEST_T_ARRAY(TEST_STR, nb, results, expecteds);

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
    TEST_T_ARRAY(TEST_STR, nb, results, expecteds);

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
    TEST_T_ARRAY(TEST_STR, nb, results, expecteds);
}


int main()
{
    test_raw_to_microjoule();
    test_get_name();
    test_label_amd_rapl();

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
