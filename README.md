# MOJITO/S

An Open Source System, Energy and Network Monitoring Tools at the O/S level

## Documentation

MojitO/S runs on GNU/Linux
Usage

```python
Usage : ./mojitos [-t time] [-f freq] [-r] [-p perf_list] [-l]  \
                  [-u] [-d network_device] [-o logfile] [-e command arguments...]
```

Timing
- If time is 0 then mojitos loops infinitively
- If -e is present, time and freq are not used

Configuration
- -r activates RAPL (deprecated)
- -R activates RAPL
- -p activates performance counters. perf_list is coma separated list of performance counters without space.
- -l lists the possible performance counters and quits
- -d activates network monitoring
- -u activates system-level load monitoring
- -s activates statistics of overhead in nanoseconds

MojitO/S is published under the GPL3 license and is part of the [Energumen Project](https://www.irit.fr/energumen/)

<img src="https://www.irit.fr/energumen/images/energumen.png" width="100">

## Installation Instructions

Dependencies
```bash
sudo apt install libpowercap0 libpowercap-dev powercap-utils python3
```
Download the source code
```bash
git clone https://gitlab.irit.fr/sepia-pub/mojitos.git
```
Compile the code
```bash
cd mojitos
make
```
To execute mojitos without being root to monitor performance counters
```bash
sudo sh -c 'echo 0 >/proc/sys/kernel/perf_event_paranoid'
```
To execute mohitos without being root for accessing RAPL
```bash
sudo chmod a+w /sys/class/powercap/intel-rapl/*/*
sudo chmod a+w /sys/class/powercap/intel-rapl/*/*/*
```

## Tutorial and Examples

RAPL values during 2 seconds with a frequency of 2 Hz
```bash
$ ./mojitos -t 2 -f 2 -r
#timestamp package-00 core0 dram0 
1036389.135659868 10986 2869 1526 
1036389.500183551 1291440 255736 515562 
1036390.000754048 1333553 228393 689513 
1036390.500113978 1581967 267944 701536 
```
Performance counters (cpu_cycle, cache_ll_r_a and page_faults) during 4 seconds with a frequency of 1Hz. For cache performance counters, _r and _w are respectively read and write, and _a, _m and _p are respectively access, miss, pending.

```bash
$ ./mojitos -t 4 -f 1 -p cpu_cycles,cache_ll_r_a,page_faults
#timestamp cpu_cycles cache_ll page_faults 
1036846.351749455 571199 1232 0 
1036847.001098880 348173344 2451387 872 
1036848.000166158 388112961 2509305 791 
1036849.000191883 402255979 2625283 799 
```

Network values with no time limit with a frequency of 1Hz. rxp and txp are the number of received and sent packets, while rxb and txp are the number of received and sent bytes.
```bash
$ ./mojitos -t 0 -f 1 -d enp0s25
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
$ ./mojitos -t 5 -f 1 -p cpu_cycles -r -s
#timestamp cpu_cycles package-00 core0 dram0 overhead 
1036988.197227391 162214 19898 4944 1586 149612 
1036989.000151326 332613664 2513116 379577 1115171 739573 
1036990.000116433 482150700 3321341 587218 1380673 315719 
1036991.000182835 525984292 3592582 691221 1385982 272182 
1036992.000165117 397678789 2770561 444030 1375729 510379 
```
