# Reg Vector Compute API Soft Simulation Summary

<!-- md-trans-meta sourceCommit=aff19f76633e6edc0b718e456c0601a03d2f7983 translatedAt=2026-11-17T03:38:46.695Z pushedAt=2026-11-17T09:52:41.767Z nl=2 -->

This section summarizes the soft simulation data types of Reg vector compute APIs. The content in this section applies only to the following models:

Ascend 950PR & 950DT products

The [Reg Data Load](../SIMD-API/basic_api/reg_vector_compute/reg_data_load/reg_data_load.md) and [Reg Data Store](../SIMD-API/basic_api/reg_vector_compute/reg_data_store/reg_data_store.md) directories provide data movement APIs between Unified Buffer (UB) and RegTensor, and between UB and MaskReg.

Data movement between UB and RegTensor is performed based on the bit width of the operand data type, independent of the data type itself. The soft simulation supported data types listed in Table 1 and Table 2 include the following cases:

- The underlying hardware does not support the data type, and it is implemented through move instructions supported by other data types with the same bit width.
- The underlying hardware does not support the data type, and it is implemented through soft simulation by combining multiple APIs.
- The underlying hardware does not support the data type, and the RegTraitNumTwo data type is implemented through soft simulation in dual move-in/dual move-out mode.

**Table 1**  Reg Data Load

| API | Soft Simulation Supported Data Types |
| :---------- | :---------------- |
| Contiguous aligned load (LoadAlign) | bool, complex32, int64_t, uint64_t, complex64 |
| Non-contiguous aligned load (LoadAlign) | bool, complex32 |
| Contiguous misaligned move in (LoadUnalign) | bool, complex32, int64_t, uint64_t, complex64 |
| MaskReg load (LoadAlign) | - |
| MaskReg load (MaskGenWithRegTensor) | - |
| Scatter load (Gather) | int64_t, uint64_t |
| Scatter load (GatherB) | - |
| Load | bool, complex32, int64_t, uint64_t, complex64 |
| Move | bool, int64_t, uint64_t, complex64 |

**Table 2**  Reg Data Store

| API | Soft Simulation Supported Data Types |
| :---------- | :---------------- |
| Contiguous aligned store (StoreAlign) | bool, complex32, int64_t, uint64_t, complex64 |
| Non-contiguous aligned store (StoreAlign) | bool, complex32 |
| Contiguous misaligned move out (StoreUnalign) | bool, complex32, int64_t, uint64_t, complex64 |
| MaskReg store (StoreAlign) | - |
| MaskReg misaligned move out (StoreUnalign) | - |
| Scatter store (Scatter) | int64_t, uint64_t |
| Store | bool, complex32, int64_t, uint64_t, complex64 |

**Table 3**  Basic Arithmetic

| API | Soft Simulation Supported Data Types |
| :--- | :--------------- |
| Exp | half (SubNormal), float (SubNormal) |
| Sqrt | half (SubNormal), float (SubNormal) |
| Add | complex32, int64_t, uint64_t, complex64 |
| AddC | - |
| Sub | complex32, int64_t, uint64_t, complex64 |
| SubC | - |
| Adds | complex32, int64_t, uint64_t, complex64 |
| Max | int64_t, uint64_t |
| Min | int64_t, uint64_t |
| Maxs | int64_t, uint64_t |
| Mins | int64_t, uint64_t |
| Div | int16_t, uint16_t, half (SubNormal), int32_t, uint32_t, float (SubNormal), complex32, int64_t, uint64_t, complex64 |
| Neg | int64_t |
| Ln | half (SubNormal), float (SubNormal) |
| Log | half (SubNormal), float (SubNormal) |
| Log2 | half, float |
| Log10 | half, float |
| Mul | complex32, int64_t, uint64_t, complex64 |
| Mull | - |
| Muls | complex32, int64_t, uint64_t, complex64 |
| Abs | complex32, int64_t, complex64 |
| Relu | int64_t |
| Prelu | - |
| LeakyRelu | - |

**Table 4**  Logical Computation

| API        | Soft Simulation Supported Data Types  |
| :---------- | :---------------- |
| Not         | int64_t, uint64_t |
| And         | int64_t, uint64_t |
| Or          | int64_t, uint64_t |
| Xor         | int64_t, uint64_t |
| ShiftLeft   | int64_t, uint64_t |
| ShiftRight  | int64_t, uint64_t |
| ShiftLefts  | int64_t, uint64_t |
| ShiftRights | int64_t, uint64_t |

**Table 5**  Compound Computation

| API      | Soft Simulation Supported Data Types  |
| :-------- | :---------------- |
| AbsSub    | int64_t           |
| ExpSub    | -                 |
| Axpy      | int64_t, uint64_t |
| MulDstAdd | -                 |
| MulAddDst | int64_t, uint64_t |
| MulsCast  | -                 |

**Table 6**  Comparison and Selection

| API     | Soft Simulation Supported Data Types        |
| :------- | :---------------------- |
| Compare  | -                       |
| Compares | -                       |
| Select   | bool, int64_t, uint64_t |
| Squeeze  | -                       |

**Table 7**  Type Conversion

| API     | Soft Simulation Supported Data Types     |
| :------- | :------------------- |
| Cast     | e8m02bf16, bf162e8m0 |
| Truncate | -                    |

**Table 8**  Reduction

| API            | Soft Simulation Supported Data Types  |
| :-------------- | :---------------- |
| Reduce          | int64_t, uint64_t |
| ReduceDataBlock | -                 |
| PairReduceElem  | -                 |

**Table 9**  Data Fill

| API | Soft Simulation Supported Data Types |
| :--- | :--------------- |
| Duplicate | bool, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, complex32, int64_t, uint64_t, complex64 |

**Table 10**  Scatter Operation

| API   | Soft Simulation Supported Data Types |
| :----- | :--------------- |
| Gather | -                |

**Table 11**  Data Rearrangement

| API         | Soft Simulation Supported Data Types  |
| :----------- | :---------------- |
| Interleave   | int64_t, uint64_t |
| DeInterleave | int64_t, uint64_t |

**Table 12**  Data Compression

| API      | Soft Simulation Supported Data Types |
| :-------- | :--------------- |
| Unsqueeze | -                |
| Pack      | -                |
| UnPack    | -                |

**Table 13**  Histogram

| API       | Soft Simulation Supported Data Types |
| :--------- | :--------------- |
| Histograms | -                |

**Table 14**  Index Operations

| API   | Soft Simulation Supported Data Types |
| :----- | :--------------- |
| Arange | int64_t          |

**Table 15**  Synchronization Control

| API        | Soft Simulation Supported Data Types |
| :---------- | :--------------- |
| LocalMemBar | -                |

**Table 16**  System Variable Access

| API     | Soft Simulation Supported Data Types |
| :------- | :--------------- |
| ClearSpr | -                |
| GetSpr   | -                |
