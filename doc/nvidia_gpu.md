# Nvidia Gpu

The `nvidia_gpu` sensor provides basic information about the gpu. Depending on
the driver version it is possible that not all sensors are supported, so an
error message will be written to `stderr` but the execution will continue.

For more information you can consult the [nvidia nvml api](https://docs.nvidia.com/deploy/index.html).

## [Clock](https://docs.nvidia.com/deploy/nvml-api/group__nvmlDeviceEnumvs.html#group__nvmlDeviceEnumvs_1g805c0647be9996589fc5e3f6ff680c64)

All speeds are in Mhz.

|Output  |Description                    |
|--------|-------------------------------|
|graphics|Graphics clock                 |
|sm      |Streaming Multiprocessor clock |
|memory  |Memory clock                   |
|video   |Video encoder/decoder clock    |

## [Memory](https://docs.nvidia.com/deploy/nvml-api/structnvmlMemory__t.html#structnvmlMemory__t)

All values are in bytes.

|Output  |Description                          |
|--------|-------------------------------------|
|free    |Unallocated device memory            |
|used    |Sum of Reserved and Allocated memory |
|total   |Total physical device memory         |


## [Utilization](https://docs.nvidia.com/deploy/nvml-api/structnvmlUtilization__t.html#structnvmlUtilization__t)

Utilization information for a device. Each sample period may be between 1
second and 1/6 second, depending on the product being queried.

All values are a percent of time over the past sample period.

|Output  |Description          |
|--------|---------------------|
|gpu     | Usage of the GPU    |
|memory  | Usage of the Memory |

## [Power](https://docs.nvidia.com/deploy/nvml-api/group__nvmlDeviceQueries.html#group__nvmlDeviceQueries_1g7ef7dff0ff14238d08a19ad7fb23fc87)

Retrieves power usage for this GPU in milliwatts and its associated circuitry (e.g. memory)

|Output  |Description              |
|--------|-------------------------|
|power   | Power consumption in mW |

## [Temperature](https://docs.nvidia.com/deploy/nvml-api/group__nvmlDeviceEnumvs.html#group__nvmlDeviceEnumvs_1g2650b526841fa38b8f293c2d509a1de0)
Temperature of the GPU.

|Output      |Description                 |
|------------|----------------------------|
|temperature | Temperature of the GPU die |

