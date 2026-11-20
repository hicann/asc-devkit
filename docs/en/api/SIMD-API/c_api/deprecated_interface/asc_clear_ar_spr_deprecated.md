---
okftype: reference.api
description: "Deprecated AR special register clearing interface. Call it before asc_squeeze_with_status to reset the AR register that accumulates the effective element byte count to zero. It takes effect only on AIV. The replacement is asc_squeeze_and_storeunalign_init."
tag:
  - roles: [control]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/deprecated_interface/asc_squeeze_with_status_deprecated.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_init.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The AR register read/write interfaces are deprecated. Use asc_get_squeeze_status and asc_squeeze_and_storeunalign_init instead. The CANN_9_2_0 deprecation table lists asc_get_ar_spr and asc_clear_ar_spr, and the include declarations have been moved to the deprecated_interface group."
---
# asc_clear_ar_spr (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:24:14.057Z pushedAt=2026-11-18T03:12:16.375Z nl=2 -->

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

The header file path is: `"c_api/reg_compute/compute/reg_permute_sel.h"`.

**This API is deprecated. Use [asc_squeeze_and_storeunalign_init](../reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_init.md) to implement this function.**

Clears the AR special register. The AR register is used together with [asc_squeeze_with_status](asc_squeeze_with_status_deprecated.md): when asc_squeeze_with_status is called, the total number of bytes of valid elements is stored in the AR register. Before calling asc_squeeze_with_status, call this API to clear the AR register.

This API is a Reg vector computation API and takes effect only on AIV.

## Prototype

```c
__simd_callee__ inline void asc_clear_ar_spr()
```

## Parameters

None

## Return Value

None

## Constraints

- This API takes effect only on AIV. Calls on non-AIV return directly.
- This API is called within a Vector Function (a function marked with `__simd_vf__`).

## Examples

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510&& ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
template <typename T>
void print_data(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

template <typename T>
bool compare_data(const std::vector<T>& actual, const std::vector<T>& expected, double tolerance = 0.0)
{
    if (actual.size() != expected.size()) return false;
    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i] == expected[i]) continue;
        const double diff = static_cast<double>(actual[i]) - static_cast<double>(expected[i]);
        if (diff > tolerance || diff < -tolerance) return false;
    }
    return true;
}

constexpr uint32_t ELEMENTS_PER_REGISTER = 128;
constexpr uint32_t REPEAT_COUNT = 2;
constexpr uint32_t INPUT_COUNT = ELEMENTS_PER_REGISTER * REPEAT_COUNT;
constexpr uint32_t OUTPUT_COUNT = INPUT_COUNT / 4;

__simd_vf__ inline void pack_selected(__ubuf__ uint16_t* output, __ubuf__ uint16_t* input)
{
    __ubuf__ uint16_t* dst = output;
    vector_uint16_t src;
    vector_uint16_t squeezed;
    vector_store_unalign unalign;
    vector_bool every_fourth = asc_create_mask_b16(PAT_M4);
    asc_clear_ar_spr();
    for (uint32_t repeat = 0; repeat < REPEAT_COUNT; ++repeat) {
        asc_loadalign(src, input + repeat * ELEMENTS_PER_REGISTER);
        asc_squeeze_with_status(squeezed, src, every_fourth);
        asc_storeunalign_postupdate(dst, unalign, squeezed);
    }
    asc_storeunalign_post(dst, unalign);
}

__global__ __vector__ void asc_clear_ar_spr_kernel(__gm__ uint16_t* output, __gm__ uint16_t* input)
{
    asc_init();
    __ubuf__ uint16_t input_local[INPUT_COUNT];
    __ubuf__ uint16_t output_local[OUTPUT_COUNT];
    asc_copy_gm2ub_align(input_local, input, INPUT_COUNT * sizeof(uint16_t));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    pack_selected(output_local, input_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, output_local, OUTPUT_COUNT * sizeof(uint16_t));
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint16_t> input(INPUT_COUNT), output(OUTPUT_COUNT, 0), golden;
    for (uint32_t i = 0; i < INPUT_COUNT; ++i) input[i] = i + 1;
    golden.reserve(OUTPUT_COUNT);
    for (uint32_t repeat = 0; repeat < REPEAT_COUNT; ++repeat) {
        for (uint32_t i = 0; i < ELEMENTS_PER_REGISTER; i += 4) {
            golden.push_back(input[repeat * ELEMENTS_PER_REGISTER + i]);
        }
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), (INPUT_COUNT) * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint16_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), (OUTPUT_COUNT) * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint16_t), input.data(), input.size() * sizeof(uint16_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_clear_ar_spr_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint16_t), output_device, output.size() * sizeof(uint16_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Packed output after AR reset", output);
    print_data("Golden every-fourth values", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_clear_ar_spr started contiguous squeeze storage at offset zero."
                         : "[Failed] asc_clear_ar_spr squeeze storage failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
