#include <stdio.h>
#include <stdint.h>
#include <nvml.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"


// -----------------------------SENSOR_KIND
typedef enum {
    CLOCK_SENSOR       = 0,
    MEMORY_SENSOR      = 1,
    UTILIZATION_SENSOR = 2,

    COUNT_SENSOR       = 3,
} SENSOR_KIND;

typedef unsigned int (Initializer) (nvmlDevice_t, void **);
typedef unsigned int (Getter)      (nvmlDevice_t, uint64_t *, void *);
typedef void         (Cleaner)     (void *);

typedef struct {
    void *data;

    Initializer *init;
    Getter *get;
    Cleaner *clean;
} ISensor;

// ----------------------------CLOCK_SENSOR
// -- All existing clocks
#if NVML_CLOCK_COUNT != 4
#error "NVML_CLOCK_COUNT must be equal 4";
#endif

// -- SM : Streaming Multiprocessor
static const nvmlClockType_t clocks[NVML_CLOCK_COUNT] = {NVML_CLOCK_GRAPHICS, NVML_CLOCK_SM, NVML_CLOCK_MEM, NVML_CLOCK_VIDEO};
static const char *clock_names[NVML_CLOCK_COUNT] = {"Graphics", "SM", "Memory", "Video"};

// -- Must contain the clocks compatible with the device
typedef struct {
    nvmlClockType_t clocks[NVML_CLOCK_COUNT];
    unsigned int count;
} ClockData;

unsigned int init_clock_sensor(nvmlDevice_t device, void** data)
{
    ClockData tmp = {0};
    nvmlReturn_t result;
    unsigned int clock;

    // -- Test all clocks
    for (unsigned int i = 0; i < NVML_CLOCK_COUNT; i++) {
        if ((result = nvmlDeviceGetClockInfo(device, clocks[i], &clock)) == NVML_SUCCESS) {
            tmp.clocks[tmp.count] = clocks[i];
            tmp.count += 1;
        } else {
            fprintf(stderr, "Failed to get %s clock : %s", clock_names[i], nvmlErrorString(result));
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

unsigned int get_clock_sensor(nvmlDevice_t device, uint64_t *results, void* data)
{
    ClockData *clock_data = (ClockData *) data;
    nvmlReturn_t err;
    unsigned int clock;

    for (unsigned int i = 0; i < clock_data->count; i++) {
        nvmlClockType_t clock_type = clock_data->clocks[i];

        if((err = nvmlDeviceGetClockInfo(device, clock_type, &clock)) != NVML_SUCCESS) {
            fprintf(stderr, "Failed to get %s clock : %s", clock_names[clock_type], nvmlErrorString(err));
            exit(99);
        }
        results[i] = clock;
    }
    return clock_data->count;
}

void clean_clock_sensor(void* data)
{
    free(data);
}

// ---------------------------MEMORY_SENSOR
typedef enum {
    FREE_MEMORY  = 0U,
    USED_MEMORY  = 1U,
    TOTAL_MEMORY = 2U,
 
    COUNT_MEMORY = 3U,
} MemoryKind;
static const char *memory_names[COUNT_MEMORY] = {"Free", "Used", "Total"};

unsigned int init_memory_sensor(nvmlDevice_t device, void **none)
{
    UNUSED(none);

    nvmlMemory_t memory;
    nvmlReturn_t result;
    if ((result = nvmlDeviceGetMemoryInfo(device, &memory)) != NVML_SUCCESS) {
        fprintf(stderr, "Failed to get device memory : %s\n", nvmlErrorString(result));
        return 0;
    }

    return COUNT_MEMORY;
}

unsigned int get_memory_sensor(nvmlDevice_t device, uint64_t *results, void *none)
{
    UNUSED(none);

    nvmlMemory_t memory;
    nvmlReturn_t result;
    if ((result = nvmlDeviceGetMemoryInfo(device, &memory)) != NVML_SUCCESS) {
        fprintf(stderr, "Failed to get device memory : %s\n", nvmlErrorString(result));
        exit(99);
    }

    results[FREE_MEMORY] = memory.free;
    results[USED_MEMORY] = memory.used;
    results[TOTAL_MEMORY] = memory.total;
    return COUNT_MEMORY;
}


void clean_memory_sensor(void *none)
{
    UNUSED(none);
}

// ----------------------UTILIZATION_SENSOR
typedef enum {
    GPU_UTILIZATION    = 0U,
    MEMORY_UTILIZATION = 1U,

    COUNT_UTILIZATION  = 2U,
} UtilizationKind;

static const char *utilization_names[COUNT_UTILIZATION] = {"Gpu", "Memory"};

unsigned int init_utilization_sensor(nvmlDevice_t device, void **none);
{
    UNUSED(none);

    nvmlReturn_t result;
    nvmlUtilization_t utilization;
    if ((result = nvmlDeviceGetUtilizationRates(device, &utilization)) != NVML_SUCCESS) {
        fprintf(stderr, "Failed to get device utilization: %s\n", nvmlErrorString(result));
        return 0;
    }

    return COUNT_UTILIZATION;
}

unsigned int get_utilization_sensor(nvmlDevice_t device, uint64_t *results, void* none)
{
    UNUSED(none);

    nvmlReturn_t result;
    nvmlUtilization_t utilization;
    if ((result = nvmlDeviceGetUtilizationRates(device, &utilization)) != NVML_SUCCESS) {
        fprintf(stderr, "Failed to get device utilization: %s\n", nvmlErrorString(result));
        exit(99);
    }

    results[GPU_UTILIZATION] = utilization.gpu;
    results[MEMORY_UTILIZATION] = utilization.memory;
    return COUNT_UTILIZATION;
}

void clean_utilization_sensor(void* none)
{
    UNUSED(none);
}

// ----------------------------ERROR_SENSOR
// TODO

// ----------------------------------------

typedef struct {
    char name[NVML_DEVICE_NAME_BUFFER_SIZE];
    nvmlDevice_t device;

    ISensor sensors[COUNT_SENSOR];
    unsigned int count;
} Device;

typedef struct {
    Device *devices;
    unsigned int count;
} NvidiaSensor;

// -------------------------AVAIBLE_SENSORS
static const ISensor avaible_sensors[COUNT_SENSOR] = {
    {.init = init_clock_sensor, .get = get_clock_sensor, .clean = clean_memory_sensor, .data = NULL};
    {.init = init_memory_sensor, .get = get_memory_sensor, .clean = clean_memory_sensor, .data = NULL};
    {.init = init_utilization_sensor, .get = get_utilization_sensor, .clean = clean_memory_sensor, .data = NULL};
};

// ------------------------DEVICE_FUNCTIONS

unsigned int init_device(unsigned int device_idx, Device *device) {
    nvmlReturn_t result;
    nvmlDevice_t nvml_device;
    if ((result = nvmlDeviceGetByIndex(i, &nvml_device) != NVML_SUCCESS) {
        fprintf(stderr, "Failed to get device handle for device %d: %s\n", device_idx, nvmlErrorString(result));
        return 0;
    }

    if ((result = nvmlDeviceGetName(nvml_device, device->name, NVML_DEVICE_NAME_BUFFER_SIZE))) {
        fprintf(stderr, "Failed to get device name for device %d: %s\n", device_idx, nvmlErrorString(result));
        return 0;
    }

    unsigned int count = 0;
    for (unsigned int i = 0; i < COUNT_SENSOR; i++) {
        void* data;
        if (avaible_sensors.init(nvml_device, &data) != 0) {
            ISensor *sensor = &device->sensors[count];
            memcpy(sensor, &avaible_sensors[i], sizeof(ISensor));
            sensor->data = data;
            count += 1;
        }
    }

    device->device = nvml_device;
    device->count = count;
    return count;
}

unsigned int get_device(Device *device, uint64_t *results) {
    unsigned int count;
    nvmlDevice_t nvml_device = device->device;
    for (unsigned int i = 0; i < device->count; i++) {
        unsigned int result = device->sensor.get(nvml_device, &device->sensor.data, results);
        count += result;
        results += result;
    }

    return count;
}
void clean_device(Device *device) {
    for (unsigned int i = 0; i < device->count; i++) {
        device->sensor.clean(&device->sensor.data);
    }
}

// ------------------------NVIDIA_INTERFACE

unsigned int init_nvidia_sensor(char *none, void **ptr)
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

    Device* devices = calloc(avaible_device_count, sizeof(Device));

    unsigned int sensor_count = 0;
    unsigned int device_count = 0;
    for (unsigned int i = 0; i < avaible_device_count; i++) {
        unsigned int initialized_count;
        if ((initialized_count = init_device(&devices[device_count])) != 0) {
            sensor_count += initialized_count;
            device_count += 1;
        }
    }

    NvidiaSensor *nvidia = (NvidiaSensor*) calloc(1, sizeof(NvidiaSensor));
    nvidia->devices = devices;
    nvidia->count = device_count;

    *ptr = (void*) nvidia;
    return count;
}


unsigned int get_nvidia_sensor(uint64_t *results, void *ptr) {
    NvidiaSensor *nvidia = (NvidiaSensor *) ptr;
    unsigned count = 0;

    for (unsigned int i = 0; i < nvidia.count; i++) {
        unsigned int result = get_device(&nvidia->devices[i], results);
        results += result;
        count += result;
    }

    return count;
}
void label_nvidia_sensor(char **labels, void *ptr);

void clean_nvidia_sensor(void *ptr)
{
    NvidiaSensor *nvidia = (NvidiaSensor *) ptr;

    for (unsigned int i = 0; i < nvidia->count; i++) {
        clean_device(&nvidia->device[i]);
    }
    nvmlShutdown();
}
