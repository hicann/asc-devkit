---
okftype: reference.api
description: "Sets the enable bit and operation type for scalar atomic operations: type specifies the data type encoding involved in the atomic operation, and op specifies the atomic operation type (currently only sum is supported); it takes effect only on Atlas A2 and Atlas A3 products. After use, it is recommended to call asc_disable_dma_atomic to clear the atomic operation state."
tag:
  - roles: [data_movement_configuration]
  - traits: [atomic]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/atomic/datamove_atomic/asc_disable_dma_atomic.md
    kind: related
    condition: "It is recommended to call it after use to clear the atomic operation state and avoid affecting subsequent instructions"
  - path: docs/zh/api/SIMD-API/c_api/atomic/scalar_atomic/asc_get_store_atomic_config.md
    kind: related
    condition: "In the example, it is used to read back and verify the configuration value written by this interface"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "After the public header file module split, this interface was moved from include/c_api/atomic/atomic.h to c_api/atomic/scalar_atomic.h as described in the main text, and the implementation was moved to impl/c_api/memory_base_impl/scalar_atomic_intf_impl.h; the encoding scheme of type_mask=0x7 and op_bit=3 and the two-parameter signature remain unchanged."
---
# asc_set_store_atomic_config_v1

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T09:45:33.213Z pushedAt=2026-11-18T08:15:34.188Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
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

The header file path is: `"c_api/atomic/scalar_atomic.h"`.

Set the value of the atomic operation enable bit and the atomic operation type.

## Prototype

```c
__aicore__ inline void asc_set_store_atomic_config_v1(uint16_t type,
                                                      uint16_t op)
```

## Parameters

**Table 1** Parameter description

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| type | Input | Atomic operation enable bit.<br>0: No atomic operation.<br>1: Enable the atomic operation, with the data type float.<br>2: Enable the atomic operation, with the data type half.<br>3: Enable the atomic operation, with the data type int16_t.<br>4: Enable the atomic operation, with the data type int32_t.<br>5: Enable the atomic operation, with the data type int8_t.<br>6: Enable the atomic operation, with the data type bfloat16_t. |
| op | Input | Atomic operation type.<br>0: Sum operation. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- After use, call [asc_disable_dma_atomic](../datamove_atomic/asc_disable_dma_atomic.md) to clear the atomic operation state, so as not to affect the subsequent related instructions.
- This instruction does not clear the GM data before execution. You need to manually add a clearing operation when required.

## Examples

Save the following code as `example.asc`, and then run the following compilation and execution command:

<!-- npu="A3,910b" id9 -->
Using [NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-2201 && ./main
```
<!-- end id9 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t ELEMENTS = 64;
constexpr uint32_t BYTES = ELEMENTS * sizeof(int32_t);

template <typename T>
void PrintData(const char* label, const std::vector<T>& data)
{
    std::cout << label << ":";
    const size_t count = data.size() < 8 ? data.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +data[i];
    if (data.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

__global__ __vector__ void AscSetStoreAtomicConfigV1Kernel(__gm__ int32_t* output, __gm__ int32_t* input,
    __gm__ uint64_t* raw_config)
{
    asc_init();
    // Enable the float atomic sum operation, then read the original value of the atomic configuration register.
    asc_set_store_atomic_config_v1(1, 0);
    asc_store_atomic_config config;
    asc_get_store_atomic_config(config);
    raw_config[0] = config.config;
    asc_dcci_single(raw_config);
    asc_sync_data_barrier(mem_dsb_t::DSB_DDR);
    asc_disable_dma_atomic();
}

} // namespace

int main()
{
    std::vector<int32_t> input(ELEMENTS), output(ELEMENTS, 10), golden(ELEMENTS);
    for (uint32_t i = 0; i < ELEMENTS; ++i) {
        input[i] = static_cast<int32_t>(i % 8 + 1);
        golden[i] = 10;
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    int32_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    uint64_t* config_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&config_device), sizeof(uint64_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, BYTES, input.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, BYTES, output.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    AscSetStoreAtomicConfigV1Kernel<<<1, 0>>>(output_device, input_device, config_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    std::vector<uint64_t> raw_config(1);
    aclrtMemcpy(raw_config.data(), sizeof(uint64_t), config_device, sizeof(uint64_t), ACL_MEMCPY_DEVICE_TO_HOST);
    PrintData("Input", input);
    PrintData("Output", output);
    PrintData("Golden", golden);
    PrintData("Raw atomic config", raw_config);
    aclrtFree(config_device);
    bool passed = output == golden && (raw_config[0] == 1);
    std::cout << (passed ? "[Success] asc_get_store_atomic_config passed." : "[Failed] asc_get_store_atomic_config failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
