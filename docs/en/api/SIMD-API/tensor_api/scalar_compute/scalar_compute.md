# Scalar Computation

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T03:10:29.023Z pushedAt=2026-11-19T05:54:06.687Z nl=2 -->

The Tensor API provides scalar bitwise operation and scalar data read/write APIs:

- **[clear_nthbit](clear_nthbit.md)**: Clears the specified binary bit of a 64-bit unsigned integer.
- **[set_nthbit](set_nthbit.md)**: Sets the specified binary bit of a 64-bit unsigned integer.
- **[clz](clz.md)**: Counts the number of leading zeros in a 64-bit unsigned integer.
- **[ffs](ffs.md)**: Finds the first binary bit with a value of 1.
- **[ffz](ffz.md)**: Finds the first binary bit with a value of 0.
- **[popc](popc.md)**: Counts the number of binary bits with a value of 1.
- **[sflbits](sflbits.md)**: Counts the number of consecutive bits that are the same as the sign bit.
- **[zero_bits_cnt](zero_bits_cnt.md)**: Counts the number of binary bits with a value of 0.
- **[load_dev](load_dev.md)**: Reads integer data from Global Memory without going through DCache.
- **[store_dev](store_dev.md)**: Writes integer data to Global Memory without going through DCache.
