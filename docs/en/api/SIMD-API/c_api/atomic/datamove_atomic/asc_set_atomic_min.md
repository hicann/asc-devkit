---
okftype: reference.api
description: "Enables atomic compare-and-minimum for subsequent data movement whose destination address is Global Memory: compares with existing data in GM and writes back the minimum value. Provides six parameterless prototypes for int8, int16, int32, float16, bfloat, and float to select the data type, which must match the subsequent movement; GM data is not automatically cleared to zero before enabling, and it can be disabled with asc_disable_dma_atomic."
tag:
  - roles: [data_movement_configuration]
  - operations: [minimum]
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
    scope: "section:Function Prototype"
    scope_basis: resolved
    note: "asc_set_atomic_min_int8, asc_set_atomic_min_int16, and the old name asc_set_atomic_min_int have been available on Ascend 950PR&950DT (3510) since 9.1.0: upstream/9.1.0's impl/c_api/instr_impl/npu_arch_3510/atomic_impl.h adds the definitions of the three; the float, float16, and bfloat minimum prototypes have been available on 3510 since 9.0.0, and the Release Notes 9.1.0 enhancement section does not list the min family, so this is based on the 3510 implementation-side evidence."
  - since: "9.2.0"
    kind: overload_added
    scope: "section:Function Prototype"
    scope_basis: resolved
    note: "Added asc_set_atomic_min_int32(), replacing asc_set_atomic_min_int(), which has been marked deprecated since 9.2.0; the Release Notes 9.2.0 new feature entry also names asc_set_atomic_min_int32, and both are int32_t atomic minimum."
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "The declaration was moved from include/c_api/atomic/atomic.h to the module-split include/c_api/atomic/datamove_atomic.h, and the header file path in the body was updated accordingly."
---
# asc_set_atomic_min

<!-- md-trans-meta sourceCommit=b31f8bdaed16db5f3a14393c0665e0940dcd6f52 translatedAt=2026-11-16T09:24:51.353Z pushedAt=2026-11-18T08:15:34.170Z nl=2 -->

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

Compares the data to be moved to GM with the existing data on GM, and then writes the smaller of the two to GM. This API enables atomic compare-and-min for subsequent data move instructions whose destination address is GM. For the data move paths supported by different products, see [Constraints](#constraints).

The API provides different function prototypes for setting different data types for compare-and-min.

## Prototype

```c
__aicore__ inline void asc_set_atomic_min_int8()
__aicore__ inline void asc_set_atomic_min_int16()
__aicore__ inline void asc_set_atomic_min_float16()
__aicore__ inline void asc_set_atomic_min_bfloat()
__aicore__ inline void asc_set_atomic_min_int32()
__aicore__ inline void asc_set_atomic_min_float()
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
- This API does not automatically zero the existing data on GM before execution. If you expect the original data on GM to be zero before the atomic compare-and-min, you must clear it manually.
- This API takes effect only on subsequent data move instructions whose destination address is GM (moved through the MTE3 and Fixpipe units), and does not take effect on instructions that write to GM via scalar operations (for example, [asc_store_dev](../../scalar_compute/scalar_store/asc_store_dev.md)).
- The synchronization between this API and the immediately following data move instruction is guaranteed by hardware, so the multi-pipeline synchronization inserted in the following example is unnecessary:

    ```c
    asc_set_atomic_min_int8();

    /*
    The synchronization between asc_set_atomic_min_int8 and asc_copy_ub2gm is guaranteed by hardware, so the following synchronization is unnecessary.
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

__global__ __vector__ void asc_set_atomic_min_kernel(__gm__ float* output, __gm__ float* input0, __gm__ float* input1)
{
    asc_init();
    __ubuf__ float local0[ELEMENTS];
    __ubuf__ float local1[ELEMENTS];
    asc_copy_gm2ub_align(local0, input0, BYTES);
    asc_copy_gm2ub_align(local1, input1, BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm(output, local0, BYTES);
    asc_set_atomic_min_float();
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
        golden[i] = input0[i] < input1[i] ? input0[i] : input1[i];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    float *input0_device = nullptr, *input1_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input0_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&input1_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input0_device, BYTES, input0.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(input1_device, BYTES, input1.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_set_atomic_min_kernel<<<1, 0>>>(output_device, input0_device, input1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input0", input0);
    print_data("Input1", input1);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_set_atomic_min passed." : "[Failed] asc_set_atomic_min failed.") << std::endl;
    aclrtFree(input0_device);
    aclrtFree(input1_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
