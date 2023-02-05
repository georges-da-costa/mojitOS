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

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "info_reader.h"
#include "util.h"

#define BUFFER_SIZE 64

// ---------------------------MSR_REGISTERS
static const uint64_t amd_energy_mask = 0xFFFFFFFF;
static const uint64_t amd_energy_unit_mask = 0x01F00;
static const uint64_t msr_rapl_power_unit = 0xC0010299;
static const uint64_t energy_core_msr = 0xC001029A;

// ------------------------------FILE_PATHS
static const char *base_str = "/dev/cpu/%d/msr";

struct cpu_sensor_t {
    unsigned int cpu_id;
    unsigned int package_id;
    unsigned int core_id;

    char *name;
    int fd;

    unsigned int energy_units;
    uint64_t core_energy;
};
typedef struct cpu_sensor_t cpu_sensor_t;

struct _amd_rapl_t {
    cpu_sensor_t *sensors;
    unsigned int sensor_count;
};
typedef struct _amd_rapl_t _amd_rapl_t;

// -----------------------------INFO_READER

#ifdef __READ_CPUINFO__
#warning "Must be modified before release"
#define MAX_CPUS 64
#define NB_KEYS 3

static const char *cpuinfo = "/proc/cpuinfo";

static GenericPointer uint_allocator(char *s)
{
    unsigned int value = atoi(s);
    return (GenericPointer) value;
}

static void _set_cpu_id(GenericPointer storage, GenericPointer data)
{
    cpu_sensor_t *cpu = (cpu_sensor_t *) storage;
    cpu->cpu_id = (unsigned int) data;
}

static void _set_package_id(GenericPointer storage, GenericPointer data)
{
    cpu_sensor_t *cpu = (cpu_sensor_t *) storage;
    cpu->package_id = (unsigned int) data;
}

static void _set_core_id(GenericPointer storage, GenericPointer data)
{
    cpu_sensor_t *cpu = (cpu_sensor_t *) storage;
    cpu->core_id = (unsigned int) data;
}

static KeyFinder keys[NB_KEYS] = {
    {"processor", ": ", uint_allocator, _set_cpu_id},
    {"physical id", ": ", uint_allocator, _set_package_id},
    {"core id", ": ", uint_allocator, _set_core_id}
};
#endif

// --------------------------------READ_MSR

uint64_t read_msr(int fd, uint64_t msr)
{
    uint64_t data;
    if (pread(fd, &data, sizeof data, msr) != sizeof data) {
        fprintf(stderr, "read_msr(%ld):", msr);
        perror("pread");
        exit(127);
    }
    return data;
}

unsigned int read_unit(int fd)
{
    uint64_t unit = read_msr(fd, msr_rapl_power_unit);
    return ((unit & amd_energy_unit_mask) >> 8);
}

uint64_t read_raw_core_energy(int fd)
{
    uint64_t energy = read_msr(fd, energy_core_msr);
    return energy & amd_energy_mask;
}

// -------------------------READ_PKG_ENERGY

#ifdef __READ_PKG_ENERGY__
// TODO: Verify if these functions are still useful (the package energy can be calculed)

static const uint64_t energy_pkg_msr = 0xC001029B;
uint64_t read_raw_pkg_energy(int fd)
{
    uint64_t energy = read_msr(fd, energy_pkg_msr);
    return energy & amd_energy_mask;
}
#endif

// ----------------------------------ENERGY

uint64_t raw_to_microjoule(uint64_t raw, unsigned int unit)
{
    static const double to_microjoule = 1000000.0;
    // raw * (1 / (unit^2)) -> Joule
    // Joule * 1000000 -> uJoule
    return (uint64_t) (((double) raw * to_microjoule) / (double)(1U << unit));
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
    printf("cpu_id : %d, package_id : %d, name : %s, fd: %d,  energy_units : %d, core_energy: %ld\n",
           sensor->cpu_id,
           sensor->package_id,
           sensor->name,
           sensor->fd,
           sensor->energy_units,
           sensor->core_energy,
          );
}

void debug_print_amd_rapl(_amd_rapl_t *rapl)
{
    for (unsigned int i = 0; i < rapl->sensor_count; i++) {
        debug_print_sensor(&rapl->sensors[i]);
    }
}

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

char *get_name(unsigned int cpu_id)
{
    static const char *base_name = "core%d";
    static const size_t max_lenght = 20;
    char *name = (char *)calloc(max_lenght, sizeof(char));
    snprintf(name, max_lenght, base_name, cpu_id);
    return name;
}

void update_cpu_sensor(cpu_sensor_t *sensor, uint64_t *energy_consumed)
{
    sensor->energy_units = read_unit(sensor->fd);
    uint64_t raw_core_energy = read_raw_core_energy(sensor->fd);
    uint64_t core_energy = raw_to_microjoule(raw_core_energy, sensor->energy_units);

    *energy_consumed = modulo_substraction(core_energy, sensor->core_energy);
    sensor->core_energy = core_energy;
}

unsigned int init_cpu_sensor(cpu_sensor_t *sensor, unsigned int cpu_id, unsigned char *cpus_map, unsigned int max_cpus)
{
    if (cpus_map[sensor->core_id * max_cpus + sensor->package_id] > 0) {
        return 0;
    }
    cpus_map[sensor->core_id * max_cpus + sensor->package_id] += 1;

    static char filename[BUFFER_SIZE];
    sprintf(filename, base_str, cpu_id);

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, base_str, cpu_id);
        perror(":open()");
        exit(127);
    }

    sensor->cpu_id = cpu_id;
    sensor->name = get_name(cpu_id);
    sensor->fd = fd;
    return 1;
}

void clean_cpu_sensor(cpu_sensor_t *sensor)
{
    close(sensor->fd);
    free(sensor->name);
}

void free_amd_rapl(_amd_rapl_t *rapl)
{
    free(rapl->sensors);
    free(rapl);
}

// ----------------------AMD_RAPL_INTERFACE

unsigned int init_amd_rapl(char *none, void **ptr)
{
    UNUSED(none);
    _amd_rapl_t *rapl = (_amd_rapl_t *) calloc(1, sizeof(_amd_rapl_t));

    unsigned int max_cpus = get_nb_cpu();
    if (max_cpus == 0) {
        fprintf(stderr, base_str, 0);
        perror(":open()");
        exit(127);
    }

    unsigned char *cpus_map = calloc(max_cpus * max_cpus, sizeof(unsigned char));
    cpu_sensor_t *cpus = (cpu_sensor_t *) calloc(max_cpus, sizeof(cpu_sensor_t));
    rapl->sensors = cpus;

    unsigned int nb_cpu = 0;
    for (unsigned int i = 0; i < max_cpus; i++) {
        nb_cpu += init_cpu_sensor(&rapl->sensors[nb_cpu], i, cpus_map, max_cpus);
    }
    rapl->sensor_count = nb_cpu;

    *ptr = (void *) rapl;
    free(cpus_map);
    return rapl->sensor_count;
}


unsigned int get_amd_rapl(uint64_t *results, void *ptr)
{
    _amd_rapl_t *rapl = (_amd_rapl_t *) ptr;
    for (unsigned int i = 0; i < rapl->sensor_count; i++) {
        update_cpu_sensor(&rapl->sensors[i], &results[i]);
    }
    return rapl->sensor_count;
}

void label_amd_rapl(char **labels, void *ptr)
{
    _amd_rapl_t *rapl = (_amd_rapl_t *) ptr;
    for (unsigned int i = 0; i < rapl->sensor_count; i++) {
        labels[i] = rapl->sensors[i].name;
    }
}

void clean_amd_rapl(void *ptr)
{
    _amd_rapl_t *rapl = (_amd_rapl_t *) ptr;

    for (unsigned int i = 0; i < rapl->sensor_count; ++i) {
        clean_cpu_sensor(&rapl->sensors[i]);
    }
    free(rapl->sensors);
    free(rapl);
}

