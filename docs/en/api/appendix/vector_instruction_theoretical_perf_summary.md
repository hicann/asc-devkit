# Vector Instruction Theoretical Performance Summary<a name="ZH-CN_TOPIC_0000002557396789"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-29T11:09:39.235Z pushedAt=2026-10-29T11:35:38.836Z nl=2 -->

This section summarizes the theoretical performance of some major Vector instructions. The following content applies to the following models:

<!-- npu="950" id2 -->
- Ascend 950PR & 950DT products
<!-- end id2 -->
<!-- npu="A3" id3 -->
- Atlas A3 products
<!-- end id3 -->
<!-- npu="910b" id4 -->
- Atlas A2 products
<!-- end id4 -->

Note:
- Cache control, synchronization control, and atomic operation instructions do not involve theoretical performance.
- For vector computation APIs, if the value of the `count` or `repeatTime` parameter passed to the API is 0, no computation is performed and the destination operand is not written. The API is treated as a NOP (no operation), but it incurs additional performance overhead compared with not calling it.

<!-- npu="950" id5 -->
For the Ascend 950PR&950DT products:
- Memory vector computation APIs are mainly implemented by Reg vector computation APIs, and these Memory vector computation APIs do not involve theoretical performance. For the theoretical performance summary of some data layout conversion, sorting combination, data movement, scalar computation, and tool interface instructions, see Table 1 to Table 5.
- For the theoretical performance of SIMT-related APIs, see [SIMT Instruction Theoretical Performance Summary](./SIMT_instruction_theoretical_perf_summary.md).

**Table 1**  Theoretical performance summary of data layout conversion instructions

<a id="table1"></a>

| API | Theoretical Parallelism Degree (bytes/cycle) |
| --- | --- |
| Transpose | 256 |

**Table 2**  Theoretical performance summary of sorting combination instructions

<a id="table2"></a>

| API | Input Data Type | Output Data Type | Theoretical Parallelism (elements/cycle) |
| --- | --- | --- | --- |
| Sort32 | half | half | 2 |
| Sort32 | float | float | 2 |
| MrgSort | half | half | 2 |
| MrgSort | float | float | 2 |

**Table 3**  Data Movement Instruction Theoretical Performance Summary

<a id="table3"></a>

| API | Theoretical Parallelism Degree (bytes/cycle) |
| --- | --- |
| DataCopy(UB->UB) | 256 |

**Table 4**  Scalar Computation Instruction Theoretical Performance Summary

<a id="table4"></a>

| API | Input Data Type | Output Data Type | CPI (cycles/instruction) |
| --- | --- | --- | --- |
| GetBitCount | uint64_t | int64_t | 1 |
| CountLeadingZero | uint64_t | int64_t | 1 |
| GetSFFValue | uint64_t | int64_t | 1 |
| CountBitsCntSameAsSignBit | int64_t | int64_t | 1 |
| Cast (float to half, int32_t) | float | half/int32_t | 1 |
| Cast (float to bfloat16_t) | - | - | Software simulation instruction, no theoretical performance involved |
| Cast (multiple types to float) | - | - | Software simulation instruction, no theoretical performance involved |
| Nop | - | - | Software simulation instruction, no theoretical performance involved |

**Table 5**  Tool Interface Instruction Theoretical Performance Summary

<a id="table5"></a>

| API | CPI (cycles/instruction) |
| --- | --- |
| InitSocState | Software simulation instruction, not involving theoretical performance. |
| Async | Static compilation, no consumption. |
| GetBlockNum | 1 |
| GetBlockIdx (AIC) | 1 |
| GetBlockIdx (AIV) | Software simulation instruction, not involving theoretical performance. |
| GetTaskRatio | 1 |
| GetSubBlockNum | 1 |
| GetSubBlockIdx | 1 |
| GetDataBlockSizeInBytes | Static compilation, no consumption, consistent behavior with directly using immediate value. |
| GetSystemCycle | 1 |
| GetProgramCounter | Software simulation instruction, not involving theoretical performance. |
| GetArchVersion | Software simulation instruction, not involving theoretical performance. |
| GetUBSizeInBytes | Static compilation, no consumption, consistent behavior with directly using immediate value. |
| GetRuntimeUBSize | Software simulation instruction, not involving theoretical performance. |
| NumericLimits class API | Software simulation instruction, not involving theoretical performance. |

<!-- end id5 -->

<!-- npu="A3,910b" id6 -->
For the following product models, refer to Table 6 to Table 18 for the Vector instruction theoretical performance summary.
<!-- npu="A3" id1 -->
- Atlas A3 products
<!-- end id1 -->
<!-- npu="910b" id7 -->
- Atlas A2 products
<!-- end id7 -->

**Table 6**  Basic arithmetic instruction theoretical performance summary

<a id="table6"></a>

| API | Input/Output Data Type | Theoretical Parallelism (elements/cycle) |
| --- | --- | --- |
| Add | half | 128 |
| Add | int16_t | 128 |
| Add | int32_t | 64 |
| Add | float | 64 |
| Exp | half | 32 |
| Exp | float | 32 |
| Ln | half | 32 |
| Ln | float | 32 |
| Abs | half | 128 |
| Abs | float | 64 |
| Reciprocal | half | 128 |
| Reciprocal | float | 64 |
| Sqrt | half | 32 |
| Sqrt | float | 32 |
| Rsqrt | half | 128 |
| Rsqrt | float | 64 |
| Relu | half | 128 |
| Relu | float | 64 |
| Relu | int32_t | 64 |
| Sub | half | 128 |
| Sub | int16_t | 128 |
| Sub | int32_t | 64 |
| Sub | float | 64 |
| Mul | half | 128 |
| Mul | int16_t | 128 |
| Mul | int32_t | 64 |
| Mul | float | 64 |
| Div | half | 32 |
| Div | float | 32 |
| Max | half | 128 |
| Max | int16_t | 128 |
| Max | int32_t | 64 |
| Max | float | 64 |
| Min | half | 128 |
| Min | int16_t | 128 |
| Min | int32_t | 64 |
| Min | float | 64 |
| BilinearInterpolation(ISASI) | half | Software simulation instruction, not involving theoretical parallelism degree. |
| Adds | half | 128 |
| Adds | int16_t | 128 |
| Adds | int32_t | 64 |
| Adds | float | 64 |
| Muls | half | 128 |
| Muls | int16_t | 128 |
| Muls | int32_t | 64 |
| Muls | float | 64 |
| Maxs | half | 128 |
| Maxs | int16_t | 128 |
| Maxs | int32_t | 64 |
| Maxs | float | 64 |
| Mins | half | 128 |
| Mins | int16_t | 128 |
| Mins | int32_t | 64 |
| Mins | float | 64 |
| LeakyRelu | half | 128 |
| LeakyRelu | float | 64 |

**Table 7**  Logical computation instruction theoretical performance summary

<a id="table7"></a>

| API | src Data Type | Theoretical Parallelism (elements/cycle) |
| --- | --- | --- |
| Not | int16_t/uint16_t | 128 |
| And | int16_t/uint16_t | 128 |
| Or | int16_t/uint16_t | 128 |
| ShiftLeft | int16_t/uint16_t | 128 |
| ShiftLeft | int32_t/uint32_t | 64 |
| ShiftRight | int16_t/uint16_t | 128 |
| ShiftRight | int32_t/uint32_t | 64 |

**Table 8**  Compound computation instruction theoretical performance summary

<a id="table8"></a>

| API | Data Type of src0 | scalar/src1 Data Type | dst Data Type | Theoretical Parallelism (elements/cycle) |
| --- | --- | --- | --- | --- |
| Axpy | half | half | half | 128 |
| Axpy | float | float | float | 64 |
| Axpy | half | half | float | 64 |
| MulAddDst | half | half | half | 64 |
| MulAddDst | float | float | float | 32 |
| MulAddDst | half | half | float | 32 |
| MulCast | half | half | uint8_t | 128 |
| MulCast | half | half | int8_t | 128 |
| AddRelu | int16_t | int16_t | int16_t | 128 |
| AddRelu | half | half | half | 128 |
| AddRelu | float | float | float | 64 |
| AddReluCast | float | float | half | 64 |
| AddReluCast | half | half | int8_t | 128 |
| AddReluCast | int16_t | int16_t | int8_t | 128 |
| SubRelu | int16_t | int16_t | int16_t | 128 |
| SubRelu | half | half | half | 128 |
| SubRelu | float | float | float | 64 |
| SubReluCast | float | float | half | 64 |
| SubReluCast | half | half | int8_t | 128 |
| SubReluCast | int16_t | int16_t | int8_t | 128 |
| FusedMulAdd | half | half | half | 128 |
| FusedMulAdd | float | float | float | 64 |
| MulAddRelu | half | half | half | 128 |
| MulAddRelu | float | float | float | 64 |
| CastDequant | int16_t | NA | int8_t/uint8_t | 128 |
| CastDequant | int32_t | NA | half | 64 |
| AddDeqRelu | - | - | - | software simulation instruction, not involving theoretical parallelism degree |

**Table 9**  Theoretical performance summary of comparison and selection APIs

<a id="table9"></a>

| API | Data Type of src0 | scalar/src1 data type | dst data type | Theoretical Parallelism (elements/cycle) |
| --- | --- | --- | --- | --- |
| Compare | half | half | uint8_t, int8_t | 128 |
| Compare | float | float | uint8_t, int8_t | 64 |
| Compare | int32_t | int32_t | uint8_t, int8_t | 64 |
| Compare (result stored in a register) | half | half | uint8_t, int8_t | 128 |
| Compare (result stored in a register) | float | float | uint8_t, int8_t | 64 |
| Compares | half | half | uint8_t | 128 |
| Compares | float | float | uint8_t | 64 |
| Compares | int32_t | int32_t | uint8_t | 64 |
| Select | half | half | uint8_t | 128 |
| Select | float | float | uint8_t | 64 |
| GetCmpMask(ISASI) | - | - | - | 1 |
| SetCmpMask(ISASI) | - | - | - | 1 |
| GatherMask | half | uint16_t | half | 64 |
| GatherMask | bfloat16_t | uint16_t | bfloat16_t | 64 |
| GatherMask | uint16_t | uint16_t | uint16_t | 64 |
| GatherMask | int16_t | uint16_t | int16_t | 64 |
| GatherMask | float | uint32_t | float | 32 |
| GatherMask | uint32_t | uint32_t | uint32_t | 32 |
| GatherMask | int32_t | uint32_t | int32_t | 32 |

**Table 10**  Theoretical performance summary of type conversion APIs

<a id="table10"></a>

| API | src Data Type | dst Data Type | Theoretical Parallelism (elements/cycle) |
| --- | --- | --- | --- |
| Cast | int4b_t | half | 128 |
| Cast | uint8_t | half | 128 |
| Cast | int8_t | half | 128 |
| Cast | int16_t | half | 128 |
| Cast | int16_t | float | 64 |
| Cast | half | int4b_t/int8_t/uint8_t/int16_t | 128 |
| Cast | half | int32_t/float | 64 |
| Cast | bfloat16_t | int32_t/float | 64 |
| Cast | int32_t | int16_t/half | 64 |
| Cast | int32_t | float | 64 |
| Cast | int32_t | int64_t | 32 |
| Cast | float | half | 64 |
| Cast | float | int16_t/bfloat16_t/int32_t/float | 64 |
| Cast | float | int64_t | 32 |

**Table 11**  Theoretical performance summary of reduction computation APIs

<a id="table11"></a>

| API | src Data Type | dst Data Type | Theoretical Parallelism (elements/cycle) |
| --- | --- | --- | --- |
| ReduceDataBlock | half | half | 128/7 |
| ReduceDataBlock | float | float | 64 |
| ReduceRepeat | half | half | 128/7 |
| ReduceRepeat | float | float | 64/7 |
| ReducePairElem | half | half | 128 |
| ReducePairElem | float | float | 64 |
| ReduceMax/ReduceMin/ReduceSum | - | - | software simulation instruction, not involving theoretical parallelism degree |

**Table 12**  Theoretical performance summary of data layout conversion instructions

<a id="table12"></a>

| API | src data type | dst data type | Theoretical Parallelism (elements/cycle) |
| --- | --- | --- | --- |
| Transpose | int8_t/uint8_t | int8_t/uint8_t | 256 |
| Transpose | int16_t/uint16_t/half | int16_t/uint16_t/half | 128 |
| Transpose | int32_t/uint32_t/float | int32_t/uint32_t/float | 64 |
| TransDataTo5HD | int8_t/uint8_t | int8_t/uint8_t | 256 |
| TransDataTo5HD | int16_t/uint16_t/half | int16_t/uint16_t/half | 128 |
| TransDataTo5HD | int32_t/uint32_t/float | int32_t/uint32_t/float | 64 |

**Table 13**  Theoretical performance summary of data fill instructions

<a id="table13"></a>

| API | src data type | dst data type | Theoretical Input Parallelism (elements/cycle) | Theoretical Output Parallelism (elements/cycle) |
| --- | --- | --- | --- | --- |
| Brcb | int16_t/uint16_t/half/bfloat16_t | int16_t/uint16_t/half/bfloat16_t | 8 | 128 |
| Brcb | int32_t/uint32_t/float | int32_t/uint32_t/float | 8 | 64 |
| Duplicate | int16_t/uint16_t/half/bfloat16_t | int16_t/uint16_t/half/bfloat16_t | - | 128 |
| Duplicate | int32_t/uint32_t/float | int32_t/uint32_t/float | - | 64 |
| CreateVecIndex | int16_t/half | int16_t/half | - | 128 |
| CreateVecIndex | int32_t/float | int32_t/float | - | 64 |

**Table 14**  Theoretical performance summary of sorting combination instructions

<a id="table14"></a>

| API | dst data type | Theoretical Parallelism (elements/cycle) |
| --- | --- | --- |
| Sort32 | half | 2 |
| Sort32 | float | 2 |
| MrgSort | half | 2 |
| MrgSort | float | 2 |

**Table 15**  Theoretical performance summary of scatter and gather instructions

<a id="table15"></a>

| API | src Data Type | dst Data Type | Theoretical Parallelism (elements/cycle) |
| --- | --- | --- | --- |
| Gather | int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float | int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float | 5 |
| Gatherb | uint16_t | uint16_t | 42 |
| Gatherb | uint32_t | uint32_t | 21 |

**Table 16**  Theoretical performance summary of mask operation instructions

<a id="table16"></a>

| API | Input data type | Theoretical Parallelism (elements/cycle) |
| --- | --- | --- |
| SetMaskCount | - | 1 |
| SetMaskNorm | - | 1 |
| SetVectorMask | uint64_t | 1 |
| SetVectorMask | int32_t | 1 |
| ResetMask | - | 1 |

**Table 17**  Theoretical performance summary of scalar computation instructions

<a id="table17"></a>

| API | Input data type | Output Data Type | CPI (cycles/instruction) |
| --- | --- | --- | --- |
| GetBitCount | uint64_t | int64_t | 1 |
| CountLeadingZero | uint64_t | int64_t | 1 |
| GetSFFValue | uint64_t | int64_t | 1 |
| CountBitsCntSameAsSignBit | int64_t | int64_t | 1 |
| Cast (float to half, int32_t) | float | half/int32_t | 1 |
| Cast (float to bfloat16_t) | - | - | Software simulation instruction, not involving theoretical performance |
| Cast (bfloat16_t to float) | - | - | Software simulation instruction, not involving theoretical performance |

**Table 18**  Theoretical performance summary of tool interface instructions

<a id="table18"></a>

| API | CPI (cycles/instruction) |
| --- | --- |
| InitSocState | Software simulation instruction; does not involve theoretical performance. |
| Async | Static compilation; no consumption. |
| GetBlockNum | 1 |
| GetBlockIdx (AIC) | 1 |
| GetBlockIdx (AIV) | Software simulation instruction; does not involve theoretical performance. |
| GetTaskRatio | 1 |
| GetSubBlockNum | 1 |
| GetSubBlockIdx | 1 |
| GetDataBlockSizeInBytes | Static compilation; no consumption. Consistent with directly using an immediate value. |
| GetSystemCycle | 1 |
| GetProgramCounter | Software simulation instruction; does not involve theoretical performance. |
| GetArchVersion | Software simulation instruction; does not involve theoretical performance. |

<!-- end id6 -->
