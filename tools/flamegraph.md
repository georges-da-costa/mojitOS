```bash
export MOJITOS=Path_to_mojitos_binary

git clone https://github.com/brendangregg/FlameGraph.git

sudo perf record -F999 -g --call-graph dwarf ${MOJITOS} -o /dev/shm/data -d X -c -m -u -r -M Active,Writeback,PageTables -f 100 -t 10

sudo perf script | ./FlameGraph/stackcollapse-perf.pl | ./FlameGraph/flamegraph.pl > out.svg

sudo perf record -F999 -g --call-graph dwarf ${MOJITOS} -o /dev/shm/data -d X -c -m -u -r -M Active,Writeback,PageTables -p instructions,branch_misses,cache_references,context_switches -f 100 -t 10

sudo perf script | ./FlameGraph/stackcollapse-perf.pl | ./FlameGraph/flamegraph.pl > out_perf.svg

sudo perf record -F999 -g --call-graph dwarf ${MOJITOS} -o /dev/shm/data -d X -c -m -u -r -M Active,Writeback,PageTables -p instructions,branch_misses,cache_references,context_switches -k FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE:PMC0,FP_ARITH_INST_RETIRED_SCALAR_DOUBLE:PMC1 -f 100 -t 10

sudo perf script | ./FlameGraph/stackcollapse-perf.pl | ./FlameGraph/flamegraph.pl > out_lik.svg

chromium out.svg
```
