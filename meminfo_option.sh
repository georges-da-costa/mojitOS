#!bin/sh

MEMINFO="/proc/meminfo"
COUNT=0

function get_meminfo_words {
	printf "char *meminfo_words[] = {\n"
	while read line; do
		COUNT=$(expr $COUNT + 1)
		word=$(echo $line | awk '{print $1}')
		printf "    \"${word::-1}\",\n"
	done <$MEMINFO

	printf "};\n\n"
}

function get_count {
	printf "static const unsigned int meminfo_count = $COUNT;\n\n"
}

function get_meminfo_setter {
	count=0
	while [ $count -lt $COUNT ]; do
		printf "void set_result_offset$count(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[$count] = (uint64_t) data;
}\n\n"
		count=$(expr $count + 1)
	done
}

function get_functions {
	printf "static void (*setter_functions[])(GenericPointer, GenericPointer) = {\n"
	count=0
	while [ $count -lt $COUNT ]; do
		printf "    set_result_offset$count,\n"
		count=$(expr $count + 1)
	done
	printf "};\n\n"
}

DEST="./src/meminfo_option.h"
printf "#include \"inttypes.h\"\n" >$DEST
printf "#include <info_reader.h>\n\n" >>$DEST
get_meminfo_words "./text.h" >>$DEST
get_count >>$DEST
get_meminfo_setter >>$DEST
get_functions >>$DEST
