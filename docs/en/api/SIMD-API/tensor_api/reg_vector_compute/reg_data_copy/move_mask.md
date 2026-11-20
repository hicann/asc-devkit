# move_mask

<!-- md-trans-meta sourceCommit=183e1cd500571375b0faeb56d73e22492d82eea8 translatedAt=2026-11-19T02:53:04.335Z pushedAt=2026-11-19T05:54:06.668Z nl=2 -->

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

Extracts bits from a specified data block of the source vector data register and extends them to generate a mask register.

- When `T` is `uint16_t`, the 256B source vector data register is divided into 16 data blocks, each 16B. Each bit of the selected data block is extended to 2 bits, generating a 32B mask register.
- When `T` is `uint32_t`, the 256B source vector data register is divided into 32 data blocks, each 8B. Each bit of the selected data block is extended to 4 bits, generating a 32B mask register.

This API is a Reg vector copy API and takes effect only on the AIV.

## Prototype

The value of `Offset` must be determined at compile time. You can pass an integer literal or a `constexpr` integer constant; passing a runtime variable is not supported.

```cpp
template <int16_t Offset, typename T>
__simd_callee__ inline reg_tensor<bool> move_mask(const reg_tensor<T>& src)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| Offset | Input | Template parameter that determines the data block extracted from `src`. When `T` is `uint16_t`, the value range is [0, 15]; when `T` is `uint32_t`, the value range is [0, 31]. |
| T | Input | Template parameter that specifies the data type of the source register. `uint16_t` and `uint32_t` are supported. |
| src | Input | Source vector data register of type `reg_tensor<T>`. The API reads its `reg` member and does not read the `mask` member. |

## Data Type

Supported data types: uint16_t, uint32_t.

## Return Value

Returns `reg_tensor<bool>`. The `reg` member stores the extracted and extended 32B mask register, and the `mask` member is a fully valid mask in b8 mode.

## Constraints

- `src` must be a `reg_tensor` of the `uint16_t` or `uint32_t` type.
- This API reads only `src.reg`, not `src.mask`.
- `Offset` must be a compile-time constant. You can pass an integer literal or a `constexpr` integer constant. Passing a runtime variable is not supported.
    - When `T` is `uint16_t`, the value range of `Offset` is [0, 15].
    - When `T` is `uint32_t`, the value range of `Offset` is [0, 31].
- Register data move-out should be performed in an `__simd_vf__` function or an `__simd_callee__` function.

## Key Features

When `T` is `uint16_t`, each element corresponds to 2 bits in the mask register, and only the least significant bit of the 2 bits is valid. The selected 16B data block contains 128 bits, and each bit is extended to 2 bits to generate a 32B mask register. A 256B source register contains 16 data blocks, so the value range of `Offset` is [0, 15].

**Figure 1** uint16_t vector data register generating a mask register<a id="fig-move-mask-b16"></a>

![uint16_t vector data register generating a mask register](../../../../figures/capi_copy_reg2mask_b16.png)

When `T` is `uint32_t`, each element corresponds to 4 bits in the mask register, and only the least significant bit of the 4 bits is valid. The selected 8B data block contains 64 bits, and each bit is extended to 4 bits to generate a 32B mask register. A 256B source register contains 32 data blocks, so the value range of `Offset` is [0, 31].

**Figure 2** uint32_t vector data register generating a mask register<a id="fig-move-mask-b32"></a>

![uint32_t vector data register generating a mask register](../../../../figures/capi_copy_reg2mask_b32.png)

## Example

```cpp
template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void MoveMaskData(SrcTensor src, DstTensor dst,
    uint32_t repeat_times, uint32_t one_repeat_size)
{
    using data_type = typename SrcTensor::data_type;
    const data_type fill_value = 1;
    for (uint32_t i = 0; i < repeat_times; ++i) {
        uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto src_reg = asc::te::experimental::load(src, coord);
        auto mask = asc::te::experimental::move_mask<0>(src_reg);
        auto result = asc::te::experimental::fill(fill_value, mask);
        asc::te::experimental::store(dst, coord, result);
    }
}
```
