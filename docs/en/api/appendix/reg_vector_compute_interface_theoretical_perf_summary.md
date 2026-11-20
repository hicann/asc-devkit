# Theoretical Performance Summary of Reg Vector Computation APIs

<!-- md-trans-meta sourceCommit=aff19f76633e6edc0b718e456c0601a03d2f7983 translatedAt=2026-11-17T03:40:06.148Z pushedAt=2026-11-17T09:52:41.770Z nl=2 -->

This section summarizes the theoretical performance of the Reg vector computation APIs. The content in this section applies only to the following models:

<!-- npu="950" id1 -->
Ascend 950PR & 950DT products
<!-- end id1 -->

The theoretical performance summarized in this section is the theoretical performance of a single instruction. The actual performance also depends on whether instructions are issued in single or dual mode. For details, see [Summary of Single/Dual Issue of Reg Vector Computation API Instructions](../appendix/reg_vector_compute_interface_instruction_single_dual_issue_summary.md). Theoretical performance is not provided for APIs and data types implemented through software simulation.

**Table 1** RegTensor data load

| API | Source Operand Data Type | Destination Operand Data Type | Theoretical Parallelism (elements/cycle) |
| -------- | ---------------- | ---------------- | ---------------- |
| Contiguous aligned load (LoadAlign) | b8 | b8 | 256 |
| Contiguous aligned load (LoadAlign) | b16 | b16 | 128 |
| Contiguous aligned load (LoadAlign) | b32 | b32 | 64 |
| Non-contiguous aligned load (LoadAlign) | b8 | b8 | 128 |
| Non-contiguous aligned load (LoadAlign) | b16 | b16 | 64 |
| Non-contiguous aligned load (LoadAlign) | b32 | b32 | 32 |
| Contiguous unaligned load-in (LoadUnalign) | b8 | b8 | 256 (LoadUnalign)/32 (LoadUnalignPre) |
| Contiguous unaligned load-in (LoadUnalign) | b16 | b16 | 128 (LoadUnalign)/16 (LoadUnalignPre) |
| Contiguous unaligned load-in (LoadUnalign) | b32 | b32 | 64 (LoadUnalign)/8 (LoadUnalignPre) |
| Scatter load (Gather) | b8 | b16 | 64 |
| Scatter load (Gather) | b16 | b16 | 64 |
| Scatter load (Gather) | b32 | b32 | 32 |
| Scatter load (GatherB) | b8 | b8 | 128 |
| Scatter load (GatherB) | b16 | b16 | 64 |
| Scatter load (GatherB) | b32 | b32 | 32 |
| Scatter load (GatherB) | b64 | b64 | 16 |
| Move | b8 | b8 | 256 |
| Move | b16 | b16 | 128 |
| Move | b32 | b32 | 64 |
| Move | b64 | b64 | 32 |

**Table 2** RegTensor data store

| API | Source Operand Data Type | Destination Operand Data Type | Theoretical Parallelism (elements/cycle) |
| -------- | ---------------- | ---------------- | ---------------- |
| Contiguous aligned store (StoreAlign) | b8 | b8 | 256 |
| Contiguous aligned store (StoreAlign) | b16 | b16 | 128 |
| Contiguous aligned store (StoreAlign) | b32 | b32 | 64 |
| Non-contiguous aligned store (StoreAlign) | b8 | b8 | 128 |
| Non-contiguous aligned store (StoreAlign) | b16 | b16 | 64 |
| Non-contiguous aligned store (StoreAlign) | b32 | b32 | 32 |
| Contiguous unaligned store-out (StoreUnalign) | b8 | b8 | 256 (StoreUnalign)/32 (StoreUnalignPost) |
| Contiguous unaligned store-out (StoreUnalign) | b16 | b16 | 128 (StoreUnalign)/16 (StoreUnalignPost) |
| Contiguous unaligned store-out (StoreUnalign) | b32 | b32 | 64 (StoreUnalign)/8 (StoreUnalignPost) |
| Scatter store (Scatter) | b8 | b8 | 128 |
| Scatter store (Scatter) | b16 | b16 | 64 |
| Scatter store (Scatter) | b32 | b32 | 32 |

**Table 3** MaskReg data load

| API | Theoretical Time Consumption (cycle) |
| -------- | ---------------- |
| MaskReg load (LoadAlign) | 1 |
| MaskReg load (MaskGenWithRegTensor) | 4 |

**Table 4** MaskReg data store

| API | Theoretical Time Consumption (cycle) |
| -------- | ---------------- |
| MaskReg store (StoreAlign) | 1 |
| MaskReg unaligned store-out (StoreUnalign) | 1 |

**Table 5**  MaskReg Computation

| API | Theoretical Time Consumption (cycle) |
| -------- | ---------------- |
| Move | 1 |
| Interleave | 1 |
| DeInterleave | 1 |
| Select | 1 |
| Pack | 1 |
| UnPack | 1 |
| MoveMask | 1 |

**Table 6**  Basic Arithmetic

| API | Source Operand Data Type | Destination Operand Data Type | Theoretical Parallelism (elements/cycle) |
| -------- | ---------------- | ---------------- | ---------------- |
| Abs | int8_t | int8_t | 128 |
| Abs | int16_t | int16_t | 64 |
| Abs | half | half | 128 |
| Abs | int32_t | int32_t | 32 |
| Abs | float | float | 64 |
| Exp | half | half | 16 |
| Exp | float | float | 16 |
| Sqrt | half | half | 16 |
| Sqrt | float | float | 16 |
| Ln | half | half | 16 |
| Ln | float | float | 16 |
| Log | half | half | 16 |
| Log | float | float | 16 |
| Neg | int8_t | int8_t | 128 |
| Neg | int16_t | int16_t | 64 |
| Neg | half | half | 128 |
| Neg | int32_t | int32_t | 32 |
| Neg | float | float | 64 |
| Relu | half | half | 128 |
| Relu | int32_t | int32_t | 64 |
| Relu | float | float | 64 |
| Prelu | half | half | 128 |
| Prelu | float | float | 64 |
| LeakyRelu | half | half | 128 |
| LeakyRelu | float | float | 64 |
| Add | int8_t | int8_t | 128 |
| Add | uint8_t | uint8_t | 128 |
| Add | int16_t | int16_t | 64 |
| Add | uint16_t | uint16_t | 64 |
| Add | half | half | 128 |
| Add | bfloat16_t | bfloat16_t | 128 |
| Add | int32_t | int32_t | 32 |
| Add | uint32_t | uint32_t | 32 |
| Add | float | float | 64 |
| AddC | int32_t | int32_t | 32 |
| AddC | uint32_t | uint32_t | 32 |
| Div | half | half | 16 |
| Div | float | float | 16 |
| Max | int8_t | int8_t | 256 |
| Max | uint8_t | uint8_t | 256 |
| Max | int16_t | int16_t | 128 |
| Max | uint16_t | uint16_t | 128 |
| Max | half | half | 128 |
| Max | bfloat16_t | bfloat16_t | 128 |
| Max | int32_t | int32_t | 64 |
| Max | uint32_t | uint32_t | 64 |
| Max | float | float | 64 |
| Min | int8_t | int8_t | 256 |
| Min | uint8_t | uint8_t | 256 |
| Min | int16_t | int16_t | 128 |
| Min | uint16_t | uint16_t | 128 |
| Min | half | half | 128 |
| Min | bfloat16_t | bfloat16_t | 128 |
| Min | int32_t | int32_t | 64 |
| Min | uint32_t | uint32_t | 64 |
| Min | float | float | 64 |
| Mul | int16_t | int16_t | 64 |
| Mul | uint16_t | uint16_t | 64 |
| Mul | half | half | 128 |
| Mul | int32_t | int32_t | 32 |
| Mul | uint32_t | uint32_t | 32 |
| Mul | float | float | 64 |
| Mull | int32_t | int32_t | 32 |
| Mull | uint32_t | uint32_t | 32 |
| Sub | int8_t | int8_t | 128 |
| Sub | uint8_t | uint8_t | 128 |
| Sub | int16_t | int16_t | 64 |
| Sub | uint16_t | uint16_t | 64 |
| Sub | half | half | 128 |
| Sub | bfloat16_t | bfloat16_t | 128 |
| Sub | int32_t | int32_t | 32 |
| Sub | uint32_t | uint32_t | 32 |
| Sub | float | float | 64 |
| SubC | int32_t | int32_t | 32 |
| SubC | uint32_t | uint32_t | 32 |
| Adds | int8_t | int8_t | 128 |
| Adds | uint8_t | uint8_t | 128 |
| Adds | int16_t | int16_t | 64 |
| Adds | uint16_t | uint16_t | 64 |
| Adds | half | half | 128 |
| Adds | bfloat16_t | bfloat16_t | 128 |
| Adds | int32_t | int32_t | 32 |
| Adds | uint32_t | uint32_t | 32 |
| Adds | float | float | 64 |
| Maxs | int8_t | int8_t | 256 |
| Maxs | uint8_t | uint8_t | 256 |
| Maxs | int16_t | int16_t | 128 |
| Maxs | uint16_t | uint16_t | 128 |
| Maxs | half | half | 128 |
| Maxs | bfloat16_t | bfloat16_t | 128 |
| Maxs | int32_t | int32_t | 64 |
| Maxs | uint32_t | uint32_t | 64 |
| Maxs | float | float | 64 |
| Mins | int8_t | int8_t | 256 |
| Mins | uint8_t | uint8_t | 256 |
| Mins | int16_t | int16_t | 128 |
| Mins | uint16_t | uint16_t | 128 |
| Mins | half | half | 128 |
| Mins | bfloat16_t | bfloat16_t | 128 |
| Mins | int32_t | int32_t | 64 |
| Mins | uint32_t | uint32_t | 64 |
| Mins | float | float | 64 |
| Muls | int16_t | int16_t | 64 |
| Muls | uint16_t | uint16_t | 64 |
| Muls | half | half | 128 |
| Muls | int32_t | int32_t | 32 |
| Muls | uint32_t | uint32_t | 32 |
| Muls | float | float | 64 |

**Table 7**  Logical Computation

| API | Source Operand Data Type | Destination Operand Data Type | Theoretical Parallelism (elements/cycle) |
| -------- | ---------------- | ---------------- | ---------------- |
| Not | int8_t | int8_t | 256 |
| Not | uint8_t | uint8_t | 256 |
| Not | int16_t | int16_t | 128 |
| Not | uint16_t | uint16_t | 128 |
| Not | half | half | 128 |
| Not | int32_t | int32_t | 64 |
| Not | uint32_t | uint32_t | 64 |
| Not | float | float | 64 |
| And | int8_t | int8_t | 256 |
| And | uint8_t | uint8_t | 256 |
| And | int16_t | int16_t | 128 |
| And | uint16_t | uint16_t | 128 |
| And | int32_t | int32_t | 64 |
| And | uint32_t | uint32_t | 64 |
| Or | int8_t | int8_t | 256 |
| Or | uint8_t | uint8_t | 256 |
| Or | int16_t | int16_t | 128 |
| Or | uint16_t | uint16_t | 128 |
| Or | int32_t | int32_t | 64 |
| Or | uint32_t | uint32_t | 64 |
| Xor | int8_t | int8_t | 256 |
| Xor | uint8_t | uint8_t | 256 |
| Xor | int16_t | int16_t | 128 |
| Xor | uint16_t | uint16_t | 128 |
| Xor | int32_t | int32_t | 64 |
| Xor | uint32_t | uint32_t | 64 |
| ShiftLeft | int8_t | int8_t | 256 |
| ShiftLeft | uint8_t | uint8_t | 256 |
| ShiftLeft | int16_t | int16_t | 128 |
| ShiftLeft | uint16_t | uint16_t | 128 |
| ShiftLeft | int32_t | int32_t | 64 |
| ShiftLeft | uint32_t | uint32_t | 64 |
| ShiftRight | int8_t | int8_t | 256 |
| ShiftRight | uint8_t | uint8_t | 256 |
| ShiftRight | int16_t | int16_t | 128 |
| ShiftRight | uint16_t | uint16_t | 128 |
| ShiftRight | int32_t | int32_t | 64 |
| ShiftRight | uint32_t | uint32_t | 64 |
| ShiftLefts | int8_t | int8_t | 256 |
| ShiftLefts | uint8_t | uint8_t | 256 |
| ShiftLefts | int16_t | int16_t | 128 |
| ShiftLefts | uint16_t | uint16_t | 128 |
| ShiftLefts | int32_t | int32_t | 64 |
| ShiftLefts | uint32_t | uint32_t | 64 |
| ShiftRights | int8_t | int8_t | 256 |
| ShiftRights | uint8_t | uint8_t | 256 |
| ShiftRights | int16_t | int16_t | 128 |
| ShiftRights | uint16_t | uint16_t | 128 |
| ShiftRights | int32_t | int32_t | 64 |
| ShiftRights | uint32_t | uint32_t | 64 |

**Table 8**  Compound Computation

| API | Source Operand Data Type | Destination Operand Data Type | Theoretical Parallelism (elements/cycle) |
| -------- | ---------------- | ---------------- | ---------------- |
| AbsSub | half | half | 128 |
| AbsSub | float | float | 64 |
| Axpy | half | half | 128 |
| Axpy | float | float | 64 |
| ExpSub | half | float | 16 |
| ExpSub | float | float | 16 |
| MulDstAdd | half | half | 128 |
| MulDstAdd | bfloat16_t | bfloat16_t | 128 |
| MulDstAdd | float | float | 64 |
| MulAddDst | int16_t | int16_t | 64 |
| MulAddDst | uint16_t | uint16_t | 64 |
| MulAddDst | half | half | 128 |
| MulAddDst | bfloat16_t | bfloat16_t | 128 |
| MulAddDst | int32_t | int32_t | 32 |
| MulAddDst | uint32_t | uint32_t | 32 |
| MulAddDst | float | float | 64 |
| MulsCast | float | half | 64 |

**Table 9**  Comparison and Selection

| API | Source Operand Data Type | Destination Operand Data Type | Theoretical Parallelism (elements/cycle) |
| -------- | ---------------- | ---------------- | ---------------- |
| Compare | int8_t | MaskReg | 256 |
| Compare | uint8_t | MaskReg | 256 |
| Compare | int16_t | MaskReg | 128 |
| Compare | uint16_t | MaskReg | 128 |
| Compare | half | MaskReg | 128 |
| Compare | bfloat16_t | MaskReg | 128 |
| Compare | int32_t | MaskReg | 64 |
| Compare | uint32_t | MaskReg | 64 |
| Compare | float | MaskReg | 64 |
| Compares | int8_t | MaskReg | 256 |
| Compares | uint8_t | MaskReg | 256 |
| Compares | int16_t | MaskReg | 128 |
| Compares | uint16_t | MaskReg | 128 |
| Compares | half | MaskReg | 128 |
| Compares | bfloat16_t | MaskReg | 128 |
| Compares | int32_t | MaskReg | 64 |
| Compares | uint32_t | MaskReg | 64 |
| Compares | float | MaskReg | 64 |
| Select | int8_t | MaskReg | 256 |
| Select | uint8_t | MaskReg | 256 |
| Select | int16_t | MaskReg | 128 |
| Select | uint16_t | MaskReg | 128 |
| Select | half | MaskReg | 128 |
| Select | bfloat16_t | MaskReg | 128 |
| Select | int32_t | MaskReg | 64 |
| Select | uint32_t | MaskReg | 64 |
| Select | float | MaskReg | 64 |
| Squeeze | int8_t | int8_t | 51.2 |
| Squeeze | uint8_t | uint8_t | 51.2 |
| Squeeze | int16_t | int16_t | 25.6 |
| Squeeze | uint16_t | uint16_t | 25.6 |
| Squeeze | half | half | 25.6 |
| Squeeze | int32_t | int32_t | 12.8 |
| Squeeze | uint32_t | uint32_t | 12.8 |
| Squeeze | float | float | 12.8 |

**Table 10**  Type Conversion

| API | Source Operand Data Type | Destination Operand Data Type | Theoretical Parallelism (elements/cycle) |
| -------- | ---------------- | ---------------- | ---------------- |
| Cast | float | int64_t | 16 |
| Cast | float | int32_t | 32 |
| Cast | float | int16_t | 32 |
| Cast | half | int32_t | 32 |
| Cast | half | int16_t | 64 |
| Cast | half | int8_t | 64 |
| Cast | float | half | 64 |
| Cast | float | bfloat16_t | 64 |
| Cast | half | float | 64 |
| Cast | bfloat16_t | float | 64 |
| Cast | float | hifloat8_t | 32 |
| Cast | hifloat8_t | float | 32 |
| Cast | half | hifloat8_t | 64 |
| Cast | hifloat8_t | half | 64 |
| Cast | float | fp8_e4m3fn_t | 32 |
| Cast | float | fp8_e5m2_t | 32 |
| Cast | fp4×2_e1m2_t | bfloat16_t | 64 |
| Cast | fp4×2_e2m1_t | bfloat16_t | 64 |
| Cast | bfloat16_t | fp4×2_e1m2_t | 64 |
| Cast | bfloat16_t | fp4×2_e2m1_t | 64 |
| Cast | uint8_t | half | 64 |
| Cast | int8_t | half | 64 |
| Cast | int16_t | half | 64 |
| Cast | int16_t | float | 32 |
| Cast | int32_t | float | 32 |
| Cast | int4×2_t | half | 64 |
| Cast | int4×2_t | int16_t | 64 |
| Cast | uint8_t | uint16_t | 64 |
| Cast | uint8_t | uint32_t | 32 |
| Cast | int8_t | int16_t | 64 |
| Cast | int8_t | int32_t | 32 |
| Cast | uint16_t | uint8_t | 64 |
| Truncate | half | half | 64 |
| Truncate | bfloat16_t | bfloat16_t | 64 |
| Truncate | float | float | 32 |

**Table 11**  Reduction

| API | Source Operand Data Type | Destination Operand Data Type | Theoretical Parallelism (elements/cycle) |
| -------- | ---------------- | ---------------- | ---------------- |
| Reduce (ReduceType::SUM) | half | half | 128 |
| Reduce (ReduceType::SUM) | int16_t | int32_t | 64 |
| Reduce (ReduceType::SUM) | uint16_t | uint32_t | 64 |
| Reduce (ReduceType::SUM) | int32_t | int32_t | 64 |
| Reduce (ReduceType::SUM) | uint32_t | uint32_t | 64 |
| Reduce (ReduceType::SUM) | float | float | 64 |
| Reduce (ReduceType::MAX) | int16_t | int16_t | 128 |
| Reduce (ReduceType::MAX) | uint16_t | uint16_t | 128 |
| Reduce (ReduceType::MAX) | half | half | 128 |
| Reduce (ReduceType::MAX) | int32_t | int32_t | 64 |
| Reduce (ReduceType::MAX) | uint32_t | uint32_t | 64 |
| Reduce (ReduceType::MAX) | float | float | 64 |
| Reduce (ReduceType::MIN) | int16_t | int16_t | 128 |
| Reduce (ReduceType::MIN) | uint16_t | uint16_t | 128 |
| Reduce (ReduceType::MIN) | half | half | 128 |
| Reduce (ReduceType::MIN) | int32_t | int32_t | 64 |
| Reduce (ReduceType::MIN) | uint32_t | uint32_t | 64 |
| Reduce (ReduceType::MIN) | float | float | 64 |
| ReduceDataBlock (ReduceType::SUM) | half | half | 128 |
| ReduceDataBlock (ReduceType::SUM) | int32_t | int32_t | 64 |
| ReduceDataBlock (ReduceType::SUM) | uint32_t | uint32_t | 64 |
| ReduceDataBlock (ReduceType::SUM) | float | float | 64 |
| ReduceDataBlock (ReduceType::MAX) | int16_t | int16_t | 128 |
| ReduceDataBlock (ReduceType::MAX) | uint16_t | uint16_t | 128 |
| ReduceDataBlock (ReduceType::MAX) | half | half | 128 |
| ReduceDataBlock (ReduceType::MAX) | int32_t | int32_t | 64 |
| ReduceDataBlock (ReduceType::MAX) | uint32_t | uint32_t | 64 |
| ReduceDataBlock (ReduceType::MAX) | float | float | 64 |
| ReduceDataBlock (ReduceType::MIN) | int16_t | int16_t | 128 |
| ReduceDataBlock (ReduceType::MIN) | uint16_t | uint16_t | 128 |
| ReduceDataBlock (ReduceType::MIN) | half | half | 128 |
| ReduceDataBlock (ReduceType::MIN) | int32_t | int32_t | 64 |
| ReduceDataBlock (ReduceType::MIN) | uint32_t | uint32_t | 64 |
| ReduceDataBlock (ReduceType::MIN) | float | float | 64 |
| PairReduceElem | half | half | 128 |
| PairReduceElem | float | float | 64 |

**Table 12**  Data Fill

| API | Source Operand Data Type | Destination Operand Data Type | Theoretical Parallelism (elements/cycle) |
| -------- | ---------------- | ---------------- | ---------------- |
| Duplicate | int8_t | int8_t | 256 |
| Duplicate | uint8_t | uint8_t | 256 |
| Duplicate | int16_t | int16_t | 128 |
| Duplicate | uint16_t | uint16_t | 128 |
| Duplicate | half | half | 128 |
| Duplicate | bfloat16_t | bfloat16_t | 128 |
| Duplicate | int32_t | int32_t | 64 |
| Duplicate | uint32_t | uint32_t | 64 |
| Duplicate | float | float | 64 |

**Table 13**  Scatter Operations

| API | Source Operand Data Type | Destination Operand Data Type | Theoretical Parallelism (elements/cycle) |
| -------- | ---------------- | ---------------- | ---------------- |
| Gather | b8 | b8 | 64 |
| Gather | b16 | b16 | 32 |
| Gather | b32 | b32 | 16 |

**Table 14**  Data Rearrangement

| API | Source Operand Data Type | Destination Operand Data Type | Theoretical Parallelism (elements/cycle) |
| -------- | ---------------- | ---------------- | ---------------- |
| Interleave | int8_t | int8_t | 256 |
| Interleave | uint8_t | uint8_t | 256 |
| Interleave | int16_t | int16_t | 128 |
| Interleave | uint16_t | uint16_t | 128 |
| Interleave | half | half | 128 |
| Interleave | bfloat16_t | bfloat16_t | 128 |
| Interleave | int32_t | int32_t | 64 |
| Interleave | uint32_t | uint32_t | 64 |
| Interleave | float | float | 64 |
| DeInterleave | int8_t | int8_t | 256 |
| DeInterleave | uint8_t | uint8_t | 256 |
| DeInterleave | int16_t | int16_t | 128 |
| DeInterleave | uint16_t | uint16_t | 128 |
| DeInterleave | half | half | 128 |
| DeInterleave | bfloat16_t | bfloat16_t | 128 |
| DeInterleave | int32_t | int32_t | 64 |
| DeInterleave | uint32_t | uint32_t | 64 |
| DeInterleave | float | float | 64 |

**Table 15**  Data Compression

| API | Source Operand Data Type | Destination Operand Data Type | Theoretical Parallelism (elements/cycle) |
| -------- | ---------------- | ---------------- | ---------------- |
| Unsqueeze | MaskReg | int8_t | 64 |
| Unsqueeze | MaskReg | uint8_t | 64 |
| Unsqueeze | MaskReg | int16_t | 32 |
| Unsqueeze | MaskReg | uint16_t | 32 |
| Unsqueeze | MaskReg | int32_t | 16 |
| Unsqueeze | MaskReg | uint32_t | 16 |
| Pack | int16_t | uint8_t | 128 |
| Pack | uint16_t | uint8_t | 128 |
| Pack | int32_t | uint16_t | 64 |
| Pack | uint32_t | uint16_t | 64 |
| UnPack | int8_t | int16_t | 128 |
| UnPack | uint8_t | uint16_t | 128 |
| UnPack | int16_t | int32_t | 64 |
| UnPack | uint16_t | uint32_t | 64 |

**Table 16**  Histogram

| API | Source Operand Data Type | Destination Operand Data Type | Theoretical Parallelism (elements/cycle) |
| -------- | ---------------- | ---------------- | ---------------- |
| Histograms | uint8_t | uint16_t | 32 |

**Table 17**  Index Operations

| API | Source Operand Data Type | Destination Operand Data Type | Theoretical Parallelism (elements/cycle) |
| -------- | ---------------- | ---------------- | ---------------- |
| Arange | int8_t | int8_t | 128 |
| Arange | int16_t | int16_t | 64 |
| Arange | half | half | 128 |
| Arange | int32_t | int32_t | 32 |
| Arange | float | float | 64 |
