# 接口变更说明<a name="ZH-CN_TOPIC_0000002547538511"></a>

为了优化API命名，使其更加清晰和易于理解，当前版本相比于CANN8.5.0及前序版本，部分API命名有变更，具体变更情况请参考以下表格：

| 原接口名 | 新接口名 | 备注 |
|---|---|---|
| ScalarGetCountOfValue | [GetBitCount](../SIMD-API/basic_api/scalar_compute/GetBitCount.md) | 无 |
| ScalarCountLeadingZero | [CountLeadingZero](../SIMD-API/basic_api/scalar_compute/CountLeadingZero.md) | 无 |
| ScalarGetSFFValue | [GetSFFValue](../SIMD-API/basic_api/scalar_compute/GetSFFValue.md) | 无 |
| ScalarCast | [Cast（float转half、int32_t）](../SIMD-API/basic_api/scalar_compute/Cast_float_to_half_int32.md) | 无 |
| ToBfloat16 | [Cast（float转bfloat16_t）](../SIMD-API/basic_api/scalar_compute/Cast_float_to_bfloat16.md) | 无 |
| ToFloat | [Cast（多类型转float）](../SIMD-API/basic_api/scalar_compute/Cast_multi_type_to_float.md) | 其中一个重载版本的返回值类型由float改为模板U, `U = float`。 |
| FusedMulAddRelu | [MulAddRelu](../SIMD-API/basic_api/memory_vector_compute/composite_compute/MulAddRelu.md) | 无 |
| CastDeq | [CastDequant](../SIMD-API/basic_api/memory_vector_compute/composite_compute/CastDequant.md) | 无 |
| CompareScalar | [Compares](../SIMD-API/basic_api/memory_vector_compute/compare_and_select/Compares.md) | 无 |
| GetAccVal | [GetReduceRepeatSumSpr](../SIMD-API/basic_api/memory_vector_compute/reduction_compute_aux_config/GetReduceRepeatSumSpr_ISASI.md) | 无 |
| GetReduceMaxMinCount | [GetReduceRepeatMaxMinSpr](../SIMD-API/basic_api/memory_vector_compute/reduction_compute_aux_config/GetReduceRepeatMaxMinSpr_ISASI.md) | 无 |
| SetAtomicNone | [DisableDmaAtomic](../SIMD-API/basic_api/atomic_operations/DisableDmaAtomic.md) | 无 |
| InitConstValue | [Fill](../SIMD-API/basic_api/cube_compute_ISASI/cube_load_aux_config/Fill.md) | 无 |
| SetMMLayoutTransform | [SetMMRowMajor](../SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_aux_config/SetMMColumnMajor-SetMMRowMajor.md)/[SetMMColumnMajor](../SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_aux_config/SetMMColumnMajor-SetMMRowMajor.md) | SetMMLayoutTransform(true)对应SetMMRowMajor()。<br>SetMMLayoutTransform(false)对应SetMMColumnMajor()。 |
| SetHF32Mode | [SetHF32Mode](../SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_aux_config/SetHF32Mode.md) | 接口名不变，参数类型由bool修改为枚举HF32Mode。 |
| SetHF32TransMode | [SetHF32TransMode](../SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_aux_config/SetHF32TransMode.md) | 接口名不变，参数类型由bool修改为枚举HF32TransMode。 |
| CalcTschBlockDim | [CalcTschNumBlocks](../Utils-API/platform_info/PlatformAscendC/CalcTschNumBlocks.md) | 无 |

以下表格的接口于9.1.0修改，相比于CANN9.0.0及前序版本，部分API命名有变更，具体变更情况请参考以下表格：

| 原接口名 | 新接口名 | 备注 |
|---|---|---|
| BlockReduceSum / BlockReduceMax / BlockReduceMin | [ReduceDataBlock](../SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReduceDataBlock.md) | &bull;提供统一的DataBlock归约操作接口，通过模板参数区分不同归约操作。 <br>&bull;调整mask与repeatTime的参数顺序。 |
| WholeReduceSum / WholeReduceMax / WholeReduceMin | [ReduceRepeat](../SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReduceRepeat.md) | 提供统一的Repeat归约操作接口，通过模板参数区分不同归约操作。 |
| RepeatReduceSum | [ReduceRepeat](../SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReduceRepeat.md) | &bull;提供统一的Repeat归约操作接口，通过模板参数区分不同归约操作。 <br>&bull;调整mask与repeatTime的参数顺序。 <br>&bull;删除无效参数dstBlkStride。 |
| PairReduceSum | [ReducePairElem](../SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReducePairElem.md) | &bull;提供统一的Pair元素归约操作接口，通过模板参数区分不同归约操作。 <br>&bull;调整mask与repeatTime的参数顺序。 |
