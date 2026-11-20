# Overview

<!-- md-trans-meta sourceCommit=7f211052f814e6d340cc6bf2dfc44c8393b5424f translatedAt=2026-11-02T07:20:06.075Z pushedAt=2026-11-18T01:25:07.058Z nl=2 -->

Compare and Compares are comparison APIs that compare the sizes of Tensor or Scalar element by element and generate a mask based on the comparison result. Select and GatherMask are selection APIs that select elements from source operands based on mask conditions and output them to the destination operand. Recommended use cases are as follows:

**Table 1** Recommended use cases for comparison and selection APIs

| API | Core Function | Mask Type | Use Case |
| --- | --- | --- | --- |
| [Compare](Compare.md) | Compares the sizes of two tensors element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0. Supports multiple comparison modes (equal to, less than, greater than, ...).<br>Does not support mask setting for Vector computation. Stores the result in UB. | Not used | Element-by-element comparison, with the result written to memory. |
| [Compare (store to register)](Compare_store_to_register.md) | Compares the sizes of two tensors element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0. Supports multiple comparison modes.<br>Supports mask setting for Vector computation. Temporarily stores the result in the CmpMask register. Refer to [Table 2 CmpMask and VectorMask](#cmpMask-vectorMask). | src passes mask for Vector computation;<br>the output result is CmpMask (comparison mask). | Element-by-element comparison, with the result written to the register. |
| [Compares](Compares.md) | Compares the elements of a tensor with another scalar element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0.<br>Supports multiple comparison modes. | Not used | Element-by-element comparison between a tensor and a scalar, with the result written to memory. |
| [Select](Select.md) | Given two source operands src0 and src1, selects elements based on the bit values of selMask (the mask used for selection).<br>Supports CmpMask setting and mask setting for Vector computation. Refer to [Table 2 CmpMask and VectorMask](#cmpMask-vectorMask). | src passes mask for Vector computation;<br>src passes selMask as CmpMask (comparison mask). | Selects output based on mask conditions. |
| [GetCmpMask(ISASI)](GetCmpMask_ISASI.md) | This API obtains the comparison result of the [Compare (store to register)](Compare_store_to_register.md) instruction. | CmpMask (comparison mask) | Reads the comparison mask from the register. |
| [SetCmpMask(ISASI)](SetCmpMask_ISASI.md) | Sets the comparison register for the [Select](Select.md) API that does not pass the mask parameter. | CmpMask (comparison mask) | Sets the comparison mask in the register. |
| [GatherMask](GatherMask.md) | Extracts valid elements from the input tensor based on the mask (used for Vector computation). | src1 passes GatherMask (data collection mask). | Extracts valid elements based on the mask. |

**Table 2** CmpMask and VectorMask<a id="cmpMask-vectorMask"></a>

| Name | Description | Data Source | Scope |
| --- | --- | --- | --- |
| CmpMask | The "result register" of the Compare instruction. | &bull;Written by the result of the [Compare (store to register)](Compare_store_to_register.md) API.<br>&bull; Passed through the selMask parameter of the Select API.<br>&bull;Set through the SetCmpMask API. | Used to record the Boolean result of element comparison, mainly for subsequent Select instructions. |
| VectorMask | A mask register consisting of a sequence of flag bits that controls whether data elements participate in computation. Refer to [mask](../SIMD_compute/mask.md). | &bull; [Setting mask inside the API](../SIMD_compute/mask.md#setting-the-mask-inside-the-api)<br>&bull; [Setting mask outside the API](../SIMD_compute/mask.md#setting-the-mask-outside-the-api) | Used to control vector computation element by element, determining which data elements participate in computation. |
