---
okftype: reference.api
description: "Configuration API that associates the 16 DataBlock address sequences required for transpose to address registers. Each address register is associated with 8 addresses, supports address arrays of 8 data types from int8_t to float, and has pipeline type PIPE_V."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "For operand address alignment and address overlap constraints, see the storage unit description and general address overlap constraints sections on this page"
  - path: docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md
    kind: related
    condition: "Mapping between product models and NPU architectures"
version: ">=9.0.0"
version_changes: []
---
# asc_set_va_reg

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.939906+00:00 pushedAt=2026-11-16T02:10:59.106Z nl=2 -->

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

The header file path is `"c_api/vector_compute/compute/vector_permute_sel.h"`.

Sets the 16 [DataBlock](../../general_description_and_constraints.md) addresses for the transpose operation and associates the operand address sequences with address registers. The API requires the first 8 and the last 8 address sequences to be associated with address registers separately.

## Prototype

```cpp
// Placeholder form.
__aicore__ inline void asc_set_va_reg(ub_addr8_t addr,
                                      __ubuf__ <dtype>** src_array)
```

### Data Types Supported by dtype

`dtype` values: `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `half`, `int32_t`, `uint32_t`, `float`.

### Typical Prototype Example

```c
__aicore__ inline void asc_set_va_reg(ub_addr8_t addr,
                                      __ubuf__ int8_t** src_array)
```

## Parameters

**Table 1** Parameters

|  Parameter  |  Input/Output  |  Description  |
| ------------ | ------------ | ------------ |
| addr | Input | Address register, of type ub_addr8_t. Possible values are:<br>&bull; VA0<br>&bull; VA1<br>&bull; VA2<br>&bull; VA3<br>&bull; VA4<br>&bull; VA5<br>&bull; VA6<br>&bull; VA7<br>The number indicates the register order. Each address register can be associated with only 8 addresses. For usage, see the example.|
| src_array | Input | Operand address sequence. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address alignment constraints, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).

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
#include <cstdlib>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t MATRIX_SIDE = 16;
constexpr uint32_t MATRIX_ELEMENTS = MATRIX_SIDE * MATRIX_SIDE;
constexpr uint32_t TOTAL_ELEMENTS = 2 * 32 * 16 * 16;
constexpr uint32_t OFFSET = 16 * 16 * 16;

__global__ __vector__ void transto5hd_kernel(__gm__ uint16_t* matrix_out,
                                             __gm__ uint16_t* matrix_in,
                                             __gm__ uint16_t* layout_out,
                                             __gm__ uint16_t* layout_in)
{
    asc_init();
    __ubuf__ uint16_t src[TOTAL_ELEMENTS];
    __ubuf__ uint16_t dst[TOTAL_ELEMENTS];
    __ubuf__ half* src_list[16];
    __ubuf__ half* dst_list[16];

    // Scenario 1: Complete a 16x16 matrix transposition in one repeat. When stride is 0, read from the start of the address list.
    asc_copy_gm2ub_align(src, matrix_in, MATRIX_ELEMENTS * sizeof(uint16_t));
    // Wait for the matrix data load to complete before transposition.
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    for (int i = 0; i < 16; ++i) {
        src_list[i] = reinterpret_cast<__ubuf__ half*>(src + i * 16);
        dst_list[i] = reinterpret_cast<__ubuf__ half*>(dst + i * 16);
    }
    asc_sync_notify(PIPE_S, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_S, PIPE_V, EVENT_ID0);
    asc_set_va_reg(VA0, dst_list);
    asc_set_va_reg(VA1, dst_list + 8);
    asc_set_va_reg(VA2, src_list);
    asc_set_va_reg(VA3, src_list + 8);
    asc_transto5hd_b16(VA0, VA2, 1, 0, 0);
    // Wait for transposition to complete before storing the matrix.
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(matrix_out, dst, MATRIX_ELEMENTS * sizeof(uint16_t));

    // Scenario 2: N=2, C=32, H=W=16. Four groups of C0=16 channels are processed separately.
    // Wait for the preceding transposition to complete before reusing src and the address list.
    asc_sync_notify(PIPE_V, PIPE_S, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_S, EVENT_ID0);
    asc_copy_gm2ub_align(src, layout_in, TOTAL_ELEMENTS * sizeof(uint16_t));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    // Before writing dst again, wait for the preceding matrix store to finish reading dst.
    asc_sync_notify(PIPE_MTE3, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE3, PIPE_V, EVENT_ID0);
    for (int j = 0; j < 4; ++j) {
        for (int i = 0; i < 16; ++i) {
            src_list[i] = reinterpret_cast<__ubuf__ half*>(src + OFFSET * j + i * 16 * 16);
            dst_list[i] = reinterpret_cast<__ubuf__ half*>(dst + OFFSET * j + i * 16);
        }
        asc_sync_notify(PIPE_S, PIPE_V, EVENT_ID0);
        asc_sync_wait(PIPE_S, PIPE_V, EVENT_ID0);
        asc_set_va_reg(VA0, dst_list);
        asc_set_va_reg(VA1, dst_list + 8);
        asc_set_va_reg(VA2, src_list);
        asc_set_va_reg(VA3, src_list + 8);
        // 16 repeats advance along the HW direction, with src_stride=1 and dst_stride=16, both in DataBlocks.
        asc_transto5hd_b16(VA0, VA2, 16, 16, 1);
        if (j < 3) {
            // The next group modifies the address list. Wait until PIPE_V has consumed the addresses of this group.
            asc_sync_notify(PIPE_V, PIPE_S, EVENT_ID0);
            asc_sync_wait(PIPE_V, PIPE_S, EVENT_ID0);
        }
    }
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(layout_out, dst, TOTAL_ELEMENTS * sizeof(uint16_t));
    asc_sync();
}

void check(aclError status, const char* operation)
{
    if (status != ACL_SUCCESS) {
        std::cerr << operation << " failed: " << status << std::endl;
        std::exit(1);
    }
}

} // namespace

int main()
{
    std::vector<uint16_t> matrix_in(MATRIX_ELEMENTS), matrix_out(MATRIX_ELEMENTS);
    std::vector<uint16_t> layout_in(TOTAL_ELEMENTS), layout_out(TOTAL_ELEMENTS);
    for (uint32_t i = 0; i < MATRIX_ELEMENTS; ++i) matrix_in[i] = i;
    for (uint32_t i = 0; i < TOTAL_ELEMENTS; ++i) layout_in[i] = i;

    check(aclInit(nullptr), "aclInit");
    check(aclrtSetDevice(0), "aclrtSetDevice");
    uint16_t *matrix_in_device = nullptr, *matrix_out_device = nullptr;
    uint16_t *layout_in_device = nullptr, *layout_out_device = nullptr;
    check(aclrtMalloc(reinterpret_cast<void**>(&matrix_in_device), MATRIX_ELEMENTS * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST), "aclrtMalloc matrix_in");
    check(aclrtMalloc(reinterpret_cast<void**>(&matrix_out_device), MATRIX_ELEMENTS * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST), "aclrtMalloc matrix_out");
    check(aclrtMalloc(reinterpret_cast<void**>(&layout_in_device), TOTAL_ELEMENTS * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST), "aclrtMalloc layout_in");
    check(aclrtMalloc(reinterpret_cast<void**>(&layout_out_device), TOTAL_ELEMENTS * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST), "aclrtMalloc layout_out");
    check(aclrtMemcpy(matrix_in_device, MATRIX_ELEMENTS * sizeof(uint16_t), matrix_in.data(), MATRIX_ELEMENTS * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE), "copy matrix_in");
    check(aclrtMemcpy(layout_in_device, TOTAL_ELEMENTS * sizeof(uint16_t), layout_in.data(), TOTAL_ELEMENTS * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE), "copy layout_in");
    transto5hd_kernel<<<1, 0>>>(matrix_out_device, matrix_in_device, layout_out_device, layout_in_device);
    check(aclrtSynchronizeDevice(), "aclrtSynchronizeDevice");
    check(aclrtMemcpy(matrix_out.data(), MATRIX_ELEMENTS * sizeof(uint16_t), matrix_out_device, MATRIX_ELEMENTS * sizeof(uint16_t), ACL_MEMCPY_DEVICE_TO_HOST), "copy matrix_out");
    check(aclrtMemcpy(layout_out.data(), TOTAL_ELEMENTS * sizeof(uint16_t), layout_out_device, TOTAL_ELEMENTS * sizeof(uint16_t), ACL_MEMCPY_DEVICE_TO_HOST), "copy layout_out");

    bool matrix_passed = true;
    for (uint32_t row = 0; row < 16; ++row) {
        for (uint32_t col = 0; col < 16; ++col) {
            matrix_passed &= matrix_out[col * 16 + row] == matrix_in[row * 16 + col];
        }
    }
    bool layout_passed = true;
    for (uint32_t n = 0; n < 2; ++n) {
        for (uint32_t c = 0; c < 32; ++c) {
            for (uint32_t hw = 0; hw < 256; ++hw) {
                uint32_t src_index = (n * 32 + c) * 256 + hw;
                uint32_t dst_index = ((n * 2 + c / 16) * 256 + hw) * 16 + c % 16;
                layout_passed &= layout_out[dst_index] == layout_in[src_index];
            }
        }
    }
    std::cout << "16x16 transpose: " << (matrix_passed ? "PASS" : "FAIL") << std::endl;
    std::cout << "NCHW to NC1HWC0: " << (layout_passed ? "PASS" : "FAIL") << std::endl;
    check(aclrtFree(matrix_in_device), "free matrix_in");
    check(aclrtFree(matrix_out_device), "free matrix_out");
    check(aclrtFree(layout_in_device), "free layout_in");
    check(aclrtFree(layout_out_device), "free layout_out");
    check(aclrtResetDevice(0), "aclrtResetDevice");
    check(aclFinalize(), "aclFinalize");
    return matrix_passed && layout_passed ? 0 : 1;
}
```
