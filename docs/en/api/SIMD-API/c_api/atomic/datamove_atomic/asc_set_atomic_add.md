---
okftype: reference.api
description: "Enables atomic accumulation for subsequent data movement whose destination address is Global Memory: sums with the existing data in GM and writes back. The accumulation type is selected by six parameterless prototypes: int8, int16, int32, float16, bfloat, and float, and must be consistent with the subsequent movement; it takes effect only for MTE3 and Fixpipe movement, scalar writes to GM are not affected, and it can be disabled with asc_disable_dma_atomic."
tag:
  - roles: [data_movement_configuration]
  - operations: [add]
  - traits: [atomic]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/atomic/datamove_atomic/asc_disable_dma_atomic.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/atomic/key_features.md
    kind: related
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: arch_support_extended
    scope: "section:function prototype"
    scope_basis: resolved
    note: "asc_set_atomic_add_int8, asc_set_atomic_add_int16, and the old name asc_set_atomic_add_int have been available on Ascend 950PR&950DT (3510) since 9.1.0: upstream/9.1.0's impl/c_api/instr_impl/npu_arch_3510/atomic_impl.h adds the definitions of the three, and the public header file also promotes them out of the architecture-specific branch; Atlas A2/A3 (2201) has declared them since 9.0.0, and the Release Notes 9.1.0 enhancement section also lists these three interfaces."
  - since: "9.2.0"
    kind: overload_added
    scope: "section:function prototype"
    scope_basis: resolved
    note: "Added asc_set_atomic_add_int32(), which uses an explicit data type suffix to replace asc_set_atomic_add_int() marked deprecated since 9.2.0; both perform int32_t atomic accumulation, and the old name is decorated with ASC_DEPRECATED(9.2.0, ...) in the header file but is still retained."
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "The declaration is moved from include/c_api/atomic/atomic.h to the module-split include/c_api/atomic/datamove_atomic.h; the header file path c_api/atomic/datamove_atomic.h marked in the body is exactly the path after 9.2.0."
---
# asc_set_atomic_add

<!-- md-trans-meta sourceCommit=b31f8bdaed16db5f3a14393c0665e0940dcd6f52 translatedAt=2026-11-16T09:21:36.281Z pushedAt=2026-11-18T08:15:34.166Z nl=2 -->

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

The header file path is: `"c_api/atomic/datamove_atomic.h"`.

Atomic add process: The data to be moved to GM is added to the existing data on GM, and the sum is written to GM. This API enables atomic add for subsequent data move instructions whose destination address is GM. For the data movement paths supported by different products, see [Constraints](#constraints).

The API provides different function prototypes for setting different accumulation data types.

## Prototype

```c
__aicore__ inline void asc_set_atomic_add_int8()
__aicore__ inline void asc_set_atomic_add_int16()
__aicore__ inline void asc_set_atomic_add_int32()
__aicore__ inline void asc_set_atomic_add_float16()
__aicore__ inline void asc_set_atomic_add_bfloat()
__aicore__ inline void asc_set_atomic_add_float()
```

## Parameters

None

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Due to different hardware architectures, each product supports different data paths, as follows:
    <!-- npu="950" id11 -->
    - Ascend 950PR&950DT products support the data path UB/L0C Buffer->GM.
    <!-- end id11 -->
    <!-- npu="A3" id8 -->
    - Atlas A3 products support the data path UB/L0C Buffer/L1 Buffer->GM.
    <!-- end id8 -->
    <!-- npu="910b" id9 -->
    - Atlas A2 products support the data path UB/L0C Buffer/L1 Buffer->GM.
    <!-- end id9 -->
- After this API is called, atomic operations are enabled for all subsequent data move instructions whose destination address is GM. You can call [asc_disable_dma_atomic](asc_disable_dma_atomic.md) to disable atomic operations.
- This API does not automatically clear the existing data on GM before execution. If you expect the original data on GM to be zero before atomic add, you must clear it manually.
- This API takes effect only on subsequent data move instructions whose destination address is GM (moved through the MTE3 and Fixpipe units), and does not take effect on instructions that write to GM via scalar operations (for example, [asc_store_dev](../../scalar_compute/scalar_store/asc_store_dev.md)).
- The synchronization between this API and the immediately following data move instruction is guaranteed by hardware, so the multi-pipeline synchronization inserted in the following example is unnecessary:

    ```c
    asc_set_atomic_add_int8();

    /*
    Synchronization between asc_set_atomic_add_int8 and asc_copy_ub2gm is guaranteed by hardware, so the following synchronization is unnecessary.
    asc_sync_notify(PIPE_S, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_S, PIPE_MTE3, EVENT_ID0);
    */

    asc_copy_ub2gm(dst, src1, total_length * sizeof(int8_t));
    // Disable the atomic operation.
    asc_disable_dma_atomic();
    ```

- The operation data type of the subsequent data move instruction must be consistent with the data type set by the selected API.
- This API does not guarantee the execution order of subsequent data move instructions. To ensure a deterministic execution order, see [Key Features](../key_features.md).

## Examples

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id10 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id10 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t ELEMENTS = 64;
constexpr uint32_t BYTES = ELEMENTS * sizeof(float);

template <typename T>
void print_data(const char* label, const std::vector<T>& data)
{
    std::cout << label << ":";
    const size_t count = data.size() < 8 ? data.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +data[i];
    if (data.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

__global__ __vector__ void asc_set_atomic_add_kernel(__gm__ float* output, __gm__ float* input0, __gm__ float* input1)
{
    asc_init();
    __ubuf__ float local0[ELEMENTS];
    __ubuf__ float local1[ELEMENTS];
    asc_copy_gm2ub_align(local0, input0, BYTES);
    asc_copy_gm2ub_align(local1, input1, BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm(output, local0, BYTES);
    asc_set_atomic_add_float();
    asc_sync_pipe(PIPE_MTE3);
    asc_copy_ub2gm(output, local1, BYTES);
    asc_disable_dma_atomic();
    asc_sync();
}

}

int main()
{
    std::vector<float> input0(ELEMENTS), input1(ELEMENTS), output(ELEMENTS), golden(ELEMENTS);
    for (uint32_t i = 0; i < ELEMENTS; ++i) {
        input0[i] = static_cast<float>(i % 8 + 1);
        input1[i] = 2.0f;
        golden[i] = input0[i] + input1[i];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    float *input0_device = nullptr, *input1_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input0_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&input1_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input0_device, BYTES, input0.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(input1_device, BYTES, input1.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_set_atomic_add_kernel<<<1, 0>>>(output_device, input0_device, input1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input0", input0);
    print_data("Input1", input1);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_set_atomic_add passed." : "[Failed] asc_set_atomic_add failed.") << std::endl;
    aclrtFree(input0_device);
    aclrtFree(input1_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
