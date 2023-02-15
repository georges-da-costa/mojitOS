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
#define __READ_CPUINFO__

// ---------------------------MSR_REGISTERS
static const uint64_t amd_energy_mask = 0xFFFFFFFF;
static const uint64_t amd_energy_unit_mask = 0x01F00;
static const uint64_t msr_rapl_power_unit = 0xC0010299;
static const uint64_t energy_core_msr = 0xC001029A;

// ------------------------------FILE_PATHS
static const char *base_str = "/dev/cpu/%d/msr";

struct CpuSensor {
    unsigned int cpu_id;
    unsigned int package_id;
    unsigned int core_id;

    char *name;
    int fd;

    unsigned int energy_units;
    uint64_t core_energy;
};
typedef struct CpuSensor CpuSensor;

struct AmdRapl {
    CpuSensor *sensors;
    unsigned int sensor_count;
};
typedef struct AmdRapl AmdRapl;

// -----------------------------INFO_READER

#ifdef __READ_CPUINFO__
#define NB_KEYS 3

static char *cpuinfo = "/proc/cpuinfo";

static GenericPointer uint_allocator(char *s)
{
    unsigned int value = atoi(s);
    return (GenericPointer) value;
}

static void _set_cpu_id(GenericPointer storage, GenericPointer data)
{
    CpuSensor *cpu = (CpuSensor *) storage;
    cpu->cpu_id = (unsigned int) data;
}

static void _set_package_id(GenericPointer storage, GenericPointer data)
{
    CpuSensor *cpu = (CpuSensor *) storage;
    cpu->package_id = (unsigned int) data;
}

static void _set_core_id(GenericPointer storage, GenericPointer data)
{
    CpuSensor *cpu = (CpuSensor *) storage;
    cpu->core_id = (unsigned int) data;
}

static KeyFinder keys[NB_KEYS] = {
    {"processor", ": ", uint_allocator, _set_cpu_id},
    {"physical id", ": ", uint_allocator, _set_package_id},
    {"core id", ": ", uint_allocator, _set_core_id}
};


static unsigned int parse_cpuinfo(CpuSensor *storage, unsigned int capacity)
{
    Parser parser = {
        .storage = (GenericPointer) storage,
        .nb_stored = 0,
        .capacity = capacity,
        .storage_struct_size = sizeof(CpuSensor),
        .keys = keys,
        .nb_keys = NB_KEYS,
        .file = fopen(cpuinfo, "r")
    };
    return parse(&parser);
}

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
void debug_print_sensor(CpuSensor *sensor)
{
    //CASSERT(sizeof(CpuSensor) == 56, amd_rapl_c);
    printf("cpu_id : %d, package_id : %d, core_id : %d, name : %s, fd: %d,  energy_units : %d, core_energy: %ld\n",
           sensor->cpu_id,
           sensor->package_id,
           sensor->core_id,
           sensor->name,
           sensor->fd,
           sensor->energy_units,
           sensor->core_energy
          );
}

void debug_print_amd_rapl(AmdRapl *rapl)
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
        snprintf(filename, BUFFER_SIZE, base_str, n_cpu);

        int fd = open(filename, O_RDONLY);
        if (fd < 0) {
            break;
        }
        close(fd);
    }
    return n_cpu;
}

void get_arch(unsigned int *ret_nb_package, unsigned int *ret_nb_core, CpuSensor *sensors, unsigned int nb_sensor)
{
    unsigned int nb_package = 0;
    unsigned int nb_core = 0;
    for (unsigned int i = 0; i < nb_sensor; i++) {
        if (sensors[i].package_id > nb_package) {
            nb_package = sensors[i].package_id;
        }
        if (sensors[i].core_id > nb_core) {
            nb_core = sensors[i].core_id;
        }
    }
    *ret_nb_package = nb_package + 1;
    *ret_nb_core = nb_core + 1;
}

char *get_name(unsigned int cpu_id)
{
    static const char *base_name = "core%d";
    static const size_t max_lenght = 20;
    char *name = (char *)calloc(max_lenght, sizeof(char));
    snprintf(name, max_lenght, base_name, cpu_id);
    return name;
}

void update_cpu_sensor(CpuSensor *sensor, uint64_t *energy_consumed)
{
    sensor->energy_units = read_unit(sensor->fd);
    uint64_t raw_core_energy = read_raw_core_energy(sensor->fd);
    uint64_t core_energy = raw_to_microjoule(raw_core_energy, sensor->energy_units);

    *energy_consumed = modulo_substraction(core_energy, sensor->core_energy);
    sensor->core_energy = core_energy;
}

unsigned int is_duplicate(CpuSensor *sensor,unsigned int nb_core, unsigned int nb_package, unsigned char map[nb_core][nb_package])
{
    if (map[sensor->core_id][sensor->package_id] == 1) {
        return 0;
    }
    map[sensor->core_id][sensor->package_id] += 1;
    return 1;
}

void init_cpu_sensor(CpuSensor *sensor, CpuSensor *cpu_info)
{
    static char filename[BUFFER_SIZE];
    snprintf(filename,BUFFER_SIZE, base_str, cpu_info->cpu_id);

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, base_str, cpu_info->cpu_id);
        perror(":open()");
        exit(127);
    }

    memcpy(sensor, cpu_info, sizeof(CpuSensor));
    sensor->name = get_name(sensor->cpu_id);
    sensor->fd = fd;
}

void clean_cpu_sensor(CpuSensor *sensor)
{
    close(sensor->fd);
    free(sensor->name);
}

void free_amd_rapl(AmdRapl *rapl)
{
    free(rapl->sensors);
    free(rapl);
}

// ----------------------AMD_RAPL_INTERFACE

unsigned int init_amd_rapl(char *none, void **ptr)
{
    UNUSED(none);

    unsigned int max_cpus = get_nb_cpu();
    if (max_cpus == 0) {
        fprintf(stderr, base_str, 0);
        perror(":open()");
        exit(127);
    }

    CpuSensor *cpu_information = (CpuSensor *) calloc(max_cpus, sizeof(CpuSensor));
    if (parse_cpuinfo(cpu_information, max_cpus)) {
        free(cpu_information);
        PANIC(1, "cpuinfo");
    }

    unsigned int nb_package;
    unsigned int nb_core;
    get_arch(&nb_package, &nb_core, cpu_information, max_cpus);

    unsigned char cpu_map[nb_core][nb_package];
    memset(cpu_map, 0, sizeof(cpu_map));
    CpuSensor *sensors = (CpuSensor *) calloc(max_cpus, sizeof(CpuSensor));

    unsigned int sensor_count = 0;
    for (unsigned int i = 0; i < max_cpus; i++) {
        if (is_duplicate(&cpu_information[i], nb_core, nb_package, cpu_map) == 1) {
            init_cpu_sensor(&sensors[sensor_count],&cpu_information[i]);
            sensor_count += 1;
        }
    }
    free(cpu_information);

    AmdRapl *rapl = (AmdRapl *) calloc(1, sizeof(AmdRapl));
    rapl->sensors = sensors;
    rapl->sensor_count = sensor_count;
    *ptr = (void *) rapl;
    return rapl->sensor_count;
}


unsigned int get_amd_rapl(uint64_t *results, void *ptr)
{
    AmdRapl *rapl = (AmdRapl *) ptr;
    for (unsigned int i = 0; i < rapl->sensor_count; i++) {
        update_cpu_sensor(&rapl->sensors[i], &results[i]);
    }
    return rapl->sensor_count;
}

void label_amd_rapl(char **labels, void *ptr)
{
    AmdRapl *rapl = (AmdRapl *) ptr;
    for (unsigned int i = 0; i < rapl->sensor_count; i++) {
        labels[i] = rapl->sensors[i].name;
    }
}

void clean_amd_rapl(void *ptr)
{
    AmdRapl *rapl = (AmdRapl *) ptr;

    for (unsigned int i = 0; i < rapl->sensor_count; ++i) {
        clean_cpu_sensor(&rapl->sensors[i]);
    }
    free(rapl->sensors);
    free(rapl);
}

