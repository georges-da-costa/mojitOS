#include "inttypes.h"
#include <info_reader.h>
char *meminfo_words[] = {
    "MemTotal",
    "MemFree",
    "MemAvailable",
    "Buffers",
    "Cached",
    "SwapCached",
    "Active",
    "Inactive",
    "Active(anon)",
    "Inactive(anon)",
    "Active(file)",
    "Inactive(file)",
    "Unevictable",
    "Mlocked",
    "SwapTotal",
    "SwapFree",
    "Zswap",
    "Zswapped",
    "Dirty",
    "Writeback",
    "AnonPages",
    "Mapped",
    "Shmem",
    "KReclaimable",
    "Slab",
    "SReclaimable",
    "SUnreclaim",
    "KernelStack",
    "PageTables",
    "SecPageTables",
    "NFS_Unstable",
    "Bounce",
    "WritebackTmp",
    "CommitLimit",
    "Committed_AS",
    "VmallocTotal",
    "VmallocUsed",
    "VmallocChunk",
    "Percpu",
    "HardwareCorrupted",
    "AnonHugePages",
    "ShmemHugePages",
    "ShmemPmdMapped",
    "FileHugePages",
    "FilePmdMapped",
    "CmaTotal",
    "CmaFree",
    "HugePages_Total",
    "HugePages_Free",
    "HugePages_Rsvd",
    "HugePages_Surp",
    "Hugepagesize",
    "Hugetlb",
    "DirectMap4k",
    "DirectMap2M",
    "DirectMap1G",
};

static const unsigned int meminfo_count = 56;

void set_result_offset0(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[0] = (uint64_t) data;
}

void set_result_offset1(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[1] = (uint64_t) data;
}

void set_result_offset2(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[2] = (uint64_t) data;
}

void set_result_offset3(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[3] = (uint64_t) data;
}

void set_result_offset4(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[4] = (uint64_t) data;
}

void set_result_offset5(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[5] = (uint64_t) data;
}

void set_result_offset6(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[6] = (uint64_t) data;
}

void set_result_offset7(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[7] = (uint64_t) data;
}

void set_result_offset8(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[8] = (uint64_t) data;
}

void set_result_offset9(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[9] = (uint64_t) data;
}

void set_result_offset10(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[10] = (uint64_t) data;
}

void set_result_offset11(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[11] = (uint64_t) data;
}

void set_result_offset12(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[12] = (uint64_t) data;
}

void set_result_offset13(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[13] = (uint64_t) data;
}

void set_result_offset14(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[14] = (uint64_t) data;
}

void set_result_offset15(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[15] = (uint64_t) data;
}

void set_result_offset16(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[16] = (uint64_t) data;
}

void set_result_offset17(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[17] = (uint64_t) data;
}

void set_result_offset18(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[18] = (uint64_t) data;
}

void set_result_offset19(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[19] = (uint64_t) data;
}

void set_result_offset20(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[20] = (uint64_t) data;
}

void set_result_offset21(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[21] = (uint64_t) data;
}

void set_result_offset22(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[22] = (uint64_t) data;
}

void set_result_offset23(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[23] = (uint64_t) data;
}

void set_result_offset24(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[24] = (uint64_t) data;
}

void set_result_offset25(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[25] = (uint64_t) data;
}

void set_result_offset26(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[26] = (uint64_t) data;
}

void set_result_offset27(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[27] = (uint64_t) data;
}

void set_result_offset28(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[28] = (uint64_t) data;
}

void set_result_offset29(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[29] = (uint64_t) data;
}

void set_result_offset30(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[30] = (uint64_t) data;
}

void set_result_offset31(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[31] = (uint64_t) data;
}

void set_result_offset32(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[32] = (uint64_t) data;
}

void set_result_offset33(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[33] = (uint64_t) data;
}

void set_result_offset34(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[34] = (uint64_t) data;
}

void set_result_offset35(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[35] = (uint64_t) data;
}

void set_result_offset36(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[36] = (uint64_t) data;
}

void set_result_offset37(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[37] = (uint64_t) data;
}

void set_result_offset38(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[38] = (uint64_t) data;
}

void set_result_offset39(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[39] = (uint64_t) data;
}

void set_result_offset40(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[40] = (uint64_t) data;
}

void set_result_offset41(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[41] = (uint64_t) data;
}

void set_result_offset42(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[42] = (uint64_t) data;
}

void set_result_offset43(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[43] = (uint64_t) data;
}

void set_result_offset44(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[44] = (uint64_t) data;
}

void set_result_offset45(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[45] = (uint64_t) data;
}

void set_result_offset46(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[46] = (uint64_t) data;
}

void set_result_offset47(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[47] = (uint64_t) data;
}

void set_result_offset48(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[48] = (uint64_t) data;
}

void set_result_offset49(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[49] = (uint64_t) data;
}

void set_result_offset50(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[50] = (uint64_t) data;
}

void set_result_offset51(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[51] = (uint64_t) data;
}

void set_result_offset52(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[52] = (uint64_t) data;
}

void set_result_offset53(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[53] = (uint64_t) data;
}

void set_result_offset54(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[54] = (uint64_t) data;
}

void set_result_offset55(GenericPointer ptr, GenericPointer data) {
    uint64_t* result = (uint64_t *) ptr;
    result[55] = (uint64_t) data;
}

static void (*setter_functions[])(GenericPointer, GenericPointer) = {
    set_result_offset0,
    set_result_offset1,
    set_result_offset2,
    set_result_offset3,
    set_result_offset4,
    set_result_offset5,
    set_result_offset6,
    set_result_offset7,
    set_result_offset8,
    set_result_offset9,
    set_result_offset10,
    set_result_offset11,
    set_result_offset12,
    set_result_offset13,
    set_result_offset14,
    set_result_offset15,
    set_result_offset16,
    set_result_offset17,
    set_result_offset18,
    set_result_offset19,
    set_result_offset20,
    set_result_offset21,
    set_result_offset22,
    set_result_offset23,
    set_result_offset24,
    set_result_offset25,
    set_result_offset26,
    set_result_offset27,
    set_result_offset28,
    set_result_offset29,
    set_result_offset30,
    set_result_offset31,
    set_result_offset32,
    set_result_offset33,
    set_result_offset34,
    set_result_offset35,
    set_result_offset36,
    set_result_offset37,
    set_result_offset38,
    set_result_offset39,
    set_result_offset40,
    set_result_offset41,
    set_result_offset42,
    set_result_offset43,
    set_result_offset44,
    set_result_offset45,
    set_result_offset46,
    set_result_offset47,
    set_result_offset48,
    set_result_offset49,
    set_result_offset50,
    set_result_offset51,
    set_result_offset52,
    set_result_offset53,
    set_result_offset54,
    set_result_offset55,
};

