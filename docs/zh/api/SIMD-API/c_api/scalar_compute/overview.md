# 概述

## API列表

### 位运算

**表1** 位运算接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_clear_nthbit](scalar_bit/asc_clear_nthbit.md) | 将64位无符号整数的指定比特位清零，其余位保持不变。 | `c_api/scalar_compute/compute/scalar_bit.h` |
| [asc_clz](scalar_bit/asc_clz.md) | 统计64位无符号整数二进制表示中的前导零个数。 | `c_api/scalar_compute/compute/scalar_bit.h` |
| [asc_ffs](scalar_bit/asc_ffs.md) | 从最低位开始查找第一个值为1的比特，返回其位索引；未找到时返回-1。 | `c_api/scalar_compute/compute/scalar_bit.h` |
| [asc_ffz](scalar_bit/asc_ffz.md) | 从最低位开始查找第一个值为0的比特，返回其位索引；未找到时返回-1。 | `c_api/scalar_compute/compute/scalar_bit.h` |
| [asc_popc](scalar_bit/asc_popc.md) | 统计64位无符号整数二进制表示中值为1的比特个数。 | `c_api/scalar_compute/compute/scalar_bit.h` |
| [asc_set_nthbit](scalar_bit/asc_set_nthbit.md) | 将64位无符号整数的指定比特位置为1，其余位保持不变。 | `c_api/scalar_compute/compute/scalar_bit.h` |
| [asc_sflbits](scalar_bit/asc_sflbits.md) | 从次高位开始，统计与符号位相同的连续比特个数；输入全0或全1时返回-1。 | `c_api/scalar_compute/compute/scalar_bit.h` |
| [asc_zero_bits_cnt](scalar_bit/asc_zero_bits_cnt.md) | 统计64位无符号整数二进制表示中值为0的比特个数。 | `c_api/scalar_compute/compute/scalar_bit.h` |

### 类型转换

**表2** 类型转换接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_float2half](scalar_convert/asc_float2half.md) | 将`float`类型的标量转换为`half`类型，舍入模式固定为ODD（最近邻奇数舍入）。 | `c_api/scalar_compute/compute/scalar_convert.h` |
| [asc_float2int32](scalar_convert/asc_float2int32.md) | 将`float`类型的标量转换为`int32_t`类型。 | `c_api/scalar_compute/compute/scalar_convert.h` |

### 数据搬入

**表3** 数据搬入接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_load_dev](scalar_load/asc_load_dev.md) | 绕过DCache，直接从GM地址读取整型数据。 | `c_api/scalar_compute/scalar_load.h` |

### 数据搬出

**表4** 数据搬出接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_store_dev](scalar_store/asc_store_dev.md) | 绕过DCache，直接向GM地址写入数据。 | `c_api/scalar_compute/scalar_store.h` |
