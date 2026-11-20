---
okftype: reference.api
description: "Stores vector or mask register data contiguously and in an aligned manner to the Unified Buffer and automatically advances the destination address pointer after the store. The vector scenario supports immediate offset and non-contiguous DataBlock modes; the mask scenario updates by byte offset."
tag:
  - roles: [data_movement]
  - operations: [store]
  - traits: [aligned_transfer, post_update]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The body explains vector data registers and unaligned registers accordingly"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: related
    condition: "The text uses this to reference the general constraints of Reg vector computation"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: related
    condition: "When there is a data dependency with other instructions, insert a synchronization instruction to serialize"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "Destination pointer changed to pass-by-reference: CANN 9.1.0 Release Notes record that the Unified Buffer destination pointer parameter requiring automatic update in APIs such as asc_storealign_postupdate is corrected to a pointer reference, ensuring that the address change after the call is visible to the caller (corresponding to __ubuf__ <dtype>*& dst in the body)."
  - since: "9.1.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "Added vector_bool output overload: CANN 9.1.0 Release Notes explicitly list asc_storealign_postupdate as adding a vector_bool output overload, corresponding to the mask register immediate offset movement mode in the body."
  - since: "9.2.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "Unified Buffer destination address changed to pass-by-reference: CANN 9.2.0 Release Notes again record that the Unified Buffer destination address of asc_storealign_postupdate is changed to pass-by-reference, ensuring that the updated result after API execution can be written back to the caller."
---
# asc_storealign_postupdate

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.352323+00:00 pushedAt=2026-11-16T02:10:57.266Z nl=2 -->

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

The header file path is `"c_api/reg_compute/store/storealign.h"`.

Performs a contiguous aligned store of data from a vector data register or mask register to the Unified Buffer (UB). Post Update mode is enabled to automatically increment the destination address pointer after the store, eliminating the need to manually maintain the destination address across multiple consecutive calls in a hardware loop. When the source operand is a vector data register, the amount stored per operation is VL (256 bytes), and a mask can be configured to indicate the elements that participate in the store: elements with mask bit 1 are written to the destination address, and elements with mask bit 0 retain the original data at the destination address. When the source operand is a mask register, the amount stored per operation is the mask register length (32 bytes), and configuring a mask is not supported. The data format and content remain unchanged during the store. The destination operand is a UB address.

The vector data register store scenario provides the following two modes:

- **Immediate offset store mode**: Performs a contiguous aligned store of data from the vector data register to the current destination address. After the store is complete, the destination address pointer is incremented based on an immediate value, in elements.
- **Non-contiguous aligned store mode**: A single instruction stores 8 DataBlocks, with each DataBlock being 32 bytes. The stride between adjacent DataBlocks is configured through parameters. After the store is complete, the destination address pointer is incremented based on an immediate value, in units of 32 bytes.

The mask register store scenario provides the following mode:

- **Immediate offset store mode**: Stores the data in the mask register using a contiguous aligned store to the current destination address. After the store is complete, the destination address pointer is incremented by the immediate value, in bytes.

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

### Vector Data Register Immediate Offset Store Mode

```c
// Placeholder form.
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ <dtype>*& dst,
                                                      vector_<dtype> src,
                                                      int32_t offset,
                                                      vector_bool mask)
```

#### Data Types Supported by dtype

`dtype` values: `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

#### Typical Prototype Example

```c
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ int8_t*& dst,
                                                      vector_int8_t src,
                                                      int32_t offset,
                                                      vector_bool mask)
```

### Vector Data Register Non-Contiguous Aligned Store Mode

```c
// Placeholder form.
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ <dtype>*& dst,
                                                       vector_<dtype> src,
                                                       uint16_t block_stride,
                                                       uint16_t repeat_stride,
                                                       vector_bool mask)
```

#### Data Types Supported by dtype

`dtype` can be: `int8_t`, `uint8_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, and `float`.

#### Typical Prototype Example

```c
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ int8_t*& dst,
                                                      vector_int8_t src,
                                                      uint16_t block_stride,
                                                      uint16_t repeat_stride,
                                                      vector_bool mask)
```

### Mask Register Immediate Offset Store Mode

```c
// Placeholder form.
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ <dtype>*& dst,
                                                       vector_bool src,
                                                       int32_t offset)
```

#### Data Types Supported by dtype

`dtype` can be `uint8_t`, `uint16_t`, or `uint32_t`.

#### Typical Prototype Example

```c
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ uint8_t*& dst,
                                                      vector_bool src,
                                                      int32_t offset)
```

## Parameters

### Vector Data Register Immediate Offset Store Mode

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Input/output | Start address of the destination operand (vector), passed by pointer reference (`__ubuf__ <dtype>*&`). dtype must be the same as src, and the start address must be 32-byte aligned. After the store is complete, this pointer is automatically updated by hardware. |
| src | Input | Source operand (vector data register). dtype must be consistent with dst. |
| offset | Input | Destination address update amount, of type `int32_t`, in elements. After the API executes, the destination address is offset by `offset × sizeof(dtype)` bytes. |
| mask | Input | Source operand mask (mask register), used to indicate the elements that participate in the data movement. A value of 1 at the corresponding position indicates participation, and 0 indicates non-participation. |

### Vector Data Register Non-Contiguous Aligned Store Mode

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Input/output | Start address of the destination operand (vector), passed by pointer reference (`__ubuf__ <dtype>*&`). dtype must be the same as src, and the start address must be 32-byte aligned. After the store is complete, this pointer is automatically updated by hardware. |
| src | Input | Source operand (vector data register). dtype must be consistent with dst. |
| block_stride | Input | Stride between the start addresses of adjacent data blocks of the destination operand, of type `uint16_t`, in 32-byte units. |
| repeat_stride | Input | Offset updated after the destination address Post Update, of type `uint16_t`, in 32-byte units. |
| mask | Input | Source operand mask (mask register), used to indicate the elements that participate in the data movement. A value of 1 at the corresponding position indicates participation, and 0 indicates non-participation. |

### Mask Register Immediate Offset Store Mode

**Table 3** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Input/output | Start address of the destination operand (vector), passed by pointer reference (`__ubuf__ <dtype>*&`). The start address must be 32-byte aligned. After the store is complete, this pointer is automatically updated by hardware. |
| src | Input | Source operand (mask register). |
| offset | Input | Destination address update amount, of type `int32_t`, in bytes. After the API executes, the underlying UB address of the dst pointer is offset by offset bytes. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The start address of dst must be 32-byte aligned; otherwise, an error is reported.
- The UB capacity is capped at 256 KB. The capacity available to users varies with compilation options and programming scenarios (by default, 6 KB of SIMD VF stack and 2 KB of Ascend C reserved space are set aside, leaving 248 KB available; in SIMD+SIMT mixed programming, an additional 32 KB to 128 KB is partitioned as Data Cache, further reducing the available capacity). The actual access address of the destination operand and the address after Post Update must not exceed the actual available capacity; otherwise, an error is reported.
- If there is a data dependency between this instruction and other instructions, you must insert the synchronization instruction [asc_mem_bar](../reg_sync/asc_mem_bar.md) to ensure that the instructions execute serially and prevent abnormal data from occurring.

### Vector Data Register Store Scenario

- Positions where the mask bit is 0 use the hold mode: the corresponding positions of dst keep their original values, and src data is not written.
- In stride parameter mode, the actual access addresses determined by block_stride and repeat_stride must fall within the UB address range, and the new dst address after Post Update must still be 32-byte aligned; otherwise, an error is reported.

### Mask Register Store Scenario

- The new dst address after Post Update must still be 32-byte aligned; otherwise, the next call to this API reports an error.

## Examples

Save the code as `examples.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
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
void print_data(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

constexpr uint32_t REGISTER_BYTES = 256;
constexpr uint32_t BUFFER_BYTES = 260;
// Round up based on the per-iteration processing capacity to ensure that a tail block shorter than a full register is also processed.
constexpr uint32_t REPEAT_COUNT = (BUFFER_BYTES + REGISTER_BYTES - 1) / REGISTER_BYTES;
constexpr uint32_t STORAGE_BYTES = REPEAT_COUNT * REGISTER_BYTES;
constexpr uint32_t DATA_BYTES = BUFFER_BYTES;
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = BUFFER_BYTES / sizeof(uint8_t);
__simd_vf__ inline void Store(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = BUFFER_BYTES;
    for (uint16_t i = 0; i < repeat_time; ++i) {
      vector_bool mask = asc_update_mask_b8(remaining_length);
      vector_uint8_t src;
      asc_loadalign(src, input + i * REGISTER_BYTES);
      asc_storealign_postupdate(output, src, REGISTER_BYTES, mask);
    }
}

__global__ __vector__ void asc_storealign_postupdate_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[STORAGE_BYTES], input_local[STORAGE_BYTES];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint8_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<Store>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(BUFFER_BYTES), output(BUFFER_BYTES, 0xff);
    for (uint32_t i = 0; i < BUFFER_BYTES; ++i) input[i] = static_cast<uint8_t>(i % 251);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), (BUFFER_BYTES) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), (BUFFER_BYTES) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint8_t), input.data(), input.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_storealign_postupdate_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    const bool passed = input == output;
    std::cout << (passed ? "[Success] asc_loadalign completed." : "[Failed] asc_loadalign output mismatch.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
