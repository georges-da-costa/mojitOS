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

// -- test :
// gcc -std=gnu99 -Wall -Wextra -Wpedantic -fsanitize=address -D__NVIDIA__MAIN__TEST nvidia_gpu.c -I/lib -I/usr/local/cuda/include -L/usr/local/cuda/lib64 -lnvidia-ml
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <nvml.h>
#pragma GCC diagnostic pop

#include "util.h"

// -----------------------------SENSOR_KIND
typedef enum {
    CLOCK_SENSOR       = 0,
    MEMORY_SENSOR      = 1,
    UTILIZATION_SENSOR = 2,
    POWER_SENSOR       = 3,
    TEMPERATURE_SENSOR = 4,

    COUNT_SENSOR       = 5,
} SENSOR_KIND;

typedef struct Device Device;
typedef struct NvidiaGpu NvidiaGpu;
typedef struct ISensor ISensor;
typedef struct Sensor Sensor;

// -- Sensor interface
typedef unsigned int (Initializer) (const Device *, void **);
typedef unsigned int (Getter)      (uint64_t *, const Device *, void *);
typedef unsigned int (Labeller)    (char **, void *);
typedef void         (Cleaner)     (void *);

struct ISensor {
    Initializer *init;
    Getter *get;
    Labeller *label;
    Cleaner *clean;
};

// -- Sensor
struct Sensor {
    void *data;
    const ISensor *fun;
};

// -- Device: represents a gpu
struct Device {
    char name[NVML_DEVICE_NAME_BUFFER_SIZE];
    nvmlDevice_t device;
    unsigned int idx;

    Sensor sensors[COUNT_SENSOR];
    unsigned int count;
};

// -- NvidiaGpu: represents the devices
struct NvidiaGpu {
    Device *devices;
    unsigned int count;
};

// -- Label template
static const char *label_template = "gpu%u_%s_%s";
static const char *short_label_template = "gpu%u_%s";

// ----------------------------CLOCK_SENSOR

#define CLOCK_LABEL_SIZE 25

// -- All existing clocks
// -- SM : Streaming Multiprocessor
static const nvmlClockType_t clocks[NVML_CLOCK_COUNT] = {NVML_CLOCK_GRAPHICS, NVML_CLOCK_SM, NVML_CLOCK_MEM, NVML_CLOCK_VIDEO};
static const char *clock_names[NVML_CLOCK_COUNT] = {"graphics", "sm", "memory", "video"};
static const char *clock_base_name = "clk";

// -- Must contain the clocks compatible with the device
typedef struct {
    nvmlClockType_t clocks[NVML_CLOCK_COUNT];
    char labels[NVML_CLOCK_COUNT][CLOCK_LABEL_SIZE];
    unsigned int count;
} ClockData;

unsigned int init_clock_sensor(const Device *device, void **data)
{
    const nvmlDevice_t nvml_device = device->device;
    const unsigned int device_idx = device->idx;
    ClockData tmp = {0};
    nvmlReturn_t err;
    unsigned int clock;

    // -- Test all clocks
    for (unsigned int i = 0; i < NVML_CLOCK_COUNT; i++) {
        if ((err = nvmlDeviceGetClockInfo(nvml_device, clocks[i], &clock)) == NVML_SUCCESS) {
            snprintf(tmp.labels[tmp.count], CLOCK_LABEL_SIZE, label_template, device_idx, clock_base_name, clock_names[i]);
            tmp.clocks[tmp.count] = clocks[i];
            tmp.count += 1;
        } else {
            fprintf(stderr, "Failed to get %s clock : %s\n", clock_names[i], nvmlErrorString(err));
        }
    }

    // -- No clock avaible
    if (tmp.count == 0) {
        return 0;
    }

    *data = calloc(1, sizeof(ClockData));
    memcpy(*data, &tmp, sizeof (ClockData));
    return tmp.count;
}

unsigned int get_clock_sensor(uint64_t *results, const Device *device, void *data)
{
    const nvmlDevice_t nvml_device = device->device;
    ClockData *clock_data = (ClockData *) data;
    nvmlReturn_t err;
    unsigned int clock;

    for (unsigned int i = 0; i < clock_data->count; i++) {
        nvmlClockType_t clock_type = clock_data->clocks[i];

        if((err = nvmlDeviceGetClockInfo(nvml_device, clock_type, &clock)) != NVML_SUCCESS) {
            fprintf(stderr, "Failed to get %s clock : %s\n", clock_names[clock_type], nvmlErrorString(err));
            exit(99);
        }
        results[i] = clock;
    }
    return clock_data->count;
}

unsigned int label_clock_sensor(char **labels, void *data)
{
    ClockData *clock_data = (ClockData *) data;

    for (unsigned int i = 0; i < clock_data->count; i++) {
        labels[i] = clock_data->labels[i];
    }

    return clock_data->count;
}

void clean_clock_sensor(void *data)
{
    free(data);
}

// ---------------------------MEMORY_SENSOR
#define MEMORY_LABEL_SIZE 25

typedef enum {
    FREE_MEMORY  = 0U,
    USED_MEMORY  = 1U,
    TOTAL_MEMORY = 2U,

    COUNT_MEMORY = 3U,
} MemoryKind;

static const char *memory_names[COUNT_MEMORY] = {"free", "used", "total"};
static const char *memory_base_name = "mem";

typedef struct {
    char labels[COUNT_MEMORY][MEMORY_LABEL_SIZE];
} MemoryData;

unsigned int init_memory_sensor(const Device *device, void **data)
{
    const nvmlDevice_t nvml_device = device->device;
    const unsigned int device_idx = device->idx;

    nvmlMemory_t memory;
    nvmlReturn_t err;
    if ((err = nvmlDeviceGetMemoryInfo(nvml_device, &memory)) != NVML_SUCCESS) {
        fprintf(stderr, "Failed to get device memory : %s\n", nvmlErrorString(err));
        return 0;
    }

    MemoryData *memory_data = (MemoryData *) calloc(1, sizeof(MemoryData));
    for (unsigned int i = 0; i < COUNT_MEMORY; i++) {
        snprintf(memory_data->labels[i], MEMORY_LABEL_SIZE, label_template, device_idx, memory_base_name, memory_names[i]);
    }

    *data = (void *) memory_data;
    return COUNT_MEMORY;
}

unsigned int get_memory_sensor(uint64_t *results, const Device *device, void *none)
{
    UNUSED(none);
    const nvmlDevice_t nvml_device = device->device;

    nvmlMemory_t memory;
    nvmlReturn_t err;
    if ((err = nvmlDeviceGetMemoryInfo(nvml_device, &memory)) != NVML_SUCCESS) {
        fprintf(stderr, "Failed to get device memory : %s\n", nvmlErrorString(err));
        exit(99);
    }

    results[FREE_MEMORY] = memory.free;
    results[USED_MEMORY] = memory.used;
    results[TOTAL_MEMORY] = memory.total;
    return COUNT_MEMORY;
}


unsigned int label_memory_sensor(char **labels, void *data)
{
    MemoryData *memory_data = (MemoryData *) data;

    for (unsigned int i = 0; i < COUNT_MEMORY; i++) {
        labels[i] = memory_data->labels[i];
    }

    return COUNT_MEMORY;
}
void clean_memory_sensor(void *data)
{
    free(data);
}

// ----------------------UTILIZATION_SENSOR
#define UTILIZATION_LABEL_SIZE 35
typedef enum {
    GPU_UTILIZATION    = 0U,
    MEMORY_UTILIZATION = 1U,

    COUNT_UTILIZATION  = 2U,
} UtilizationKind;

typedef struct {
    char labels[COUNT_UTILIZATION][UTILIZATION_LABEL_SIZE];
} UtilizationData;

static const char *utilization_names[COUNT_UTILIZATION] = {"gpu", "memory"};
static const char *utilization_base_name = "utilization";

unsigned int init_utilization_sensor(const Device *device, void **data)
{
    const nvmlDevice_t nvml_device = device->device;
    const unsigned int device_idx = device->idx;

    nvmlReturn_t err;
    nvmlUtilization_t utilization;
    if ((err = nvmlDeviceGetUtilizationRates(nvml_device, &utilization)) != NVML_SUCCESS) {
        fprintf(stderr, "Failed to get device utilization: %s\n", nvmlErrorString(err));
        return 0;
    }

    UtilizationData *utilization_data = (UtilizationData *) calloc(1, sizeof(UtilizationData));
    for (unsigned int i = 0; i < COUNT_UTILIZATION; i++) {
        snprintf(utilization_data->labels[i], UTILIZATION_LABEL_SIZE, label_template, device_idx, utilization_base_name, utilization_names[i]);
    }

    *data = (void *) utilization_data;
    return COUNT_UTILIZATION;
}

unsigned int get_utilization_sensor(uint64_t *results, const Device *device, void *none)
{
    UNUSED(none);
    const nvmlDevice_t nvml_device = device->device;

    nvmlReturn_t err;
    nvmlUtilization_t utilization;
    if ((err = nvmlDeviceGetUtilizationRates(nvml_device, &utilization)) != NVML_SUCCESS) {
        fprintf(stderr, "Failed to get device utilization: %s\n", nvmlErrorString(err));
        exit(99);
    }

    results[GPU_UTILIZATION] = utilization.gpu;
    results[MEMORY_UTILIZATION] = utilization.memory;
    return COUNT_UTILIZATION;
}

unsigned int label_utilization_sensor(char **labels, void *data)
{
    UtilizationData *utilization_data = (UtilizationData *) data;

    for (unsigned int i = 0; i < COUNT_UTILIZATION; i++) {
        labels[i] = utilization_data->labels[i];
    }

    return COUNT_UTILIZATION;
}

void clean_utilization_sensor(void *data)
{
    free(data);
}

// ----------------------------POWER_SENSOR

#define POWER_LABEL_SIZE 25
#define COUNT_POWER 1

static const char *power_base_name = "power";

typedef struct {
  char label[POWER_LABEL_SIZE];
} PowerData;


unsigned int init_power_sensor(const Device *device, void** data) {
    const nvmlDevice_t nvml_device = device->device;
    const unsigned int device_idx = device->idx;

    unsigned int power;
    nvmlReturn_t err;
    if ((err = nvmlDeviceGetPowerUsage(nvml_device, &power)) != NVML_SUCCESS) {
      printf("Failed to get the device power consumption: %s\n", nvmlErrorString(err));
      return 0;
    }

    PowerData *power_data = (PowerData *) calloc(1, sizeof(PowerData));
    snprintf(power_data->label, POWER_LABEL_SIZE, short_label_template, device_idx, power_base_name);

    *data = (void *) power_data;
    return COUNT_POWER;
}

unsigned int get_power_sensor(uint64_t *results, const Device *device, void *none) {
    UNUSED(none);
    const nvmlDevice_t nvml_device = device->device;

    unsigned int power;
    nvmlReturn_t err;
    if ((err = nvmlDeviceGetPowerUsage(nvml_device, &power)) != NVML_SUCCESS) {
      printf("Failed to get the device power consumption: %s\n", nvmlErrorString(err));
      exit(99);
    }

    *results = power;
    return COUNT_POWER;
}

unsigned int label_power_sensor(char** labels, void *data) {
    PowerData *power_data = (PowerData *) data;
    *labels = power_data->label;
    return COUNT_POWER;
}

void clean_power_sensor(void *data) {
    free(data);
}

// ----------------------TEMPERATURE_SENSOR


#define TEMPERATURE_LABEL_SIZE 35
#define COUNT_TEMPERATURE 1

static const char *temperature_base_name = "temperature";

typedef struct {
  char label[TEMPERATURE_LABEL_SIZE];
} TemperatureData;

unsigned int init_temperature_sensor(const Device *device, void** data) {
    const nvmlDevice_t nvml_device = device->device;
    const unsigned int device_idx = device->idx;

    unsigned int temperature;
    nvmlReturn_t err;
    if ((err = nvmlDeviceGetTemperature(nvml_device, NVML_TEMPERATURE_GPU, &temperature)) != NVML_SUCCESS) {
        printf("Failed to get the device temperature: %s\n", nvmlErrorString(err));
        return 0;
    }

    TemperatureData *temperature_data = (TemperatureData *) calloc(1, sizeof(TemperatureData));
    snprintf(temperature_data->label, TEMPERATURE_LABEL_SIZE, short_label_template, device_idx, temperature_base_name);

    *data = (void *) temperature_data;
    return COUNT_TEMPERATURE;
}

unsigned int get_temperature_sensor(uint64_t *results, const Device *device, void *none) {
    UNUSED(none);
    const nvmlDevice_t nvml_device = device->device;

    unsigned int temperature;
    nvmlReturn_t err;
    if ((err = nvmlDeviceGetTemperature(nvml_device, NVML_TEMPERATURE_GPU, &temperature)) != NVML_SUCCESS) {
        printf("Failed to get the device temperature: %s\n", nvmlErrorString(err));
        exit(99);
    }

    *results = temperature;
    return COUNT_TEMPERATURE;
}

unsigned int label_temperature_sensor(char** labels, void *data) {
    TemperatureData *temperature_data = (TemperatureData *) data;
    *labels = temperature_data->label;
    return COUNT_TEMPERATURE;
}

void clean_temperature_sensor(void *data) {
    free(data);
}
// // Get the temperature
// result = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temperature);
// if (NVML_SUCCESS != result) {
//   printf("Failed to get temperature for device %d: %s\n", i, nvmlErrorString(result));
//   continue;
// }
// printf("\t - temperature: %u\n", temperature);

// ----------------------------------------


// -------------------------AVAIBLE_SENSORS
static const ISensor avaible_sensors[COUNT_SENSOR] = {
    {.init = init_clock_sensor, .get = get_clock_sensor, .label = label_clock_sensor, .clean = clean_clock_sensor},
    {.init = init_memory_sensor, .get = get_memory_sensor, .label = label_memory_sensor, .clean = clean_memory_sensor},
    {.init = init_utilization_sensor, .get = get_utilization_sensor, .label = label_utilization_sensor, .clean = clean_utilization_sensor},
    {.init = init_power_sensor, .get = get_power_sensor, .label = label_power_sensor, .clean = clean_power_sensor},
    {.init = init_temperature_sensor, .get = get_temperature_sensor, .label = label_temperature_sensor, .clean = clean_temperature_sensor},
};

// ------------------------DEVICE_FUNCTIONS

unsigned int init_device(unsigned int device_idx, Device *device)
{
    nvmlReturn_t result;
    nvmlDevice_t nvml_device;
    if ((result = nvmlDeviceGetHandleByIndex(device_idx, &nvml_device)) != NVML_SUCCESS) {
        fprintf(stderr, "Failed to get device handle for device %d: %s\n", device_idx, nvmlErrorString(result));
        return 0;
    }

    if ((result = nvmlDeviceGetName(nvml_device, device->name, NVML_DEVICE_NAME_BUFFER_SIZE))) {
        fprintf(stderr, "Failed to get device name for device %d: %s\n", device_idx, nvmlErrorString(result));
        return 0;
    }

    device->device = nvml_device;
    device->idx = device_idx;

    unsigned int sensor_count = 0;
    unsigned int total_count = 0;

    for (unsigned int i = 0; i < COUNT_SENSOR; i++) {
        Sensor *sensor = &device->sensors[sensor_count];
        sensor->fun = &avaible_sensors[i];
        unsigned int count;

        if ((count = sensor->fun->init(device, &sensor->data)) != 0) {
            sensor_count += 1;
            total_count += count;
        }
    }

    device->count = sensor_count;
    return total_count;
}

unsigned int get_device(uint64_t *results, Device *device)
{
    unsigned int count = 0;
    for (unsigned int i = 0; i < device->count; i++) {
        Sensor *sensor = &device->sensors[i];
        unsigned int result = sensor->fun->get(results, device, sensor->data);
        count += result;
        results += result;
    }

    return count;
}

unsigned int label_device(char **labels, Device *device)
{
    unsigned int count = 0;
    for (unsigned int i = 0; i < device->count; i++) {
        Sensor *sensor = &device->sensors[i];
        unsigned int result = sensor->fun->label(labels, sensor->data);
        labels += result;
        count += result;
    }

    return count;
}

void clean_device(Device *device)
{
    for (unsigned int i = 0; i < device->count; i++) {
        Sensor *sensor = &device->sensors[i];
        sensor->fun->clean(sensor->data);
    }
}


// ------------------------NVIDIA_INTERFACE

unsigned int init_nvidia_gpu(char *none, void **ptr)
{
    UNUSED(none);
    UNUSED(ptr);

    nvmlReturn_t result;
    if ((result = nvmlInit()) != NVML_SUCCESS) {
        fprintf(stderr, "Failed to initialize NVML: %s\n", nvmlErrorString(result));
        exit(1);
    }

    unsigned int avaible_device_count;
    if ((result = nvmlDeviceGetCount(&avaible_device_count)) != NVML_SUCCESS) {
        fprintf(stderr, "Failed to get device count : %s\n", nvmlErrorString(result));
        nvmlShutdown();
        exit(1);
    }

    Device *devices = calloc(avaible_device_count, sizeof(Device));

    unsigned int sensor_count = 0;
    unsigned int device_count = 0;
    for (unsigned int i = 0; i < avaible_device_count; i++) {
        unsigned int initialized_count;
        if ((initialized_count = init_device(i, &devices[device_count])) != 0) {
            sensor_count += initialized_count;
            device_count += 1;
        }
    }

    NvidiaGpu *nvidia = (NvidiaGpu *) calloc(1, sizeof(NvidiaGpu));
    nvidia->devices = devices;
    nvidia->count = device_count;

    *ptr = (void *) nvidia;
    return sensor_count;
}


unsigned int get_nvidia_gpu(uint64_t *results, void *ptr)
{
    NvidiaGpu *nvidia = (NvidiaGpu *) ptr;
    unsigned count = 0;

    for (unsigned int i = 0; i < nvidia->count; i++) {
        unsigned int result = get_device(results, &nvidia->devices[i]);
        results += result;
        count += result;
    }

    return count;
}

unsigned int label_nvidia_gpu(char **labels, void *ptr)
{
    NvidiaGpu *nvidia = (NvidiaGpu *) ptr;
    unsigned count = 0;

    for (unsigned int i = 0; i < nvidia->count; i++) {
        unsigned int result = label_device(labels, &nvidia->devices[i]);
        labels += result;
        count += result;
    }

    return count;
}

void clean_nvidia_gpu(void *ptr)
{
    NvidiaGpu *nvidia = (NvidiaGpu *) ptr;

    for (unsigned int i = 0; i < nvidia->count; i++) {
        clean_device(&nvidia->devices[i]);
    }

    free(nvidia->devices);
    free(nvidia);
    nvmlShutdown();
}

// -------------------------------TEST_MAIN

#ifdef __NVIDIA__MAIN__TEST
int main()
{
    void *ptr = NULL;
    char *none = NULL;

    unsigned int sensor_count = init_nvidia_gpu(none, &ptr);

    NvidiaGpu *nvidia = (NvidiaGpu *) ptr;
    printf("%d\n", nvidia->count);
    printf("%u\n", sensor_count);



    uint64_t results[sensor_count];
    char *labels[sensor_count];

    memset(results, 0, sensor_count * sizeof(uint64_t));
    memset(labels, 0, sensor_count * sizeof(char **));


    unsigned count_label = label_nvidia_gpu(labels, ptr);
    unsigned count_get = get_nvidia_gpu(results, ptr);
    printf("total : %u, get : %u, label : %u\n", sensor_count, count_get, count_label);


    for (unsigned int i = 0; i < sensor_count; i++) {
        printf("%s ", labels[i]);
    }
    printf("\n");
    for (unsigned int i = 0; i < sensor_count; i++) {
        printf("%lu ", results[i]);
    }
    printf("\n");
    printf("sensor_count: %d\n", sensor_count);

    clean_nvidia_gpu(ptr);
}
#endif

