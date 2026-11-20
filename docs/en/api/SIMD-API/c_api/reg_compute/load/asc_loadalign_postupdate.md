---
okftype: reference.api
description: "Loads data from a 32-byte aligned address in UB into a vector or mask register and automatically updates the source pointer after the load, supporting both immediate offset and non-contiguous DataBlock interval load modes."
tag:
  - roles: [data_movement]
  - operations: [load]
  - traits: [post_update]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The body describes vector data registers and mask registers accordingly"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_addr_reg/asc_update_addr_reg.md
    kind: related
    condition: "In the int64_t mask example, asc_update_addr_reg_b32 is used to generate the address register offset"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_permute_sel/asc_unpack.md
    kind: related
    condition: "When dtype is int64_t, the b32 mask must be expanded into a b64 usable mask"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: related
    condition: "When there is a data dependency with other instructions, insert a synchronization instruction to serialize"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "The UB source pointer parameter is changed to pass by reference: CANN 9.1.0 Release Notes record that the UB source pointer parameters requiring automatic update in Post Update APIs such as asc_loadalign_postupdate are corrected to pointer references, ensuring that address changes after the call are visible to the caller (consistent with the reference form __ubuf__ <dtype>*& src in the body)."
  - since: "9.1.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "Adds a vector_bool overload for loading into a mask register: CANN 9.1.0 Release Notes record that asc_loadalign_postupdate adds a vector_bool overload, corresponding to the single prototype asc_loadalign_postupdate(vector_bool& dst, __ubuf__ uint32_t*& src, int32_t offset)."
  - since: "9.2.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "Fixes overload defects: CANN 9.2.0 Release Notes record the fix for the vector_bool overload missing the offset parameter (making the public declaration consistent with the implementation), and the fix for overloads with block_stride and repeat_stride only updating a local source pointer copy, ensuring that in consecutive calls the source address update is written back to the caller and loading continues from the correct position."
---
# asc_loadalign_postupdate

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.382874+00:00 pushedAt=2026-11-16T02:10:55.537Z nl=2 -->

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

The header file path is `"c_api/reg_compute/load/loadalign.h"`.

Reads data from a 32-byte-aligned start address in the Unified Buffer (UB) and loads it into a vector data register or mask register. The data format and content remain unchanged during the load. The API uses Post Update mode and automatically updates the source address pointer after the load is complete. This API provides two functional modes:

- **Immediate offset load mode**: Loads data from the current source address, and automatically updates the source address by the specified stride after the load is complete.
- **Non-contiguous aligned load mode**: Loads 8 `DataBlock` blocks non-contiguously from the current source address at the specified data block interval. Each `DataBlock` is 32 bytes. After the load is complete, the source address is automatically updated by the specified stride.

**Figure 1** Immediate offset load mode

![](../../../../figures/capi_loadalign_norm.png)

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

### Immediate Offset Load Mode

```c
// Load into a vector data register.
__simd_callee__ inline void asc_loadalign_postupdate(vector_<dtype>& dst,
                                                     __ubuf__ <dtype>*& src,
                                                     int32_t offset)
// Load into a mask register.
__simd_callee__ inline void asc_loadalign_postupdate(vector_bool& dst,
                                                     __ubuf__ uint32_t*& src,
                                                     int32_t offset)
```

#### Data Types Supported by dtype

`dtype` can be `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, `int64_t`, or `uint64_t`. When `dtype` is `int4b_t`, the actual type of dst is `vector_int4x2_t`.

#### Typical Prototype Example

```c
// Example: float type.
__simd_callee__ inline void asc_loadalign_postupdate(vector_float& dst,
                                                     __ubuf__ float*& src,
                                                     int32_t offset)
```

### Non-Contiguous Aligned Load Mode

```c
__simd_callee__ inline void asc_loadalign_postupdate(vector_<dtype>& dst,
                                                     __ubuf__ <dtype>*& src,
                                                     uint16_t block_stride,
                                                     uint16_t repeat_stride,
                                                     vector_bool mask)
```

#### Data Types Supported by dtype

`dtype` can be `int4b_t`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, or `int64_t`. When `dtype` is `int4b_t`, the actual type of dst is `vector_int4x2_t`.

#### Typical Prototype Example

```c
// Example: float type.
__simd_callee__ inline void asc_loadalign_postupdate(vector_float& dst,
                                                     __ubuf__ float*& src,
                                                     uint16_t block_stride,
                                                     uint16_t repeat_stride,
                                                     vector_bool mask)
```

## Parameters

### Immediate Offset Load Mode

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination vector data register or mask register.<br>&bull; When `dst` is a vector data register, the dtype must be the same as `src`, and VL bytes of data are loaded.<br>&bull; When `dst` is a mask register, VL/8 bytes of data are loaded. |
| src | Input/output | Source UB address. The actual read address must be aligned to 32 bytes. |
| offset | Input | Post Update stride. The API updates `src` after completing the load from the current `src` address.<br>&bull; When `dst` is a vector data register, the unit is the number of elements, and `src` increases by `offset × sizeof(dtype)` bytes.<br>&bull; When `dst` is a mask register, the unit is bytes, and the `src` pointer increases by `offset` bytes. |

### Non-Contiguous Aligned Load Mode

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination vector data register. The dtype must be the same as `src`, and VL bytes of data are loaded. |
| src | Input/output | Source UB address. The actual read address must be aligned to 32 bytes. |
| block_stride | Input | Stride between the start addresses of adjacent `DataBlock` units in the source operand, in 32-byte units. |
| repeat_stride | Input | Post Update stride, in 32-byte units. After the API completes this load, it increases the `src` address by `repeat_stride × 32` bytes. |
| mask | Input | Mask register that indicates which elements participate in the computation. This API uses `DataBlock` as the data movement unit.<br>&bull; When any element in a `DataBlock` is selected as valid by `mask`, all data in that `DataBlock` is loaded into the vector data register.<br>&bull; When all elements in a `DataBlock` are selected as invalid by `mask`, the data in that `DataBlock` is not loaded into the vector data register, and the elements at the corresponding positions are set to 0. No error is reported even if the UB access is out of bounds. |

For details about vector data registers and mask registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- In each functional mode, the current actual read address and the next actual read address after the Post Update must be 32-byte aligned, and the actual read range must be within the UB address space without exceeding the boundary; otherwise, an error is reported. In non-contiguous aligned load mode, when all elements contained in the `mask` of a `DataBlock` are invalid, no error is reported for that `DataBlock` even if it is out of bounds.
- The UB capacity is capped at 256 KB. The capacity available to users varies with compilation options and programming scenarios (by default, 6 KB of SIMD VF stack and 2 KB of Ascend C reserved space are set aside, leaving 248 KB available; in SIMD+SIMT mixed programming, an additional 32 KB to 128 KB is partitioned as Data Cache, further reducing the available capacity). The UB address after applying the offset must not exceed the actual available capacity; otherwise, an error is reported.
- If there is a data dependency between this instruction and other instructions, you must insert the synchronization instruction [asc_mem_bar](../reg_sync/asc_mem_bar.md) to ensure that the instructions execute serially and prevent abnormal data from occurring.

### Non-Contiguous Aligned Load Mode

- This mode does not support a mask register as the destination operand.
- When the dtype is `int64_t`, because the mask creation API does not support the b64 mode, first generate a b32 mask through [asc_create_mask_b32](../reg_mask/asc_create_mask.md), and then expand it into a mask suitable for b64 data through [asc_unpack_lower](../reg_permute_sel/asc_unpack.md) before passing it to this API. In the expanded mask, every 8 consecutive bits form a group, and only the lowest bit in each group is valid, controlling the corresponding b64 element.

    ```c
    vector_int64_t src_reg;
    vector_bool mask_b32 = asc_create_mask_b32(PAT_ALL);
    vector_bool mask_b64;
    asc_unpack_lower(mask_b64, mask_b32);
    addr_reg load_offset;
    addr_reg store_offset;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        load_offset = asc_update_addr_reg_b32(elements_vl * 2);
        // After the data is stored to UB, the data type is B32, so the offset does not need to be multiplied by 2.
        store_offset = asc_update_addr_reg_b32(elements_vl);
        asc_loadalign(src_reg, input, load_offset);
        asc_storealign_pack(output, src_reg, store_offset, mask_b64);
    }
    ```

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
void print_data(const char* label, const std::vector<T>& dst_regs)
{
    std::cout << label << ":";
    const size_t count = dst_regs.size() < 8 ? dst_regs.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +dst_regs[i];
    if (dst_regs.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

constexpr uint32_t BUFFER_ELEMENTS = 260;
constexpr uint32_t ELEMENTS_PER_REGISTER = 256;
constexpr uint32_t BUFFER_STORAGE_ELEMENTS =
    AscendC::Std::ceil_div(BUFFER_ELEMENTS, ELEMENTS_PER_REGISTER) * ELEMENTS_PER_REGISTER;
constexpr uint32_t BUFFER_BYTES = BUFFER_ELEMENTS * sizeof(uint8_t);
// Record the actual length to calculate the iteration count and limit the valid range of the final iteration.
constexpr uint32_t TOTAL_LENGTH = BUFFER_ELEMENTS;
constexpr uint32_t DATA_BYTES = TOTAL_LENGTH * sizeof(uint8_t);
__simd_vf__ inline void asc_loadalign_postupdate_vf(__ubuf__ uint8_t* output,
                                                    __ubuf__ uint8_t* input,
                                                    uint32_t one_repeat_size, uint16_t repeat_time)
{
    // Keep the number of remaining valid elements for updating the mask to handle the tail block.
    uint32_t remaining_length = BUFFER_ELEMENTS;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        vector_bool mask = asc_update_mask_b8(remaining_length);
        vector_uint8_t src_reg;
        asc_loadalign_postupdate(src_reg, input, one_repeat_size);
        asc_storealign(output + i * one_repeat_size, src_reg, mask);
    }
}

__global__ __vector__ void asc_loadalign_postupdate_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[BUFFER_STORAGE_ELEMENTS], input_local[BUFFER_STORAGE_ELEMENTS];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);

    asc_unlock(PIPE_MTE2, MUTEX_ID);


    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint8_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<asc_loadalign_postupdate_vf>(output_local, input_local, one_repeat_size, repeat_time);

    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);

    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(BUFFER_ELEMENTS), output(BUFFER_ELEMENTS, 0xff);
    for (uint16_t i = 0; i < BUFFER_ELEMENTS; ++i) input[i] = static_cast<uint8_t>(i % 251);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BUFFER_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BUFFER_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint8_t), input.data(), input.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_loadalign_postupdate_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    const bool passed = input == output;
    std::cout << (passed ? "[Success] asc_loadalign_postupdate completed."
                         : "[Failed] asc_loadalign_postupdate output mismatch.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
