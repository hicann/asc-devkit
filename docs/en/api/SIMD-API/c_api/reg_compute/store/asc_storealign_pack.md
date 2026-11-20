---
okftype: reference.api
description: "Compresses and moves vector or mask register data out to the Unified Buffer: in the vector scenario, the low-half bits of valid elements are written contiguously according to the mask; in the mask scenario, 1 bit is discarded every 1-bit interval for compressed storage. Three offset modes are provided for each."
tag:
  - roles: [data_movement]
  - operations: [compression, store]
  - traits: [aligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The main text describes vector data registers/mask registers accordingly"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: related
    condition: "The text uses this to reference the general constraints of Reg vector computation"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_addr_reg/asc_update_addr_reg.md
    kind: related
    condition: "In address register offset mode, it must be called first to generate the offset"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: related
    condition: "When there is a data dependency with other instructions, insert a synchronization instruction to serialize"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "Added vector_bool output overload: CANN 9.1.0 Release Notes explicitly list asc_storealign_pack as adding a vector_bool output overload, corresponding to the three modes for compressed movement of mask register data in the body (the offset unit changes from elements to bytes)."
---
# asc_storealign_pack

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.324238+00:00 pushedAt=2026-11-16T02:10:57.218Z nl=2 -->

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

Compresses and stores data from a vector data register or mask register to the Unified Buffer (UB). When the source operand is a vector data register, `mask` indicates the elements that participate in the store. When the corresponding position in `mask` is 1, the lower half of the bit data of the valid element is written to the corresponding compressed destination position. When the corresponding position in `mask` is 0, the corresponding compressed destination position retains its original value. When the source operand is a mask register, every other bit is discarded, and the retained bits are contiguously compressed and stored to the destination address. Mask configuration is not supported in the mask register store scenario.

This API provides six functional modes with different parameter lists. Users can select the corresponding overload as needed.

The vector data register compressed store scenario provides the following three modes:

- **Contiguous aligned store mode**: Stores the compressed data to the UB start address, and the user updates the destination address manually.
- **Immediate offset store mode**: An `int32_t offset` specifies the offset relative to the destination start address, in **elements**. The user can choose to update the offset or the destination address.
- **Address register offset compressed store mode**: An address register `addr_reg offset` specifies the offset relative to the destination start address. This mode is commonly used in compressed store scenarios where the offset changes with the loop count in a hardware loop. It must be used together with [asc_update_addr_reg](../reg_addr_reg/asc_update_addr_reg.md).

The mask register compressed store scenario provides the following three modes:

- **Contiguous aligned store mode**: Stores the compressed data to the UB start address, and the user updates the destination address manually.
- **Immediate offset store mode**: Uses `int32_t offset` to specify the offset relative to the destination start address, in **bytes**. You can choose to update the offset or update the destination address.
- **Address register offset store mode**: Uses the address register `addr_reg offset` to specify the offset relative to the destination start address. It is commonly used in compressed store scenarios where the offset changes with the loop count inside a Hardware Loop. It must be used together with [asc_update_addr_reg](../reg_addr_reg/asc_update_addr_reg.md).

The following figure illustrates an example of **compressing data from b32 to b16 and storing it**:

**Figure 1** Compressed store data

![](../../../../figures/reg_store_pack.png)

API for compressed store from a vector data register to UB. Based on `mask`, the lower half bits of the valid elements in `src` are stored contiguously in `dst`. Supported data types are `b16`, `b32`, and `b64`.

This API is a Reg vector data movement API and takes effect only on AIV.

## Prototype

### Vector Data Register Contiguous Aligned Store Mode

```c
// Placeholder form.
__simd_callee__ inline void asc_storealign_pack(__ubuf__ <dtype>* dst,
                                                vector_<dtype> src,
                                                vector_bool mask)
```

#### Data Types Supported by dtype

`dtype` can be `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, `int64_t`, or `uint64_t`. The corresponding vector register type is `vector_<dtype>`.

#### Typical Prototype Example

```c
__simd_callee__ inline void asc_storealign_pack(__ubuf__ int16_t* dst,
                                                vector_int16_t src,
                                                vector_bool mask)
```

### Vector Data Register Immediate Offset Store Mode

```c
__simd_callee__ inline void asc_storealign_pack(__ubuf__ <dtype>* dst,
                                                vector_<dtype> src,
                                                int32_t offset,
                                                vector_bool mask)
```

#### Data Types Supported by dtype

`dtype` can be `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, `int64_t`, or `uint64_t`. The corresponding vector register type is `vector_<dtype>`.

#### Typical Prototype Example

```c
__simd_callee__ inline void asc_storealign_pack(__ubuf__ int16_t* dst,
                                                vector_int16_t src,
                                                int32_t offset,
                                                vector_bool mask)
```

### Compressed Store from a Vector Data Register with an Address Register Offset

```c
__simd_callee__ inline void asc_storealign_pack(__ubuf__ <dtype>* dst,
                                                vector_<dtype> src,
                                                addr_reg offset,
                                                vector_bool mask)
```

#### Data Types Supported by dtype

`dtype` can be `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, `int64_t`, or `uint64_t`. The corresponding vector register type is `vector_<dtype>`.

#### Typical Prototype Example

```c
__simd_callee__ inline void asc_storealign_pack(__ubuf__ int16_t* dst,
                                                vector_int16_t src,
                                                addr_reg offset,
                                                vector_bool mask)
```

### Mask Register Contiguous Aligned Store Mode

```c
__simd_callee__ inline void asc_storealign_pack(__ubuf__ <dtype>* dst,
                                                vector_bool src)
```

#### Data Types Supported by dtype

`dtype` can be `uint8_t`, `uint16_t`, or `uint32_t`.

#### Typical Prototype Example

```c
__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint8_t* dst,
                                                vector_bool src)
```

### Compressed Store from a Mask Register with an Immediate Offset

```c
__simd_callee__ inline void asc_storealign_pack(__ubuf__ <dtype>* dst,
                                                vector_bool src,
                                                int32_t offset)
```

#### Data Types Supported by dtype

`dtype` can be `uint8_t`, `uint16_t`, or `uint32_t`.

#### Typical Prototype Example

```c
__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint8_t* dst,
                                                vector_bool src,
                                                int32_t offset)
```

### Compressed Store from a Mask Register with an Address Register Offset

```c
__simd_callee__ inline void asc_storealign_pack(__ubuf__ <dtype>* dst,
                                                vector_bool src,
                                                addr_reg offset)
```

#### Data Types Supported by dtype

`dtype` can be `uint8_t`, `uint16_t`, or `uint32_t`.

#### Typical Prototype Example

```c
__simd_callee__ inline void asc_storealign_pack(__ubuf__ uint8_t* dst,
                                                vector_bool src,
                                                addr_reg offset)
```

## Parameters

### Vector Data Register Contiguous Aligned Store Mode

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Start address of the destination operand. The type is `__ubuf__ <dtype>*`, and the start address must be 32-byte aligned. |
| src | Input | Source operand (vector data register). `dtype` must be the same as `dst`. |
| mask | Input | Source operand mask (mask register), used to indicate the elements that participate in the data movement. A value of 1 at the corresponding position indicates participation, and 0 indicates non-participation. |

### Vector Data Register Immediate Offset Store Mode

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Start address of the destination operand. The type is `__ubuf__ <dtype>*`, and the start address must be 32-byte aligned. |
| src | Input | Source operand (vector data register). `dtype` must be the same as `dst`. |
| offset | Input | Offset relative to the start address of `dst`, of type `int32_t`, in elements. |
| mask | Input | Source operand mask (mask register), used to indicate the elements that participate in the data movement. A value of 1 at the corresponding position indicates participation, and 0 indicates non-participation. |

### Vector Data Register Address Register Offset Store Mode

**Table 3** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Start address of the destination operand. The type is `__ubuf__ <dtype>*`, and the start address must be 32-byte aligned. |
| src | Input | Source operand (vector data register). `dtype` must be the same as `dst`. |
| offset | Input | Address register offset, of type `addr_reg`, which must be constructed in advance through the corresponding address register construction API. |
| mask | Input | Source operand mask (mask register), used to indicate the elements that participate in the data movement. A value of 1 at the corresponding position indicates participation, and 0 indicates non-participation. |

### Mask Register Contiguous Aligned Store Mode

**Table 4** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Start address of the destination operand. The type is `__ubuf__ <dtype>*`, and the start address must be 16-byte aligned. |
| src | Input | Source operand (mask register). |

### Mask Register Immediate Offset Store Mode

**Table 5** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Start address of the destination operand. The type is `__ubuf__ <dtype>*`, and the start address must be 16-byte aligned. |
| src | Input | Source operand (mask register). |
| offset | Input | Offset relative to the `dst` start address. The type is `int32_t`, in bytes. |

### Mask Register Address Register Offset Store Mode

**Table 6** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Start address of the destination operand. The type is `__ubuf__ <dtype>*`, and the start address must be 16-byte aligned. |
| src | Input | Source operand (mask register). |
| offset | Input | Address register offset, of type `addr_reg`, which must be constructed in advance through the corresponding address register construction API. |

For details about vector data registers, mask registers, and address registers, see [Register Type Definition](../../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

### General Constraints

- For the general constraints on Reg vector computation C APIs, see [General Constraints](../overview.md#general-constraints).
- The source operand is a vector data register or mask register, and the destination operand is a UB address. Pointers outside the UB address space cannot be passed as the destination address.
- The total UB capacity is 256 KB. The capacity available to users varies with compilation options and programming scenarios (by default, 6 KB of SIMD VF stack and 2 KB of Ascend C reserved space are set aside, leaving 248 KB available; in SIMD+SIMT mixed programming, an additional 32 KB to 128 KB is partitioned as Data Cache, further reducing the available capacity). The UB range corresponding to the destination operand address after applying the offset must not exceed the actual available capacity; otherwise, an error is reported.
- If there is a data dependency between this instruction and other instructions, you must insert the synchronization instruction [asc_mem_bar](../reg_sync/asc_mem_bar.md) to ensure that the instructions execute serially and prevent abnormal data from occurring.

### Vector Data Register Store Scenario

- The start address of `dst` must be 32-byte aligned.
- Source operand elements whose `mask` bit is 1 participate in the store, and their lower half bits are written to the corresponding compressed destination positions. Elements whose bit is 0 do not participate in the store, and the corresponding compressed destination positions retain their original values.
- The unit of `int32_t offset` is elements. The actual access address after applying the `offset` parameter must fall within the UB address range and must still be 32-byte aligned; otherwise, an error is reported.
- `addr_reg offset` must be constructed in advance by the corresponding address register construction API. The actual access address after applying the offset must fall within the UB address range and must still be 32-byte aligned; otherwise, an error is reported.

### Mask Register Store Scenario

- The start address of `dst` must be 16-byte aligned.
- The unit of `int32_t offset` is bytes. The actual access address after applying the `offset` parameter must fall within the UB address range and must still be 16-byte aligned; otherwise, an error is reported.
- `addr_reg offset` must be constructed in advance by the corresponding address register construction API. The actual access address after applying the offset must fall within the UB address range and must still be 16-byte aligned; otherwise, an error is reported.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

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

constexpr uint32_t MASK_BYTES = 32;
constexpr uint32_t PACKED_BYTES = 16;
// Valid packed bytes in each partition. This length can be adjusted; 19 covers a tail shorter than a full block.
constexpr uint32_t TOTAL_LENGTH = 19;
constexpr uint32_t DATA_BYTES = TOTAL_LENGTH * sizeof(uint8_t);
// Mask packing always writes 16 bytes per operation. Round up to allocate full write blocks.
constexpr uint32_t REPEAT_TIME = AscendC::Std::ceil_div(TOTAL_LENGTH, PACKED_BYTES);
// Write 16 bytes each time and reserve a 16-byte sentinel region; all start addresses are 16-byte aligned.
constexpr uint32_t STEP_BYTES = 32;
// Reserve one extra stride so that the address after the final Post Update remains within this partition.
constexpr uint32_t SECTION_BYTES = (REPEAT_TIME + 1) * STEP_BYTES;
constexpr uint32_t OUTPUT_BYTES = 3 * SECTION_BYTES;
constexpr uint8_t SENTINEL = 0xa5;

constexpr uint8_t MUTEX_ID = 0;

// This mask register overload does not support a tail mask: the final iteration still writes a full 16 bytes, and the tail padding is checked within the reserved UB space.
__simd_vf__ inline void pack_vf(__ubuf__ uint8_t* dst, __ubuf__ uint32_t* src, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_bool src_reg;
    asc_loadalign(src_reg, src);
    for (uint16_t i = 0; i < repeat_time; ++i) {
        // After each full write, retain a PACKED_BYTES-byte sentinel region; the next write address remains 16-byte aligned.
        const uint32_t offset = i * (one_repeat_size + PACKED_BYTES);
        asc_storealign_pack(dst + offset, src_reg);
        asc_storealign_pack(reinterpret_cast<__ubuf__ uint16_t*>(
            dst + SECTION_BYTES + offset), src_reg);
        asc_storealign_pack(reinterpret_cast<__ubuf__ uint32_t*>(
            dst + 2 * SECTION_BYTES + offset), src_reg);
    }
}

__global__ __vector__ void asc_storealign_pack_kernel(__gm__ uint8_t* dst, __gm__ uint32_t* src)
{
    asc_init();
    __ubuf__ uint8_t dst_local[OUTPUT_BYTES];
    __ubuf__ uint32_t src_local[MASK_BYTES / sizeof(uint32_t)];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub(dst_local, dst, OUTPUT_BYTES);
    asc_copy_gm2ub(src_local, src, MASK_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    // The source mask register contains 32 bytes; packing every other bit produces 16 bytes per iteration.
    uint32_t one_repeat_size = PACKED_BYTES;
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<pack_vf>(dst_local, src_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm(dst, dst_local, OUTPUT_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint32_t> src(MASK_BYTES / sizeof(uint32_t), 0x55555555u);
    std::vector<uint8_t> output(OUTPUT_BYTES, SENTINEL);
    std::vector<uint8_t> golden(OUTPUT_BYTES, SENTINEL);
    // Always write a full PACKED_BYTES each time: verify both valid data and padding in the final iteration, while the sentinels between blocks and at the ends of partitions remain unchanged.
    for (uint32_t section = 0; section < 3; ++section) {
        for (uint32_t repeat = 0; repeat < REPEAT_TIME; ++repeat) {
            for (uint32_t i = 0; i < PACKED_BYTES; ++i) {
                golden[section * SECTION_BYTES + repeat * STEP_BYTES + i] = 0xff;
            }
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint32_t* mask_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&mask_device), MASK_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(mask_device, MASK_BYTES, src.data(), MASK_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_storealign_pack_kernel<<<1, 0>>>(output_device, mask_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_storealign_pack passed."
                         : "[Failed] asc_storealign_pack output mismatch.")
              << std::endl;
    aclrtFree(mask_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
