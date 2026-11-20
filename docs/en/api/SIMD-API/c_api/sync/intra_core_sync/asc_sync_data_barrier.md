---
okftype: reference.api
description: "Blocks subsequent instructions until all previously issued but incomplete memory access instructions are complete. The memory range to wait for can be limited through parameters."
tag:
  - roles: [control]
  - operations: [synchronization]
  - traits: [inter_core_synchronization]
dependencies: []
version: ">=9.0.0"
version_changes: []
---
# asc_sync_data_barrier

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.543861+00:00 pushedAt=2026-11-16T02:10:57.773Z nl=2 -->

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

The header file path is `"c_api/sync/sync.h"`.

Blocks subsequent instruction execution until all previously issued but not yet completed memory access instructions finish. The `arg` parameter specifies the memory range affected by the blocking. For the value range and the meaning of each value, see [Parameters](#parameters).

## Prototype

```c
__aicore__ inline void asc_sync_data_barrier(mem_dsb_t arg)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :---  | :--- | :--- |
| arg | Input | Indicates the memory location to wait for. The type is mem_dsb_t, and the value can be: <br>&bull; DSB_ALL: Waits for all memory access instructions. <br>&bull; DSB_DDR: Waits for GM access instructions.<br>&bull; DSB_UB: Waits for UB access instructions.<br>&bull; DSB_SEQ: Reserved parameter, not yet enabled, reserved for future function extension.|

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- The input parameter of the mem_dsb_t type must directly pass an enumeration value literal.
- When the memory location to wait for is DSB_ALL, only access instructions to GM and Unified Buffer (UB) are waited for, excluding other memory locations such as L0C Buffer and L1 Buffer.

## Examples

This example starts two AIV cores and uses `asc_sync_data_barrier` to guarantee the write order of data when accessing GM across cores. Core 0 writes data first, then calls `asc_sync_data_barrier(mem_dsb_t::DSB_DDR)` to wait for the write operation to complete, and finally writes the synchronization flag. Core 1 polls the synchronization flag, reads the data after seeing the flag, and writes the computation result to GM.

Save the code as `example.asc` and compile and run it with the `bisheng` command. The `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <iostream>

#include "acl/acl.h"
#include "c_api/scalar_compute/scalar_load.h"
#include "c_api/scalar_compute/scalar_store.h"
#include "c_api/asc_simd.h"
#include "c_api/utils/sys_init.h"

namespace {

constexpr int32_t SYNC_FLAG = 6;
constexpr int32_t DATA_VALUE = 7;
constexpr int32_t EXPECTED = 2 * DATA_VALUE;
constexpr size_t SHARED_BYTES = 2 * sizeof(int32_t);
constexpr size_t OUTPUT_BYTES = sizeof(int32_t);

/*
 * The two cores write data and the synchronization flag separately to guarantee the data write order.
 * 1. The host initializes shared[0], shared[1], and output to 0 and starts two cores.
 * 2. Core 0 first writes DATA_VALUE to shared[1]. This write operation is the data protected by the synchronization flag.
 * 3. Core 0 calls DSB_DDR to wait for shared[1] to be written to GM, then writes SYNC_FLAG to shared[0] to publish the data.
 * 4. Core 1 polls shared[0] bypassing DCache and reads shared[1] after seeing SYNC_FLAG. The barrier guarantees that the data is visible at this point.
 * 5. Core 1 multiplies the data by 2 and writes it to output, then exits after the result is written to GM. The host should finally read EXPECTED.
 * If core 0 does not use DSB_DDR, the completion order of the two Scalar writes to GM is not guaranteed. Core 1 may see the synchronization flag first and then read data that has not yet been updated from shared[1].
 */
__global__ __vector__ void asc_sync_data_barrier_kernel(__gm__ int32_t* shared, __gm__ int32_t* output)
{
    asc_init();

    if (block_idx == 0) {
        // Write the depended-upon data first.
        asc_store_dev(shared + 1, DATA_VALUE);
        // Wait for the data to be written to GM before publishing the synchronization flag, ensuring that the data is visible when core 1 reads the flag.
        asc_sync_data_barrier(mem_dsb_t::DSB_DDR);
        asc_store_dev(shared, SYNC_FLAG);
    } else if (block_idx == 1) {
        // Continuously poll the synchronization flag. asc_load_dev bypasses DCache and reads GM directly each time.
        while (asc_load_dev(shared) != SYNC_FLAG) {
        }
        const int32_t value = asc_load_dev(shared + 1);
        asc_store_dev(output, 2 * value);
        // Wait for the result to be written to GM before exiting the kernel function (Kernel).
        asc_sync_data_barrier(mem_dsb_t::DSB_DDR);
    }
}

} // namespace

int main()
{
    int32_t output = 0;
    int32_t* sharedDevice = nullptr;
    int32_t* output_device = nullptr;

    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&sharedDevice), SHARED_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemset(sharedDevice, SHARED_BYTES, 0, SHARED_BYTES);
    aclrtMemset(output_device, OUTPUT_BYTES, 0, OUTPUT_BYTES);

    asc_sync_data_barrier_kernel<<<2, 0>>>(sharedDevice, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(&output, OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = output == EXPECTED;
    std::cout << (passed ? "[Success] asc_sync_data_barrier passed."
                          : "[Failed] asc_sync_data_barrier failed.") << std::endl;

    aclrtFree(sharedDevice);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
