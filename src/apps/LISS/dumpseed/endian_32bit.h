#ifndef ORDER32_H
#define ORDER32_H

#include <limits.h>
#include <stdint.h>

#if CHAR_BIT != 8
#error "unsupported char size"
#endif

enum
{
    ENDIAN_LITTLE = 0x03020100ul,
    ENDIAN_BIG = 0x00010203ul,
    ENDIAN_PDP = 0x01000302ul
};

static const union { unsigned char bytes[4]; uint32_t value; } host_order =
    { { 0, 1, 2, 3 } };

#define HOST_ORDER (host_order.value)

#endif

#define SwapTwoBytes(data)  ( (((data) >> 8) & 0x00FF) | (((data) << 8) & 0xFF00) )

#define SwapFourBytes(data)  ( (((data) >> 24) & 0x000000FF) | (((data) >> 8) & 0x0000FF00) |  (((data) << 8) & 0x00FF0000) | (((data) << 24) & 0xFF000000) )

#define SwapEightBytes(data)  ( (((data) >> 56) & 0x00000000000000FF) | (((data) >> 40) & 0x000000000000FF00) |  (((data) >> 24) & 0x0000000000FF0000) | (((data) >> 8) & 0x00000000FF000000) | (((data) << 8) & 0x000000FF00000000) | (((data) << 24) & 0x0000FF0000000000) |  (((data) << 40) & 0x00FF000000000000) | (((data) << 56) & 0xFF00000000000000) )

