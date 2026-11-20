# load

<!-- md-trans-meta sourceCommit=83bafdf59a5b8695f652c2e8bbc7b9f91b0d8584 translatedAt=2026-11-19T02:58:13.454Z pushedAt=2026-11-19T05:54:06.677Z nl=2 -->

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

`load` is used to load data from the Unified Buffer (UB) into [reg_tensor](../reg_tensor/reg_tensor.md). The API supports the following two types of destination registers:

- [Vector Data Register Load](#vector-data-register-load): Deduces the register type based on the element type of the source tensor, and selects normal load, upsampling, downsampling, expand, or deinterleave via `load_sideband_mode`. The `deintlv` mode returns `reg_pair`, and other modes return `reg_tensor`.
- [Mask Register Load](#mask-register-load): Loads mask data bit by bit from a `uint32_t` UB tensor via `load<bool>`, supports the `direct`, `upsample`, and `downsample` modes, and returns `reg_tensor<bool>`.

The API supports the following two addressing modes:

- When `coord` is passed, the move-in starts from the element corresponding to `tensor.engine() + tensor.layout()(coord)`.
- When `coord` is not passed, the move-in starts from the start position of `tensor`.

## Prototype

### Vector Data Register Load

- Specify the coordinate:

    ```cpp
    template <load_sideband_mode sideband_mode = load_sideband_mode::direct,
              typename Tensor,
              typename Coord>
    __simd_callee__ inline auto load(const Tensor& tensor, const Coord& coord)
    ```

- Move in from the start position of the Tensor:

    ```cpp
    template <load_sideband_mode sideband_mode = load_sideband_mode::direct,
              typename Tensor>
    __simd_callee__ inline auto load(const Tensor& tensor)
    ```

### Mask Register Load

- Specified coordinate:

    ```cpp
    template <typename RegDataType,
              load_sideband_mode sideband_mode = load_sideband_mode::direct,
              typename Tensor,
              typename Coord>
    __simd_callee__ inline auto load(const Tensor& tensor, const Coord& coord)
    ```

- Load from the start position of the Tensor:

    ```cpp
    template <typename RegDataType,
              load_sideband_mode sideband_mode = load_sideband_mode::direct,
              typename Tensor>
    __simd_callee__ inline auto load(const Tensor& tensor)
    ```

## Parameters

### Vector Data Register Load

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| sideband_mode | Load mode, of type `load_sideband_mode`, with a default value of `load_sideband_mode::direct`. For supported modes, see [Table 3](#table-3-load_sideband_mode-parameters). |
| Tensor | Source Tensor type, automatically deduced from the function argument. The Tensor must be located in the UB. |
| Coord | Coordinate type of the source Tensor, automatically deduced from the function argument. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tensor | Input | Source Tensor, which must be located in UB. |
| coord | Input | Coordinates of the move-in start element in the logical layout of `tensor`. |

<a id="table-3-load_sideband_mode-parameters"></a>

**Table 3** Vector data register load mode

In the table, VL indicates the length of one vector data register, in bytes. Data of the `int4b_t` type is stored in registers as `int4x2_t`. The `int4x2_t` data type packs two 4-bit integers into one 8-bit storage unit, and the `fp4x2_e2m1_t` and `fp4x2_e1m2_t` data types pack two independent 4-bit floating-point numbers into one 8-bit storage unit. Therefore, for these three types, the "element" described in the following table refers to one 8-bit transfer unit, rather than a single 4-bit value. Operations such as zero padding, duplication, sampling, and interleaving are all performed at the granularity of this transfer unit.

| Value | Meaning | Move-in Start Address Alignment Constraint (Byte) |
| --- | --- | --- |
| load_sideband_mode::direct | Normal mode, which continuously reads VL data size from UB and moves it into one `reg_tensor`. | 32 |
| load_sideband_mode::deintlv | Deinterleave mode, which continuously reads 2\*VL data size from UB and moves the even-indexed and odd-indexed elements into the `first` and `second` members of the return value, respectively. | 32 |
| load_sideband_mode::downsample | 2x downsample mode, which reads 2\*VL data size from UB, keeps every other element, and obtains VL data size. | 32 |
| load_sideband_mode::unpack | 2x unpack mode, which reads VL/2 data size from UB, appends a 0 element after each source element, and expands it to VL data size. | min(32, VL/2) |
| load_sideband_mode::unpack4 | 4x unpack mode, which reads VL/4 data size from UB, appends three 0 elements after each source element, and expands it to VL data size. | min(32, VL/4) |
| load_sideband_mode::upsample | 2x upsample mode, which reads VL/2 data size from UB, and each source element is repeated twice in the destination register. | min(32, VL/2) |

### Mask Register Load

**Table 4** Template parameters

| Parameter | Description |
| --- | --- |
| RegDataType | Destination register element type, which must be explicitly specified as `bool`. |
| sideband_mode | Load mode, of type `load_sideband_mode`, with a default value of `load_sideband_mode::direct`. For supported modes, see [Table 6](#table-6-mask-register-load-mode). |
| Tensor | Source tensor type, automatically deduced from function arguments. The tensor must be located in UB, and its element type must be `uint32_t`. |
| Coord | Coordinate type of the source tensor, automatically deduced from function arguments. |

**Table 5** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tensor | Input | UB tensor of type `uint32_t` that stores mask bits. |
| coord | Input | Coordinates of the load start `uint32_t` element in the logical layout of `tensor`. |

<a id="table-6-mask-register-load-mode"></a>

**Table 6** Mask register load mode

Mask register load processes data in bits, rather than sampling in units of `uint32_t` elements. When VL is 256 bytes, the mask register length is VL/8, that is, 32 bytes.

| Value | Meaning | UB read size (Byte) | Load start address alignment constraint (Byte) |
| --- | --- | --- | --- |
| load_sideband_mode::direct | Loads the source bits as-is. | VL/8 (32) | 32 |
| load_sideband_mode::upsample | Repeats each source bit twice. | VL/16 (16) | 16 |
| load_sideband_mode::downsample | Retains the source bits at even indices (starting from 0). | VL/4 (64) | 32 |

## Data Type

### Vector Data Register Load

The source Tensor supports the following element types. Unlisted type or mode combinations cause a compile-time error.

| sideband_mode | Supported Data Types |
| --- | --- |
| direct | `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, `int64_t` |
| deintlv | `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float` |
| downsample, upsample | `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t` |
| unpack | `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float` |
| unpack4 | `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t` |

### Mask Register Load

The element type of the source tensor only supports `uint32_t`, and the element type of the destination register only supports `bool`. For supported modes, see [Table 6](#table-6-mask-register-load-mode).

## Return Value

The API returns a register object by value. The specific type is determined by the source tensor element type, the load mode, and whether `RegDataType` is explicitly specified.

### Vector Data Register Load

- For non-`int4b_t` types, `deintlv` mode returns `reg_pair<typename Tensor::data_type>`, whose `first.reg` and `second.reg` members store the move-in results of even-indexed and odd-indexed elements, respectively.
- For non-`int4b_t` types, other modes return `reg_tensor<typename Tensor::data_type>`, whose `reg` member stores the move-in result.
- When the source Tensor element type is `int4b_t`, `deintlv` mode returns `reg_pair<int4x2_t>`, and other modes return `reg_tensor<int4x2_t>`.

### Mask Register Load

Returns `reg_tensor<bool>`, whose `reg` member stores the loaded mask bits. The return object can be used directly as the argument of `with_mask`.

## Constraints

- Only Tensor located in the UB is supported.
- The actual load start address must satisfy the alignment constraint in [Table 3](#table-3-load_sideband_mode-parameters) or [Table 6](#table-6-mask-register-load-mode), and the entire read range must not exceed the UB buffer of the source Tensor.
- When `coord` is passed, the coordinate dimensionality must match the Tensor layout, and the coordinates must not be out of bounds.
- The load API initializes only the `reg` member of the return object, not the `mask` member. Before using the vector data register result in register calculation that reads `mask` or moving it out, you must call `with_mask` to bind a valid mask.
- Register data move-in must be performed in a `__simd_vf__` function or a `__simd_callee__` function.

## Example

### Normal Move-in

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/reg_data_load.h"

template <typename T, typename SrcTensor>
__simd_vf__ inline void LoadData(const SrcTensor& src)
{
    auto value = asc::te::experimental::load(src, asc::te::make_coord(0));
    value.with_mask(asc::te::experimental::all_mask<T>());
    // Use value to perform subsequent register computation.
}
```

### Deinterleaved Move-in

```cpp
template <typename T, typename SrcTensor>
__simd_vf__ inline void LoadDeinterleaved(const SrcTensor& src)
{
    auto value = asc::te::experimental::load<asc::te::experimental::load_sideband_mode::deintlv>(
        src, asc::te::make_coord(0));

    auto mask = asc::te::experimental::all_mask<T>();
    value.first.with_mask(mask);
    value.second.with_mask(mask);
    // value.first and value.second store the even-indexed and odd-indexed transfer units, respectively.
}
```

### Mask Register Load

`src` is a UB Tensor of the `uint32_t` type. The following example loads the raw mask from its start position and demonstrates upsampling and downsampling. To satisfy the requirements of all three calls, the `src` start address must be 32-byte aligned, and the buffer must contain at least 64 bytes.

```cpp
template <typename SrcTensor>
__simd_vf__ inline void LoadMasks(SrcTensor src)
{
    using namespace asc::te::experimental;
    auto direct_mask = load<bool>(src);
    auto upsampled_mask = load<bool, load_sideband_mode::upsample>(src);
    auto downsampled_mask = load<bool, load_sideband_mode::downsample>(src, asc::te::make_coord(0));
    // The reg member of each return value stores the corresponding mask bit.
}
```
