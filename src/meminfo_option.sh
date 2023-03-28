#!/bin/sh

# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2018-2023 Georges Da Costa <georges.da-costa@irit.fr>

MEMINFO_PATH="/proc/meminfo"

FUNCTION_TEMPLATE="void set_result_offset%s(GenericPointer ptr, GenericPointer data)
{
    uint64_t* result = (uint64_t *) ptr;
    result[%s] = (uint64_t) data;
}\n\n"

nb_counters=0

echo '#include <inttypes.h>'
echo "#include <info_reader.h>"
echo

echo 'static char *memory_counters[] = {'
while read line; do
  nb_counters=$(expr $nb_counters + 1)
  word=$(echo $line | awk '{print $1}')
  echo "    \"${word::-1}\","
done <$MEMINFO_PATH
echo "};"
echo

echo "#define NB_COUNTERS $nb_counters"
echo

count=0
while [ $count -lt $nb_counters ]; do
  printf "$FUNCTION_TEMPLATE" $count $count
  count=$(expr $count + 1)
done

echo "static void (*setter_functions[])(GenericPointer, GenericPointer) = {"
count=0
while [ $count -lt $nb_counters ]; do
  echo "    set_result_offset$count,"
  count=$(expr $count + 1)
done
echo "};"
echo

