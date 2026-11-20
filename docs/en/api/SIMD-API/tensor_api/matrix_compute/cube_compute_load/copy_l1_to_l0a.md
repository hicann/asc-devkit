# copy (L1 Buffer to L0A Buffer Movement)

<!-- md-trans-meta sourceCommit=2fe3a64c1bb11110e316a708c0d8e9dc67631487 translatedAt=2026-10-27T02:41:58.911Z pushedAt=2026-10-27T13:03:35.328Z nl=2 -->

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

The header file path is `tensor_api/tensor.h`.

The Tensor API uses the `copy` API to uniformly perform movement across different paths. This API is used to move the left cube data in the L1 Buffer to the L0A Buffer.

The API supports complete tensor movement. It also supports regional movement by specifying the start coordinate in the destination tensor, the start coordinate in the source tensor, and the shape of the movement region through `dst_coord`, `src_coord`, and `copy_shape`.

This path uses a 512-byte data fractal as the basic movement unit. The fractal cube shapes corresponding to different data types are as follows:

**Table 1**  Data bit width and fractal cube shape

| Data Bit Width | Fractal Cube Shape |
| :--- | :--- |
| b4 | 16 * 64 |
| b8 | 16 * 32 |
| b16 | 16 * 16 |
| b32 | 16 * 8 |

The API supports non-transpose movement, transpose movement, and Img2Col convolution movement. The Layout of the source tensor and the destination tensor must match the format combinations supported by the current path, as shown in Table 2.

**Table 2**  Copy mode and format combination

| Copy mode | Source layout -> Destination layout |
| :--- | :--- |
| Non-transpose movement | `Nz -> Nz` |
| Transpose movement | `Zn -> Nz` |
| Img2Col copy | `NC1HWC0 -> Nz` or `NDC1HWC0 -> Nz` |

In the Img2Col scenario, the convolution feature map is unfolded into a cube during the movement process.

The API supports Batch mode. In Batch mode, the Layout of the source tensor and the destination tensor needs to add a Batch dimension at the front of the original fractal Layout, with a shape structure of `(b, single-cube shape)`, where `b` indicates the number of Batches. The number of Batches in the source tensor and the destination tensor must be consistent, and the Batch dimension Stride needs to be consistent with the physical space occupied by a single batch of data, that is, `Batch dimension Stride = single Batch Layout` [capacity()](../../layout/capacity.md). You can use `make_frame_layout<nz_layout_ptn, DataType>(b, m, k)` or `make_frame_layout<zn_layout_ptn, DataType>(b, m, k)` to construct a contiguous Batch Layout.

## Function Prototype

- Performs movement from the L1 Buffer to the L0A Buffer.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst, const SrcTensor& src)
    ```

- Automatically derives the movement path based on the storage locations of the source tensor and the destination tensor, and uses the [default trait](../../algorithm.md#copy_traits) to perform movement from the L1 Buffer to the L0A Buffer.

    ```cpp
    template <typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)
    ```

- Performs the L1 Buffer to L0A Buffer movement according to the specified source coordinate, destination coordinate, and movement shape.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- Automatically derives the movement path based on the storage locations of the source tensor and the destination tensor, and uses the default trait to perform the L1 Buffer to L0A Buffer movement according to the specified source coordinate, destination coordinate, and movement shape.

    ```cpp
    template <typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- Constructs a movement atomic object using the default trait.

    ```cpp
    template <typename CopyOperation>
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation)
    ```

- Constructs a movement atomic object using the specified trait.

    ```cpp
    template <typename CopyOperation, typename CopyTrait>
    __aicore__ inline constexpr auto make_copy(
        const CopyOperation& operation, const CopyTrait& trait)
    ```

For Img2Col movement, after binding `img2col_params` through `copy_atom::with`, the same `copy(atom, dst, src)` function prototype is used for execution.

## Parameters

**Table 3** Template parameter description of the `copy` API

| Parameter | Description |
| :--- | :--- |
| Atom | Template parameter of `copy_atom`, derived by the `make_copy` API. |
| DstTensor | Destination tensor type. |
| SrcTensor | Source tensor type. |
| DstCoord | Start coordinate type of the destination tensor, which can also be `zero_coord`. |
| SrcCoord | Start coordinate type of the source tensor, which can also be `zero_coord`. |
| CopyShape | Type of the movement region shape. |

**Table 4** Template parameter description of the `make_copy` API

| Parameter | Description |
| :--- | :--- |
| CopyOperation | Type of the movement operation object. |
| CopyTrait | Type of the movement trait object. |

**Table 5**  Parameters of the `copy` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| atom | Input | Movement atomic object, which encapsulates a combination of Operation, Trait, and optional runtime Params for a specific movement path. It can be constructed by the `make_copy` API. For details, see [Algorithm](../../algorithm.md#copy_atom). |
| dst | Output | Destination tensor, stored at `location::l0a`. |
| src | Input | Source tensor, stored at `location::l1`. |
| dst_coord | Input | Start coordinate of the movement region in the destination tensor; can also be `zero_coord`. |
| src_coord | Input | Start coordinate of the movement region in the source tensor; can also be `zero_coord`. |
| copy_shape | Input | Shape of the movement region, used to specify the size of the movement region. |

**Table 6**  Parameters of the `make_copy` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| operation | Input | Movement operation object. The L1 Buffer to L0A Buffer path uses `copy_l1_to_l0a{}`. |
| trait | Input | Movement trait object, used to specify movement characteristics. The default trait for L1 Buffer to L0A Buffer uses `l1_to_l0a_trait_default{}`. |

### copy_l1_to_l0a Description

`copy_l1_to_l0a` identifies the L1 Buffer-to-L0A Buffer movement path. Only its public content is listed:

```cpp
struct copy_l1_to_l0a {
    template <typename Trait, const Trait& trait, typename... Args>
    __aicore__ inline static void copy(const Args&... args);
};
```

The `copy` static member function receives the Trait and movement parameters, and dispatches the L1 Buffer-to-L0A Buffer movement.

### l1_to_l0a_trait Description

`l1_to_l0a_trait` represents the static configuration of this path. Currently, there are no configurable members, and it is defined as follows:

```cpp
struct l1_to_l0a_trait {};
```

`l1_to_l0a_trait_default` provides the default trait type and default value for this path:

```cpp
struct l1_to_l0a_trait_default {
    using trait_type = l1_to_l0a_trait;
    static constexpr const trait_type value = {};
};
```

### img2col_params Description

`img2col_params<PadType>` is used to configure the destination cube range, convolution kernel size, sliding stride, dilation factor, padding size, and fill value for Img2Col copy.

```cpp
template <typename PadType>
struct img2col_params {
    uint16_t m_extension = 0;
    uint16_t k_extension = 0;
    uint16_t m_start_pos = 0;
    uint16_t k_start_pos = 0;
    uint8_t pad_list[4] = {0, 0, 0, 0};
    uint8_t stride_w = 1;
    uint8_t stride_h = 1;
    uint8_t filter_w = 1;
    uint8_t filter_h = 1;
    uint8_t dilation_filter_w = 1;
    uint8_t dilation_filter_h = 1;
    bool enable_filter_w_extend = false;
    bool enable_filter_h_extend = false;
    bool enable_transpose = false;
    bool enable_f_matrix_ctrl = false;
    PadType pad_value = 0;
};
```

**Table 7**  `img2col_params` member description

| Member | Default Value | Description |
| :--- | :--- | :--- |
| m_extension | `0` | Number of elements to move along the M axis of the destination cube. The value range is [0, 65535]. When the value is 0, no movement is performed. When the movement region does not cover the bottommost fractal of the destination cube, the value must be a multiple of 16 for the b4, b8, and b16 data types, while the b32 data type has no multiple requirement. When the movement region covers the bottommost fractal, there is no multiple requirement. |
| k_extension | `0` | Number of elements to move along the K axis of the destination cube. The value range is [0, 65535]. When the value is 0, no movement is performed. When the movement region does not cover the rightmost fractal of the destination cube, the value must be a multiple of 64, 32, 16, and 8 for the b4, b8, b16, and b32 data types, respectively. When the movement region covers the rightmost fractal, there is no multiple requirement. |
| m_start_pos | `0` | Copy start position along the M axis of the destination cube. The value range is [0, 32767]. |
| k_start_pos | `0` | Start position of this instruction in the width dimension of the destination operand. For the b32 type, it must be a multiple of 8; for the b16 type, a multiple of 16; for the b8 type, a multiple of 32; and for the b4 type, a multiple of 64. The value range is [0, 65535]. |
| pad_list | `{0, 0, 0, 0}` | Padding size, in the order of left, right, top, and bottom. The value range of each element is [0, 255]. |
| stride_w | `1` | Sliding stride of the convolution kernel along the W axis of the source feature map. The value range is [0, 63]. |
| stride_h | `1` | Sliding stride of the convolution kernel along the H axis of the source feature map. The value range is [0, 63]. |
| filter_w | `1` | Width of the convolution kernel. The value range is [0, 255]. |
| filter_h | `1` | Height of the convolution kernel. The value range is [0, 255]. |
| dilation_filter_w | `1` | Dilation factor of the convolution kernel along the W axis. The value range is [0, 255]. |
| dilation_filter_h | `1` | Dilation factor of the convolution kernel along the H axis. The value range is [0, 255]. |
| enable_filter_w_extend | `false` | Whether to increase the convolution kernel width by 256 on the basis of `filter_w`. |
| enable_filter_h_extend | `false` | Whether to increase the convolution kernel height by 256 on the basis of `filter_h`. |
| enable_transpose | `false` | Whether to transpose the expanded destination cube. |
| enable_f_matrix_ctrl | `false` | FeatureMap attribute selection flag. Currently, only `false` is supported. |
| pad_value | `0` | Fill value for the padding region. The template parameter PadType usually needs to be consistent with the element type of the destination tensor. When the destination tensor is of the `fp8_e4m3fn_t`, `fp8_e5m2_t`, or `hifloat8_t` type, the corresponding 8-bit floating-point type cannot be directly used as PadType. Instead, the fill value must first be converted to an integer encoding supported by the API before being passed in. |


## Data Type

The supported data types include:

`fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `half`, `bfloat16_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, `float`.

The data types of the source tensor and the destination tensor must be consistent.

## Return Value

- `copy` has no return value.
- `make_copy` returns a `copy_atom` object.

## Constraints

- The destination address is located in the L0A Buffer, and its start address must meet the 512-byte alignment requirement.
- The source address is located in the L1 Buffer, and its start address must meet the 32-byte alignment requirement.
- Special data type constraints: When transpose is enabled on the L1 Buffer-to-L0A Buffer path, that is, `ZN2NZ`, the supported data type constraints are as follows:
  - The b32 data type requires the source cube shape to be 16-aligned in the K-axis direction.
  - The b8 data type requires the source cube shape to be 32-aligned in the M-axis direction.
  - The b4 data type requires the source cube shape to be 64-aligned in the M-axis direction.
- Batch mode requires the source tensor and destination tensor to have the same Batch count, and the tensor data must be contiguously arranged in L1 Buffer and L0A Buffer.
- Img2Col movement currently supports only `NC1HWC0` source tensors with N equal to 1, and the destination tensor must be in `NZ` format.
- The PadType of `img2col_params<PadType>` usually needs to be consistent with the element type of the L0A Buffer destination tensor. When the destination tensor is of type `fp8_e4m3fn_t`, `fp8_e5m2_t`, or `hifloat8_t`, the corresponding 8-bit floating-point type cannot be used directly as PadType; instead, the fill value must first be converted to an integer encoding supported by the API before being passed in.

## Key Features

### Non-transpose Movement

When the data format of the source tensor and the destination tensor is `NZ`, the API copies data from the L1 Buffer to the L0A Buffer in a non-transpose manner.

**Figure 1**  Non-transpose movement schematic diagram

![Non-transpose movement schematic diagram](../../../../figures/load2dv2_start_addr_demo.png)

### Transpose Movement

When the source tensor data format is `ZN` and the destination tensor data format is `NZ`, the API copies data in transpose mode. Transpose movement transposes each fractal cube in the source tensor.

The transpose movement schematic diagrams for different data bit widths are as follows.

#### b4 Transpose Movement

**Figure 2**  b4 transpose movement schematic diagram

![b4 transpose movement schematic diagram](../../../../figures/load2dv2_l12l0_trans_b4.png)

#### b8 Transpose Movement

**Figure 3**  b8 transpose movement schematic diagram

![b8 transpose movement schematic diagram](../../../../figures/load2dv2_l12l0_trans_b8.png)

#### b16 Transpose Movement

**Figure 4**  b16 transpose movement schematic diagram

![b16 transpose movement schematic diagram](../../../../figures/load2dv2_l12l0_trans_b16.png)

#### b32 transpose Movement

**Figure 5**  b32 transpose movement schematic diagram

![b32 transpose movement schematic diagram](../../../../figures/load2dv2_l12l0_trans_b32.png)

### Batch Copy

When both the source tensor and the destination tensor use a Layout with a Batch dimension, the `copy` API completes the movement from L1 Buffer to L0A Buffer according to the data range described by the Batch dimension. For non-transpose movement, both the source tensor and the destination tensor use the `NZ` format; for transpose movement, the source tensor uses the `ZN` format and the destination tensor uses the `NZ` format.

### Img2Col Movement

Img2Col movement unfolds an `NC1HWC0`-format feature map into an `NZ`-format cube according to the convolution kernel window. In the unfolded cube, the M axis corresponds to the spatial positions of the output feature map, and the K axis corresponds to the H, W, and input channels of the convolution kernel. You configure the unfolding range and convolution parameters through `img2col_params`, and then bind the parameters to the movement atomic object through `atom.with(params)`.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l1_to_l0a_example()
{
    using a_type = half;
    constexpr uint32_t m = 16;
    constexpr uint32_t k = 16;

    __cbuf__ a_type l1_buf[m * k];
    __ca__ a_type l0a_buf[m * k];

    auto l1_tensor = make_tensor(make_mem_ptr(l1_buf), make_frame_layout<nz_layout_ptn, a_type>(m, k));

    auto l0a_tensor = make_tensor(make_mem_ptr(l0a_buf), make_frame_layout<nz_layout_ptn, a_type>(m, k));

    auto atom = make_copy(copy_l1_to_l0a{}, l1_to_l0a_trait_default{});
    copy(atom, l0a_tensor, l1_tensor);
}
```

The following shows an example of Batch mode.

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l1_to_l0a_batch_example()
{
    using a_type = half;
    constexpr uint32_t batch = 2;
    constexpr uint32_t m = 16;
    constexpr uint32_t k = 16;

    __cbuf__ a_type l1_buf[batch * m * k];
    __ca__ a_type l0a_buf[batch * m * k];

    auto layout = make_frame_layout<nz_layout_ptn, a_type>(batch, m, k);
    auto l1_tensor = make_tensor(make_mem_ptr(l1_buf), layout);
    auto l0a_tensor = make_tensor(make_mem_ptr(l0a_buf), layout);

    auto atom = make_copy(copy_l1_to_l0a{}, l1_to_l0a_trait_default{});
    copy(atom, l0a_tensor, l1_tensor);
}
```

The following shows an Img2Col movement example. The input feature map has a shape of `(1, 2, 5, 5, 16)`, the convolution kernel is 3×3, the stride is 1, and the padding is 1. The logical shape of the unfolded cube is `(25, 288)`.

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_img2col_l1_to_l0a()
{
    constexpr uint32_t n = 1;
    constexpr uint32_t c1 = 2;
    constexpr uint32_t h = 5;
    constexpr uint32_t w = 5;
    constexpr uint32_t c0 = 16;
    constexpr uint32_t m = 25;
    constexpr uint32_t k = 3 * 3 * c1 * c0;
    constexpr uint32_t m_align = 32;

    __cbuf__ half l1_buf[n * c1 * h * w * c0];
    __ca__ half l0a_buf[m_align * k];

    auto l1_feature = make_tensor(
        make_mem_ptr(l1_buf), make_frame_layout<nc1hwc0_layout_ptn>(n, c1, h, w, c0));
    auto l0a_matrix = make_tensor(
        make_mem_ptr(l0a_buf), make_frame_layout<nz_layout_ptn, half>(m, k));

    img2col_params<half> params;
    params.m_extension = m;
    params.k_extension = k;
    params.m_start_pos = 0;
    params.k_start_pos = 0;
    params.filter_w = 3;
    params.filter_h = 3;
    params.stride_w = 1;
    params.stride_h = 1;
    params.pad_list[0] = 1;
    params.pad_list[1] = 1;
    params.pad_list[2] = 1;
    params.pad_list[3] = 1;

    auto copy_l1_to_l0a_atom = make_copy(copy_l1_to_l0a{}, l1_to_l0a_trait_default{});
    copy(copy_l1_to_l0a_atom.with(params), l0a_matrix, l1_feature);
}
```
