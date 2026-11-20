---
okftype: reference.api
description: "Configures the scaling factor, offset, and sign of the quantization result required for real-time Scalar quantization during L0C Buffer move-out. Supports passing them item by item or as a packed 64-bit configuration value. The item-by-item form is supported only by the Ascend 950PR&950DT products, and must be called before the move-out interface is executed with the corresponding Scalar quantization mode selected."
tag:
  - roles: [data_movement_configuration]
  - traits: [quantization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md
    kind: requires
    condition: "Used when the Ascend 950PR&950DT products move data out via GM"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_3510.md
    kind: requires
    condition: "Used when the Ascend 950PR&950DT products move data out via L1"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2ub.md
    kind: requires
    condition: "Used when the Ascend 950PR&950DT products move data out via UB"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/accompanying_quantization.md
    kind: related
    condition: "Page describing quantization mode and config encoding"
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_quant_mode.md
    kind: requires
    condition: "The quant_pre_mode of the store interface must select the Scalar quantization enum value from it"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "overload:asc_set_l0c_copy_prequant__f32_u16_b"
    scope_basis: resolved
    note: "Added a (float, uint16_t, bool) overload that passes scale, offset, and is_signed item by item, so callers no longer need to assemble the 64-bit config themselves; this overload is supported only on Ascend 950PR&950DT products (dav-3510), while the (uint64_t config) overload since 9.0.0 continues to cover Atlas A2/A3 and Ascend 950PR&950DT."
---
# asc_set_l0c_copy_prequant

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:52:57.294Z pushedAt=2026-11-18T08:15:34.360Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
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

This API is used to configure the scale factor, offset, and sign of the quantization result required for quantization computation when performing real-time Scalar quantization during L0C Buffer copy-out. This API takes effect only on AIC, and the actual quantization is performed by the subsequent cube copy-out API. Therefore, you also need to select the corresponding Scalar quantization mode in the copy-out API.

<!-- npu="950" id11 -->
For Ascend 950PR&950DT products:

First call this API to configure the quantization parameters, then call [asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm_arch_3510.md), [asc_copy_l0c2l1](../cube_compute_store/asc_copy_l0c2l1_arch_3510.md), or [asc_copy_l0c2ub](../cube_compute_store/asc_copy_l0c2ub.md) to copy out, and select the Scalar quantization mode in [asc_quant_mode](../../defs/enum/asc_quant_mode.md) through `quant_pre_mode`.

The Scalar quantization enum values supported by the `quant_pre_mode` quantization mode parameter are as follows:

- DEQF16: Quantize int32_t to half, Scalar quantization.
- REQ4: Quantize int32_t to int4b_t, Scalar quantization.
- REQ8: Quantize int32_t to int8_t/uint8_t, Scalar quantization.
- QS322BF16_PRE: Quantize int32_t to bfloat16_t, Scalar quantization.
- QF322F16_PRE: float quantized to half, scalar quantization.
- QF322BF16_PRE: float quantized to bfloat16_t, scalar quantization.
- QF322S4_PRE: float quantized to int4b_t, scalar quantization.
- QF322B8_PRE: float quantized to int8_t/uint8_t, scalar quantization.
- QF322FP8_PRE: float quantized to fp8_e4m3fn_t, scalar quantization.
- QF322HIF8_PRE: float quantized to hifloat8_t (Half to Away Round), scalar quantization.
- QF322HIF8_PRE_HYBRID: float quantized to hifloat8_t (Hybrid Round), scalar quantization.
- QF322F32_PRE: float quantized to float, scalar quantization, with precision up to two thousandths but not up to two ten-thousandths.
<!-- end id11 -->

<!-- npu="A3,910b" id12 -->
For the following product models:

<!-- npu="A3" id13 -->
- Atlas A3 products
<!-- end id13 -->
<!-- npu="910b" id14 -->
- Atlas A2 products
<!-- end id14 -->

Only the interface with the input parameter `uint64_t config` is supported for configuring quantization parameters, which are then moved out through [asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm_arch_2201.md) or [asc_copy_l0c2l1](../cube_compute_store/asc_copy_l0c2l1_arch_2201.md).
<!-- end id12 -->

For the quantization mode, parameter encoding, and preparation process, see [real-time quantization](../cube_store_key_features/accompanying_quantization.md). For the combination of quantization and activation parameters, see [real-time quantization and real-time ReLU scenario combination](../cube_store_key_features/accompanying_quantization_and_relu_scenario_combination.md).

## Prototype

<!-- npu="950" id18 -->
The Ascend 950PR&950DT products support the following two function prototypes for parameter passing:

```cpp
__aicore__ inline void asc_set_l0c_copy_prequant(float scale, uint16_t offset, bool is_signed);

__aicore__ inline void asc_set_l0c_copy_prequant(uint64_t config);
```
<!-- end id18 -->

<!-- npu="A3,910b" id17 -->
The following products support only the packed parameters:

<!-- npu="A3" id9 -->
- Atlas A3 products
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 products
<!-- end id10 -->

```cpp
__aicore__ inline void asc_set_l0c_copy_prequant(uint64_t config);
```
<!-- end id17 -->

## Parameters

<!-- npu="950" id20 -->
For the Ascend 950PR&950DT products, the parameter description and config bit meanings are as follows:

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| scale | Input | Scale factor used for real-time scalar quantization. The value cannot be inf or nan. |
| offset | Input | Offset required for real-time quantization computation. The value range is [0, 511]. When no offset is used, set it to 0. It takes effect in REQ8/QF322B8_PRE/REQ4/QF322S4_PRE modes. |
| is_signed | Input | Whether the quantization result is signed data. When the value is `true`, the quantization result is int8_t; when the value is `false`, the quantization result is uint8_t. It takes effect in REQ8/QF322B8_PRE modes. |
| config | Input | Scalar quantization parameter assembled from `scale`, `offset`, and `is_signed`. For the meaning of each config bit, see Table 2. |

**Table 2** Meaning of config bits

| Bit Width | Variable Name | Description |
| --- | --- | --- |
| 0~12 | - | Invalid bit. |
| 13~31 | scale | The data type is treated as float, and the hardware performs computation in the (1, 8, 10) format. Bit 31 is the sign bit, bits 23~30 are the exponent bits, and bits 13~22 are the mantissa bits. The value cannot be inf or nan. |
| 32~36 | - | Invalid bit. |
| 37~45 | offset | 9-bit integer data. The result of multiplying the source data by the quantization coefficient is added to Offset. When no offset is used, configure it to 0. It takes effect in REQ8/QF322B8_PRE/REQ4/QF322S4_PRE modes. |
| 46 | is_signed | When the value is 1, the quantization result is int8_t; when the value is 0, the quantization result is uint8_t. It takes effect in REQ8/QF322B8_PRE modes. |
| 47~63 | - | Invalid bit. |
<!-- end id20 -->

<!-- npu="A3,910b" id19 -->
For the following product models:

<!-- npu="A3" id15 -->
- Atlas A3 products
<!-- end id15 -->
<!-- npu="910b" id16 -->
- Atlas A2 products
<!-- end id16 -->

For the `config` encoding, see [Real-time Quantization Parameter Code Table](../cube_store_key_features/accompanying_quantization.md#quant-pre-2201). The encoding values cannot be directly reused across products.
<!-- end id19 -->

Python sample code for the REQ8 quantization algorithm is currently provided. See [\(V\)REQ8 quantization algorithm](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2gm/scripts/gen_data_s322s8.py).

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- The quantization parameter cannot be inf/nan or a denormalized number.
- This API returns directly when it is not called by AIC.
- This API must be called before the corresponding L0C Buffer move-out API is executed, and the corresponding Scalar quantization mode must be configured in the move-out API.

## Examples

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

The following call example code is supported only by the Ascend 950PR&950DT products.

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t DIM = 16;
constexpr uint32_t ELEMENTS = DIM * DIM;
constexpr uint16_t HALF_ONE = 0x3c00;
constexpr float SCALE = 2.0f;
constexpr uint16_t OFFSET = 3;

__global__ __cube__ void asc_set_l0c_copy_prequant_kernel(
    __gm__ uint16_t* a, __gm__ uint16_t* b, __gm__ int8_t* output)
{
    asc_init();
    __cbuf__ half a_l1[ELEMENTS], b_l1[ELEMENTS];
    __ca__ half a_l0[ELEMENTS];
    __cb__ half b_l0[ELEMENTS];
    __cc__ float c_l0[ELEMENTS];

    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(a_l1, reinterpret_cast<__gm__ half*>(a), DIM * sizeof(half), asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, DIM, DIM, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(b_l1, reinterpret_cast<__gm__ half*>(b), DIM * sizeof(half), asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, DIM, DIM, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, 1, 1, 1, 1);
    asc_copy_l12l0b_transpose(b_l0, b_l1, 0, 0, 1, 1, 1, 1);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_mmad(c_l0, a_l0, b_l0, DIM, DIM, DIM, asc_unit_flag_mode::DISABLE, false, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    asc_set_l0c_copy_prequant(SCALE, OFFSET, true);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, DIM, DIM, DIM, DIM, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM,
        asc_unit_flag_mode::DISABLE, QuantMode_t::QF322B8_PRE, asc_relu_pre_mode::NONE, false, true, false, false);
    asc_sync();
}

void print_row(const char* label, const std::vector<int8_t>& data)
{
    std::cout << label << ':';
    for (uint32_t i = 0; i < 8; ++i) std::cout << ' ' << static_cast<int32_t>(data[i]);
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    std::vector<uint16_t> a(ELEMENTS), b(ELEMENTS);
    std::vector<int8_t> output(ELEMENTS), golden(ELEMENTS, static_cast<int8_t>(OFFSET));
    for (uint32_t i = 0; i < DIM; ++i) {
        a[i * DIM + i] = HALF_ONE;
        b[i * DIM + i] = HALF_ONE;
        golden[i * DIM + i] = static_cast<int8_t>(SCALE + OFFSET);
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t *a_device = nullptr, *b_device = nullptr;
    int8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(int8_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size() * sizeof(uint16_t), a.data(), a.size() * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size() * sizeof(uint16_t), b.data(), b.size() * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE);
    asc_set_l0c_copy_prequant_kernel<<<1, 0>>>(a_device, b_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int8_t), output_device, output.size() * sizeof(int8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_row("Quantized output row 0", output);
    print_row("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_set_l0c_copy_prequant scalar quantization passed."
                         : "[Failed] asc_set_l0c_copy_prequant result mismatch.") << std::endl;
    aclrtFree(a_device); aclrtFree(b_device); aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
<!-- end id8 -->
