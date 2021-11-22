#! /usr/bin/python3

# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2018-2021 Georges Da Costa <georges.da-costa@irit.fr>

linux_include = '/usr/include/linux/perf_event.h'

string = """#include <linux/perf_event.h>

typedef struct counter_option {
  char *name;
  __u32 perf_type;
  __u64 perf_key;
} counter_option;

static counter_option perf_static_info[] = {"""
print(string)

nb = 0

black_list = ['stalled_cycles_frontend','stalled_cycles_backend',
              'cache_l1i', 'cache_op_write', 'cache_result_miss']

with open(linux_include, 'r') as infile:
    mode = ''
    for line in infile:
        if 'perf_hw_id' in line:
            mode = 'PERF_TYPE_HARDWARE'
        elif 'perf_hw_cache_' in line:
            mode = 'PERF_TYPE_HW_CACHE'
        elif 'perf_sw_id' in line:
            mode = 'PERF_TYPE_SOFTWARE'
        elif 'PERF_COUNT_' in line and '=' in line:
            perf_name = line.split()[0]
            short_perf = perf_name[14:].lower()
            if short_perf in black_list:
                continue
            res = '{ .name = "'+short_perf+'", .perf_type = '+mode+', .perf_key = '+perf_name+'},'
            print(res)
            nb += 1


print('};')

print('static unsigned int nb_counter_option =',nb,';')
