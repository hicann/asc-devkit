---
okftype: reference.api
description: "Updates the address register addr_reg to support 1- to 4-dimensional iterative addressing. In the b8, b16, and b32 bit-width modes, it calculates the offset of each dimension in units of 1, 2, and 4 bytes respectively, for Reg load and store APIs to increment the Unified Buffer address in loops."
tag:
  - roles: [data_movement_configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The text uses this to describe the addr_reg address register type"
  - path: docs/zh/api/SIMD-API/c_api/deprecated_interface/asc_create_iter_reg_deprecated.md
    kind: related
    condition: "This API family replaces the deprecated asc_create_iter_reg_* and iter_reg types"
version: ">=9.2.0"
version_changes:
  - since: "9.2.0"
    kind: renamed
    scope: "page"
    scope_basis: kind_default
    note: "The API family and type are uniformly renamed: the New Features section of the CANN 9.2.0 Release Notes states that addr_reg is used to store offsets, replacing the original iter_reg API family. The deprecated API table lists iter_reg and asc_create_iter_reg_b8/b16/b32 as renamed to addr_reg and asc_update_addr_reg_b8/b16/b32; this page describes the renamed new API family."
---
# asc_update_addr_reg

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.471601+00:00 pushedAt=2026-11-16T02:10:55.626Z nl=2 -->

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

The header file path is `"c_api/reg_compute/compute/reg_addr_reg.h"`.

Updates the address register ([addr_reg](../../defs/type/data_type_definition.md#address-register)) for iterative addressing. Supports 1 to 4 dimension offset settings, with bit widths supporting b8, b16, and b32 modes:

- b8 mode: The address offset per iteration is in units of 1 byte, suitable for operations on int8_t, uint8_t, and similar types.
- b16 mode: The address offset per iteration is in units of 2 bytes, suitable for operations on half, int16_t, and similar types.
- b32 mode: The address offset per iteration is in units of 4 bytes, suitable for operations on float, int32_t, and similar types.

The address offset calculation formula for each iteration is: offset = index0×stride0 + index1×stride1 + index2×stride2 + index3×stride3, where index_i is the loop iteration variable of each dimension and stride_i is the stride of each dimension. For example, in a 4D scenario (N, C, H, W), offset0 corresponds to the W dimension (innermost), offset1 corresponds to H×W, offset2 corresponds to C×H×W, and offset3 corresponds to N×C×H×W. After addr_reg is updated, it is used with APIs such as asc_loadalign/asc_storealign to automatically increment the Unified Buffer (UB) address in a loop according to the configured offset.

> [!NOTE]
>The legacy APIs `asc_create_iter_reg_*` are deprecated. Use `asc_update_addr_reg_*` instead. The legacy return type `iter_reg` has also been renamed to `addr_reg`.

## Prototype

- 1D offset

    ```c
    __simd_callee__ inline addr_reg asc_update_addr_reg_b8(uint32_t offset)
    __simd_callee__ inline addr_reg asc_update_addr_reg_b16(uint32_t offset)
    __simd_callee__ inline addr_reg asc_update_addr_reg_b32(uint32_t offset)
    ```

- 2D offset

    ```c
    __simd_callee__ inline addr_reg asc_update_addr_reg_b8(uint32_t offset0,
                                                           uint32_t offset1)
    __simd_callee__ inline addr_reg asc_update_addr_reg_b16(uint32_t offset0,
                                                            uint32_t offset1)
    __simd_callee__ inline addr_reg asc_update_addr_reg_b32(uint32_t offset0,
                                                            uint32_t offset1)
    ```

- 3D offset

    ```c
    __simd_callee__ inline addr_reg asc_update_addr_reg_b8(uint32_t offset0,
                                                           uint32_t offset1,
                                                           uint32_t offset2)
    __simd_callee__ inline addr_reg asc_update_addr_reg_b16(uint32_t offset0,
                                                            uint32_t offset1,
                                                            uint32_t offset2)
    __simd_callee__ inline addr_reg asc_update_addr_reg_b32(uint32_t offset0,
                                                            uint32_t offset1,
                                                            uint32_t offset2)
    ```

- 4D offset

    ```c
    __simd_callee__ inline addr_reg asc_update_addr_reg_b8(uint32_t offset0,
                                                           uint32_t offset1,
                                                           uint32_t offset2,
                                                           uint32_t offset3)
    __simd_callee__ inline addr_reg asc_update_addr_reg_b16(uint32_t offset0,
                                                            uint32_t offset1,
                                                            uint32_t offset2,
                                                            uint32_t offset3)
    __simd_callee__ inline addr_reg asc_update_addr_reg_b32(uint32_t offset0,
                                                            uint32_t offset1,
                                                            uint32_t offset2,
                                                            uint32_t offset3)
    ```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| offset | Input | Offset of the 1st dimension (innermost), in units of elements.<br>Data type: uint32_t.<br>For example, in b32 mode, offset=64 means 64 float elements (one VL, that is, 256B). |
| offset0 | Input | Offset of the 1st dimension (innermost, such as the W dimension), in units of elements.<br>Data type: uint32_t. |
| offset1 | Input | Offset of the 2nd dimension (such as the total stride of the H×W dimension), in units of elements.<br>Data type: uint32_t. |
| offset2 | Input | Offset of the 3rd dimension (such as the total stride of the C×H×W dimension), in units of elements.<br>Data type: uint32_t. |
| offset3 | Input | Offset of the 4th dimension (outermost, such as the total stride of the N×C×H×W dimension), in units of elements.<br>Data type: uint32_t. |

## Return Value

Returns addr_reg, the address register.

## Constraints

- Do not use if/else inside the loop.
- addr_reg must be assigned and used within the innermost loop body.
- For operands with different strides, use multiple addr_reg instances to avoid address overwriting.
- The maximum number of addr_reg instances is 8. Exceeding 8 may cause performance degradation. The compiler automatically reuses addr_reg registers whose lifetimes have ended.
- Due to hardware (HardwareLoop) limitations, addr_reg supports a maximum of 4 loop levels.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"

namespace {
constexpr uint8_t MUTEX_ID = 0;


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

constexpr uint16_t ONE_REPEAT_SIZE = 256;
constexpr uint32_t DATA_BYTES = 523;
constexpr uint16_t REPEAT_COUNT = (DATA_BYTES + ONE_REPEAT_SIZE - 1) / ONE_REPEAT_SIZE;
constexpr uint32_t TOTAL_BYTES = ONE_REPEAT_SIZE * REPEAT_COUNT;

// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint8_t);
__simd_vf__ inline void exercise(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input,
                                uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint8_t value;
    addr_reg offset_reg;
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = DATA_BYTES;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        vector_bool mask = asc_update_mask_b8(remaining_length);
        offset_reg = asc_update_addr_reg_b8(one_repeat_size);
        asc_loadalign(value, input, offset_reg);
        asc_storealign(output, value, offset_reg, mask);
    }
}

__global__ __vector__ void asc_update_addr_reg_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[TOTAL_BYTES];
    __ubuf__ uint8_t input_local[TOTAL_BYTES];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, TOTAL_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint8_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<exercise>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, TOTAL_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(TOTAL_BYTES, 0), output(TOTAL_BYTES, 0xff), golden(TOTAL_BYTES, 0xff);
    for (uint32_t i = 0; i < DATA_BYTES; ++i) input[i] = static_cast<uint8_t>(i % 251);
    golden = input;
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), TOTAL_BYTES * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), TOTAL_BYTES * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint8_t), input.data(), input.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_update_addr_reg_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (uint32_t i = 0; i < DATA_BYTES; ++i) passed = passed && output[i] == golden[i];
    std::cout << (passed ? "[Success] asc_update_addr_reg_b8 passed." : "[Failed] asc_update_addr_reg_b8 failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
