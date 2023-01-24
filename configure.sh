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

target=src/captors.h

noncaptor='counters_option|optparse|captors'

hdr_blacklist=$noncaptor
hdr_whitelist=''

usage() {
	printf -- 'Usage: %s [-l] [-e <captor>] [-i <captor>]\n' "$(basename "$0")" >&2
	printf -- '-e | --exclude      :   exclude captor, can be called multiple times\n' >&2
	printf -- '-i | --include      :   include captor, can be called multiple times\n' >&2
	printf -- '-l | --list-captors :   list all captors and exit\n' >&2
	exit 1
}

ls_captors() {
	try cd src
	printf -- 'captors:\n' >&2
	ls -1 *.h |
		grep -vE "^($hdr_blacklist)\.h$" |
		grep -E  "^($hdr_whitelist)\.h$" |
		sed 's/\.h$//' |
		tee /dev/stderr
}

gen_captors_h() {
	captors=$(ls_captors)

	[ -n "$captors" ] || return

	# gen includes
	for captor in $captors; do
		printf '#include "%s.h"\n' "$captor"
	done
	printf '\n'

	# gen `init_captors()`
	printf 'void init_captors()\n{\n'
	for captor in $captors; do
		printf '    longopts[NB_MAX_OPTS + nb_defined_captors] = %s_opt;\n' "$captor"
		printf '    captors[nb_defined_captors++] = %s;\n' "$captor"
	done
	printf '}\n'
}

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
	--help|-h)
		usage
		;;
	esac
	shift
done

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

gen_captors_h > "$target"
