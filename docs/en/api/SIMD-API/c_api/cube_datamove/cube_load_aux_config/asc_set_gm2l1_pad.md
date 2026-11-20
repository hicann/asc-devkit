---
okftype: reference.api
description: "Sets the constant padding value for the asc_copy_gm2l1_align API, which is used conditionally by the Normal, Compact, or left/right padding modes. The low bits of the uint32_t argument are taken as a single padding element according to the dtype. It takes effect only on the AIC."
tag:
  - roles: [data_movement_configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l1_align.md
    kind: configures
    condition: "Takes effect when enable_data_select is true and the left/right padding count is 0, or when entering left/right padding mode; otherwise, the padding value is taken from the first element of the data block"
version: ">=9.0.0"
version_changes: []
---
# asc_set_gm2l1_pad

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:32:27.095Z pushedAt=2026-11-18T08:15:34.327Z nl=2 -->

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

The header file path is `"c_api/cube_datamove/cube_datamove.h"`.

Used to set the constant padding value for the [asc_copy_gm2l1_align](../cube_compute_load/asc_copy_gm2l1_align.md) API. When calling `asc_copy_gm2l1_align`, the value set by this API is used in the following scenarios:

- When `enable_data_select` is set to `true` and both `left_padding_count` and `right_padding_count` are `0`, the padding data in Normal padding mode or Compact padding mode uses the value set by this API.
- When `left_padding_count` or `right_padding_count` is non-`0`, the left/right padding mode is entered. In this case, the hardware forcibly uses the value set by this API, and the `enable_data_select` setting is invalid.

When `enable_data_select` is set to `false` and the left/right padding mode is not enabled, the padding data takes the first element of each contiguous data block, and the value set by this API does not take effect.

This API takes effect only on AIC.

## Prototype

```cpp
__aicore__ inline void asc_set_gm2l1_pad(uint32_t pad_val)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| pad_val | Input | Constant padding value. The API parameter is `uint32_t`, and `asc_copy_gm2l1_align` uses the corresponding low bits as the value of a single padding element according to `dtype`. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- This API returns directly when called on non-AIC.
- Call this API to configure the padding value before calling [asc_copy_gm2l1_align](../cube_compute_load/asc_copy_gm2l1_align.md).

## Examples

The following sample code is only a register configuration example. For complete Normal padding, Compact padding, and left/right padding scenario examples, see [asc_copy_gm2l1_align Example](../cube_compute_load/asc_copy_gm2l1_align.md#examples).

After saving the sample code as `examples.asc`, you can compile it into an object file using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile command is as follows:

```bash
bisheng -c examples.asc -o examples.o --npu-arch=dav-3510
```
<!-- end id8 -->

The following example moves three 48B `uint8_t` data blocks, using the constant `0xA5` to pad the trailing 16B of each data block, so that each data block is written to the L1 Buffer with a 64B stride.

```cpp
#include <cstdint>
#include "c_api/asc_simd.h"

// This kernel moves data from GM to L1 on the AIC.
__global__ __cube__ void Gm2L1AlignPadExample(__gm__ uint8_t* src)
{
    asc_init();
    __cbuf__ uint8_t dst[192];

    uint8_t PAD_VALUE = 0xA5;
    constexpr uint32_t N_BURST = 3;
    constexpr uint32_t LEN_BURST = 48;
    constexpr uint32_t DST_STRIDE = 64;
    constexpr asc_load_l2_cache_mode L2_CACHE_MODE = asc_load_l2_cache_mode::NOTALLOC_KEEP;
    // Configure the constant padding value; the uint8_t padding byte is 0xA5, and the actual argument is promoted to the uint32_t type required by the API.
    asc_set_gm2l1_pad(*reinterpret_cast<uint8_t*>(&PAD_VALUE));
    // enable_data_select=true, in Normal mode the trailing 16B of each 48B data block is padded with the constant value.
    asc_copy_gm2l1_align(dst, src, N_BURST, LEN_BURST, 0, 0, true, L2_CACHE_MODE, LEN_BURST, DST_STRIDE);
}
```
