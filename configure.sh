#!/bin/sh

try() { "$@" || die "cannot $*"; }
die() { yell "$*"; exit 111; }
yell() { echo "$0: $*" >&2; }
echo() { printf '%s\n' "$*"; }

target=src/captors.h
hdr_blacklist='counters_option|optparse|captors'

usage() {
	printf -- 'Usage: %s [-l] [-e <captor>] [-i <captor>]\n' "$(basename "$0")" >&2
	printf -- '-e | --exclude      :   exclude captor, can be called multiple times\n' >&2
	printf -- '-i | --include      :   include captor, can be called multiple times\n' >&2
	printf -- '-l | --list-captors :   list all captors and exit\n' >&2
	exit 1
}

ls_captors() {
	try cd src
	ls *.h | grep -vE "($hdr_blacklist)" | sed 's/\.h$//'
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
		# no-op for now
		:
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

gen_captors_h > "$target"
