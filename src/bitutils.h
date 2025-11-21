#ifndef TLBT_BITUTILS_H
#define TLBT_BITUTILS_H

#define TLBT_CLEAR_BIT(value, bit) ((value) & ~(1 << (bit)))
#define TLBT_SET_BIT(value, bit) ((value) | (1 << (bit)))
#define TLBT_TOGGLE_BIT(value, bit) ((value) ^ (1 << (bit)))
#define TLBT_CHECK_BIT(value, bit) (((value) & (1 << (bit))) == (1 << (bit)))
#define TLBT_COPY_BIT(dest, src, bit) (TLBT_CLEAR_BIT(dest, bit) | ((src) & (1 << (bit))))

#endif

