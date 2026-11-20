---
okftype: reference.api
description: "Broadcast a scalar fill value to multiple 32-byte fractal blocks starting at dst in the L1 Buffer, configuring the repeat count, block count per operation, and block gap according to asc_fill_value_config, for memory initialization of the L1 Buffer."
tag:
  - roles: [data_initialization]
  - operations: [fill]
  - traits: [datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/overview/cube_compute_flow.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/L1_L0A_B_memory_structure_intro.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/defs/union/asc_fill_value_config.md
    kind: requires
    condition: "All overloads require passing config to specify repeat, blk_num, and dst_gap"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "L1 Buffer 512KB capacity, 32-byte start alignment, and general address overlap constraints"
  - path: docs/zh/api/SIMD-API/c_api/sync/system_sync_overview.md
    kind: requires
    condition: "When this instruction has data dependencies with other instructions, synchronization instructions must be inserted to serialize them"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: arch_support_extended
    scope: "page"
    scope_basis: kind_default
    note: "The 9.0.0 implementation only has the npu_arch_2201 version. Starting from 9.1.0, asc_fill_l1 appears in impl/c_api/instr_impl/npu_arch_3510/cube_datamove_impl.h, which is consistent with Release Notes 9.1.0 listing asc_fill_l1 as a newly added data movement interface for the 3510 architecture. In the page product support table, support for Ascend 950PR&950DT starts from 9.1.0."
  - since: "9.1.0"
    kind: overload_added
    scope: "overload:asc_fill_l1__PL1bf16_f16_RCfillcfg"
    scope_basis: resolved
    note: "Starting from 9.1.0, include and impl add an overload with bfloat16_t* dst paired with a half fill value; 9.0.0 only has bfloat16_t paired with bfloat16_t. In the current include, this overload has @override_id asc_fill_l1__PL1bf16_f16_RCfillcfg and @archs 2201 3510."
  - since: "9.1.0"
    kind: overload_added
    scope: "overload:asc_fill_l1__PL1bf16_u32_RCfillcfg"
    scope_basis: resolved
    note: "Since 9.1.0, include and impl add an overload with bfloat16_t* dst paired with a uint32_t fill value; 9.0.0 only has bfloat16_t paired with bfloat16_t. Currently, the @override_id of this overload in include is asc_fill_l1__PL1bf16_u32_RCfillcfg, and @archs are 2201 3510."
---
# asc_fill_l1

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:26:19.044Z pushedAt=2026-11-18T08:15:34.318Z nl=2 -->

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

The header file path is: `"c_api/cube_datamove/cube_datamove.h"`.

Writes a scalar fill value to the destination address of the L1 Buffer and broadcasts the fill value to multiple 32-byte fractal blocks, which can be used for memory initialization of the L1 Buffer. The repetition count, the number of fractal blocks per repetition, and the gap between adjacent repetitions need to be configured through [asc_fill_value_config](../../defs/union/asc_fill_value_config.md).

This interface is valid only when executed on the AIC.

The fill operation is used to prepare the cube input buffer. For the connection between subsequent data loading and cube multiply-accumulate, see [Cube Computation Flow](../../cube_compute/overview.md#cube-computation-flow); for the input buffer layout, see [Introduction to L1 Buffer, L0A Buffer/L0B Buffer Memory Structure](../cube_compute_load/L1_L0A_B_memory_structure_intro.md).

## Prototype

```cpp
__aicore__ inline void asc_fill_l1(__cbuf__ <dst_dtype>* dst,
                                   <value_dtype> value,
                                   const asc_fill_value_config& config)
```

### Data Types Supported by dtype

dst dtype and value dtype support the following combinations:

- When `dst_dtype` is `int16_t`, `uint16_t`, `half`, `int32_t`, `uint32_t`, or `float`, `value_dtype` supports `half` and `uint32_t`.
- When `dst_dtype` is `bfloat16_t`, `value_dtype` supports `half`, `bfloat16_t`, and `uint32_t`.

### Prototype Example

```cpp
// Example: Fill an L1 Buffer of type half with a value of type half.
__aicore__ inline void asc_fill_l1(__cbuf__ half* dst,
                                   half value,
                                   const asc_fill_value_config& config)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | meaning |
|---|---|---|
| dst | Output | Destination operand, the start address of the destination cube, stored in the L1 Buffer. The start address must be 32-byte aligned. |
| value | Input | Source operand, the scalar fill value.|
| config | Input | The structure [asc_fill_value_config](../../defs/union/asc_fill_value_config.md) for fill configuration. See [Table 2](#fill_value_config_fields) for the structure description. |

**Table 2** Description of config fields <a id="fill_value_config_fields"></a>

| Field name | Meaning |
|---|---|
| repeat | Number of repetitions, that is, the total number of times the fill value is written to the L1 Buffer in `blk_num` 32-byte blocks and then repeatedly written at intervals of `dst_gap`. Value range: [1, 32767]. |
| blk_num | Number of 32-byte blocks written in a single repetition, that is, the number of 32-byte blocks filled consecutively within one repetition. Value range: [1, 32767]. |
| dst_gap | Interval between two adjacent repetitions, in units of 32-byte blocks. The start address offset between two adjacent fills is $(\mathrm{blk}_{\mathrm{num}} + \mathrm{dst}_{\mathrm{gap}}) \times 32$ bytes. Value range: [0, 32767]. |

As shown in [Figure 1](#fill_l1_layout), `value = 0` indicates that the fill value is 0, `repeat = 5` indicates that filling is performed 5 times, `blk_num = 3` indicates that 3 32-byte blocks are filled consecutively each time, and `dst_gap = 5` indicates that the interval between two adjacent fills is 5 32-byte blocks.

**Figure 1** Fill diagram in L1 Buffer<a id="fill_l1_layout"></a>
![Data layout of Fill in L1 Buffer](../../../../figures/Fill_L1.png "Fill diagram in L1 Buffer")

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- This interface returns directly for non-AIC calls.
- The start address of `dst` must be aligned to 32 bytes (L1 Buffer alignment requirement); otherwise, an address alignment exception is triggered.
- The total capacity of the L1 Buffer is 512 KB. The sum of the `dst` offset and the actual occupied space must not exceed the L1 Buffer capacity; otherwise, an address overflow exception is triggered.
- If this instruction has a data dependency with other instructions, a [synchronization instruction](../../sync/overview.md#synchronization-control-overview) must be inserted to ensure that multiple instructions are serialized and to prevent abnormal data.
- The data type of `value` must satisfy the combination relationship with the data type of `dst` described in the Function Prototype section. When the two data types are different, the value of `value` must be converted according to the data type of `dst`. For details, see the Calling Example.

## Examples

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id9 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id9 -->

The sample occupies 64KB L1 Buffer, 32KB L0A Buffer, 32KB L0B Buffer, and 64KB L0C Buffer on a single core. The sample reuses `EVENT_ID0` in sequence to complete the synchronization from MTE2 to MTE1, MTE1 to M, and M to FIX.

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t M = 128;
constexpr uint32_t K = 128;
constexpr uint32_t N = 128;
constexpr uint32_t ELEMENTS = M * N;
constexpr uint32_t CUBE_BLOCK = 16;
constexpr uint32_t BLOCK_BYTES = 32;

__global__ __cube__ void AscFillL1Kernel(__gm__ float* output)
{
    asc_init();
    __cbuf__ half a_l1[M * K], b_l1[N * K];
    __ca__ half a_l0[M * K];
    __cb__ half b_l0[K * N];
    __cc__ float c_l0[ELEMENTS];

    // Use two value types to fill all A and B cubes in L1 with 1.
    asc_fill_value_config config;
    config.repeat = M;
    config.blk_num = K * sizeof(half) / BLOCK_BYTES;
    config.dst_gap = 0;
    asc_fill_l1(a_l1, static_cast<half>(1.0f), config);

    config.repeat = N;
    // 0x3C00 is 1.0 of the half type, and two such values are packed in uint32_t.
    asc_fill_l1(b_l1, 0x3C003C00U, config);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // Move the filled cubes into L0A and L0B.
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_BLOCK, K / CUBE_BLOCK,
        M / CUBE_BLOCK, M / CUBE_BLOCK);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE_BLOCK, K / CUBE_BLOCK,
        N / CUBE_BLOCK, N / CUBE_BLOCK);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // Compute the cube multiplication; both input cubes are 1, so each result element is K.
    asc_mmad(c_l0, a_l0, b_l0, M, K, N, asc_unit_flag_mode::DISABLE, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, N, M, N, M, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM,
        asc_unit_flag_mode::DISABLE, QuantMode_t::NoQuant, asc_relu_pre_mode::NONE, false, true, false, false);
    asc_sync_pipe(PIPE_FIX);
}

void PrintRow(const char* label, const std::vector<float>& data)
{
    std::cout << label << ':';
    for (uint32_t i = 0; i < 8; ++i) {
        std::cout << ' ' << data[i];
    }
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    std::vector<float> output(ELEMENTS);
    const std::vector<float> golden(ELEMENTS, static_cast<float>(K));

    // Allocate Device memory, launch the kernel, and verify the result.
    aclInit(nullptr);
    aclrtSetDevice(0);
    float* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    AscFillL1Kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), output_device,
        output.size() * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);

    PrintRow("Output row 0", output);
    PrintRow("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_fill_l1 result is correct."
                         : "[Failed] asc_fill_l1 result mismatch.") << std::endl;

    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
