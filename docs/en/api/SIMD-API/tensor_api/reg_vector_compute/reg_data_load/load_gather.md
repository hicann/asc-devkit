# load_gather

<!-- md-trans-meta sourceCommit=183e1cd500571375b0faeb56d73e22492d82eea8 translatedAt=2026-11-19T02:54:14.351Z pushedAt=2026-11-19T05:54:06.671Z nl=2 -->

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

The header file path is `"tensor_api/experimental/arch/vector/reg_data_load.h"`, and the namespace is `asc::te::experimental`.

`load_gather` gathers data from discrete positions of a Unified Buffer (UB) Tensor to [reg_tensor](../reg_tensor/reg_tensor.md) based on the index register. `gather_mode::element` indicates gathering by element, and `gather_mode::data_block` indicates gathering by DataBlock (32B).

When `coord` is passed in, `tensor.engine() + tensor.layout()(coord)` is used as the base address; when `coord` is not passed in, the start address of `tensor` is used as the base address.

## Prototype

```cpp
template <gather_mode mode = gather_mode::element,
          typename Tensor, typename Coord, typename Index>
__simd_callee__ inline auto load_gather(
    const Tensor& tensor,
    const Coord& coord,
    const reg_tensor<Index> index,
    const reg_tensor<bool>& gather_mask)

template <gather_mode mode = gather_mode::element,
          typename Tensor, typename Index>
__simd_callee__ inline auto load_gather(
    const Tensor& tensor,
    const reg_tensor<Index> index,
    const reg_tensor<bool>& gather_mask)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| `mode` | Gather mode, of type `gather_mode`, with a default value of `gather_mode::element`. For specific values, see [Table 3](#table-3-gather_mode-parameter-description). |
| `Tensor` | Source Tensor type, automatically deduced from the function argument. The Tensor must be located in the UB. |
| `Coord` | Coordinate type of the source Tensor, automatically deduced from the function argument. |
| `Index` | Data type of the index, automatically deduced from the function argument. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| `tensor` | Input | Source Tensor residing in UB. The base address must be 32B-aligned. |
| `coord` | Input | Coordinates of the base address in the logical layout of `tensor`. |
| `index` | Input | Index relative to the base address. In element mode, the unit is element; in DataBlock mode, the unit is byte, and only the first 8 elements of `index` are valid. The index may contain duplicate values. |
| `gather_mask` | Input | Valid-bit mask for the gather operation. In element mode, it takes effect element by element; in DataBlock mode, it is interpreted in b32 format, where one DataBlock corresponds to 4 bits, only the lowest bit of each 4-bit group is valid, and only bits 0, 4, 8, 12, 16, 20, 24, and 28 are valid. |

<a id="table-3-gather_mode-parameter-description"></a>

**Table 3** `gather_mode` parameter description

| Value | Meaning | Index Unit |
| --- | --- | --- |
| `gather_mode::element` | Collects data from the Unified Buffer to the destination register element by element according to `index`. | Element |
| `gather_mode::data_block` | Collects data from the Unified Buffer to the destination register in units of DataBlock (32B) according to `index`. | Byte |

## Data Type

| `mode` | Tensor element type | `Index` type |
| --- | --- | --- |
| `element` | `int8_t` | `uint16_t` |
| `element` | `uint8_t` | `uint16_t` |
| `element` | `int16_t` | `uint16_t` |
| `element` | `int16_t` | `uint32_t` |
| `element` | `uint16_t` | `uint16_t` |
| `element` | `uint16_t` | `uint32_t` |
| `element` | `half` | `uint16_t` |
| `element` | `half` | `uint32_t` |
| `element` | `bfloat16_t` | `uint16_t` |
| `element` | `bfloat16_t` | `uint32_t` |
| `element` | `int32_t` | `uint32_t` |
| `element` | `uint32_t` | `uint32_t` |
| `element` | `float` | `uint32_t` |
| `element` | `hifloat8_t` | `uint16_t` |
| `element` | `fp8_e4m3fn_t` | `uint16_t` |
| `element` | `fp8_e5m2_t` | `uint16_t` |
| `element` | `fp8_e8m0_t` | `uint16_t` |
| `data_block` | `int4b_t`, `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, `int64_t`, `uint64_t`, `half`, `bfloat16_t`, `float`, `hifloat8_t`, `fp8_e4m3fn_t`, `fp8_e5m2_t`, `fp8_e8m0_t`, `fp4x2_e1m2_t`, `fp4x2_e2m1_t` | `uint32_t` |

Type or mode combinations that are not listed cause a compile-time error.

## Return Value

The API returns `reg_tensor<typename Tensor::data_type>` by value; when the Tensor element type is `int4b_t`, it returns `reg_tensor<int4x2_t>`. The `reg` member of the returned object stores the collected result, and the `mask` member is identical to the input parameter `gather_mask.reg`.

## Constraints

- When `coord` is passed, the coordinate dimensionality must match the **Tensor** layout, and the coordinates must not be out of bounds.
- The base address must be 32B-aligned. When `coord` is passed in, the base address is `tensor.engine() + tensor.layout()(coord)`; when `coord` is not passed in, the base address is the start address of `tensor`.
- The data corresponding to the index must be within the valid address range of the UB.
- `gather_mode::data_block` uses only the first 8 elements of `index`, and each index must be greater than or equal to 0 and 32B-aligned.
- The indexes may contain identical values.
- In `gather_mode::element` mode, the result register elements not selected by `gather_mask` are set to 0.
- In `gather_mode::element` mode, when the element bit width of `index` is greater than that of `tensor`, the data is written to the even-index positions of the result register, and the odd-index positions are set to zero.
- Register data move-in must be performed in a `__simd_vf__` function or a `__simd_callee__` function.

## Example

```cpp
template <typename SrcTensor, typename IndexTensor, typename DstTensor>
__simd_vf__ inline void GatherData(SrcTensor src, IndexTensor index,
    DstTensor dst, uint32_t repeat_times, uint32_t total_length,
    uint32_t one_repeat_size)
{
    using data_type = typename SrcTensor::data_type;
    uint32_t count = total_length;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        auto mask = asc::te::experimental::update_mask<data_type>(count);
        uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto index_reg = asc::te::experimental::load(index);
        auto src_reg = asc::te::experimental::load_gather<
            asc::te::experimental::gather_mode::element>(src, coord, index_reg, mask);
        asc::te::experimental::store(dst, coord, src_reg);
    }
}
```
