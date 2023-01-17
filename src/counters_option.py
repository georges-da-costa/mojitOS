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
            if mode == 'PERF_TYPE_HW_CACHE':
                for op_id, op_id_str in enumerate(['r', 'w', 'p']):
                    op_id_names = ['PERF_COUNT_HW_CACHE_OP_READ', 'PERF_COUNT_HW_CACHE_OP_WRITE', 'PERF_COUNT_HW_CACHE_OP_PREFETCH']
                    for result_id, result_id_str in enumerate(['a', 'm']):
                        result_id_names = ['PERF_COUNT_HW_CACHE_RESULT_ACCESS', 'PERF_COUNT_HW_CACHE_RESULT_MISS']

                        res = '{ .name = "%s_%s_%s", .perf_type = %s, .perf_key = %s | (%s >> 8) | (%s >> 16) },' % (
                            short_perf, op_id_str, result_id_str,
                            mode,
                            perf_name,
                            op_id_names[op_id],
                            result_id_names[result_id])
                                                                                     
                        print(res)
                        nb += 1

            else:
                res = '{ .name = "'+short_perf+'", .perf_type = '+mode+', .perf_key = '+perf_name+'},'
                print(res)
                nb += 1


print('};')

print('static unsigned int nb_counter_option =',nb,';')
