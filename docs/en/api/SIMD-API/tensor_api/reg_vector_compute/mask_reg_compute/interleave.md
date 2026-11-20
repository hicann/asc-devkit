# interleave (mask interleave)
<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-10-27T03:52:25.942Z pushedAt=2026-10-27T13:03:35.433Z nl=2 -->

 
## Applicable Products
 
<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- AI Core in Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Vector Core in Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
 
## Description
 
The header file path is `"tensor_api/experimental/arch/vector/mask_reg_compute.h"`, and the namespace is `asc::te::experimental`.
 
Interleaves the `reg` of two `reg_tensor<bool>` registers according to the element width corresponding to `DataType`, and stores the lower half and upper half of the interleaved result in `first` and `second` of the return value, respectively.
 
For the b8, b16, and b32 data types, this API interleaves in groups of 1 bit, 2 bits, and 4 bits, respectively. The groups in the first half of `src0` and `src1` are written alternately into `first.reg`, and the groups in the second half are written alternately into `second.reg`.
 
This API processes only registers whose input is of the `reg_tensor<bool>` type. For the interleave API of data registers, see [data reorder](../data_reorder/data_reorder.md).
 
## Function Prototype
 
```cpp
template <typename DataType>
__simd_callee__ inline reg_pair<bool> interleave(
    reg_tensor<bool> src0, reg_tensor<bool> src1)
```
 
## Parameters
 
| Parameter | Input/Output | Description |
| --- | --- | --- |
| DataType | Input | Template parameter that determines the element grouping width of the Mask. It supports the b8, b16, and b32 data types. |
| src0 | Input | First source Mask register, of the `reg_tensor<bool>` type. Its `reg` member must be initialized. |
| src1 | Input | Second source Mask register, of the `reg_tensor<bool>` type. Its `reg` member must be initialized. |
 
## Return Value
 
Returns `reg_pair<bool>`:
 
- `first.reg` stores the lower half of the interleaved result.
- `second.reg` stores the upper half of the interleaved result.
 
The returned object guarantees only that `first.reg` and `second.reg` are valid; it does not initialize the `mask` member of either.
 
## Constraints
 
- `src0` and `src1` can be the same `reg_tensor<bool>` object.
- This API must be called within a Vector Function (a function marked with `__simd_vf__`).
- The maximum number of Mask registers is 8. Register data exceeding this limit is written to the reserved 8K UB memory, which may cause performance degradation.
 
## Example
 
```cpp
auto mask0 = asc::te::experimental::make_mask<
    asc::te::experimental::mask_pattern::every3, float>();
auto mask1 = asc::te::experimental::make_mask<
    asc::te::experimental::mask_pattern::every4, float>();
 
auto result = asc::te::experimental::interleave<float>(mask0, mask1);
asc::te::experimental::reg_tensor<bool> lowMask = result.first;
asc::te::experimental::reg_tensor<bool> highMask = result.second;
```
