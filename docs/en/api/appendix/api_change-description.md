# API Change Description<a name="ZH-CN_TOPIC_0000002547538511"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-09-06T08:33:53.501Z pushedAt=2026-09-10T08:31:16.427Z -->

To optimize API naming and make it clearer and easier to understand, some API names have been changed in the current version compared with CANN8.5.0 and earlier versions. For details, see the following table.

| Original API Name | New API Name | Remarks |
|---|---|---|
| ScalarGetCountOfValue | [GetBitCount](../SIMD-API/basic_api/scalar_compute/GetBitCount.md) | None |
| ScalarCountLeadingZero | [CountLeadingZero](../SIMD-API/basic_api/scalar_compute/CountLeadingZero.md) | None |
| ScalarGetSFFValue | [GetSFFValue](../SIMD-API/basic_api/scalar_compute/GetSFFValue.md) | None |
| ScalarCast | [Cast (float to half, int32_t)](../SIMD-API/basic_api/scalar_compute/Cast_float_to_half_int32.md) | None |
| ToBfloat16 | [Cast (float to bfloat16_t)](../SIMD-API/basic_api/scalar_compute/Cast_float_to_bfloat16.md) | None |
| ToFloat | [Cast (multi-type to float)](../SIMD-API/basic_api/scalar_compute/Cast_multi_type_to_float.md) | The return value type of one overloaded version is changed from **float** to template **U**, where `U = float`. |
| FusedMulAddRelu | [MulAddRelu](../SIMD-API/basic_api/memory_vector_compute/composite_compute/MulAddRelu.md) | None |
| CastDeq | [CastDequant](../SIMD-API/basic_api/memory_vector_compute/composite_compute/CastDequant.md) | None |
| CompareScalar | [Compares](../SIMD-API/basic_api/memory_vector_compute/compare_and_select/Compares.md) | None |
| GetAccVal | [GetReduceRepeatSumSpr](../SIMD-API/basic_api/memory_vector_compute/reduction_compute_aux_config/GetReduceRepeatSumSpr_ISASI.md) | None |
| GetReduceMaxMinCount | [GetReduceRepeatMaxMinSpr](../SIMD-API/basic_api/memory_vector_compute/reduction_compute_aux_config/GetReduceRepeatMaxMinSpr_ISASI.md) | None |
| SetAtomicNone | [DisableDmaAtomic](../SIMD-API/basic_api/atomic_operations/DisableDmaAtomic.md) | None |
| InitConstValue | [Fill](../SIMD-API/basic_api/cube_compute_ISASI/cube_load_aux_config/Fill.md) | None |
| SetMMLayoutTransform | [SetMMRowMajor](../SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_aux_config/SetMMColumnMajor-SetMMRowMajor.md)/[SetMMColumnMajor](../SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_aux_config/SetMMColumnMajor-SetMMRowMajor.md) | SetMMLayoutTransform(true) corresponds to SetMMRowMajor().<br>SetMMLayoutTransform(false) corresponds to SetMMColumnMajor(). |
| SetHF32Mode | [SetHF32Mode](../SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_aux_config/SetHF32Mode.md) | The API name remains unchanged, and the parameter type is changed from **bool** to the **HF32Mode** enum. |
| SetHF32TransMode | [SetHF32TransMode](../SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_aux_config/SetHF32TransMode.md) | The API name remains unchanged, and the parameter type is changed from bool to the HF32TransMode enum. |
| CalcTschBlockDim | [CalcTschNumBlocks](../Utils-API/platform_info/PlatformAscendC/CalcTschNumBlocks.md) | None |

The APIs in the following table were modified in 9.1.0. Compared with CANN9.0.0 and earlier versions, some API names have been changed. For details, see the following table.

| Original API Name | New API Name | Remarks |
|---|---|---|
| BlockReduceSum / BlockReduceMax / BlockReduceMin | [ReduceDataBlock](../SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReduceDataBlock.md) | &bull;Provides a unified **DataBlock** reduction operation API, distinguishing different reduction operations through template parameters. <br>&bull;Adjusts the parameter order of **mask** and **repeatTime**. |
| WholeReduceSum / WholeReduceMax / WholeReduceMin | [ReduceRepeat](../SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReduceRepeat.md) | Provides a unified **Repeat** reduction operation API, distinguishing different reduction operations through template parameters. |
| RepeatReduceSum | [ReduceRepeat](../SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReduceRepeat.md) | &bull;Provides a unified **Repeat** reduction operation API, distinguishing different reduction operations through template parameters. <br>&bull;Adjusts the parameter order of **mask** and **repeatTime**. <br>&bull;Removes the invalid parameter **dstBlkStride**. |
| PairReduceSum | [ReducePairElem](../SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReducePairElem.md) | &bull;Provides a unified **Pair** element reduction operation API, distinguishing different reduction operations through template parameters. <br>&bull;Adjusts the parameter order of **mask** and **repeatTime**. |
