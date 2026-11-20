---
okftype: reference.api
description: "On the AIC, performs 3D im2col expansion on NC1HWC0-format Feature Map and copies it from L1 Buffer to L0A Buffer. It can also be used for 2D-format transfer. The Feature Map attributes, padding value, and repeat parameters must be preconfigured."
tag:
  - roles: [data_movement]
  - operations: [copy, img2col]
  - traits: [datablock_granularity, high_dimensional, padding]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/cube_compute_input_move_constraint.md
    kind: constrained_by
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_l12l0a_3d_padding.md
    kind: requires
    condition: "Padding value and padding mode must be configured first."
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_l13d_fmatrix.md
    kind: requires
    condition: "In 3D mode, the Feature Map attribute description must be configured first"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_l13d_rpt.md
    kind: requires
    condition: "When using repeat mode, the repeat direction/count/stride must be configured first"
version: ">=9.1.0"
version_changes: []
---
# asc_copy_l12l0a (NPU Architecture Version 3510, 3D Cube Copy)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:43:03.857Z pushedAt=2026-11-18T08:15:34.271Z nl=2 -->

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
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is: `"c_api/cube_datamove/cube_datamove.h"`.

3D convolution data movement is used to perform Image to Column expansion on a Feature Map in NC1HWC0 format, and then select a specified data block from the expanded 2D cube and copy it into the corresponding memory location. See Figure 1.

During the copy process, data is copied in units of 512-byte data fractals. This API can also be used to copy 2D format data required for normal cube computation. For how to use this API to copy 2D format data, see [Key Feature Description](#l12l0a_3d_key_features). Data movement from L1 Buffer to L0A Buffer is supported.

**Figure 1** copyl12l0a_3d functional diagram<a id="l12l0a_3d_data_move"></a>

![](../../../../figures/load3d_l1tol0a_demo.png "copyl12l0a_3d functional diagram")

In 3D img2col transfer mode, the attribute description of the Feature Map (fm_w, fm_h, and padding on all four sides) must be preconfigured through [asc_set_l13d_fmatrix](../cube_load_aux_config/asc_set_l13d_fmatrix.md), and the padding value and padding mode must be preconfigured through [asc_set_l12l0a_3d_padding](../cube_load_aux_config/asc_set_l12l0a_3d_padding.md); if the repeat mode is required, the repeat direction, count, and stride must be preconfigured through [asc_set_l13d_rpt](../cube_load_aux_config/asc_set_l13d_rpt.md).

This API is valid only when executed on the AIC.

For the data arrangement before copying, see [Key Fractal Format Details](../../cube_compute/cube_compute_fractal_intro/key_fractal_format_details.md); for address alignment and copy granularity requirements, see [Cube Computation Input Copy Constraints](cube_compute_input_move_constraint.md), and use them together with the parameter and constraint descriptions of this API.

## Prototype

### 3D Transfer Mode

```cpp
__aicore__ inline void asc_copy_l12l0a(__ca__ <dtype>* dst,
                                       __cbuf__ <dtype>* src,
                                       uint16_t k_extension,
                                       uint16_t m_extension,
                                       uint16_t k_start_pt,
                                       uint16_t m_start_pt,
                                       uint8_t stride_w,
                                       uint8_t stride_h,
                                       uint8_t filter_w,
                                       uint8_t filter_h,
                                       uint8_t dilation_filter_w,
                                       uint8_t dilation_filter_h,
                                       bool filter_size_w,
                                       bool filter_size_h,
                                       bool transpose,
                                       bool f_matrix_ctrl,
                                       uint16_t channel_size)
```

#### Data Types Supported by dtype

`dtype` can be: `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

#### Prototype Example

```cpp
// Example: 3D transfer of the half type.
__aicore__ inline void asc_copy_l12l0a(__ca__ half* dst,
                                       __cbuf__ half* src,
                                       uint16_t k_extension,
                                       uint16_t m_extension,
                                       uint16_t k_start_pt,
                                       uint16_t m_start_pt,
                                       uint8_t stride_w,
                                       uint8_t stride_h,
                                       uint8_t filter_w,
                                       uint8_t filter_h,
                                       uint8_t dilation_filter_w,
                                       uint8_t dilation_filter_h,
                                       bool filter_size_w,
                                       bool filter_size_h,
                                       bool transpose,
                                       bool f_matrix_ctrl,
                                       uint16_t channel_size)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|---|---|---|
| `dst` | Output | Destination operand, stored in the L0A Buffer. The destination address must be 512-byte aligned. |
| `src` | Input | Source operand, stored in the L1 Buffer. The source address must be 32-byte aligned. |
| `k_extension` | Input | Transfer length of the destination operand in the width dimension. If the rightmost fractal is not covered, it must be a multiple of 8 for the b32 type, a multiple of 16 for the b16 type, and a multiple of 32 for the b8 type; if covered, no multiple requirement applies to any data type. Value range: [1, 65535]. |
| `m_extension` | Input | Transfer length of the destination operand in the height dimension. If the bottommost fractal is not covered, it must be a multiple of 16 for the b8 and b16 types, with no requirement for the b32 type; if covered, no multiple requirement applies to any data type. Value range: [0, 65535]. |
| `k_start_pt` | Input | Starting point of the destination operand in the width dimension. It must be a multiple of 8 for the b32 type, a multiple of 16 for the b16 type, and a multiple of 32 for the b8 type. Value range: [0, 65535]. |
| `m_start_pt` | Input | Starting point of the destination operand in the height dimension. Value range: [0, 32767]. |
| `stride_w` | Input | Step by which the convolution kernel slides in the width dimension of the source operand. Value range: [1, 63]. |
| `stride_h` | Input | Step by which the convolution kernel slides in the height dimension of the source operand. Value range: [1, 63]. |
| `filter_w` | Input | Width of the convolution kernel. Value range: [1, 255]. |
| `filter_h` | Input | Height of the convolution kernel. Value range: [1, 255]. |
| `dilation_filter_w` | Input | Dilation factor of the convolution kernel width. Value range: [1, 255]. |
| `dilation_filter_h` | Input | Dilation factor of the convolution kernel height. Value range: [1, 255]. |
| `filter_size_w` | Input | Whether to increase the convolution kernel width by 256 elements on top of `filter_w`.<br>&nbsp;&nbsp;&bull; `true`: increase;<br>&nbsp;&nbsp;&bull; `false`: do not increase. |
| `filter_size_h` | Input | Whether to increase the convolution kernel height by 256 elements on top of `filter_h`.<br>&nbsp;&nbsp;&bull; `true`: increase;<br>&nbsp;&nbsp;&bull; `false`: do not increase. |
| `transpose` | Input | Whether to enable the transpose function, which transposes the entire destination cube.<br>&nbsp;&nbsp;&bull; `false`: do not enable transpose;<br>&nbsp;&nbsp;&bull; `true`: enable transpose. |
| `f_matrix_ctrl` | Input | Selection bit of the attribute description register group of the Feature Map for 3D data movement. Generally set to `false`.<br>&nbsp;&nbsp;&bull; `true`: obtain the attribute description of the Feature Map from the right cube, used together with [asc_set_l13d_fmatrix_b](../cube_load_aux_config/asc_set_l13d_fmatrix_b.md), [asc_set_l12l0b_3d_padding](../cube_load_aux_config/asc_set_l12l0b_3d_padding.md), and [asc_set_l13d_rpt_b](../cube_load_aux_config/asc_set_l13d_rpt_b.md);<br>&nbsp;&nbsp;&bull; `false`: obtain the attribute description of the Feature Map from the left cube, used together with [asc_set_l13d_fmatrix](../cube_load_aux_config/asc_set_l13d_fmatrix.md), [asc_set_l12l0a_3d_padding](../cube_load_aux_config/asc_set_l12l0a_3d_padding.md), and [asc_set_l13d_rpt](../cube_load_aux_config/asc_set_l13d_rpt.md). |
| `channel_size` | Input | Number of channels N of the source operand.<br>For the b32 type, the remainder of `channel_size` divided by 8 must be 0 or 4, and the maximum value is 65532.<br>For the b16 type, the remainder of `channel_size` divided by 16 must be 0, 4, or 8, and the maximum value is 65528.<br>For the b8 type, the remainder of `channel_size` divided by 32 must be 0, 4, 8, or 16, and the maximum value is 65520. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

### General Constraints

- If this API is not called by the AIC, it returns directly.
- The dst start address must be 512-byte aligned (L0A Buffer alignment requirement); otherwise, the transfer becomes abnormal.
- The src start address must be 32-byte aligned (L1 Buffer alignment requirement); otherwise, the transfer becomes abnormal.
- If this instruction has data dependencies with other instructions, insert a [synchronization instruction](../../sync/overview.md#synchronization-control-overview) to serialize multiple instructions and prevent abnormal data.
- L0A Buffer capacity limit: The total L0A Buffer capacity is 64 KB. The sum of the dst offset and the transfer size must not exceed the boundary; otherwise, an exception is triggered.
- L1 Buffer capacity limit: The total L1 Buffer capacity is 512 KB. The sum of the src offset and the source cube occupied size must not exceed the boundary; otherwise, an exception is triggered.

### 3D img2col Transfer Mode Constraints

- Before calling this API, you must call [asc_set_l13d_rpt](../cube_load_aux_config/asc_set_l13d_rpt.md) to configure the dst_stride parameter. dst_stride is the size of the output cube after alignment in the height dimension.
- Before calling this API, you must call the matching register configuration APIs [asc_set_l13d_fmatrix](../cube_load_aux_config/asc_set_l13d_fmatrix.md) and [asc_set_l12l0a_3d_padding](../cube_load_aux_config/asc_set_l12l0a_3d_padding.md) to preconfigure the feature map description, padding value, and padding mode. Otherwise, the transfer result may not meet expectations. To use the repeat mode, you must first call [asc_set_l13d_rpt](../cube_load_aux_config/asc_set_l13d_rpt.md) to configure the repeat direction, count, and stride.
- When `k_extension`, `m_extension`, or `channel_size` is `0`; when `filter_w` is `0` and `filter_size_w` is `false`; or when `filter_h` is `0` and `filter_size_h` is `false`, no transfer is performed and this API is treated as a NOP (no operation).
- For the b32 type, the remainder of `channel_size` divided by 8 must be 0 or 4, and the maximum value is 65532. For the b16 type, the remainder of `channel_size` divided by 16 must be 0, 4, or 8, and the maximum value is 65528. For the b8 type, the remainder of `channel_size` divided by 32 must be 0, 4, 8, or 16, and the maximum value is 65520. Otherwise, the transfer result may not meet expectations.
- `stride_w` and `stride_h` cannot be 0. If 0 is passed in, the hardware treats it as 1.
- The value range of `stride_w` is [1, 63]. Values outside this range are clipped, causing the transfer result to not meet expectations.
- The value range of `stride_h` is [1, 63]. Values outside this range are clipped, causing the transfer result to not meet expectations.

## Key Feature Description<a id="l12l0a_3d_key_features"></a>

### 3D img2col Data Format Description

The input feature map and filter must be in NC1HWC0 format, where C0 is the lowest dimension and is fixed at 16 (32 for u8/s8 types), and C1=C/C0.

To simplify the scenario, the following scenario assumes that the input feature map has 4 channels, that is, Ci=4. The shape of the input feature maps in the L1 Buffer is (Hi,Wi,Ci), and after 3D transfer processing, the data shape in the L0A Buffer is (Wo\*Ho, Hk\*Wk\*Ci). Here, Wo and Ho are the output shape after convolution, and Hk and Wk are the shape of the filter.

Intuitively, the img2col process is one in which the filter sweeps over the feature map and expands the corresponding feature map data into each row of the output data. The filter first slides Wo steps in the W direction, then moves one step in the H direction and repeats the above process, ultimately outputting Wo * Ho rows of data. In the following figure, the red and yellow data represent the first row and the second row, respectively. The numbers indicate the association among the original input data, the filter, and the output data. It can be seen that the 3D transfer processing first transfers the 4 numbers corresponding to 00 in the Ci dimension of the input data, then transfers the four numbers corresponding to 01, and finally the size of this row is Hk\*Wk\*Ci, that is, 3\*3\*4=36 numbers.

- The corresponding feature map format is shown in the following figure:

    **Figure 2** feature map format

    ![ ](../../../../figures/load3d_01.png "feature map format")

- The corresponding filter format is shown in the following figure:

    **Figure 3** filter format

    ![ ](../../../../figures/load3d_02.png "filter format")

Here, n is the number of filters. It can be seen that the dimension arrangement is (Hk, Wk, Ci, n), but note that the format in the following figure still needs to be converted according to the format of the B cube in Mmad.

**Figure 4** Convolution computation blocking

In actual operation, due to limitations of storage space or computing capability, we usually block the entire convolution computation and copy and compute only a small block of data at a time.

![ ](../../../../figures/load3d_03.png "Convolution computation blocking")

For the feature map in the L0A Buffer, there are two schemes: horizontal blocking and vertical blocking. They correspond to rpt_mode values 0 and 1 in the [asc_set_l13d_rpt](../cube_load_aux_config/asc_set_l13d_rpt.md) parameter, respectively.

Note: The fractal cube in the following figure is 4x4 in size, but it should actually be 16x16 (16x32 for the u8/s8 types).

When rpt_mode = 0, each repeat changes the position where the data point is read in the filter window, and then jumps to the next C0 position.

**Figure 5** Filter window when rpt_mode = 0

![ ](../../../../figures/load3d_04.png "Filter window when rpt_mode = 0")

When rpt_mode = 1, the position where data is read in the filter window remains unchanged, and each repeat advances by C0 elements in the feature map.

**Figure 6** Filter window when rpt_mode = 1

![ ](../../../../figures/load3d_05.png "Filter window when rpt_mode = 1")

### Using 3D Convolution Data Movement to Move 2D Format Data Required for Cube Computation

For data in NC1HWC0 format, when N is 1, the convolution kernel width and height are 1, padding is 0, the stride of the convolution kernel sliding over the width and height dimensions of the source operand is 1, and the dilation factors of the convolution kernel width and height are 1, the data arrangement after image to column expansion can be regarded as the NZ fractal arrangement on the L1 Buffer during cube computation. Therefore, in this scenario, the LoadData (convolution data movement) interface can be used to move the 2D format data required for cube computation. The schematic diagram is as follows:

![](../../../../figures/load3d_3d22d_demo.png)

#### Non-transpose Scenario Transfer

The fractal arrangement on the L1 Buffer is exactly the Nz fractal arrangement required in the L0A Buffer. The 3D convolution transfer does not perform fractal arrangement conversion, as shown in the following diagram for the b16 type scenario:

![](../../../../figures/load3d_l1tol0a_b16_nontrans_950.png)

#### Transpose Scenario Movement

3D data movement supports transpose for the b8/b16/b32 data types. On the L1 Buffer->L0A Buffer path, transpose is enabled by turning on the `transpose` parameter. The details are described by type below.

- For the b8 scenario: each fractal is 16×32 in size.

    On the L1 Buffer->L0A Buffer path, in the transpose scenario, two consecutive 16×32 fractals are concatenated into a 32×32 square cube, which is then transposed and split back into two 16×32 fractals. The transpose diagram is as follows:

    ![](../../../../figures/load3d_l1tol0a_b8_trans_950.png)

- For the b16 scenario: each fractal is 16×16 in size.

    On the L1 Buffer->L0A Buffer path, in the transpose scenario, in addition to each fractal cube itself being transposed, the position of each fractal cube in the entire 2D feature map is also transposed. The transpose diagram is as follows:

    ![](../../../../figures/load3d_l1tol0a_b16_trans_950.png)

- For the b32 scenario: each fractal is 16×8 in size.

    On the L1 Buffer->L0A Buffer path, the transpose scenario diagram is as follows:

    ![](../../../../figures/load3d_l1tol0a_b32_trans_950.png)

### Feature Map and Pad Attribute Description and Repeat Mode Register Settings

The Feature Map attribute description, Pad attribute description, and Repeat Mode setting register serve as parameters for Image to Column expansion. When performing cube computation, for high-performance data movement, a large block of data is often moved from GM to the L1 Buffer, that is, a relatively large block of data is cached in the L1 Buffer, and then the L1 Buffer -> L0 Buffer transfer and Mmad computation are performed through a for loop. When the L1 Buffer -> L0 Buffer transfer is performed through the 3D data movement interface, the Feature Map attribute, Pad attribute, and Repeat Mode parameters usually need to be set only once and do not need to be set repeatedly.

## Examples

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t BATCH = 4;
constexpr uint32_t C1 = 2;
constexpr uint32_t H = 9;
constexpr uint32_t W = 9;
constexpr uint32_t C0 = 16;
constexpr uint32_t CIN = C1 * C0;
constexpr uint32_t COUT = 16;
constexpr uint32_t KH = 3;
constexpr uint32_t KW = 3;
constexpr uint32_t STRIDE_H = 2;
constexpr uint32_t STRIDE_W = 2;
constexpr uint32_t DILATION_H = 2;
constexpr uint32_t DILATION_W = 2;
constexpr uint32_t PAD_LEFT = 2;
constexpr uint32_t PAD_RIGHT = 2;
constexpr uint32_t PAD_TOP = 2;
constexpr uint32_t PAD_BOTTOM = 2;
constexpr int32_t PAD_VALUE = 0;
constexpr uint32_t HOUT = 5;
constexpr uint32_t WOUT = 5;
constexpr uint32_t M = HOUT * WOUT;
constexpr uint32_t K = CIN * KH * KW;
constexpr uint32_t N = COUT;
constexpr uint32_t CUBE_BLOCK = 16;
constexpr uint32_t M_ALIGN = 32;
constexpr uint32_t K_ALIGN = 288;
constexpr uint32_t N_ALIGN = 16;
constexpr uint32_t FMAP_BATCH_SIZE = H * W * CIN;
constexpr uint32_t WEIGHT_SIZE = K * COUT;
constexpr uint32_t OUTPUT_BATCH_SIZE = HOUT * WOUT * COUT;
constexpr uint32_t FRACTAL_ELEMENTS = 512 / sizeof(half);
constexpr uint32_t LOAD3D_K_EXTENSION = K_ALIGN;

__global__ __cube__ void Conv2dForwardCapi(
    __gm__ half* fmap, __gm__ half* weight, __gm__ half* output)
{
    asc_init();
    const uint32_t batch_idx = static_cast<uint32_t>(block_idx);
    if (batch_idx >= BATCH) {
        return;
    }

    __cbuf__ half fmap_l1[FMAP_BATCH_SIZE];
    __cbuf__ half weight_l1[K_ALIGN * N_ALIGN];
    __ca__ half fmap_l0[M_ALIGN * K_ALIGN];
    __cb__ half weight_l0[K_ALIGN * N_ALIGN];
    __cc__ float output_l0[M_ALIGN * N_ALIGN];

    asc_set_gm2l1_nz_para(1, 1, H * W, 0);
    asc_copy_gm2l1_nd2nz(
        fmap_l1, fmap + batch_idx * FMAP_BATCH_SIZE, CIN * sizeof(half), asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM,
        H * W, CIN, 0, false);

    asc_set_gm2l1_nz_para(1, 1, K_ALIGN, 0);
    asc_copy_gm2l1_nd2nz(weight_l1, weight, COUT * sizeof(half), asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, K, COUT, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    asc_l13d_fmatrix_config fmatrix_config;
    fmatrix_config.l1_width = W;
    fmatrix_config.l1_height = H;
    fmatrix_config.padding_left_size = PAD_LEFT;
    fmatrix_config.padding_right_size = PAD_RIGHT;
    fmatrix_config.padding_top_size = PAD_TOP;
    fmatrix_config.padding_bottom_size = PAD_BOTTOM;
    asc_set_l13d_fmatrix(fmatrix_config);
    asc_set_l12l0a_3d_padding(static_cast<half>(PAD_VALUE));
    constexpr uint16_t repeat_stride = 0;
    constexpr uint8_t repeat_time = 1;
    constexpr uint8_t repeat_mode = 0;
    constexpr uint16_t dst_stride = M_ALIGN / CUBE_BLOCK;
    asc_load3d_v2_config repeat_config;
    // SetLoadDataRepeatWithStride encodes dstStride in bits [47:32].
    repeat_config.config = static_cast<uint64_t>(repeat_stride) |
        (static_cast<uint64_t>(repeat_time) << 16) |
        (static_cast<uint64_t>(repeat_mode) << 24) |
        (static_cast<uint64_t>(dst_stride) << 32);
    asc_set_l13d_rpt(repeat_config);
    asc_copy_l12l0a(
        fmap_l0, fmap_l1, LOAD3D_K_EXTENSION, M, 0, 0, STRIDE_W, STRIDE_H, KW, KH, DILATION_W, DILATION_H,
        false, false, false, false, CIN);

    for (uint16_t i = 0; i < K_ALIGN / CUBE_BLOCK; ++i) {
        asc_copy_l12l0b_transpose(weight_l0 + i * FRACTAL_ELEMENTS, weight_l1, 0, i, 1, 1, 1, 1);
    }
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    asc_mmad(output_l0, fmap_l0, weight_l0, M, K_ALIGN, N, asc_unit_flag_mode::DISABLE, false, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(
        output + batch_idx * OUTPUT_BATCH_SIZE, output_l0, N, M, N, M_ALIGN,
        asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, asc_unit_flag_mode::DISABLE, QuantMode_t::F322F16,
        asc_relu_pre_mode::NONE, false, true, false, false);
    asc_sync_pipe(PIPE_FIX);
}

size_t FmapIndex(uint32_t n, uint32_t h, uint32_t w, uint32_t c)
{
    return ((n * H + h) * W + w) * CIN + c;
}

size_t WeightIndex(uint32_t c1, uint32_t kh, uint32_t kw, uint32_t c0, uint32_t co)
{
    const uint32_t k = ((c1 * KH + kh) * KW + kw) * C0 + c0;
    return k * COUT + co;
}

std::vector<half> MakeGolden(const std::vector<half>& fmap, const std::vector<half>& weight)
{
    std::vector<half> golden(BATCH * OUTPUT_BATCH_SIZE);
    for (uint32_t n = 0; n < BATCH; ++n) {
        for (uint32_t ho = 0; ho < HOUT; ++ho) {
            for (uint32_t wo = 0; wo < WOUT; ++wo) {
                for (uint32_t co = 0; co < COUT; ++co) {
                    float acc = 0.0f;
                    for (uint32_t c1 = 0; c1 < C1; ++c1) {
                        for (uint32_t kh = 0; kh < KH; ++kh) {
                            for (uint32_t kw = 0; kw < KW; ++kw) {
                                const int32_t hi = static_cast<int32_t>(ho * STRIDE_H + kh * DILATION_H) - PAD_TOP;
                                const int32_t wi = static_cast<int32_t>(wo * STRIDE_W + kw * DILATION_W) - PAD_LEFT;
                                for (uint32_t c0 = 0; c0 < C0; ++c0) {
                                    const uint32_t ci = c1 * C0 + c0;
                                    const float x = hi < 0 || hi >= static_cast<int32_t>(H) || wi < 0 ||
                                            wi >= static_cast<int32_t>(W)
                                        ? static_cast<float>(PAD_VALUE)
                                        : static_cast<float>(fmap[FmapIndex(n, hi, wi, ci)]);
                                    acc += x * static_cast<float>(weight[WeightIndex(c1, kh, kw, c0, co)]);
                                }
                            }
                        }
                    }
                    golden[((n * HOUT + ho) * WOUT + wo) * COUT + co] = static_cast<half>(acc);
                }
            }
        }
    }
    return golden;
}
} // namespace

int main()
{
    std::vector<half> fmap(BATCH * FMAP_BATCH_SIZE);
    std::vector<half> weight(WEIGHT_SIZE);
    std::vector<half> output(BATCH * OUTPUT_BATCH_SIZE);
    for (size_t i = 0; i < fmap.size(); ++i) {
        fmap[i] = static_cast<half>(static_cast<int32_t>(i % 5) - 2);
    }
    for (uint32_t c1 = 0; c1 < C1; ++c1) {
        for (uint32_t kh = 0; kh < KH; ++kh) {
            for (uint32_t kw = 0; kw < KW; ++kw) {
                for (uint32_t c0 = 0; c0 < C0; ++c0) {
                    for (uint32_t co = 0; co < COUT; ++co) {
                        weight[WeightIndex(c1, kh, kw, c0, co)] =
                            static_cast<half>(static_cast<int32_t>((c1 + kh + 2 * kw + c0 + co) % 3) - 1);
                    }
                }
            }
        }
    }
    const std::vector<half> golden = MakeGolden(fmap, weight);

    half *fmap_device = nullptr, *weight_device = nullptr, *output_device = nullptr;
    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&fmap_device), fmap.size() * sizeof(half), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&weight_device), weight.size() * sizeof(half), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(half), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(fmap_device, fmap.size() * sizeof(half), fmap.data(), fmap.size() * sizeof(half), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(weight_device, weight.size() * sizeof(half), weight.data(), weight.size() * sizeof(half), ACL_MEMCPY_HOST_TO_DEVICE);
    Conv2dForwardCapi<<<BATCH, 0>>>(fmap_device, weight_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(half), output_device, output.size() * sizeof(half), ACL_MEMCPY_DEVICE_TO_HOST);

    size_t mismatch_count = 0;
    float max_diff = 0.0f;
    for (size_t i = 0; i < output.size(); ++i) {
        const float diff = std::fabs(static_cast<float>(output[i]) - static_cast<float>(golden[i]));
        max_diff = diff > max_diff ? diff : max_diff;
        mismatch_count += diff > 1e-3f;
    }
    std::cout << "Example 1: max diff=" << max_diff
              << ", mismatches=" << mismatch_count << '/' << output.size() << std::endl;
    std::cout << (mismatch_count == 0 ? "[Success] conv2d_forward C API passed."
                                     : "[Failed] conv2d_forward C API mismatch.") << std::endl;

    aclrtFree(fmap_device);
    aclrtFree(weight_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return mismatch_count == 0 ? 0 : 1;
}

```
