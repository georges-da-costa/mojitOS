#!/bin/sh

# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2018-2023 Georges Da Costa <georges.da-costa@irit.fr>

linux_include=/usr/include/linux/perf_event.h

echo '
typedef struct counter_option {
    char *name;
    __u32 perf_type;
    __u64 perf_key;
} counter_option;

static counter_option perf_static_info[] = {'

nb=0

while IFS= read line; do
	case $line in
	*perf_hw_id*)
        mode=PERF_TYPE_HARDWARE
		;;
    *perf_hw_cache_*)
        mode=PERF_TYPE_HW_CACHE
		;;
    *perf_sw_id*)
        mode=PERF_TYPE_SOFTWARE
		;;
    *PERF_COUNT_*=*)
		perf_name=$(echo "$line" | awk '{print $1}')
		short_perf=$(echo "$perf_name" | sed 's/PERF_COUNT_[HS]W_//' | tr 'A-Z' 'a-z')
		case $short_perf in
		# blacklist
		stalled_cycles_frontend|stalled_cycles_backend|cache_l1i|cache_op_write|cache_result_miss)
			continue
			;;
		esac

		if [ "$mode" != 'PERF_TYPE_HW_CACHE' ]; then
			printf '    { .name = "%s", .perf_type = %s, .perf_key = %s},\n' \
					"$short_perf" \
					"$mode" \
					"$perf_name"

            : $((nb += 1))
			continue
		fi

		# $mode == PERF_TYPE_HW_CACHE
        for op_id in \
        	'r PERF_COUNT_HW_CACHE_OP_READ' \
        	'w PERF_COUNT_HW_CACHE_OP_WRITE' \
        	'p PERF_COUNT_HW_CACHE_OP_PREFETCH'
        do
            op_id_str=${op_id% *}
            op_id_name=${op_id#* }

            for result_id in \
            	'a PERF_COUNT_HW_CACHE_RESULT_ACCESS' \
            	'm PERF_COUNT_HW_CACHE_RESULT_MISS'
            do
                result_id_str=${result_id% *}
                result_id_name=${result_id#* }

            	printf '    {'
            	printf ' .name = "%s_%s_%s",' \
            		"$short_perf" \
            		"$op_id_str" \
            		"$result_id_str"
            	printf ' .perf_type = %s,' \
            		"$mode"
            	printf ' .perf_key = %s | (%s >> 8) | (%s >> 16)' \
            		"$perf_name" \
            		"$op_id_name" \
            		"$result_id_name"
            	printf ' },\n'
                : $((nb += 1))
            done
        done
		;;
	esac
done < "$linux_include"

echo '};'

printf '\nstatic unsigned int nb_counter_option = %d;\n' "$nb"

