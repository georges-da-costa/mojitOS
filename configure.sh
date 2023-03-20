#!/bin/sh

# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2023-2023 Georges Da Costa <georges.da-costa@irit.fr>

try() { "$@" || die "cannot $*"; }
die() {
	yell "$*"
	exit 111
}
yell() { echo "$0: $*" >&2; }
echo() { printf '%s\n' "$*"; }
isnum() {
	case "${1#[+-]}" in
	*[!0-9]* | '') return 1 ;;
	*) return 0 ;;
	esac
}
dprint() {
	for v in "$@"; do
		decho "$v : $(eval "echo \$$v")"
	done
}
decho() {
	[ "$debug" = '1' ] && echo "$@"
}

debug=0
target_hdr=src/sensors.h
target_mk=sensors.mk

nonsensor='counters_option|sensors|util'

hdr_blacklist=$nonsensor
hdr_whitelist=''

usage() {
	printf -- 'Usage: %s [-la] [-e <sensor>] [-i <sensor>] [-u <sensor>]\n' "$(basename "$0")" >&2
	printf -- '-e | --exclude      :   exclude sensor, can be called multiple times\n' >&2
	printf -- '-i | --include      :   include sensor, can be called multiple times\n' >&2
	printf -- '-l | --list-sensors :   list all sensors and exit\n' >&2
	printf -- '-u | --unique       :   only include the specified sensor\n' >&2
	printf -- '                        if this option is used, any usage of `-e` or `-i` will be ignored\n' >&2
	printf -- '-a | --all          :   include all sensors, meant to be used only by the makefile\n' >&2
	exit 1
}

ls_sensors() {
	try cd src

	[ -z "$hdr_whitelist" ] && hdr_whitelist='.*'
	dprint hdr_blacklist >&2
	dprint hdr_whitelist >&2

	ls -1 *.h |
		grep -xEv "($hdr_blacklist)\.h" |
		grep -xE "($hdr_whitelist)\.h" |
		sed 's/\.h$//'
}

# gen_sensors_h(sensor, nb_sensors)
gen_sensors_h() {
	sensors=$1
	nb_sensors=$2
	nb_sensor_opts=$(
		for sensor in $sensors; do
			sed -n 's/.*'"${sensor}"'_opt\[\([0-9]\+\)\].*/\1/p' "src/${sensor}.h"
		done |
			paste -s -d '+'
	)
	nb_sensor_opts=$(eval "echo \$(($nb_sensor_opts))")

	dprint sensors >&2
	dprint nb_sensor_opts >&2
	isnum "$nb_sensor_opts" || die "could not get total number of sensors's command-line options"

	# gen includes
	for sensor in $sensors; do
		printf '#include "%s.h"\n' "$sensor"
	done
	printf '\n'

	printf '#define NB_SENSOR %d\n' "$nb_sensors"
	printf '#define NB_SENSOR_OPT %d\n' "$nb_sensor_opts"
	printf '\n'

	# gen `init_sensors()`
	printf 'void init_sensors(Optparse *opts, Sensor *sensors, size_t len, size_t offset, int *nb_defined)\n{\n'
	printf '    int opt_idx = offset;\n'
	for sensor in $sensors; do
		cat <<-!
			    for (int i = 0; i < ${sensor}.nb_opt; i++) {
			        opts[opt_idx++] = ${sensor}_opt[i];
			    }
			    sensors[(*nb_defined)++] = ${sensor};
		!
	done
	printf '    assert((offset + *nb_defined) <= len);\n'
	printf '}\n'
}

gen_sensors_mk() {
	sensors=$1
	printf 'CAPTOR_OBJ = '
	for sensor in $sensors; do
		printf '$(OBJ_DIR)/%s.o ' "$sensor"
	done
	printf '\n'
}

detect_caps() {
	[ -r /usr/include/linux/perf_event.h ] && hdr_whitelist=counters
	[ -d /sys/class/infiniband ] && hdr_whitelist="${hdr_whitelist}|infiniband"
	[ -r /proc/stat ] && hdr_whitelist="${hdr_whitelist}|load"

	if [ -r /proc/net/route ]; then
		dev=$(awk 'NR == 2 { print $1 }' /proc/net/route)
		[ -e "/sys/class/net/$dev" ] && hdr_whitelist="${hdr_whitelist}|network"
	fi

	if [ -e /usr/local/cuda/lib64 ] && [ -e /usr/local/cuda/include ]; then
		hdr_whitelist="${hdr_whitelist}|nvidia_gpu"
		NVML_LDFLAGS="-L/usr/local/cuda/lib64 -lnvidia-ml"
		NVML_IFLAGS="-I/usr/local/cuda/include"
	fi

	vendor=$(awk '/vendor_id/ {print $3; exit}' /proc/cpuinfo)
	vendor_lc=$(echo "$vendor" | tr 'A-Z' 'a-z')
	case $vendor_lc in
	*intel*)
		hdr_whitelist="${hdr_whitelist}|rapl"
		;;
	*amd*)
		family=$(awk '/cpu[ \t]*family/ {print $4; exit}' /proc/cpuinfo)
		if isnum "$family"; then
			[ $family -ge 17 ] && hdr_whitelist="${hdr_whitelist}|amd_rapl"
		fi
		;;
	*)
		yell "unsupported processor vendor id: $vendor"
		;;
	esac

	[ $(ls -1 /sys/class/hwmon | wc -l) -gt 0 ] && hdr_whitelist="${hdr_whitelist}|temperature"
}

case $1 in
--all | -a)
	all=1
	;;
esac

[ "$all" ] || detect_caps

[ "$all" ] ||
	while [ "$1" ]; do
		case $1 in
		--include | -i)
			shift
			[ "$1" ] || usage
			hdr_whitelist="${hdr_whitelist}|${1}"
			;;
		--exclude | -e)
			shift
			[ "$1" ] || usage
			hdr_blacklist="${hdr_blacklist}|${1}"
			;;
		--list-sensors | -l)
			ls_sensors
			exit 0
			;;
		--unique | -u)
			shift
			[ "$1" ] || usage
			hdr_whitelist=$1
			;;
		--help | -h)
			usage
			;;
		esac
		shift
	done

sensors=$(ls_sensors)
nb_sensors=$(echo "$sensors" | sed '/^$/d' | wc -l)

if [ "$nb_sensors" -eq 0 ]; then
	printf -- '0 sensors are selected. cannot build.\n' >&2
	exit 1
fi

try gen_sensors_h "$sensors" "$nb_sensors" >"$target_hdr"
try gen_sensors_mk "$sensors" >"$target_mk"
try printf "LDFLAGS += %s\n" "$NVML_LDFLAGS" >>"$target_mk"
try printf "IFLAGS += %s\n" "$NVML_IFLAGS" >>"$target_mk"

printf -- 'Run `make` to build `bin/mojitos`.\n' >&2
printf -- 'The resulting binary will have the %d following sensors:\n' "$nb_sensors" >&2
echo "$sensors" >&2

make clean >/dev/null
