#include "util.h"

inline uint64_t substractAcc(const uint64_t l, const uint64_t r)
{
    return (l < r) ? UINT64_MAX + (l - r) : l - r;
}