---
okftype: reference.api
description: "Sorts 32 elements in descending order by score within one iteration. score is float or half, and index is fixed as uint32_t. The sorted result is written to dst as an 8-byte (score, index) structure. The repeat parameter controls multiple iterations that are not sorted with each other. Each iteration skips 32 elements and 256 bytes."
tag:
  - roles: [compute]
  - operations: [sort]
  - traits: [datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "The general address overlap constraints must be followed"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_sort/asc_mrgsort4.md
    kind: related
    condition: "The sorted result is usually used as the input queue of this API."
version: ">=9.0.0"
version_changes: []
---
# asc_bitsort

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:04.035716+00:00 pushedAt=2026-11-16T02:10:59.341Z nl=2 -->

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

The header file path is `"c_api/vector_compute/compute/vector_sort.h"`.

This API implements descending sorting of 32 numbers within one iteration. It supports sorting multiple iterations through the `repeat` parameter. Each iteration processes 32 elements; in the next iteration, `src0` and `src1` each skip 32 elements, and `dst` skips 256 bytes. No sorting is performed between different iterations.

As shown in the following figure, src0 and src1 represent the data score to be sorted and the index of the corresponding RP, respectively. After sorting is complete, the result is stored in dst in the (score, index) structure.

src1 is fixed as the uint32\_t type. Regardless of whether src0 is of the float or half type, the (score, index) structure in dst always occupies 8B of space. Specifically:

- When src0 is of the float type, the index in dst is stored in the upper 4B, and the score is stored in the lower 4B.
- When src0 is of the half type, the index in dst is stored in the upper 4B, the score is stored in the lower 2B, and the middle 2B are reserved.

**Figure 1**  asc_bitsort sorting diagram

![](../../../../figures/sort32.png)<a id="figure-1-asc_bitsort-sorting-diagram"></a>

## Prototype

```cpp
// Placeholder form.
__aicore__ inline void asc_bitsort(__ubuf__ <dtype>* dst,
                                   __ubuf__ <dtype>* src0,
                                   __ubuf__ uint32_t* src1,
                                   int32_t repeat)
```

### Data Types Supported by dtype

`dtype` values: `half`, `float`.

### Typical Prototype Example

```cpp
__aicore__ inline void asc_bitsort(__ubuf__ float* dst,
                                   __ubuf__ float* src0,
                                   __ubuf__ uint32_t* src1,
                                   int32_t repeat)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Start address of the destination operand in UB. |
| src0 | Input | Start address of score in UB. |
| src1 | Input | Start address of index in UB. |
| repeat | Input | Number of iterations, with a value range of [0, 255]. Each iteration processes 32 elements. In the next iteration, `src0` and `src1` each skip 32 elements, and `dst` skips 256 bytes. No sorting is performed between different iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of `dst`, `src0`, and `src1` must be 32-byte aligned.

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
#include <cstring>
#include <iostream>
#include <vector>

#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
template <typename T>
void print_data(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

constexpr uint32_t ELEMENT_COUNT = 32;
constexpr uint32_t INPUT_BYTES = ELEMENT_COUNT * sizeof(float);
constexpr uint32_t OUTPUT_BYTES = ELEMENT_COUNT * 8;

__global__ __vector__ void bitsort_kernel(__gm__ float* dst, __gm__ float* src0, __gm__ uint32_t* src1)
{
    asc_init();
    __ubuf__ float scores[ELEMENT_COUNT];
    __ubuf__ uint32_t indices[ELEMENT_COUNT];
    __ubuf__ float sorted[ELEMENT_COUNT * 2];

    asc_copy_gm2ub_align(scores, src0, INPUT_BYTES);
    asc_copy_gm2ub_align(indices, src1, ELEMENT_COUNT * sizeof(uint32_t));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_bitsort(sorted, scores, indices, 1);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(dst, sorted, OUTPUT_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> scores(ELEMENT_COUNT), output_scores(ELEMENT_COUNT), golden_scores(ELEMENT_COUNT);
    std::vector<uint32_t> indices(ELEMENT_COUNT), output_indices(ELEMENT_COUNT), golden_indices(ELEMENT_COUNT);
    std::vector<float> result(ELEMENT_COUNT * 2, 0);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        scores[i] = static_cast<float>(i + 1);
        indices[i] = i;
        golden_scores[i] = static_cast<float>(ELEMENT_COUNT - i);
        golden_indices[i] = ELEMENT_COUNT - 1 - i;
    }

    if (aclInit(nullptr) != ACL_SUCCESS || aclrtSetDevice(0) != ACL_SUCCESS) {
        std::cerr << "Device initialization failed\n";
        return 1;
    }
    float* scores_device = nullptr;
    uint32_t* indices_device = nullptr;
    float* result_device = nullptr;
    if (aclrtMalloc(reinterpret_cast<void**>(&scores_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&indices_device), ELEMENT_COUNT * sizeof(uint32_t), ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&result_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS) {
        std::cerr << "Device allocation failed\n";
        return 1;
    }

    bool ok = aclrtMemcpy(scores_device, INPUT_BYTES, scores.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE) == ACL_SUCCESS &&
              aclrtMemcpy(indices_device, ELEMENT_COUNT * sizeof(uint32_t), indices.data(),
                          ELEMENT_COUNT * sizeof(uint32_t), ACL_MEMCPY_HOST_TO_DEVICE) == ACL_SUCCESS;
    if (ok) {
        bitsort_kernel<<<1, 0>>>(result_device, scores_device, indices_device);
        ok = aclrtSynchronizeDevice() == ACL_SUCCESS &&
             aclrtMemcpy(result.data(), OUTPUT_BYTES, result_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST) == ACL_SUCCESS;
    }
    if (ok) {
        for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
            output_scores[i] = result[2 * i];
            std::memcpy(&output_indices[i], &result[2 * i + 1], sizeof(uint32_t));
        }
    }
    print_data("Input scores", scores);
    print_data("Input indices", indices);
    print_data("Output scores", output_scores);
    print_data("Output indices", output_indices);
    print_data("Golden scores", golden_scores);
    print_data("Golden indices", golden_indices);
    const bool passed = ok && output_scores == golden_scores && output_indices == golden_indices;
    std::cout << (passed ? "[Success] asc_bitsort passed." : "[Failed] asc_bitsort failed.") << std::endl;

    aclrtFree(scores_device);
    aclrtFree(indices_device);
    aclrtFree(result_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
