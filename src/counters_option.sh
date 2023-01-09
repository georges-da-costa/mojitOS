#!/bin/bash

# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2018-2021 Georges Da Costa <georges.da-costa@irit.fr>

linux_include="/usr/include/linux/perf_event.h"

string="#include <linux/perf_event.h>

typedef struct counter_option {
  char *name;
  __u32 perf_type;
  __u64 perf_key;
} counter_option;

static counter_option perf_static_info[] = {"
echo "$string"

nb=0

black_list=( "stalled_cycles_frontend" "stalled_cycles_backend" "cache_l1i" "cache_op_write" "cache_result_miss" )

while IFS= read -r line; do
    if [[ $line == *"perf_hw_id"* ]]; then
        mode="PERF_TYPE_HARDWARE"
    elif [[ $line == *"perf_hw_cache_"* ]]; then
        mode="PERF_TYPE_HW_CACHE"
    elif [[ $line == *"perf_sw_id"* ]]; then
        mode="PERF_TYPE_SOFTWARE"
    elif [[ $line == *"PERF_COUNT_"* && $line == *"="* ]]; then
        perf_name=$(echo "$line" | awk '{print $1}')
        short_perf=$(echo "${perf_name:14}" | tr '[:upper:]' '[:lower:]')
        if [[ " ${black_list[@]} " == *" $short_perf "* ]]; then
            continue
        fi
        if [[ $mode == "PERF_TYPE_HW_CACHE" ]]; then
			op_id=0
            for op_id_str in 'r' 'w' 'p'; do
                op_id_names=("PERF_COUNT_HW_CACHE_OP_READ" "PERF_COUNT_HW_CACHE_OP_WRITE" "PERF_COUNT_HW_CACHE_OP_PREFETCH")
				result_id=0
                for result_id_str in 'a' 'm'; do
                    result_id_names=("PERF_COUNT_HW_CACHE_RESULT_ACCESS" "PERF_COUNT_HW_CACHE_RESULT_MISS")

					printf "{ .name = \"%s_%s_%s\", .perf_type = %s, .perf_key = %s | (%s >> 8) | (%s >> 16) },\n" \
							$short_perf                    														   \
							$op_id_str                    													       \
							$result_id_str                  												       \
							$mode                    														       \
							$perf_name                      												       \
							${op_id_names[$op_id]}          												       \
							${result_id_names[$result_id]}  												     

                    nb=$((nb + 1))
					((result_id++))
                done
				((op_id++))
            done
        else
			printf "{ .name = \"%s\", .perf_type = %s, .perf_key = %s},\n" \
					$short_perf											   \
					$mode      									   		   \
					$perf_name  	

            nb=$((nb + 1))
        fi
    fi
done < "$linux_include"

echo '};'

echo "static unsigned int nb_counter_option = $nb ;"
