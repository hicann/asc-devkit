# Overview

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-08T13:55:19.401Z pushedAt=2026-09-09T07:33:53.611Z -->

Compare and Compares are comparison APIs used to compare the sizes of tensors or scalars element by element and generate a mask based on the comparison result. Select and GatherMask are selection APIs used to select elements from source operands based on mask conditions and output them to the destination operand. The recommended scenarios are as follows:

**Table 1** Recommended scenarios for comparison and selection APIs

| API | Core Function | Mask Type | Scenario |
| --- | --- | --- | --- |
| [Compare](Compare.md) | Compares the sizes of two tensors element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0. Supports multiple comparison modes (equal to, less than, greater than...).<br>Does not support mask setting for Vector computation. Stores the result in UB. | Not used | Element-by-element comparison, with the result written to memory. |
| [Compare (Result Stored in a Register)](Compare_store_to_register.md) | Compares the sizes of two tensors element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0. Supports multiple comparison modes.<br>Supports mask setting for Vector computation. Temporarily stores the result in the CmpMask register. For details, see [Table 2 CmpMask and VectorMask](#cmpMask-vectorMask). | The src parameter mask is used for Vector computation;<br>the output result is CmpMask (comparison mask). | Element-by-element comparison, with the result written to a register. |
| [Compares](Compares.md) | Compares the size of an element in a tensor with that of a scalar element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0.<br>Supports multiple comparison modes. | Not used | Element-by-element comparison between a tensor and a scalar, with the result written to memory. |
| [Select](Select.md) | Given two source operands src0 and src1, selects elements based on the bit values of selMask (the mask used for selection).<br>Supports CmpMask setting and mask setting for Vector computation. For details, see [Table 2 CmpMask and VectorMask](#cmpMask-vectorMask). | The src parameter mask is used for Vector computation;<br>the src parameter selMask is CmpMask (comparison mask). | Selecting output based on mask conditions. |
| [GetCmpMask (ISASI)](GetCmpMask_ISASI.md) | This API is used to obtain the comparison result of the [Compare (Result Stored in a Register)](Compare_store_to_register.md) instruction. | CmpMask (comparison mask) | Reading the comparison mask from a register. |
| [SetCmpMask (ISASI)](SetCmpMask_ISASI.md) | Sets the comparison register for the [Select](Select.md) API that does not pass the mask parameter. | CmpMask (comparison mask) | Setting the comparison mask in a register. |
| [GatherMask](GatherMask.md) | Extracts valid elements from the input tensor based on the mask (used for Vector computation). | The src1 parameter passes GatherMask (data collection mask). | Extracting valid elements by mask. |

**Table 2** CmpMask and VectorMask<a id="cmpMask-vectorMask"></a>

| Name | Description | Data Source | Scope |
| --- | --- | --- | --- |
| CmpMask | The "result register" of the Compare instruction. | &bull; Written by the result of the [Compare (Result Stored in a Register)](Compare_store_to_register.md) API.<br>&bull; Passed in through the selMask parameter of the Select API.<br>&bull; Set through the SetCmpMask API. | Used to record the Boolean result of element comparison, mainly for use by subsequent Select instructions. |
| VectorMask | A mask register that controls the sequence of flag bits indicating whether data elements participate in computation. For details, see [Mask](../SIMD_compute/mask.md). | &bull; [Setting the mask inside the API](../SIMD_compute/mask.md)<br>&bull; [Setting the mask outside the API](../SIMD_compute/mask.md) | Used to control vector computation element by element and determine which data elements participate in computation. |
