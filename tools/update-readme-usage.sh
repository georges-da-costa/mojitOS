#!/bin/sh

# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2023-2023 Georges Da Costa <georges.da-costa@irit.fr>

die() { yell "$*"; exit 111; }
try() { "$@" || die "cannot $*"; }
yell() { echo "$0: $*" >&2; }
echo() { printf '%s\n' "$*"; }

usage=$(./bin/mojitos)
[ -n "$usage" ] || die 'empty usage. try to recompile mojitos.'

try awk -v "usage=$usage" '
	/^Usage/ {
		print usage
		del = 1
	}
	{
		if (del == 1) {
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
