# MOJITO/S

An Open Source System, Energy and Network Monitoring Tools at the O/S level
MojitO/S runs on GNU/Linux

## Usage

```bash
Usage : ./bin/mojitos [OPTIONS] [SENSOR ...] [-- <cmd> <argument>...]

OPTIONS:
-f|--freq <freq>
	set amount of measurements per second.
-t|--time <time>
	set duration value (seconds). If 0, then loops infinitely.
-o|--option <output file> or <port number>
	specify a log file for MojitO/S or a port number for prometeus_mojitO/S.
-s|--overhead-stats
	enable overhead statistics (nanoseconds).

```

If a command is provided, mojitos will start the command with its arguments and will stop either when it finishes or when the timer is reached.

The following is an exhaustive list of all the sensors (it is very likely
that one will not have all the sensors activated in his build):
```bash
SENSORS:
-r|--amd-rapl
	AMD RAPL (micro-joules)
-p|--perf-list <perf_list>
	performance counters
	perf_list is a coma separated list of performance counters.
	Ex: instructions,cache_misses
-l|--list
	list the available performance counters and quit
-i|--monitor-infiniband <infiniband_path>
	infiniband monitoring (if infiniband_path is X, tries to detect it automatically)
-u|--sysload
	system load
-d|--net-dev <net_dev>
	network monitoring (if network_device is X, tries to detect it automatically)
-n|--nvidia-gpu
	provides basic gpu information [clocks, memory, utilization, power, temperature].
-r|--intel-rapl
	INTEL RAPL (micro-joules)
-c|--cpu-temp
	processor temperature
-m|--memory
	Retrieves information about the memory via the syscall 'sysinfo(2)'.
-M|--memory-counters <memory_list>
	memory counters
	memory_list is a coma separated list of memory counters.
	Ex: Zswap,Zswapped
-L|--memory-list
	list the available memory counters and quit
-k|--likwid <perf_list>
	performance counters
	perf_list is a coma separated list of performance counters with associated register.
	Ex: FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE:PMC0,FP_ARITH_INST_RETIRED_SCALAR_DOUBLE:PMC1
-w|--list-likwid
	list the available performance counters and quit
```

## Installation Instructions

Download the source code
```bash
git clone https://gitlab.irit.fr/sepia-pub/mojitos.git
```
The quickest way to compile the code is:
```bash
cd mojitos
./configure.sh
make
```
You may want to run `./configure.sh --help` to see configuration options.

To use `amd_rapl` you have to load the module `msr` and use mojitO/S as root
```bash
sudo modprobe msr
```
To execute mojitos without being root to monitor performance counters
```bash
sudo sh -c 'echo 0 >/proc/sys/kernel/perf_event_paranoid'
```
To execute mojitos without being root for accessing RAPL on Intel
```bash
sudo chmod o+r /sys/class/powercap/intel-rapl/*/*
sudo chmod o+r /sys/class/powercap/intel-rapl/*/*/*
```

For now likwid backend needs mojitos to run root privileges

## Tutorial and Examples

RAPL values during 2 seconds with a frequency of 2 Hz
```bash
$ ./bin/mojitos -t 2 -f 2 -r
#timestamp package-00 core0 dram0
1036389.135659868 10986 2869 1526
1036389.500183551 1291440 255736 515562
1036390.000754048 1333553 228393 689513
1036390.500113978 1581967 267944 701536
```
Performance counters (cpu_cycle, cache_ll_r_a and page_faults) during 4 seconds with a frequency of 1Hz. For cache performance counters, _r and _w are respectively read and write, and _a, _m and _p are respectively access, miss, pending.

```bash
$ ./bin/mojitos -t 4 -f 1 -p cpu_cycles,cache_ll_r_a,page_faults
#timestamp cpu_cycles cache_ll page_faults
1036846.351749455 571199 1232 0
1036847.001098880 348173344 2451387 872
1036848.000166158 388112961 2509305 791
1036849.000191883 402255979 2625283 799
```

Network values with no time limit with a frequency of 1Hz. rxp and txp are the number of received and sent packets, while rxb and txp are the number of received and sent bytes.
```bash
$ ./bin/mojitos -t 0 -f 1 -d enp0s25
#timestamp rxp rxb txp txb
1036559.277376027 0 0 0 0
1036560.000161101 4 581 2 179
1036561.000083968 178 268675 55 4954
1036562.000076162 11 1010 5 510
1036563.000069724 17 1643 12 3602
1036564.000113394 990 1493008 369 27299
```

Overhead of the monitoring for RAPL and cpu_cycle
```bash
$ ./bin/mojitos -t 5 -f 1 -p cpu_cycles -r -s
#timestamp cpu_cycles package-00 core0 dram0 overhead
1036988.197227391 162214 19898 4944 1586 149612
1036989.000151326 332613664 2513116 379577 1115171 739573
1036990.000116433 482150700 3321341 587218 1380673 315719
1036991.000182835 525984292 3592582 691221 1385982 272182
1036992.000165117 397678789 2770561 444030 1375729 510379
```

Execution of a command. All elements after the <dash><dash> will be executed
```bash
$ ./bin/mojitos -u -f 4 -- sleep 1
#timestamp user nice system idle iowait irq softirq steal guest guest_nice
682986.913305929 0 0 0 0 0 0 0 0 0 0
682987.000213649 0 0 0 69 0 0 0 0 0 0
682987.250266242 1 0 0 199 0 0 0 0 0 0
682987.500133767 0 0 0 198 0 0 0 0 0 0
682987.750316921 1 0 0 199 0 0 0 0 0 0
```

If time and a command are provided the first to occur will stop the monitoring
```bash
$ ./bin/mojitos -u -f 4 -t 1 -- sleep 2
#timestamp user nice system idle iowait irq softirq steal guest guest_nice
683088.111452691 0 0 0 0 0 0 0 0 0 0
683088.250211614 1 0 0 110 0 0 0 0 0 0
683088.500342921 1 0 1 199 0 0 0 0 0 0
683088.750341019 1 0 0 199 0 0 0 0 0 0
```


## License

MojitO/S is published under the GPL3 license and is part of the [Energumen Project](https://www.irit.fr/energumen/)

<img src="https://www.irit.fr/energumen/images/energumen.png" width="100">

## Citation

If you use this software for your publication, please cite:

```
Georges da Costa. MojitO/S. 2021, ⟨swh:1:dir:06cf89641458237dc2b498c0a6bfdbf86069f9be;origin=https://hal.archives-ouvertes.fr/hal-03453537;visit=swh:1:snp:669100e5754a91c1e9b61626bb8e2de2a5bbee05;anchor=swh:1:rel:c8d27b5ae7715198e3e51e4ea87ec9d3afe18f61;path=/⟩. ⟨hal-03453537⟩
```

Or directly use the following bibtex entry:

```
@softwareversion{dacosta:hal-03453537v1,
  TITLE = {{MojitO/S}},
  AUTHOR = {da Costa, Georges},
  URL = {https://hal.science/hal-03453537},
  NOTE = {},
  YEAR = {2021},
  MONTH = Nov,
  SWHID = {swh:1:dir:06cf89641458237dc2b498c0a6bfdbf86069f9be;origin=https://hal.archives-ouvertes.fr/hal-03453537;visit=swh:1:snp:669100e5754a91c1e9b61626bb8e2de2a5bbee05;anchor=swh:1:rel:c8d27b5ae7715198e3e51e4ea87ec9d3afe18f61;path=/},
  VERSION = {1.0},
  REPOSITORY = {https://gitlab.irit.fr/sepia-pub/mojitos},
  LICENSE = {GNU General Public License v3.0 or later},
  KEYWORDS = {Performance counters ; Power monitoring ; Monitoring Software},
  FILE = {https://hal.science/hal-03453537/file/mojitos-v1.0.zip},
  HAL_ID = {hal-03453537},
  HAL_VERSION = {v1},
}
```
