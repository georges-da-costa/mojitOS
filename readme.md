see https://sourcesup.renater.fr/mojitos

* Compiling

make

or

make debug

on Grid'5000, install the .deb files in the grid5000 directory with

sudo-g5k dpkg -i grid5000/*deb

* Executing

sudo ./mojitos time frequency
sudo ./mojitos time 
sudo ./mojitos 

time      : Execution time in seconds (-1 for infinity), default 2
frequency : Number of measures per seconds, default 4

If compiled in debugmode, the innertime is displayed on stderr

* to use as non-root :

** for performance counters:
sudo sh -c 'echo 0 >/proc/sys/kernel/perf_event_paranoid'

** for rapl

sudo chmod a+w /sys/class/powercap/intel-rapl/*/*
sudo chmod a+w /sys/class/powercap/intel-rapl/*/*/*
