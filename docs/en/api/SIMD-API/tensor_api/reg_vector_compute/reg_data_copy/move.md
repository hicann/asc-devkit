# move

<!-- md-trans-meta sourceCommit=183e1cd500571375b0faeb56d73e22492d82eea8 translatedAt=2026-11-19T02:53:36.717Z pushedAt=2026-11-19T05:54:06.670Z nl=2 -->

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

The header file path is `"tensor_api/experimental/arch/vector/reg_data_copy.h"`, and the API namespace is `asc::te::experimental`.

Completes data movement between registers. Copies the valid elements in `src` filtered by `src.mask` to the corresponding positions in the destination register.

- When `T` is a non-`bool` type, the two-parameter overload is called. Positions not filtered by `src.mask` use the Merging mode and keep the original values in `dst.reg`.
- When `T` is the `bool` type, the single-parameter overload is called. The bits in `src.reg` filtered by `src.mask` are copied to the return value, and the bits not filtered by `src.mask` are set to 0.

The two-parameter overload uses only `src.mask` to control the elements that participate in the move, does not modify `dst.mask`, and the `mask` member of the return value is the same as `dst.mask` before the call. The single-parameter overload writes `src.mask` to the `mask` member of the return value.

This API is a Reg vector copy API and takes effect only on the AIV.

## Prototype

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> move(reg_tensor<T>& dst, const reg_tensor<T>& src)

__simd_callee__ inline reg_tensor<bool> move(const reg_tensor<bool>& src)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| T | Input | Template parameter of the two-parameter overload, specifying the data type of the source and destination registers. `bool` is not supported. The single-parameter overload has no template parameter. |
| dst | Input/Output | Destination register of the two-parameter overload, of type `reg_tensor<T>`. Its data type must be exactly the same as that of `src`. When `src.mask` is partially valid, `dst.reg` must be pre-initialized; otherwise, unfiltered positions will contain unknown values. The API does not modify `dst.mask`. |
| src | Input | Source register, of type `reg_tensor<T>`. Its `mask` member indicates the elements that participate in the copy. For the two-parameter overload, elements whose corresponding positions are invalid retain the original values in `dst.reg`; for the single-parameter overload, bits whose corresponding positions are invalid are set to 0 in the return value. |

## Data Type

- The data types supported by the two-parameter overload are: uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t, and float.
- The data type supported by the single-parameter overload is: bool.

## Return Value

The two-parameter overload returns a destination register of the `reg_tensor<T>` type, whose `mask` member is the same as `dst.mask` before the call. The single-parameter overload returns `reg_tensor<bool>`, whose `mask` member is the same as `src.mask`.

## Constraints

- In the two-parameter overload, `dst` and `src` must be `reg_tensor` of the same data type, and `T` cannot be `bool`.
- The single-parameter overload supports only `reg_tensor<bool>`.
- Before calling the API, initialize `src.mask` to specify the elements to be copied.
- In the two-parameter overload, positions not filtered by `src.mask` keep the original values in `dst.reg`. When `src.mask` is partially valid, `dst.reg` must be initialized before calling the API.
- The two-parameter overload does not modify `dst.mask`, and the `mask` member of the return value is the same as `dst.mask` before the call.
- In the single-parameter overload, the bits in `src.mask` that are not filtered are set to 0 in the return value.
- The single-parameter overload writes `src.mask` to the `mask` member of the return value.
- Register movement should be performed in an `__simd_vf__` function or an `__simd_callee__` function.

## Example

Call the two-parameter overload for non-`bool` types:

```cpp
template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void MoveData(SrcTensor src, DstTensor dst,
    uint32_t repeat_times, uint32_t total_length,
    uint32_t one_repeat_size)
{
    using data_type = typename SrcTensor::data_type;
    const data_type init_value = static_cast<data_type>(1);
    uint32_t remain = total_length;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        auto mask = asc::te::experimental::update_mask<data_type>(remain);
        uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto src_reg = asc::te::experimental::load(src, coord).with_mask(mask);
        auto dst_reg = asc::te::experimental::fill(init_value);
        auto result = asc::te::experimental::move(dst_reg, src_reg);
        asc::te::experimental::store(dst, coord, result);
    }
}
```

Call the single-parameter overload for the `bool` type:

```cpp
template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void MoveMask(SrcTensor src, DstTensor dst,
    uint32_t repeat_times, uint32_t total_length,
    uint32_t one_repeat_size)
{
    using data_type = typename SrcTensor::data_type;
    uint32_t remain = total_length;
    auto mask = asc::te::experimental::all_mask<data_type>();
    for (uint16_t i = 0; i < repeat_times; ++i) {
        auto src_reg = asc::te::experimental::update_mask<data_type>(remain);
        src_reg.with_mask(mask);
        auto result = asc::te::experimental::move(src_reg);
        uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto data_reg = asc::te::experimental::load(src, coord).with_mask(result);
        asc::te::experimental::store(dst, coord, data_reg);
    }
}
```
