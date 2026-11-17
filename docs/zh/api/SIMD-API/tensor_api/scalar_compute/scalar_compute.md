# 标量计算

Tensor API提供标量位运算和标量数据读写接口：

- **[clear_nthbit](clear_nthbit.md)**：清除64位无符号整数的指定二进制位。
- **[set_nthbit](set_nthbit.md)**：置位64位无符号整数的指定二进制位。
- **[clz](clz.md)**：统计64位无符号整数的前导零个数。
- **[ffs](ffs.md)**：查找第一个值为1的二进制位。
- **[ffz](ffz.md)**：查找第一个值为0的二进制位。
- **[popc](popc.md)**：统计值为1的二进制位个数。
- **[sflbits](sflbits.md)**：统计与符号位相同的连续比特位个数。
- **[zero_bits_cnt](zero_bits_cnt.md)**：统计值为0的二进制位个数。
- **[load_dev](load_dev.md)**：不经过DCache从Global Memory读取整型数据。
- **[store_dev](store_dev.md)**：不经过DCache向Global Memory写入整型数据。
