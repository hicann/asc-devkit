# gather

<!-- md-trans-meta sourceCommit=db86c75495b5d041b217977e24030960f4f2178e translatedAt=2026-11-19T02:49:12.129Z pushedAt=2026-11-19T05:54:06.663Z nl=2 -->

## Supported Products

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
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Function Description

The header file path is `"tensor_api/experimental/arch/vector/gather_compute.h"`, and the namespace is `asc::te::experimental`.

Gathers elements from the source register `src` according to the per-element indexes stored in `index`, and generates a new [reg_tensor](../reg_tensor/reg_tensor.md). The indexes are in units of elements, not byte offsets.

Let `VL_T` be the number of `DataType` elements that a vector data register can hold. The computation relationship is `dst.reg[i] = src.reg[index.reg[i] % VL_T]`. For example, if the first four elements of `src` are `{10, 20, 30, 40}` and the first four elements of `index` are `{3, 1, 0, 2}`, then the first four elements of the result are `{40, 20, 10, 30}`.

This API takes effect only on the AIV.

## Prototype

```cpp
template <typename DataType, typename IndexType>
__simd_callee__ inline reg_tensor<DataType> gather(const reg_tensor<DataType>& src, const reg_tensor<IndexType>& index);
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| DataType | Element type of the source operand and the returned result. For supported type combinations, see [data type](#data-type). |
| IndexType | Element type of the index register, which must be an unsigned integer type with the same bit width as DataType. |

**Table 2** Function parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source register, of type `reg_tensor<DataType>`. |
| index | Input | Index register, of type `reg_tensor<IndexType>`, where `index.reg` provides the per-element indexes. |

## Data Type

| DataType | IndexType |
| --- | --- |
| int8_t, uint8_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t | uint8_t |
| int16_t, uint16_t, half, bfloat16_t | uint16_t |
| int32_t, uint32_t, float | uint32_t |

## Return Value

Returns `reg_tensor<DataType>`, whose `reg` saves the collected data and whose `mask` is `all_mask<DataType>().reg`, that is, the all-valid mask set according to the resultant element type.

## Constraints

- When an index exceeds the range of elements that the source register can hold, it is accessed after taking the modulo as `index.reg[i] % VL_T`.
- When only part of the result is needed, rebind the valid mask through [with_mask](../reg_tensor/reg_tensor.md#with_mask) before subsequent computation or write-back.

## Example

The following code snippet is called in the VF. The source data and the index each occupy a full register, all index values and the indexed source elements are initialized, and the destination tensor has space to accommodate the result of one register. The example first moves data from the UB to the register, then collects and writes back all results. No mask needs to be bound for the input, and the return result carries an all-valid mask.

```cpp
template <typename SrcTensor, typename IndexTensor, typename DstTensor>
__simd_vf__ inline void gather_vf(SrcTensor src_tensor, IndexTensor index_tensor, DstTensor dst_tensor)
{
    const auto coord = asc::te::make_coord(0);
    auto src_reg = asc::te::experimental::load(src_tensor, coord);
    auto index_reg = asc::te::experimental::load(index_tensor, coord);

    auto result = asc::te::experimental::gather(src_reg, index_reg);
    asc::te::experimental::store(dst_tensor, coord, result);
}
```
