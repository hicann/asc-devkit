# CrossCoreWaitFlag\(ISASI\)<a id="ZH-CN_TOPIC_0000001787470374"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:37:53.406Z pushedAt=2026-10-26T07:41:44.791Z nl=2 -->

## Applicable Products<a id="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT product series: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 product series: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 product series: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI_res.md#id1 -->

## Description<a id="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_block_sync_intf.h"`.

In the inter-core synchronization scenario, the **CrossCoreSetFlag** and **CrossCoreWaitFlag** APIs work in pairs. For details, see [CrossCoreSetFlag Description](CrossCoreSetFlag_ISASI.md#section618mcpsimp).

## Prototype<a id="section620mcpsimp"></a>

```cpp
template <uint8_t modeId = 0, pipe_t pipe = PIPE_S>
__aicore__ inline void CrossCoreWaitFlag(uint16_t flagId)
```

## Parameter Description<a id="section622mcpsimp"></a>

### Template Parameters and Input Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| modeId | Inter-core synchronization mode, with a default value of 0. For the effective condition of this parameter and the supported pipeline types on different products, see [Template Parameter Defaults and Effective Conditions](#template-parameter-defaults).<br>For the support of synchronization modes on different products, see [modeId Supported Value Description](#modeid-supported-value-description).<br>For the corresponding kernel function (Kernel) types supported by each mode, see [Table 3](#table3). |
| pipe | Sets the pipeline type on which this API blocks, with a default value of **PIPE_S**. For the effective condition of this parameter and the supported pipeline types on different products, see [Template Parameter Defaults and Effective Conditions](#template-parameter-defaults). |

### Template Parameter Defaults and Effective Conditions<a id="template-parameter-defaults"></a>

<!-- npu="950" id8 -->
- For the Ascend 950PR&950DT product series, the two template parameters are described as follows:

    - modeId
        - `modeId` is not directly used as the input of a hardware instruction. CrossCoreWaitFlag selects the corresponding hardware instruction based on this parameter.
        - Mode 0, mode 1, and mode 2 use the same hardware instruction. When `modeId` is omitted, the default value `0` is used, which does not affect the API function. However, it is **recommended to explicitly pass the same `modeId` as that of CrossCoreSetFlag** to facilitate confirming that the synchronization modes of the two APIs match.
        - Mode 4 uses a hardware instruction different from those of mode 0, mode 1, and mode 2. You **must explicitly pass `modeId=4`**; otherwise, the API executes in the default mode 0, causing the synchronization behavior to be inconsistent with expectations.
    - pipe
        - `pipe` is used as the input of a hardware instruction, and the parameter value **takes effect** to specify the pipeline on which CrossCoreWaitFlag blocks.
        - When `pipe` is omitted, the default value `PIPE_S` is used.
        - Mode 0, mode 1, mode 2, and mode 4 support `PIPE_S`, `PIPE_V`, `PIPE_M`, `PIPE_MTE1`, `PIPE_MTE2`, `PIPE_MTE3`, and `PIPE_FIX`, but do not support `PIPE_ALL`.
<!-- end id8 -->
<!-- npu="A3,910b" id9 -->
- For the following products, **the parameter values do not affect the hardware instruction function**. The usage of the two template parameters is described as follows:

    <!-- npu="A3" id10 -->
    - Atlas A3 product series
    <!-- end id10 -->
    <!-- npu="910b" id11 -->
    - Atlas A2 product series
    <!-- end id11 -->

    `modeId` and `pipe` are not used as inputs to the hardware instruction. Configure them as follows:

    - modeId: Omitting this parameter does not affect the API function. However, it is **recommended to explicitly pass the same `modeId` as that of CrossCoreSetFlag** to facilitate confirming that the synchronization modes of the two APIs match.
    - pipe: It is **recommended to omit this parameter and use the default value `PIPE_S`**. Regardless of the value of `pipe`, CrossCoreWaitFlag blocks subsequent instructions of **all pipelines**.
<!-- end id9 -->

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| flagId | Input | Flag for inter-core synchronization. For the value range description of flagId for different products, see [flagId value range description](#flagId-value-range-description). |

### modeId Supported Value Description<a id="modeId-supported-value-description"></a>

The support for synchronization modes by different products is as follows:
<!-- npu="950" id12 -->
- For the Ascend 950PR&950DT product series, the supported synchronization modes are: mode 0, mode 1, mode 2, and mode 4.
<!-- end id12 -->
<!-- npu="A3,910b" id13 -->
- For the following products, the supported synchronization modes are: mode 0, mode 1, and mode 2.
    <!-- npu="A3" id14 -->
    - Atlas A3 product series
    <!-- end id14 -->
    <!-- npu="910b" id15 -->
    - Atlas A2 product series
    <!-- end id15 -->
<!-- end id13 -->

### flagId Value Range Description<a id="flagId-value-range-description"></a>

- When the inter-core synchronization mode is mode 0, 1, or 2, the supported value range is 0-15.
<!-- npu="950" id16 -->
- For Ascend 950PR&950DT product series, when the inter-core synchronization mode is mode 4, the supported value range is as follows:
    - The **CrossCoreSetFlag** operation with **flagId** 0-15 initiated by **AIV0** corresponds to the operation with **flagId** 0-15 in **AIC** **CrossCoreWaitFlag**.
    - The **CrossCoreSetFlag** operation with **flagId** 0-15 initiated by **AIV1** corresponds to the operation with **flagId** 16-31 in **AIC** **CrossCoreWaitFlag**.
    - The **CrossCoreSetFlag** operation with **flagId** 0-15 initiated by **AIC** corresponds to the operation with **flagId** 0-15 in **AIV0** **CrossCoreWaitFlag**.
    - The **CrossCoreSetFlag** operation with **flagId** 16-31 initiated by **AIC** corresponds to the operation with **flagId** 0-15 in **AIV1** **CrossCoreWaitFlag**.
<!-- end id16 -->

In addition, when used together with the **Matmul** high-level API or the **SyncAll** API, the **flagId** used by developers must also avoid conflicts with the **flagId** already occupied internally by these APIs. For details, see [Inter-core Synchronization flagId Usage](key_features.md#inter_core_sync_flagid_usage).

## Return Value<a id="section640mcpsimp"></a>

None

## Constraints<a id="section633mcpsimp"></a>

- When the kernel type is **KERNEL_TYPE_AIC_ONLY** or **KERNEL_TYPE_AIV_ONLY**, the hardware does not enable the scheduling module, so inter-core synchronization cannot proceed normally. Therefore, the [kernel type](../../Kernel-Tiling/set_Kernel_type.md) or [function execution space qualifier](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#function-execution-space-qualifiers) configured for different synchronization modes is as follows:
    - In a pure Vector/Cube scenario (mode 0 or mode 1), it is recommended to set the kernel function type to KERNEL\_TYPE\_MIX\_AIV\_1\_0 or KERNEL\_TYPE\_MIX\_AIC\_1\_0. For other supported kernel function types, see Table 3.
    - For a mixed Vector and Cube scenario (mode 2 and mode 4), configure the kernel type flexibly based on the ratio of AIC to AIV in the AI Core. For the function execution space qualifiers and kernel types supported by different modes, see Table 3.

        **Table 3**  Mode and supported kernel function type configuration<a id="table3"></a>

        | Mode | Supported function execution space qualifier | Supported kernel type configuration |
        | --- | --- | --- |
        | 0 | \_\_mix\_\_(0, 1), \_\_mix\_\_(1, 0), \_\_mix\_\_(1, 1), \_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIV\_1\_0, KERNEL\_TYPE\_MIX\_AIC\_1\_0, KERNEL\_TYPE\_MIX\_AIC\_1\_1, KERNEL\_TYPE\_MIX\_AIC\_1\_2 |
        | 1 | \_\_mix\_\_(1, 1), \_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIC\_1\_1, KERNEL\_TYPE\_MIX\_AIC\_1\_2 |
        | 2 | \_\_mix\_\_(1, 1), \_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIC\_1\_1, KERNEL\_TYPE\_MIX\_AIC\_1\_2 |
        | 4 | \_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIC\_1\_2 |

- **CrossCoreWaitFlag** must be used together with the [CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md) API to prevent the computation core from remaining in a blocked state. It is recommended to explicitly configure the same **modeId** as the paired **CrossCoreSetFlag** to confirm that Set and Wait use the same synchronization mode.

- The internal implementations of the following APIs use **CrossCoreSetFlag** and **CrossCoreWaitFlag**, which occupy a portion of **flagId**. When developers use **CrossCoreSetFlag**/**CrossCoreWaitFlag** together with these APIs, they must avoid **flagId** conflicts. For a complete description of the **flagId** occupied by each API, see [inter-core synchronization flagId usage](key_features.md#inter_core_sync_flagid_usage).

- Constraints related to **flagId**:
    - For mode 0, 1, and 2, each AIC and each AIV has 16 **flagId** values, with a supported value range of 0-15. If the value of **flagId** exceeds this range, the low 4 bits are truncated (for example, when **flagId**=17, the value after truncation is 1).
    <!-- npu="950" id17 -->
    - For Ascend 950PR&950DT product series, when the inter-core synchronization mode is mode 4, AIC has 32 flagIds with a supported value range of 0-31. If the value of **flagId** exceeds this range, the low 5 bits are truncated (for example, when **flagId**=35, the value after truncation is 3). AIV has 16 flagIds with a supported value range of 0-15. If the value of **flagId** exceeds this range, the low 4 bits are truncated (for example, when **flagId**=17, the value after truncation is 1).
    <!-- end id17 -->
    - Each **flagId** corresponds to a counter. When [CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md) is called, if the counter value is 0, the dispatch of subsequent instructions is blocked, while already-dispatched instructions can execute normally. When the scheduling module detects that all inter-core synchronization (CrossCoreSetFlag) operations are complete, it increments the counter corresponding to **CrossCoreWaitFlag** by 1. At this point, the counter value is non-zero, the blocking is released, and the corresponding counter value is decremented by 1 to restore it. For the specific execution logic and details, see [Key Features](key_features.md). The counter corresponding to **flagId** has a value range of 0-15. If the counter value exceeds this range, an error is reported and the process is interrupted.
- Constraints on using the same **flagId** for different inter-core synchronization modes under modes 0, 1, and 2:
    - On the same core, if the same **flagId** needs to be used for different inter-core synchronization modes, all synchronization operations of the previous mode must be completed before switching modes, that is, ensure that all **CrossCoreSetFlag** calls associated with this **flagId** and their matching **CrossCoreWaitFlag** calls have been fully executed.
    - For different cores, the same **flagId** can be directly used for different inter-core synchronization modes, specifically including the following two scenarios:
        - Across multiple AI Cores, use **flagId**=0 to synchronize all AICs (mode 0); within a single AI Core, use **flagId**=0 to synchronize all AIVs (mode 1).
        - Within a single AI Core, use **flagId**=0 to synchronize all AIVs (mode 1); within another AI Core, use **flagId**=0 to synchronize the AIC and all AIVs (mode 2).
- When using mode 0 of this API, it is recommended to enable batchmode mode so that the operator exclusively occupies all required core resources. Otherwise, a deadlock may occur if the following conditions are met:
    - Multi-stream concurrent scenario (≥2 execution streams).
    - ≥2 operators executing concurrently.
    - The total number of cores of all concurrent operators exceeds the number of physical cores.
    - At least two concurrent operators use the inter-core synchronization function.

    Specifically, in a multi-stream scenario, although the inter-core synchronization operator of a stream is allocated n physical cores, only n-m cores may be scheduled and executed first, while the remaining m cores have not yet started because they are preempted by the inter-core synchronization operators of other streams. The n-m cores that start first wait for the remaining m cores to complete when they reach the inter-core synchronization point, while the remaining m cores cannot be released because they are occupied by the inter-core synchronization operators of other streams, resulting in a deadlock.

    In the kernel function (Kernel) launch scenario, the batchmode mode is set through the [\_\_schedmode\_\_\(mode\)](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md) qualifier; in the engineering operator development scenario, the batchmode mode is set through the SetScheduleMode API of TilingContext. For details, see [Basic Data Structures and APIs](https://gitcode.com/cann/metadef/blob/9.2.0/docs/en/api/README.md).

## Example<a id="section837496171220"></a>

**Table 4** Sample description

| SCENARIO_NUM Value | Business Scenario | Used Synchronization Mode |
| --- | --- | --- |
| 0 | Pure Vector computation scenario (16 AIVs) | mode0 (AIV full-core synchronization) |
| 1 | Pure Vector computation scenario (2 AIVs) | mode1 |
| 2 | Cube and Vector fusion computation scenario | mode2 (AIC waits for AIV), mode2 (AIV waits for AIC), mode0 (AIC full-core synchronization) |

As shown in the table above, when SCENARIO_NUM takes different values, the specific usage of the three synchronization modes in the pure Vector computation scenario and the Cube and Vector fusion computation scenario is demonstrated respectively. The following shows part of the calling code for the pure Vector computation scenario:

```cpp
    AscendC::Muls(xLocal, xLocal, float(AscendC::GetBlockIdx()), this->blockLength);
    AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);

    // Enable atomic accumulation for Unified Buffer (UB) to GM transfer.
    AscendC::SetAtomicAdd<float>();
    // DataCopy is a PIPE_MTE3 pipeline operation.
    AscendC::DataCopy(atomicResultGm, xLocal, this->blockLength);
    // After this AIV completes the preceding PIPE_MTE3 (DataCopy) pipeline operation, notify other AIV cores that this AIV has completed.
    AscendC::CrossCoreSetFlag<0, PIPE_MTE3>(0);
    // Block this AIV from continuing to execute instructions until all other AIVs have completed the PIPE_MTE3 pipeline operation, then unblock and continue execution.
    AscendC::CrossCoreWaitFlag<0>(0);
    // Disable atomic accumulation.
    AscendC::DisableDmaAtomic();

    if (AscendC::GetBlockIdx() == 0) {
        AscendC::DataCopy(yLocal, atomicResultGm, this->blockLength);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
        AscendC::DataCopy(atomicResultGm, yLocal, this->blockLength);
        return;
    }
```

For the complete sample, see [CrossCoreSetFlag and CrossCoreWaitFlag inter-core synchronization sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/cross_core_set_wait_flag).
