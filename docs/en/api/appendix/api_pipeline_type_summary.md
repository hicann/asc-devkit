# API Pipeline Type Summary<a name="ZH-CN_TOPIC_0000002533354434"></a>

<!-- md-trans-meta sourceCommit=c14ae98d8405d60be337fd017a9005dd55037dc9 translatedAt=2026-09-06T08:38:01.692Z pushedAt=2026-09-07T03:48:44.352Z -->

This section summarizes the pipeline types of some major APIs. The following content applies to the following models:

<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products
<!-- end id3 -->

**Table 1**  Memory vector computation API pipeline type summary

<a name="table132070121817"></a>
| API Type | API Name | Execution Core (AIC/AIV) | Pipeline Type |
| --- | --- | --- | --- |
| Basic Arithmetic | Exp | AIV | PIPE_V |
| Basic Arithmetic | Ln | AIV | PIPE_V |
| Basic Arithmetic | Abs | AIV | PIPE_V |
| Basic Arithmetic | Reciprocal | AIV | PIPE_V |
| Basic Arithmetic | Sqrt | AIV | PIPE_V |
| Basic Arithmetic | Rsqrt | AIV | PIPE_V |
| Basic Arithmetic | Relu | AIV | PIPE_V |
| Basic Arithmetic | Sub | AIV | PIPE_V |
| Basic Arithmetic | Mul | AIV | PIPE_V |
| Basic Arithmetic | Div | AIV | PIPE_V |
| Basic Arithmetic | Max | AIV | PIPE_V |
| Basic Arithmetic | Min | AIV | PIPE_V |
| Basic Arithmetic | Adds | AIV | PIPE_V |
| Basic Arithmetic | Muls | AIV | PIPE_V |
| Basic Arithmetic | Maxs | AIV | PIPE_V |
| Basic Arithmetic | Mins | AIV | PIPE_V |
| Basic Arithmetic | LeakyRelu | AIV | PIPE_V |
| Logical Computation | Not | AIV | PIPE_V |
| Logical Computation | And | AIV | PIPE_V |
| Logical Computation | Or | AIV | PIPE_V |
| Logical Computation | ShiftLeft | AIV | PIPE_V |
| Logical Computation | ShiftRight | AIV | PIPE_V |
| Compound Computation | Axpy | AIV | PIPE_V |
| Compound Computation | MulAddDst | AIV | PIPE_V |
| Compound Computation | MulCast | AIV | PIPE_V |
| Compound Computation | AddRelu | AIV | PIPE_V |
| Compound Computation | AddReluCast | AIV | PIPE_V |
| Compound Computation | SubRelu | AIV | PIPE_V |
| Compound Computation | SubReluCast | AIV | PIPE_V |
| Compound Computation | FusedMulAdd | AIV | PIPE_V |
| Compound Computation | MulAddRelu | AIV | PIPE_V |
| Compound Computation | CastDequant | AIV | PIPE_V |
| Compound Computation | AddDeqRelu | AIV | PIPE_V |
| Comparison and Selection | Compare | AIV | PIPE_V |
| Comparison and Selection | Compare (result stored in register) | AIV | PIPE_V |
| Comparison and Selection | Compares | AIV | PIPE_V |
| Comparison and Selection | GetCmpMask(ISASI) | AIV | PIPE_V |
| Comparison and Selection | SetCmpMask(ISASI) | AIV | PIPE_V |
| Comparison and Selection | Select | AIV | PIPE_V |
| Comparison and Selection | GatherMask | AIV | PIPE_V |
| Type Casting | Cast | AIV | PIPE_V |
| Reduction Computation | ReduceDataBlock | AIV | PIPE_V |
| Reduction Computation | ReduceRepeat | AIV | PIPE_V |
| Reduction Computation | ReducePairElem | AIV | PIPE_V |
| Reduction Computation | ReduceMax | AIV | PIPE_V |
| Reduction Computation | ReduceMin | AIV | PIPE_V |
| Reduction Computation | ReduceSum | AIV | PIPE_V |
| Reduction Computation | GetReduceRepeatSumSpr | AIV | PIPE_S |
| Reduction Computation | GetReduceRepeatMaxMinSpr | AIV | PIPE_S |
| Data Layout Conversion | Transpose | AIV | PIPE_V |
| Data Layout Conversion | TransDataTo5HD | AIV | PIPE_V |
| Data Fill | Brcb | AIV | PIPE_V |
| Data Fill | Duplicate | AIV | PIPE_V |
| Data Fill | CreateVecIndex | AIV | PIPE_V |
| Sort and Combine | Sort32 | AIV | PIPE_V |
| Sort and Combine | MrgSort | AIV | PIPE_V |
| Sort and Combine | GetMrgSortResult | AIV | PIPE_S |
| Discrete and Aggregate | Gather | AIV | PIPE_V |
| Discrete and Aggregate | Gatherb | AIV | PIPE_V |
| Mask Operation | SetMaskCount | AIV | PIPE_S |
| Mask Operation | SetMaskNorm | AIV | PIPE_S |
| Mask Operation | SetVectorMask | AIV | PIPE_S |
| Mask Operation | ResetMask | AIV | PIPE_S |
| Data Movement | DataCopy(GM->UB) | AIV | PIPE_MTE2 |
| Data Movement | DataCopy(UB->GM) | AIV | PIPE_MTE3 |
| Data Movement | DataCopyPad(GM->UB) | AIV | PIPE_MTE2 |
| Data Movement | DataCopyPad(UB->GM) | AIV | PIPE_MTE3 |
| Data Movement | DataCopy(UB->UB) | AIV | PIPE_V |
| Data Movement | Copy | AIV | PIPE_V |

**Table 2**  Cube API pipeline type summary

<a name="table12311219174812"></a>
| API Name | Execution Core (AIC/AIV) | Pipeline Type |
| --- | --- | --- |
| DataCopy(GM->L1) | AIC | PIPE_MTE2 |
| Load2D(GM->L1) | AIC | PIPE_MTE2 |
| Load2DV2(GM->L1) | AIC | PIPE_MTE2 |
| Load2D(GM->L0) | AIC | PIPE_MTE2 |
| Load2D(L1->L0) | AIC | PIPE_MTE1 |
| Load3D(L1->L0) | AIC | PIPE_MTE1 |
| LoadDataWithTranspose(L1->L0) | AIC | PIPE_MTE1 |
| LoadDataWithSparse(L1->L0) | AIC | PIPE_MTE1 |
| Bias Table Buffer(L1->BT) | AIC | PIPE_MTE1 |
| Fixpipe Buffer(L1->FB) | AIC | PIPE_MTE1 |
| Fill(L1) | AIC | PIPE_MTE2 |
| Fill(L0) | AIC | PIPE_MTE1 |
| SetFmatrix | AIC | PIPE_S |
| SetLoadDataBoundary | AIC | PIPE_S |
| SetLoadDataRepeat | AIC | PIPE_S |
| SetLoadDataPaddingValue | AIC | PIPE_S |
| Mmad | AIC | PIPE_M |
| MmadWithSparse | AIC | PIPE_M |
| SetMMColumnMajor/SetMMRowMajor | AIC | PIPE_S |
| SetHF32Mode | AIC | PIPE_S |
| SetHF32TransMode | AIC | PIPE_S |
| DataCopy(L0C->GM) | AIC | PIPE_FIX |
| DataCopy(L0C->L1) | AIC | PIPE_FIX |
| DataCopy(L1->GM) | AIC | PIPE_MTE3 |
| Fixpipe(L0C->GM) | AIC | PIPE_FIX |
| Fixpipe(L0C->L1) | AIC | PIPE_FIX |
| SetFixPipeConfig | AIC | PIPE_S |
| SetFixpipePreQuantFlag | AIC | PIPE_S |
| SetFixpipeNz2ndFlag | AIC | PIPE_S |
| SetLreluALPHA | AIC | PIPE_S |

**Table 3**  Scalar computation instruction pipeline type summary

| API Name | Pipeline Type |
| --- | --- |
| GetBitCount | PIPE_S |
| CountLeadingZero | PIPE_S |
| GetSFFValue | PIPE_S |
| CountBitsCntSameAsSignBit | PIPE_S |
| Cast (float to half, int32_t) | PIPE_S |
| Cast (float to bfloat16_t) | PIPE_S |
| Cast (bfloat16_t to float) | PIPE_S |
| ReadGmByPassDCache(ISASI) | PIPE_S |
| WriteGmByPassDCache(ISASI) | PIPE_S |

**Table 4**  Synchronization instruction pipeline type summary

<a name="table1054353913238"></a>
| API Name | Pipeline Type |
| --- | --- |
| SetFlag/WaitFlag | PIPE_S |
| PipeBarrier | PIPE_S |
| DataSyncBarrier | PIPE_S |
| CrossCoreSetFlag | PIPE_S |
| CrossCoreWaitFlag | PIPE_S |
| SyncAll (hardware synchronization API) | PIPE_S |
| SetNextTaskStart | PIPE_S |
| WaitPreTaskEnd | PIPE_S |

**Table 5**  Atomic operation pipeline type summary

<a name="table25871912193814"></a>
| API Name | Pipeline Type |
| --- | --- |
| SetAtomicAdd | PIPE_S |
| SetAtomicMax | PIPE_S |
| SetAtomicMin | PIPE_S |
| SetAtomicType | PIPE_S |
| DisableDmaAtomic | PIPE_S |
| GetStoreAtomicConfig(ISASI) | PIPE_S |
| SetStoreAtomicConfig(ISASI) | PIPE_S |

**Table 6**  Tool API pipeline type summary

<a name="table1147331317457"></a>
| API Name | Pipeline Type |
| --- | --- |
| InitSocState | PIPE_S |
| GetBlockNum | PIPE_S |
| GetBlockIdx | PIPE_S |
| GetTaskRatio | PIPE_S |
| GetSubBlockNum | PIPE_S |
| GetSubBlockIdx | PIPE_S |
| GetDataBlockSizeInBytes | PIPE_S |
| GetSystemCycle | PIPE_S |
| GetProgramCounter | PIPE_S |
| GetArchVersion | PIPE_S |

**Table 7**  Cache control API pipeline type summary

| API Name | Pipeline Type |
| --- | --- |
| DataCachePreload | PIPE_S |
| DataCacheCleanAndInvalid | PIPE_S |
| ICachePreLoad(ISASI) | PIPE_S |
| GetICachePreloadStatus(ISASI) | PIPE_S |

