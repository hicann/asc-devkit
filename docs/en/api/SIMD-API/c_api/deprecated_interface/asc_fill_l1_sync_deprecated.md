---
okftype: reference.api
description: "Deprecated L1 Buffer initialization synchronous interface. It broadcasts the scalar fill value to multiple 32-byte blocks according to repeat, blk_num, and dst_gap in asc_fill_value_config. It takes effect only on AIC and completes synchronization internally. The alternative is asc_fill_l1 combined with asc_sync."
tag:
  - roles: [data_initialization]
  - operations: [fill]
  - traits: [datablock_granularity, scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_fill_l1.md
    kind: replaced_by
    condition: "The non-synchronous fill interface replaces the synchronous interface on this page"
  - path: docs/zh/api/SIMD-API/c_api/defs/union/asc_fill_value_config.md
    kind: requires
    condition: "repeat, blk_num, and dst_gap are carried by this structure"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: requires
    condition: "After switching to the non-synchronous interface, you must call asc_sync to complete synchronization."
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header uniformly marks all overloads with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_fill_l1 and asc_sync); the CANN_9_2_0 Release Notes deprecation table covers the asc_fill_*_sync family; the interfaces are retained and available on both 2201 and 3510, and have not been removed."
---
# asc_fill_l1_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:00:46.095Z pushedAt=2026-11-18T03:12:16.481Z nl=2 -->

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

The header file path is: `"c_api/composite/cube_datamove_composite.h"`.

**The `asc_fill_l1_sync` API is deprecated. Use the [corresponding non-synchronous API](../cube_datamove/cube_load_aux_config/asc_fill_l1.md) and the synchronous API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Writes a scalar fill value to the destination address of the L1 Buffer and broadcasts the fill value to multiple 32-byte fractal blocks. It can be used for memory initialization of the L1 Buffer. The repeat count, the number of fractal blocks per repetition, and the interval between adjacent repetitions need to be configured through [asc_fill_value_config](../defs/union/asc_fill_value_config.md).

This API takes effect only when executed on the AIC.

## Prototype

```cpp
__aicore__ inline void asc_fill_l1_sync(__cbuf__ <dst_dtype>* dst,
                                        <value_dtype> value,
                                        const asc_fill_value_config& config)
```

### Data Types Supported by dtype

The dst dtype and value dtype support the following combinations:

- When `dst_dtype` is `int16_t`, `uint16_t`, `half`, `int32_t`, `uint32_t`, or `float`, `value_dtype` supports `half` and `uint32_t`.
- When `dst_dtype` is `bfloat16_t`, `value_dtype` supports `half`, `bfloat16_t`, and `uint32_t`.

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Meaning |
|---|---|---|
| dst | Output | Destination operand, the start address of the destination matrix. The storage location is the L1 Buffer, and the start address must be 32-byte aligned. |
| value | Input | Source operand, the scalar fill value. |
| config | Input | The structure [asc_fill_value_config](../defs/union/asc_fill_value_config.md) for fill configuration. For the structure description, see [Table 2](#fill_value_config_fields). |

**Table 2** config field description <a id="fill_value_config_fields"></a>

| field name | meaning |
|---|---|
| repeat | Number of repetitions, that is, the total number of times the fill value is written repeatedly at intervals of `dst_gap` after being written into the L1 Buffer as `blk_num` 32-byte blocks. Value range: [1, 32767]. |
| blk_num | Number of 32-byte blocks written in a single repetition, that is, the number of 32-byte blocks filled consecutively within one repetition. Value range: [1, 32767]. |
| dst_gap | Interval between two adjacent repetitions, in units of 32-byte blocks. The start address offset between two adjacent fills is $(\mathrm{blk}_{\mathrm{num}} + \mathrm{dst}_{\mathrm{gap}}) \times 32$ bytes. Value range: [0, 32767]. |

As shown in [Figure 1](#fill_l1_layout), `value = 0` indicates that the fill value is 0, `repeat = 5` indicates that 5 fills are performed, `blk_num = 3` indicates that 3 32-byte blocks are filled consecutively each time, and `dst_gap = 5` indicates that the interval between two adjacent fills is 5 32-byte blocks.

**Figure 1** Fill layout in the L1 Buffer<a id="fill_l1_layout"></a>
![Data layout of Fill in the L1 Buffer](../../../figures/Fill_L1.png "Fill layout in the L1 Buffer")

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- If this interface is not called by AIC, it returns directly.
- The `dst` start address must be aligned to 32 bytes (L1 Buffer alignment requirement); otherwise, an address alignment exception is triggered.
- The total capacity of the L1 Buffer is 512KB. The sum of the `dst` offset and the actual occupied space must not exceed the L1 Buffer capacity; otherwise, an address overflow exception is triggered.
- If the destination address of this instruction overlaps with that of other instructions, synchronization instructions ([asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md)) must be inserted to serialize multiple instructions and prevent abnormal data. When `asc_fill_l1_sync` is called, synchronization is completed inside the interface.
- The data type of `value` and the data type of `dst` must satisfy the combination relationship in the function prototype section. When the two data types are different, the value of `value` must be converted according to the data type of `dst`. For details, see the calling example.

## Examples

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id9 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id9 -->

The sample occupies 64KB L1 Buffer, 32KB L0A Buffer, 32KB L0B Buffer, and 64KB L0C Buffer on a single core. The sample sequentially reuses `EVENT_ID0` to complete synchronization from MTE2 to MTE1, MTE1 to M, and M to FIX.

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

    // Use two value types to fill all A and B matrices in L1 with 1.
    asc_fill_value_config config;
    config.repeat = M;
    config.blk_num = K * sizeof(half) / BLOCK_BYTES;
    config.dst_gap = 0;
    asc_fill_l1_sync(a_l1, static_cast<half>(1.0f), config);

    config.repeat = N;
    // 0x3C00 is 1.0 of the half type, with two such values packed in uint32_t.
    asc_fill_l1_sync(b_l1, 0x3C003C00U, config);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // Move the filled matrices into L0A and L0B.
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_BLOCK, K / CUBE_BLOCK,
        M / CUBE_BLOCK, M / CUBE_BLOCK);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE_BLOCK, K / CUBE_BLOCK,
        N / CUBE_BLOCK, N / CUBE_BLOCK);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // Compute matrix multiplication; both input matrices are 1, so each result element is K.
    asc_mmad(c_l0, a_l0, b_l0, M, K, N, 0, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, N, M, N, M, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
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
