# Vector Instruction Theoretical Performance Summary<a name="ZH-CN_TOPIC_0000002557396789"></a>

<!-- md-trans-meta sourceCommit=1dd6f87db24cdbe0c70c00ede7f4eb4aca608287 translatedAt=2026-09-06T08:54:39.796Z pushedAt=2026-09-11T01:07:40.353Z -->

This section summarizes the theoretical performance of some major Vector instructions. The following content applies to the following models:
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products
<!-- end id3 -->

Note:
- Cache control, synchronization control, and atomic operation instructions do not involve theoretical performance.
- For vector computation APIs, if the value of the `count` or `repeatTime` parameter passed to an API is 0, no computation is performed and no write is made to the destination operand. The API is treated as a NOP (no operation), but it incurs additional performance overhead compared with not calling it.

**Table 1**  Summary of theoretical performance of basic arithmetic instructions

<a name="table291476204714"></a>
| API | Input/Output Data Type | Hardware Parallelism (Unit: Element/Cycle) |
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
| BilinearInterpolation(ISASI) | half | Software-simulated instruction, not involving theoretical parallelism |
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

**Table 2**  Summary of theoretical performance of logical computation instructions

<a name="table116881509216"></a>
| API | src Data Type | Hardware Parallelism (Unit: Element/Cycle) |
| --- | --- | --- |
| Not | int16_t/uint16_t | 128 |
| And | int16_t/uint16_t | 128 |
| Or | int16_t/uint16_t | 128 |
| ShiftLeft | int16_t/uint16_t | 128 |
| ShiftLeft | int32_t/uint32_t | 64 |
| ShiftRight | int16_t/uint16_t | 128 |
| ShiftRight | int32_t/uint32_t | 64 |

**Table 3**  Summary of theoretical performance of compound computation instructions

<a name="table1089823515222"></a>
| API | src0 Data Type | scalar/src1 Data Type | dst Data Type | Hardware Parallelism (Unit: Element/Cycle) |
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
| AddDeqRelu | - | - | - | Software-simulated instruction; no theoretical parallelism involved |

**Table 4**  Summary of theoretical performance of comparison and selection instructions

<a name="table205131952256"></a>
| API | src0 Data Type | scalar/src1 Data Type | dst Data Type | Hardware Parallelism (Unit: Element/Cycle) |
| --- | --- | --- | --- | --- |
| Compare | half | half | uint8_t, int8_t | 128 |
| Compare | float | float | uint8_t, int8_t | 64 |
| Compare | int32_t | int32_t | uint8_t, int8_t | 64 |
| Compare (result stored in register) | half | half | uint8_t, int8_t | 128 |
| Compare (result stored in register) | float | float | uint8_t, int8_t | 64 |
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

**Table 5**  Summary of theoretical performance of type conversion instructions

<a name="table1712055602918"></a>
| API | src Data Type | dst Data Type | Theoretical Parallelism (Element/Cycle) |
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

**Table 6**  Summary of theoretical performance of reduction computation instructions

<a name="table76761043412"></a>
| API | src Data Type | dst Data Type | Theoretical Parallelism (Element/Cycle) |
| --- | --- | --- | --- |
| ReduceDataBlock | half | half | 128/7 |
| ReduceDataBlock | float | float | 64 |
| ReduceRepeat | half | half | 128/7 |
| ReduceRepeat | float | float | 64/7 |
| ReducePairElem | half | half | 128 |
| ReducePairElem | float | float | 64 |
| ReduceMax/ReduceMin/ReduceSum | - | - | Software-simulated instruction, no theoretical parallelism involved |

**Table 7**  Summary of theoretical performance of data layout conversion instructions

<a name="table2514311153610"></a>
| API | src Data Type | dst Data Type | Theoretical Parallelism (Element/Cycle) |
| --- | --- | --- | --- |
| Transpose | int8_t/uint8_t | int8_t/uint8_t | 256 |
| Transpose | int16_t/uint16_t/half | int16_t/uint16_t/half | 128 |
| Transpose | int32_t/uint32_t/float | int32_t/uint32_t/float | 64 |
| TransDataTo5HD | int8_t/uint8_t | int8_t/uint8_t | 256 |
| TransDataTo5HD | int16_t/uint16_t/half | int16_t/uint16_t/half | 128 |
| TransDataTo5HD | int32_t/uint32_t/float | int32_t/uint32_t/float | 64 |

**Table 8**  Summary of theoretical performance of data fill instructions

<a name="table18675133833617"></a>
| API | src Data Type | dst Data Type | Theoretical Input Parallelism (Element/Cycle) | Theoretical Output Parallelism (Element/Cycle) |
| --- | --- | --- | --- | --- |
| Brcb | int16_t/uint16_t/half/bfloat16_t | int16_t/uint16_t/half/bfloat16_t | 8 | 128 |
| Brcb | int32_t/uint32_t/float | int32_t/uint32_t/float | 8 | 64 |
| Duplicate | int16_t/uint16_t/half/bfloat16_t | int16_t/uint16_t/half/bfloat16_t | - | 128 |
| Duplicate | int32_t/uint32_t/float | int32_t/uint32_t/float | - | 64 |
| CreateVecIndex | int16_t/half | int16_t/half | - | 128 |
| CreateVecIndex | int32_t/float | int32_t/float | - | 64 |

**Table 9**  Summary of theoretical performance of sorting and combination instructions

<a name="table1361419514367"></a>
| API | dst Data Type | Theoretical Parallelism (Element/Cycle) |
| --- | --- | --- |
| Sort32 | half | 2 |
| Sort32 | float | 2 |
| MrgSort | half | 2 |
| MrgSort | float | 2 |

**Table 10**  Theoretical performance summary of scatter and gather instructions

<a name="table111588914379"></a>
| API | src Data Type | dst Data Type | Theoretical Parallelism (Element/Cycle) |
| --- | --- | --- | --- |
| Gather | int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float | int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float | 5 |
| Gatherb | uint16_t | uint16_t | 42 |
| Gatherb | uint32_t | uint32_t | 21 |

**Table 11**  Theoretical performance summary of mask operation instructions

<a name="table12905102516375"></a>
| API | Input Data Type | Theoretical Parallelism (Element/Cycle) |
| --- | --- | --- |
| SetMaskCount | - | 1 |
| SetMaskNorm | - | 1 |
| SetVectorMask | uint64_t | 1 |
| SetVectorMask | int32_t | 1 |
| ResetMask | - | 1 |

**Table 12**  Theoretical performance summary of scalar computation instructions

<a name="table12906133883716"></a>
| API | Input Data Type | Output Data Type | Theoretical Parallelism (cycle/instruction) |
| --- | --- | --- | --- |
| GetBitCount | uint64_t | int64_t | 1 |
| CountLeadingZero | uint64_t | int64_t | 1 |
| GetSFFValue | uint64_t | int64_t | 1 |
| CountBitsCntSameAsSignBit | int64_t | int64_t | 1 |
| Cast (float to half, int32_t) | float | half/int32_t | 1 |
| Cast (float to bfloat16_t) | - | - | Software-simulated instruction, not involving theoretical parallelism |
| Cast (bfloat16_t to float) | - | - | Software-simulated instruction, not involving theoretical parallelism |

**Table 13**  Summary of theoretical performance of tool API instructions

<a name="table1843362415383"></a>
| API | Theoretical Parallelism (Cycle/Instruction) |
| --- | --- |
| InitSocState | Software-simulated instruction; not involving theoretical parallelism |
| Async | Static compilation, no consumption |
| GetBlockNum | 1 |
| GetBlockIdx (AIC) | 1 |
| GetBlockIdx (AIV) | Software-simulated instruction; not involving theoretical parallelism |
| GetTaskRatio | 1 |
| GetSubBlockNum | 1 |
| GetSubBlockIdx | 1 |
| GetDataBlockSizeInBytes | Static compilation with no runtime overhead, and behaves the same as using an immediate value directly |
| GetSystemCycle | 1 |
| GetProgramCounter | Software-simulated instruction; not involving theoretical parallelism |
| GetArchVersion | Software-simulated instruction; not involving theoretical parallelism |

