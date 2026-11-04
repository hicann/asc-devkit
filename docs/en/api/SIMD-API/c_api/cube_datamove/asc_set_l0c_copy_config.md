# asc_set_l0c_copy_config

<!-- md-trans-meta sourceCommit=c891f0c5d5b2ac25c5683ff40810fa381d654ece translatedAt=2026-10-22T13:25:16.493Z pushedAt=2026-10-28T12:45:45.476Z nl=2 -->

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
- Atlas inference product AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference product Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is `"c_api/cube_datamove/cube_datamove.h"`.

This API is used to configure the associated tensor quantization, associated tensor activation, and UnitFlag functions in the L0C Buffer move-out scenario. The specific functions are described as follows:

- Function 1: Set the start address index of the associated tensor quantization parameter. When the L0C Buffer move-out interface enables tensor quantization mode, the hardware reads the N-dimensional quantization parameter from the Fixpipe Buffer according to the start address index set by this API, where N represents the number of columns of the cube to be moved out.
- Function 2: Set the start address index of the associated tensor activation parameter. When the L0C Buffer move-out interface enables tensor activation mode, the hardware reads the N-dimensional activation parameter from the Fixpipe Buffer according to the start address index set by this API, where N represents the number of columns of the cube to be moved out.
- Function 3: Initialize the UnitFlag global state. When the UnitFlag function of the L0C Buffer move-out interface needs to be disabled, or when the data volume of cube multiply-accumulate computation differs from that of the move-out, this API can set the global state of the UnitFlag function to disabled.

This API is a cube move-out configuration API and takes effect only on the AIC.

## Prototype

```c
__aicore__ inline void asc_set_l0c_copy_config(uint64_t relu_pre_addr,
                                               uint64_t quant_pre_addr,
                                               bool is_clean_unit_flag);
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|---|---|---|
| relu_pre_addr | Input | Start address index of the tensor activation parameter in the Fixpipe Buffer. The address index is calculated as `start address of the tensor activation parameter in the Fixpipe Buffer / 64`, in units of 64 bytes. The start address of the tensor activation parameter in the Fixpipe Buffer must be 64-byte aligned. This parameter takes effect only when the subsequent L0C Buffer move-out interface enables tensor activation mode. |
| quant_pre_addr | Input | Start address index of the tensor quantization parameter in the Fixpipe Buffer. The address index is calculated as `start address of the tensor quantization parameter in the Fixpipe Buffer / 128`, in units of 128 bytes. The start address of the tensor quantization parameter in the Fixpipe Buffer must be 128-byte aligned. This parameter takes effect only when the subsequent L0C Buffer move-out interface enables tensor quantization mode. |
| is_clean_unit_flag | Input | Whether to initialize the UnitFlag flags of all storage blocks in the L0C Buffer.<br>&nbsp;&nbsp;&bull; `false`: Do not initialize.<br>&nbsp;&nbsp;&bull; `true`: Initialize. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- The quantization parameter cannot be inf, nan, or a denormalized number.
- This API takes effect only on the AIC. Calling it on the AIV returns immediately.
<!-- npu="950" id8 -->
- For the Ascend 950PR&950DT products, the Fixpipe Buffer for storing tensor quantization parameters is 4KB, and the Fixpipe Buffer for storing tensor activation parameters is 2KB. The two storage spaces are independent of each other, and their addresses both start from 0. The start addresses and occupied space of the tensor quantization parameters and tensor activation parameters must not exceed the range of the corresponding Buffer.
<!-- end id8 -->
<!-- npu="A3,910b" id9 -->
- For the following product models:

  <!-- npu="A3" id10 -->
  Atlas A3 products
  <!-- end id10 -->
  <!-- npu="910b" id11 -->
  Atlas A2 products
  <!-- end id11 -->

  The Fixpipe Buffer for storing tensor quantization parameters is 2KB, and the Fixpipe Buffer for storing tensor activation parameters is 1KB. The two storage spaces are independent of each other, and their addresses both start from 0. The start addresses and occupied space of the tensor activation parameters and tensor activation parameters must not exceed the range of the corresponding Buffer.
<!-- end id9 -->
- If the L0C Buffer move-out APIs [asc_copy_l0c2gm](../cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md), [asc_copy_l0c2l1](../cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_3510.md), and [asc_copy_l0c2ub](../cube_datamove/asc_copy_l0c2ub.md) need to enable the in-line quantization or in-line activation function, this API must be called before the move-out APIs.
- The configuration of this API remains in effect until it is overwritten by calling this API again. When different move-out tasks use different configurations, reconfigure before the corresponding move-out API is executed. Each call overwrites all three parameters at the same time.

<!-- npu="950" id12 -->
## Examples

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command. The `--npu-arch` parameter must be set to the NPU architecture corresponding to the actual product model. For the mapping between products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

The following example code is supported only by Ascend 950PR&950DT products.

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t DIM = 16, ELEMENTS = DIM * DIM;
constexpr uint16_t HALF_ONE = 0x3c00, HALF_THREE = 0x4200;
constexpr uint64_t SIGNED_SCALE_TWO = 0x40000000ULL | (1ULL << 46);

__global__ __cube__ void asc_set_l0c_copy_config_kernel(
    __gm__ uint16_t* a, __gm__ uint16_t* b, __gm__ uint64_t* scales, __gm__ int8_t* output)
{
    asc_init();
    __cbuf__ half a_l1[ELEMENTS], b_l1[ELEMENTS];
    __cbuf__ uint64_t scale_l1[DIM];
    __fbuf__ uint64_t scale_fb[DIM];
    __ca__ half a_l0[ELEMENTS];
    __cb__ half b_l0[ELEMENTS];
    __cc__ float c_l0[ELEMENTS];
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(a_l1, reinterpret_cast<__gm__ half*>(a), DIM * sizeof(half),
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, DIM, DIM, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(b_l1, reinterpret_cast<__gm__ half*>(b), DIM * sizeof(half),
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, DIM, DIM, 0, false);
    asc_copy_gm2l1(reinterpret_cast<__cbuf__ void*>(scale_l1), reinterpret_cast<__gm__ void*>(scales), 1, 4,
        asc_channel_pad_mode::NONE, 0, 0);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, 1, 1, 1, 1);
    asc_copy_l12l0b_transpose(b_l0, b_l1, 0, 0, 1, 1, 1, 1);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_mmad(c_l0, a_l0, b_l0, DIM, DIM, DIM, asc_unit_flag_mode::DISABLE, true, false, true);
    asc_sync_notify(PIPE_MTE2, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_FIX, EVENT_ID0);
    asc_copy_l12fb(reinterpret_cast<__fbuf__ void*>(scale_fb), reinterpret_cast<__cbuf__ void*>(scale_l1), 1, 2, 0, 0);
    const uint64_t scale_address = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(scale_fb)) / 128;
    asc_set_l0c_copy_config(0, scale_address, false);
    asc_sync_pipe(PIPE_FIX);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, DIM, DIM, DIM, DIM,
        asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, asc_unit_flag_mode::DISABLE,
        asc_quant_mode::VQF322B8_PRE, asc_relu_pre_mode::NONE, false, true, false, false);
    asc_sync_pipe(PIPE_ALL);
}

void print_row(const char* label, const std::vector<int8_t>& data)
{
    std::cout << label << ':';
    for (uint32_t i = 0; i < 8; ++i) std::cout << ' ' << +data[i];
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    std::vector<uint16_t> a(ELEMENTS), b(ELEMENTS);
    std::vector<uint64_t> scales(DIM, SIGNED_SCALE_TWO);
    std::vector<int8_t> output(ELEMENTS), golden(ELEMENTS);
    for (uint32_t i = 0; i < DIM; ++i) {
        a[i * DIM + i] = HALF_ONE;
        b[i * DIM + i] = HALF_THREE;
        golden[i * DIM + i] = 6;
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t *a_device = nullptr, *b_device = nullptr;
    uint64_t* scales_device = nullptr;
    int8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&scales_device), scales.size() * sizeof(uint64_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size() * sizeof(uint16_t), a.data(), a.size() * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size() * sizeof(uint16_t), b.data(), b.size() * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(scales_device, scales.size() * sizeof(uint64_t), scales.data(), scales.size() * sizeof(uint64_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_set_l0c_copy_config_kernel<<<1, 0>>>(a_device, b_device, scales_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size(), output_device, output.size(), ACL_MEMCPY_DEVICE_TO_HOST);
    print_row("Quantized output row 0", output);
    print_row("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_set_l0c_copy_config applied tensor quantization scale 2."
                         : "[Failed] asc_set_l0c_copy_config result mismatch.") << std::endl;
    aclrtFree(a_device); aclrtFree(b_device); aclrtFree(scales_device); aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
<!-- end id12 -->
