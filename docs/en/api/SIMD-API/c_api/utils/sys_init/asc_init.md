---
okftype: reference.api
description: "Restores the control state that may be left by preceding operators on the current AI Core to the initial state specified by the API. Only initializes the control state and does not clear user data."
tag:
  - roles: [control]
dependencies: []
version: ">=8.5.0"
version_changes: []
---
# asc_init

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.589437+00:00 pushedAt=2026-11-16T02:10:57.875Z nl=2 -->

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

The header file path is `"c_api/utils/sys_init.h"`.

This API restores the global state that may be left by preceding operators on the current AI Core to the initial state defined by the API, preventing these states from affecting the computation results of the current operator. This API initializes only control state and does not clear global memory, local memory, or user data.

The initialization scope and post-initialization state vary by product as follows.

<!-- npu="950" id8 -->
**Table 1**  Initialization results for Ascend 950PR&950DT products

| State Category | Initialization Item                                           | State After Initialization                                                                                                                 |
| -------- | -------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------ |
| Common state | Atomic accumulation state                                       | Clears the atomic accumulation state so that subsequent data movement does not perform atomic accumulation.                                                                                 |
| Common state | `Mask` working mode                                     | Sets the mask mode to `Normal` mode.                                                                                                           |
| Common state | `CTRL` register                                       | Restores the `CTRL` register to the initial state defined by the API, where the values of the hold bits remain the same as before this API is called.                                                   |
| Common state | Atomic operation enable bit and atomic operation type                       | The register configuration value is set to `0b00100100`, where the data type field is `half` and the operation type field is sum. The atomic accumulation state has been cleared, so no atomic accumulation is performed after initialization. |
| AIC      | `Padding` value for 3D-format data movement by `asc_copy_l12l0a` | Set to 0.                                                                                                                      |
| AIV      | `Mask`                                             | Both the high `Mask` and low `Mask` are set to `0xffffffffffffffff`. In `Normal` mode, all elements within a single iteration participate in computation.                         |
| AIV      | Number of iterations for `GM->UB` and `UB->GM` data movement                   | Both the inner loop count and outer loop count are set to 1, restoring normal data movement mode.                                                                      |

<!-- end id8 -->

<!-- npu="A3" id9 -->
**Table 2**  Initialization results for Atlas A3 products

| State Category | Initialization Item       | State After Initialization                                                                                         |
| -------- | -------------- | ------------------------------------------------------------------------------------------------------ |
| AIC      | Global state       | Does not modify the state of the Cube Core.                                                                                |
| AIV      | Atomic accumulation state   | Clears the atomic accumulation state so that subsequent data movement does not perform atomic accumulation.                                                         |
| AIV      | `Mask` working mode | Sets the mask mode to `Normal` mode.                                                                                   |
| AIV      | `Mask`         | Both the high `Mask` and low `Mask` are set to `0xffffffffffffffff`. In `Normal` mode, all elements within a single iteration participate in computation. |

<!-- end id9 -->

<!-- npu="910b" id10 -->
**Table 3** Initialization results for Atlas A2 products

| State Category | Initialization Item       | State After Initialization                                                                                         |
| -------- | -------------- | ------------------------------------------------------------------------------------------------------ |
| AIC      | Global state       | Does not modify the state of the Cube Core.                                                                                |
| AIV      | Atomic accumulation state   | Clears the atomic accumulation state so that subsequent data movement does not perform atomic accumulation.                                                         |
| AIV      | `Mask` working mode | Sets the mask mode to `Normal` mode.                                                                                   |
| AIV      | `Mask`         | Both the high `Mask` and low `Mask` are set to `0xffffffffffffffff`. In `Normal` mode, all elements within a single iteration participate in computation. |

<!-- end id10 -->

## Prototype

```cpp
__aicore__ inline void asc_init()
```

## Parameters

None

## Return Value

None

## Pipeline Type

`PIPE_S`

## Constraints

In actual execution, these values may be modified by previously executed operators. If this API is not called for initialization, unexpected values may cause precision errors in computation results.

For example, if a preceding operator uses `Counter` mode but does not reset it to `Normal` mode, setting `Mask` in the default `Normal` mode for the current operator will cause the `Mask` setting to be unexpected, leading to precision errors.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id11 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id11 -->

```c
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t ELEMENTS = 16;
constexpr uint32_t BYTES = ELEMENTS * sizeof(uint64_t);

void print_data(const char* label, const std::vector<uint64_t>& data)
{
    std::cout << label << ":";
    const size_t count = data.size() < 8 ? data.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << data[i];
    if (data.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

__global__ __vector__ void asc_init_kernel(__gm__ uint64_t* output, __gm__ uint64_t* input)
{
    asc_init();
    __ubuf__ uint32_t local[BYTES / sizeof(uint32_t)];
    asc_copy_gm2ub_align(local, reinterpret_cast<__gm__ uint32_t*>(input), BYTES);
    // PIPE_MTE3 waits for the PIPE_MTE2 load to complete before copying UB data to GM.
    asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_set_atomic_add_int32();
    // Initialize again to clear the previously set atomic mode so that it does not affect subsequent ordinary stores.
    asc_init();
    // The atomic mode has been reset. This store should perform an ordinary UB-to-GM copy, producing a result identical to the input.
    asc_copy_ub2gm(reinterpret_cast<__gm__ uint32_t*>(output), local, BYTES);
    asc_sync();
}

} // namespace

int main()
{
    std::vector<uint64_t> input(ELEMENTS), output(ELEMENTS, 0);
    for (uint32_t i = 0; i < ELEMENTS; ++i) { input[i] = i + 1; output[i] = 10; }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint64_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, BYTES, input.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, BYTES, output.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_init_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output after asc_init reset", output);
    print_data("Golden ordinary copy", input);
    const bool passed = output == input;
    std::cout << (passed ? "[Success] asc_init passed." : "[Failed] asc_init failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
