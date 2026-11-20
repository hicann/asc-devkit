# Mask API Summary

<!-- npu="A3,910b" id3 -->
<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-17T03:12:29.565Z pushedAt=2026-11-17T09:52:41.747Z nl=2 -->

The following content applies to the following models:
<!-- npu="A3" id1 -->
- Atlas A3 products
<!-- end id1 -->
<!-- npu="910b" id2 -->
- Atlas A2 products
<!-- end id2 -->

This section summarizes the support for the isSetMask template parameter in each API.

For the two APIs of computing the first n numbers of a tensor and high-dimensional slicing computation of a tensor, the Mask setting methods fall into the following cases:

- isSetMask parameter cases:

    - The template parameter has isSetMask, and [setting Mask outside the API](../SIMD-API/basic_api/memory_vector_compute/SIMD_compute/mask.md#setting-the-mask-outside-the-api) is supported.

    - The template parameter does not have isSetMask, and setting Mask outside the API is not supported.

    - The template parameter has isSetMask, but this parameter does not take effect.

- Mask setting methods:

    - **Mask is already set inside the API**: The API internally calls [SetMaskCount](../SIMD-API/basic_api/memory_vector_compute/mask_operations/SetMaskCount.md), [SetVectorMask](../SIMD-API/basic_api/memory_vector_compute/mask_operations/SetVectorMask.md), [SetMaskNorm](../SIMD-API/basic_api/memory_vector_compute/mask_operations/SetMaskNorm.md), and [ResetMask](../SIMD-API/basic_api/memory_vector_compute/mask_operations/ResetMask.md).

    - **Controlled by the template parameter inside the API**: The isSetMask template parameter controls how Mask is set inside the API. When isSetMask is true, Mask is set inside the API; when isSetMask is false, Mask is set outside the API, and you need to use the [SetVectorMask](../SIMD-API/basic_api/memory_vector_compute/mask_operations/SetVectorMask.md) API to set the Mask value.

    - **The API does not support Mask or no Mask needs to be set**.

For more information about Mask, see [Computation Performance Optimization Based on Global Mask Reuse](../../guide/operator_practice/simd_operator_optimization/vector_compute/mask_reuse_optimization.md).

Note: `-` in the table indicates that no API of this type exists.

**Table 1**  Summary of isSetMask template parameter support for each API

| Category | API Name | Tensor First-n Element Computation | Tensor High-Dimensional Split Computation | 
| --- | --- | --- | --- |
| Basic Arithmetic | [Add](../SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Add.md), [Exp](../SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Exp.md), [Ln](../SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Ln.md), [Abs](../SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Abs.md), [Reciprocal](../SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Reciprocal.md), [Sqrt](../SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Sqrt.md), [Rsqrt](../SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Rsqrt.md), [Relu](../SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Relu.md), [Sub](../SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Sub.md), [Mul](../SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Mul.md), [Div](../SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Div.md), [Max](../SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Max.md), [Min](../SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Min.md) | &bull; No isSetMask.<br>&bull; Mask is set inside the API. | &bull; Has isSetMask.<br>&bull; Controlled by the template parameter inside the API. |
| Basic Arithmetic | [BilinearInterpolation](../SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/BilinearInterpolation_ISASI.md) | - | &bull; No isSetMask.<br>&bull; Mask is set inside the API. |
| Basic Arithmetic | [Adds](../SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Adds.md), [Muls](../SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Muls.md), [Maxs](../SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Maxs.md), [Mins](../SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Mins.md), [LeakyRelu](../SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/LeakyRelu.md) | &bull; Has isSetMask.<br>&bull; Mask is set inside the API. | &bull; Has isSetMask.<br>&bull; Controlled by the template parameter inside the API. |
| Logical Calculation | [Not](../SIMD-API/basic_api/memory_vector_compute/logical_compute/Not.md), [And](../SIMD-API/basic_api/memory_vector_compute/logical_compute/And.md), [Or](../SIMD-API/basic_api/memory_vector_compute/logical_compute/Or.md) | &bull; No isSetMask.<br>&bull; Mask is set inside the API. | &bull; Has isSetMask.<br>&bull; Controlled by the template parameter inside the API. |
| Logical Calculation | [ShiftLeft](../SIMD-API/basic_api/memory_vector_compute/logical_compute/ShiftLeft.md), [ShiftRight](../SIMD-API/basic_api/memory_vector_compute/logical_compute/ShiftRight.md) | &bull; Has isSetMask.<br>&bull; Controlled by the template parameter inside the API. | &bull; Has isSetMask.<br>&bull; Controlled by the template parameter inside the API. |
| Compound Calculation | [Axpy](../SIMD-API/basic_api/memory_vector_compute/composite_compute/Axpy.md), [MulAddDst](../SIMD-API/basic_api/memory_vector_compute/composite_compute/MulAddDst.md), [MulCast](../SIMD-API/basic_api/memory_vector_compute/composite_compute/MulCast.md), [AddRelu](../SIMD-API/basic_api/memory_vector_compute/composite_compute/AddRelu.md), [AddReluCast](../SIMD-API/basic_api/memory_vector_compute/composite_compute/AddReluCast.md), [SubRelu](../SIMD-API/basic_api/memory_vector_compute/composite_compute/SubRelu.md), [SubReluCast](../SIMD-API/basic_api/memory_vector_compute/composite_compute/SubReluCast.md), [FusedMulAdd](../SIMD-API/basic_api/memory_vector_compute/composite_compute/FusedMulAdd.md), [MulAddRelu](../SIMD-API/basic_api/memory_vector_compute/composite_compute/MulAddRelu.md), [CastDequant](../SIMD-API/basic_api/memory_vector_compute/composite_compute/CastDequant.md), [AddDeqRelu](../SIMD-API/basic_api/memory_vector_compute/composite_compute/AddDeqRelu.md) | &bull; No isSetMask.<br>&bull; Mask is set inside the API. | &bull; Has isSetMask.<br>&bull; Controlled by the template parameter inside the API. |
| Comparison and selection | [Compare](../SIMD-API/basic_api/memory_vector_compute/compare_and_select/Compare.md) | &bull; No isSetMask.<br>&bull; The API does not support Mask. | &bull; Has isSetMask, which does not take effect.<br>&bull; No Mask needs to be set for the API. |
| Comparison and selection | [Compare (result stored in register)](../SIMD-API/basic_api/memory_vector_compute/compare_and_select/Compare_store_to_register.md) | - | &bull; Has isSetMask.<br>&bull; Controlled by the template parameter inside the API. |
| Comparison and selection | [Compares](../SIMD-API/basic_api/memory_vector_compute/compare_and_select/Compares.md) | &bull; No isSetMask.<br>&bull; The API does not support Mask. | &bull; Has isSetMask.<br>&bull; No Mask needs to be set for the API. |
| Comparison and selection | [Select](../SIMD-API/basic_api/memory_vector_compute/compare_and_select/Select.md) | &bull; No isSetMask.<br>&bull; Mask is set inside the API. | &bull; Has isSetMask, which does not take effect.<br>&bull; To set Mask outside the API, call the [API that does not pass the mask parameter](../SIMD-API/basic_api/memory_vector_compute/compare_and_select/Select.md#prototype). |
| Comparison and selection | [GatherMask](../SIMD-API/basic_api/memory_vector_compute/compare_and_select/GatherMask.md) | - | &bull; No isSetMask.<br>&bull; Mask is set inside the API. |
| Type Conversion | [Cast](../SIMD-API/basic_api/memory_vector_compute/type_conversion/Cast.md) | &bull; No isSetMask.<br>&bull; Mask is set inside the API. | &bull; Has isSetMask.<br>&bull; Controlled by the template parameter inside the API. |
| Reduction Computation | [ReduceDataBlock](../SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReduceDataBlock.md), [ReduceRepeat](../SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReduceRepeat.md), [ReducePairElem](../SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReducePairElem.md) | - | &bull; Has isSetMask.<br>&bull; Controlled by the template parameter inside the API. |
| Reduction Computation | [ReduceSum](../SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReduceSum.md) | &bull; Has isSetMask.<br>&bull; Controlled by the template parameter inside the API. | &bull; No isSetMask.<br>&bull; Mask is set inside the API. |
| Reduction Computation | [ReduceMax](../SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReduceMax.md), [ReduceMin](../SIMD-API/basic_api/memory_vector_compute/reduction_compute/ReduceMin.md) | &bull; No isSetMask.<br>&bull; Mask is set inside the API. | &bull; No isSetMask.<br>&bull; Mask is set inside the API. |
| Data Layout Conversion | [Transpose](../SIMD-API/basic_api/memory_vector_compute/data_layout_conversion/Transpose.md), [TransDataTo5HD](../SIMD-API/basic_api/memory_vector_compute/data_layout_conversion/TransDataTo5HD.md) | &bull; No isSetMask.<br>&bull; The API does not support Mask. | &bull; No isSetMask.<br>&bull; The API does not support Mask. |
| Data padding | [Brcb](../SIMD-API/basic_api/memory_vector_compute/data_padding/Brcb.md) | - | &bull; No isSetMask.<br>&bull; The API does not support Mask. |
| Data padding | [Duplicate](../SIMD-API/basic_api/memory_vector_compute/data_padding/Duplicate.md) | &bull; No isSetMask.<br>&bull; Mask is set inside the API. | &bull; Has isSetMask.<br>&bull; Controlled by the template parameter inside the API. |
| Data padding | [CreateVecIndex](../SIMD-API/basic_api/memory_vector_compute/data_padding/CreateVecIndex.md) | &bull; No isSetMask.<br>&bull; The API does not support Mask. | &bull; No isSetMask.<br>&bull; The API does not support Mask. |
| Sort and Merge (ISASI) | [Sort32](../SIMD-API/basic_api/memory_vector_compute/sort_and_merge_ISASI/Sort32.md), [MrgSort](../SIMD-API/basic_api/memory_vector_compute/sort_and_merge_ISASI/MrgSort.md) | - | &bull; No isSetMask.<br>&bull; The API does not support Mask. |
| Scatter and Gather | [Gather](../SIMD-API/basic_api/memory_vector_compute/scatter_gather/Gather.md), [Gatherb](../SIMD-API/basic_api/memory_vector_compute/scatter_gather/Gatherb_ISASI.md) | &bull; No isSetMask.<br>&bull; The API does not support Mask. | &bull; No isSetMask.<br>&bull; The API does not support Mask. |
| Mask Operations | [SetMaskCount](../SIMD-API/basic_api/memory_vector_compute/mask_operations/SetMaskCount.md), [SetMaskNorm](../SIMD-API/basic_api/memory_vector_compute/mask_operations/SetMaskNorm.md), [SetVectorMask](../SIMD-API/basic_api/memory_vector_compute/mask_operations/SetVectorMask.md), [ResetMask](../SIMD-API/basic_api/memory_vector_compute/mask_operations/ResetMask.md) | - | - |

<!-- end id3 -->
