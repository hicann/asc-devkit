# Reg矢量计算接口软仿情况汇总

本节汇总介绍了Reg矢量计算接口软仿数据类型的情况，本节内容仅针对如下型号生效：

Ascend 950PR/Ascend 950DT

[Reg数据搬入](../SIMD-API/basic_api/reg_vector_compute/reg_data_load/reg_data_load.md)和[Reg数据搬出](../SIMD-API/basic_api/reg_vector_compute/reg_data_store/reg_data_store.md)目录下提供了Unified Buffer（UB）与RegTensor、UB与MaskReg之间的数据搬运接口。

UB与RegTensor之间的数据搬运根据操作数的数据类型位宽来搬运，与数据类型本身无关，表1和表2列出的软仿支持的数据类型包括以下情况：

- 底层硬件不支持该数据类型，通过其他位宽相同的数据类型支持的搬运指令实现。
- 底层硬件不支持该数据类型，通过多种接口组合软仿实现。
- 底层硬件不支持该数据类型，通过双搬入/双搬出模式软仿实现RegTraitNumTwo的数据类型。

**表1**  Reg数据搬入

| 接口 | 软仿支持数据类型 |
| :---------- | :---------------- |
| 连续对齐搬入（LoadAlign） | bool、complex32、int64_t、uint64_t、complex64 |
| 非连续对齐搬入（LoadAlign） | bool、complex32 |
| 连续非对齐搬入（LoadUnAlign） | bool、complex32、int64_t、uint64_t、complex64 |
| MaskReg搬入（LoadAlign） | - |
| MaskReg搬入（MaskGenWithRegTensor） | - |
| 离散搬入（Gather） | int64_t、uint64_t |
| 离散搬入（GatherB） | - |
| Load | bool、complex32、int64_t、uint64_t、complex64 |
| Move | bool、int64_t、uint64_t、complex64|

**表2**  Reg数据搬出

| 接口 | 软仿支持数据类型 |
| :---------- | :---------------- |
| 连续对齐搬出（StoreAlign） | bool、complex32、int64_t、uint64_t、complex64 |
| 非连续对齐搬出（StoreAlign） | bool、complex32 |
| 连续非对齐搬出（StoreUnAlign） | bool、complex32、int64_t、uint64_t、complex64 |
| MaskReg搬出（StoreAlign） | - |
| MaskReg非对齐搬出（StoreUnAlign） | - |
| 离散搬出（Scatter） | int64_t、uint64_t |
| Store | bool、complex32、int64_t、uint64_t、complex64 |

**表3**  基础算术

| 接口 | 软仿支持数据类型 |
| :--- | :--------------- |
| Exp | half（SubNormal）、float（SubNormal） |
| Sqrt | half（SubNormal）、float（SubNormal） |
| Add | complex32、int64_t、uint64_t、complex64 |
| AddC | - |
| Sub | complex32、int64_t、uint64_t、complex64 |
| SubC | - |
| Adds | complex32、int64_t、uint64_t、complex64 |
| Max | int64_t、uint64_t |
| Min | int64_t、uint64_t |
| Maxs | int64_t、uint64_t |
| Mins | int64_t、uint64_t |
| Div | int16_t、uint16_t、half（SubNormal）、int32_t、uint32_t、float（SubNormal）、complex32、int64_t、uint64_t、complex64 |
| Neg | int64_t |
| Ln | half（SubNormal）、float（SubNormal） |
| Log | half（SubNormal）、float（SubNormal） |
| Log2 | half、float |
| Log10 | half、float |
| Mul | complex32、int64_t、uint64_t、complex64 |
| Mull | - |
| Muls | complex32、int64_t、uint64_t、complex64 |
| Abs | complex32、int64_t、complex64 |
| Relu | int64_t |
| Prelu | - |
| LeakyRelu | - |

**表4**  逻辑计算

| 接口        | 软仿支持数据类型  |
| :---------- | :---------------- |
| Not         | int64_t、uint64_t |
| And         | int64_t、uint64_t |
| Or          | int64_t、uint64_t |
| Xor         | int64_t、uint64_t |
| ShiftLeft   | int64_t、uint64_t |
| ShiftRight  | int64_t、uint64_t |
| ShiftLefts  | int64_t、uint64_t |
| ShiftRights | int64_t、uint64_t |

**表5**  复合计算

| 接口      | 软仿支持数据类型  |
| :-------- | :---------------- |
| AbsSub    | int64_t           |
| ExpSub    | -                 |
| Axpy      | int64_t、uint64_t |
| MulDstAdd | -                 |
| MulAddDst | int64_t、uint64_t |
| MulsCast  | -                 |

**表6**  比较与选择

| 接口     | 软仿支持数据类型        |
| :------- | :---------------------- |
| Compare  | -                       |
| Compares | -                       |
| Select   | bool、int64_t、uint64_t |
| Squeeze  | -                       |

**表7**  类型转换

| 接口     | 软仿支持数据类型     |
| :------- | :------------------- |
| Cast     | e8m02bf16、bf162e8m0 |
| Truncate | -                    |

**表8**  归约计算

| 接口            | 软仿支持数据类型  |
| :-------------- | :---------------- |
| Reduce          | int64_t、uint64_t |
| ReduceDataBlock | -                 |
| PairReduceElem  | -                 |

**表9**  数据填充

| 接口 | 软仿支持数据类型 |
| :--- | :--------------- |
| Duplicate | bool、fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、complex32、int64_t、uint64_t、complex64 |

**表10**  离散操作

| 接口   | 软仿支持数据类型 |
| :----- | :--------------- |
| Gather | -                |

**表11**  数据重排

| 接口         | 软仿支持数据类型  |
| :----------- | :---------------- |
| Interleave   | int64_t、uint64_t |
| DeInterleave | int64_t、uint64_t |

**表12**  数据压缩

| 接口      | 软仿支持数据类型 |
| :-------- | :--------------- |
| Unsqueeze | -                |
| Pack      | -                |
| UnPack    | -                |

**表13**  直方图计算

| 接口       | 软仿支持数据类型 |
| :--------- | :--------------- |
| Histograms | -                |

**表14**  索引操作

| 接口   | 软仿支持数据类型 |
| :----- | :--------------- |
| Arange | int64_t          |

**表15**  同步控制

| 接口        | 软仿支持数据类型 |
| :---------- | :--------------- |
| LocalMemBar | -                |

**表16**  系统变量访问

| 接口     | 软仿支持数据类型 |
| :------- | :--------------- |
| ClearSpr | -                |
| GetSpr   | -                |
