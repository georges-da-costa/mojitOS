#!/bin/sh

# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2023-2023 Georges Da Costa <georges.da-costa@irit.fr>

die() { yell "$*"; exit 111; }
try() { "$@" || die "cannot $*"; }
yell() { echo "$0: $*" >&2; }
echo() { printf '%s\n' "$*"; }

try ./configure.sh --all
try make mojitos
usage=$(
	./bin/mojitos |
		awk '
			/^SENSORS/ {
				$0 = ""
				printf "```\n"
				printf "\n"
				printf "The following is an exhaustive list of all the sensors (it is very likely\n"
				printf "that one will not have all the sensors activated in his build):\n"
				printf "```bash\n"
				printf "SENSORS:"
			}
			{ print }
		'
)
[ -n "$usage" ] || die 'empty usage. cannot continue.'

try awk -v "usage=$usage" '
	/^Usage/ {
		print usage
		del = 1
	}
	{
		if (del == 1 || del == 2) {
			if (match($0, "^```")) {
				del++
			}
		} else if (del == 3) {
			if (match($0, "^```")) {
				del = 0
				print $0
			}
		} else {
			print $0
		}
	}
' README.md > README.tmp
try mv README.tmp README.md
