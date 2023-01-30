#!/bin/sh

try() { "$@" || die "cannot $*"; }
die() { yell "$*"; exit 111; }
yell() { echo "$0: $*" >&2; }
echo() { printf '%s\n' "$*"; }
isnum() {
	case "${1#[+-]}" in
	*[!0-9]*|'') return 1 ;;
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
target=src/captors.h

noncaptor='counters_option|optparse|captors|util|info_reader'

hdr_blacklist=$noncaptor
hdr_whitelist=''

usage() {
	printf -- 'Usage: %s [-l] [-e <captor>] [-i <captor>] [-u <captor>]\n' "$(basename "$0")" >&2
	printf -- '-e | --exclude      :   exclude captor, can be called multiple times\n' >&2
	printf -- '-i | --include      :   include captor, can be called multiple times\n' >&2
	printf -- '-l | --list-captors :   list all captors and exit\n' >&2
	printf -- '-u | --unique       :   only include the specified captor\n' >&2
	printf -- '                        if this option is used, any usage of `-e` or `-i` will be ignored\n' >&2
	exit 1
}

ls_captors() {
	try cd src

	[ -z "$hdr_whitelist" ] && hdr_whitelist='.*'
	dprint hdr_blacklist
	dprint hdr_whitelist

	ls -1 *.h |
		grep -xEv "($hdr_blacklist)\.h" |
		grep -xE  "($hdr_whitelist)\.h" |
		sed 's/\.h$//'
}

gen_captors_h() {
	captors=$(ls_captors)
	nb_captors=$(echo "$captors" | sed '/^$/d' | wc -l)
	printf -- 'Run `make` to build `bin/mojitos`.\n' >&2
	printf -- 'The resulting binary will have the %d following captors:\n' "$nb_captors" >&2
	echo "$captors" >&2

	[ -n "$captors" ] || return

	# gen includes
	for captor in $captors; do
		printf '#include "%s.h"\n' "$captor"
	done
	printf '\n#define NB_CAPTORS %d\n\n' "$nb_captors"

	# gen `init_captors()`
	printf 'void init_captors(struct optparse_long *longopts, struct captor *captors, size_t len, size_t offset, int *nb_defined)\n{\n'
	for captor in $captors; do
		printf '    longopts[offset + *nb_defined] = %s_opt;\n' "$captor"
		printf '    captors[(*nb_defined)++] = %s;\n' "$captor"
	done
	printf '    assert((offset + *nb_defined) <= len);\n'
	printf '}\n'
}

detect_caps() {
	[ -r /usr/include/linux/perf_event.h ] && hdr_whitelist=counters
	[ -d /sys/class/infiniband ] && hdr_whitelist=${hdr_whitelist}|infiniband
	[ -r /proc/stat ] && hdr_whitelist="${hdr_whitelist}|load"

	if [ -r /proc/net/route ]; then
		dev=$(awk 'NR == 2 { print $1 }' /proc/net/route)
		[ -e "/sys/class/net/$dev" ] && hdr_whitelist="${hdr_whitelist}|network"
	fi

	vendor=$(awk '/vendor_id/ {print $3; exit}' /proc/cpuinfo)
	vendor_lc=$(echo "$vendor" | tr 'A-Z' 'a-z')
	case $vendor_lc in
	*intel*)
		hdr_whitelist="${hdr_whitelist}|rapl"
		;;
	*amd*)
		family=$(awk '/cpu[ \t]*family/ {print $3; exit}' /proc/cpuinfo)
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

detect_caps

while [ "$1" ]; do
	case $1 in
	--include|-i)
		shift; [ "$1" ] || usage
		hdr_whitelist="${hdr_whitelist}|${1}"
		;;
	--exclude|-e)
		shift; [ "$1" ] || usage
		hdr_blacklist="${hdr_blacklist}|${1}"
		;;
	--list-captors|-l)
		ls_captors
		exit 0
		;;
	--unique|-u)
		shift; [ "$1" ] || usage
		hdr_whitelist=$1
		;;
	--help|-h)
		usage
		;;
	esac
	shift
done

gen_captors_h > "$target"
