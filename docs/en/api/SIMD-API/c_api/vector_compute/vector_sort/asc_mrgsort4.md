---
okftype: reference.api
description: "Merges 2, 3, or 4 queues already sorted in descending order by score into one ordered queue. The input usually comes from asc_bitsort. Supports half and float, and the source and destination pointer types must be consistent. When repeat is greater than 1, the lengths of the four queues must be the same and advance by 4 × L × 8 bytes."
tag:
  - roles: [compute]
  - operations: [sort]
  - traits: [datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "The general address overlap constraints must be followed"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_sort/asc_bitsort.md
    kind: requires
    condition: "The input queues are generally ordered data processed by this API."
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_sort/asc_get_vms4_sr.md
    kind: related
    condition: "When if_exhausted_suspension is true, you can use this API to query the number of processed elements in each queue."
version: ">=9.0.0"
version_changes: []
---
# asc_mrgsort4

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:04.042764+00:00 pushedAt=2026-11-16T02:10:59.365Z nl=2 -->

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

This API merges 2, 3, or 4 queues that are already sorted by score in descending order into one ordered queue, maintaining the descending order by score.

This API does not sort unordered input queues. The input is generally the output data processed by the [asc_bitsort](asc_bitsort.md) instruction. For the detailed queue structure, see the description in [asc_bitsort](asc_bitsort.md#description).

## Prototype

```cpp
// Placeholder form.
__aicore__ inline void asc_mrgsort4(__ubuf__ <dtype>* dst,
                                    __ubuf__ <dtype>* src[ASC_C_API_MRGSORT_ELEMENT_LEN],
                                    uint8_t repeat,
                                    uint16_t element_length_0,
                                    uint16_t element_length_1,
                                    uint16_t element_length_2,
                                    uint16_t element_length_3,
                                    bool if_exhausted_suspension,
                                    uint8_t valid_bit)
```

### Data Types Supported by dtype

`dtype` values are `half` and `float`. The pointer types of the source operands and the destination operand must be the same.

### Typical Prototype Example

```cpp
__aicore__ inline void asc_mrgsort4(__ubuf__ float* dst,
                                    __ubuf__ float* src[ASC_C_API_MRGSORT_ELEMENT_LEN],
                                    uint8_t repeat,
                                    uint16_t element_length_0,
                                    uint16_t element_length_1,
                                    uint16_t element_length_2,
                                    uint16_t element_length_3,
                                    bool if_exhausted_suspension,
                                    uint8_t valid_bit)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Start address of the destination queue in UB, which must be 32-byte aligned. |
| src | Input | Pointer array containing the start addresses of 4 source queues. `ASC_C_API_MRGSORT_ELEMENT_LEN` is `4`. The start address of each queue must be 8-byte aligned. Array subscripts 0 to 3 correspond to the 4 queues. All 4 array items must be initialized. Invalid queues can reuse a valid UB address. |
| repeat | Input | Number of iterations. When performing merging, the value range is [1, 255]. If the value is greater than 1, the multi-iteration constraints in [Constraints](#constraints) must be met. Multiple iterations produce multiple separately ordered result queues and do not automatically merge them into one. |
| element_length_0 | Input | Length of the queue whose start address is `src[0]`, that is, the number of data units in the queue. Each data unit occupies 8 bytes, so the unit is 8 bytes.<br>For a valid queue, the value range is [1, 4095]. For an invalid queue, the value is 0. |
| element_length_1 | Input | Length of the queue whose start address is `src[1]`. The unit and range are the same as those of `element_length_0`. |
| element_length_2 | Input | Length of the queue whose start address is `src[2]`. The unit and range are the same as those of `element_length_0`. |
| element_length_3 | Input | Length of the queue whose start address is `src[3]`. The unit and range are the same as those of `element_length_0`. |
| if_exhausted_suspension | Input | Controls whether the instruction stops executing when a queue is exhausted. The type is bool. The values are described as follows:<br>&nbsp;&nbsp;&bull; `false`: During merge sorting, after a valid queue is exhausted, the remaining queues continue to be processed, and the values of all valid queues are output. The output length of a single iteration is the sum of the lengths of the valid queues.<br>&nbsp;&nbsp;&bull; `true`: During merge sorting, sorting stops when any valid queue is exhausted. Only the ordered prefix that has been sorted is output. The sum of the lengths of the valid queues cannot be used as the actual output length of a single iteration. |
| valid_bit | Input | Valid queue bitmask. Valid values are `3`, `7`, and `15`, with the following meanings:<br>&nbsp;&nbsp;&bull; `3` (`0b0011`): The valid queues are those whose start addresses are `src[0]` and `src[1]`.<br>&nbsp;&nbsp;&bull; `7` (`0b0111`): The valid queues are those whose start addresses are `src[0]`, `src[1]`, and `src[2]`.<br>&nbsp;&nbsp;&bull; `15` (`0b1111`): All 4 queues are valid. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- Overlapping between the source and destination operand addresses is not supported.
- The start address of dst must be 32-byte aligned, and the start addresses in the src array must be 8-byte aligned.
- If score\[i\]==score\[j\] and i\>j, score\[j\] is preferentially placed first.
- Data within each iteration is sorted, while data across different iterations is not sorted.
- All 4 elements of the src array must be initialized, even if the corresponding queue is not selected by `valid_bit`.
- When `repeat = 1`, valid source queues can have different lengths and non-contiguous storage. When `repeat > 1`, the following conditions must be met simultaneously:
    - The 4 source queues have the same length, which is `L`.
    - Each group of 4 queues is stored contiguously in the order from `src[0]` to `src[3]`, and the groups are also stored contiguously.
    - `if_exhausted_suspension = false`.
    - `valid_bit = 15`.

  For the `r`-th iteration (starting from 0), the source and destination addresses are offset by `r * 4 * L * 8` bytes from the initial addresses, and each iteration produces an ordered queue containing `4 * L` data units.
<!-- npu="A3,910b,950" id12 -->
- When the parameter `repeat = 0`, the behavior of this API is as follows:
    <!-- npu="A3,910b" id10 -->
    - For the following models, no computation is performed and no write is made to the destination operand. The API is treated as a NOP (no operation).
       <!-- npu="A3" id13 -->
       - Atlas A3 products
       <!-- end id13 -->
       <!-- npu="910b" id9 -->
       - Atlas A2 products
       <!-- end id9 -->
    <!-- end id10 -->
    <!-- npu="950" id11 -->
    - For Ascend 950PR&950DT products, it is not guaranteed that this API is treated as a NOP (no operation).
    <!-- end id11 -->
<!-- end id12 -->

<!-- npu="950" id8 -->

## Examples

The following example takes 256 unordered float scores and their indexes as inputs, and uses [asc_bitsort](asc_bitsort.md) together with `asc_mrgsort4` to complete global descending sorting. Each output data unit contains a score and its corresponding index, occupying 8 bytes. The overall computation process is as follows:

1. Call `asc_bitsort` with `repeat = 8` to sort each group of 32 elements separately, generating 8 ordered queues. At this point, each queue is internally ordered, but all data is not yet globally ordered.
2. Call `asc_mrgsort4` with `repeat = 2` to merge two groups of 4 queues each, generating two ordered queues of 128 data units each.
3. Call `asc_mrgsort4` again with `repeat = 1` and `valid_bit = 0b0011` to merge the two queues, obtaining a globally ordered result containing 256 data units.

On the host side, use the CPU sorting result to verify all 256 scores and their corresponding indexes one by one.

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

```cpp
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "c_api/asc_simd.h"
#include "acl/acl.h"

#define ACL_CHECK(call) do { \
    aclError error = (call); \
    if (error != ACL_SUCCESS) { \
        std::cerr << #call << " failed: " << error << std::endl; \
        std::exit(1); \
    } \
} while (0)

constexpr uint16_t QUEUE_LENGTH = 32;
constexpr uint32_t QUEUE_COUNT = 8;
constexpr uint32_t RECORD_COUNT = QUEUE_LENGTH * QUEUE_COUNT;
// The index stores a uint32_t bit pattern; the index value is not converted to float.
struct Record {
    float score;
    uint32_t index;
};
static_assert(sizeof(Record) == 8, "Each record must occupy 8 bytes.");
constexpr uint32_t WORD_COUNT = RECORD_COUNT * 2;
constexpr uint32_t BYTE_COUNT = RECORD_COUNT * sizeof(Record);
constexpr uint32_t SCORE_BYTES = RECORD_COUNT * sizeof(float);
constexpr uint32_t INDEX_BYTES = RECORD_COUNT * sizeof(uint32_t);

__global__ __vector__ void mrgsort_kernel(
    __gm__ float* dst, __gm__ float* scores, __gm__ uint32_t* indices)
{
    asc_init();
    __ubuf__ float score_local[RECORD_COUNT];
    __ubuf__ uint32_t index_local[RECORD_COUNT];
    __ubuf__ float sorted_local[WORD_COUNT];
    __ubuf__ float tmp_local[WORD_COUNT];
    __ubuf__ float dst_local[WORD_COUNT];

    asc_copy_gm2ub_align(score_local, scores, SCORE_BYTES);
    asc_copy_gm2ub_align(index_local, indices, INDEX_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

    // Step 1: Sort 32 scores per iteration; 8 iterations generate 8 independent ordered queues.
    // The output interleaves (score, index); each data unit occupies 8 bytes, that is, 2 float positions.
    asc_bitsort(sorted_local, score_local, index_local, QUEUE_COUNT);
    asc_sync();

    // Step 2: Merge 4 queues per group; repeat=2 yields two queues of 128 data units.
    __ubuf__ float* queues[ASC_C_API_MRGSORT_ELEMENT_LEN] = {
        sorted_local, sorted_local + 2 * QUEUE_LENGTH,
        sorted_local + 4 * QUEUE_LENGTH, sorted_local + 6 * QUEUE_LENGTH
    };
    asc_mrgsort4(tmp_local, queues, 2, QUEUE_LENGTH, QUEUE_LENGTH,
        QUEUE_LENGTH, QUEUE_LENGTH, false, 0b1111);
    // The next merge reads the current result, so complete synchronization first.
    asc_sync();

    // Step 3: Merge the two queues of 128 data units into a globally ordered result.
    queues[0] = tmp_local;
    queues[1] = tmp_local + 2 * (4 * QUEUE_LENGTH);
    // Invalid queues are also initialized to valid UB addresses with length set to 0.
    queues[2] = tmp_local;
    queues[3] = tmp_local;
    asc_mrgsort4(dst_local, queues, 1, 4 * QUEUE_LENGTH,
        4 * QUEUE_LENGTH, 0, 0, false, 0b0011);

    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(dst, dst_local, BYTE_COUNT);
    asc_sync();
}

int main()
{
    std::vector<float> scores(RECORD_COUNT);
    std::vector<uint32_t> indices(RECORD_COUNT);
    std::vector<Record> golden(RECORD_COUNT), output(RECORD_COUNT);
    for (uint32_t i = 0; i < RECORD_COUNT; ++i) {
        // 73 is coprime with 256, generating non-repeating shuffled scores from 1 to 256; each group of 32 elements is also unsorted.
        scores[i] = static_cast<float>((i * 73 + 19) % RECORD_COUNT + 1);
        indices[i] = i;
        golden[i] = {scores[i], indices[i]};
    }
    std::stable_sort(golden.begin(), golden.end(),
        [](const Record& a, const Record& b) { return a.score > b.score; });

    ACL_CHECK(aclInit(nullptr));
    ACL_CHECK(aclrtSetDevice(0));
    float* score_device = nullptr;
    uint32_t* index_device = nullptr;
    float* dst_device = nullptr;
    ACL_CHECK(aclrtMalloc(reinterpret_cast<void**>(&score_device), SCORE_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST));
    ACL_CHECK(aclrtMalloc(reinterpret_cast<void**>(&index_device), INDEX_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST));
    ACL_CHECK(aclrtMalloc(reinterpret_cast<void**>(&dst_device), BYTE_COUNT,
        ACL_MEM_MALLOC_HUGE_FIRST));
    ACL_CHECK(aclrtMemcpy(score_device, SCORE_BYTES, scores.data(), SCORE_BYTES,
        ACL_MEMCPY_HOST_TO_DEVICE));
    ACL_CHECK(aclrtMemcpy(index_device, INDEX_BYTES, indices.data(), INDEX_BYTES,
        ACL_MEMCPY_HOST_TO_DEVICE));
    mrgsort_kernel<<<1, 0>>>(dst_device, score_device, index_device);
    ACL_CHECK(aclrtSynchronizeDevice());
    ACL_CHECK(aclrtMemcpy(output.data(), BYTE_COUNT, dst_device, BYTE_COUNT,
        ACL_MEMCPY_DEVICE_TO_HOST));

    bool passed = true;
    for (uint32_t i = 0; i < RECORD_COUNT; ++i) {
        if (output[i].score != golden[i].score || output[i].index != golden[i].index) {
            std::cerr << "Mismatch at record " << i << std::endl;
            passed = false;
            break;
        }
    }
    std::cout << "First 8 (score, index):";
    for (uint32_t i = 0; i < 8; ++i) {
        std::cout << " (" << output[i].score << ", " << output[i].index << ")";
    }
    std::cout << '\n' << (passed ? "[Success] asc_mrgsort4 passed."
                                : "[Failed] asc_mrgsort4 failed.") << std::endl;
    ACL_CHECK(aclrtFree(dst_device));
    ACL_CHECK(aclrtFree(index_device));
    ACL_CHECK(aclrtFree(score_device));
    ACL_CHECK(aclrtResetDevice(0));
    ACL_CHECK(aclFinalize());
    return passed ? 0 : 1;
}
```
<!-- end id8 -->
