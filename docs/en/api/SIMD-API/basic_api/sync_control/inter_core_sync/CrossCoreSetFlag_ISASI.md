# CrossCoreSetFlag\(ISASI\)<a id="ZH-CN_TOPIC_0000001834069637"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:37:44.030Z pushedAt=2026-10-26T07:41:44.789Z nl=2 -->

## Applicable Products<a id="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI_res.md#id1 -->

## Description<a id="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_block_sync_intf.h"`.

In the inter-core synchronization scenario, the **CrossCoreSetFlag** API works in pair with the [CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md) API. When using them, you need to pass in the flag ID (**flagId**) for inter-core synchronization, and each ID corresponds to a counter used for synchronization control.

- **CrossCoreSetFlag**: After the AIC/AIV in the AI Core completes the preceding pipe pipeline task of this core, it calls this API to report to the scheduling module that the task of this core is complete. The scheduling module collects the report information from all participating cores as the global completion flag.
- **CrossCoreWaitFlag**: Used to block the issuance of subsequent instructions of this core (already-issued instructions can execute normally) until the scheduling module confirms that all cores that should call **CrossCoreSetFlag** have reported completion, at which point the blocking is released and the core is allowed to continue issuing instructions.

As shown in [Figure 1](#fig37581010773) (in the figure, block represents AIC and sub_block represents AIV), synchronization control is divided into the following modes:

- Mode 0: Synchronization control between AI Cores. For the AIC scenario, synchronize all AIC cores. Only when all AIC cores have executed **CrossCoreSetFlag** will the subsequent instruction of **CrossCoreWaitFlag** execute. For the AIV scenario, synchronize all AIV cores. Only when all AIV cores have executed **CrossCoreSetFlag** will the subsequent instruction of **CrossCoreWaitFlag** execute.
- Mode 1: Synchronization control between AIV cores within the AI Core. If both AIV cores have run **CrossCoreSetFlag**, the subsequent instruction of **CrossCoreWaitFlag** will execute.
- Mode 2: Synchronization control between the AIC and all AIVs within the AI Core. After the AIC core executes **CrossCoreSetFlag**, the subsequent instructions of **CrossCoreWaitFlag** on the two AIVs will continue to execute. After both AIVs execute **CrossCoreSetFlag**, the subsequent instructions of **CrossCoreWaitFlag** on the AIC can execute.
- Mode 4: Synchronization control between the AIC and a single AIV within the AI Core. After the AIC core executes **CrossCoreSetFlag**, the subsequent instructions after CrossCoreWaitFlag on AIV0 or AIV1 will continue to execute. After AIV0 or AIV1 executes **CrossCoreSetFlag**, the subsequent instructions after CrossCoreWaitFlag on the AIC can execute.

> [!NOTE]
> - Different models support different modes. For details about the supported modes, see [modeId Supported Value Description](#modeid-supported-value-description).
> - For the specific execution logic and details of each mode, see [key features](key_features.md).

**Figure 1**  Synchronization control mode diagram<a id="fig37581010773"></a>  
![](../../../../figures/block_subblock_relationship_3510.png "Synchronization control mode diagram")

## Function Prototype<a id="section620mcpsimp"></a>

```cpp
template <uint8_t modeId, pipe_t pipe>
__aicore__ inline void CrossCoreSetFlag(uint16_t flagId)
```

## Parameters<a id="section622mcpsimp"></a>

### Template Parameters and Input Parameters

The template parameters `modeId` and `pipe` of **CrossCoreSetFlag** **have no default values**. Their values are used to configure hardware instructions and must be **explicitly passed in** when calling the API. Unlike this API, the two template parameters of **CrossCoreWaitFlag** both have default values, and their effective behavior differs across different products and synchronization modes. Therefore, **the two APIs have different requirements for passing template parameters**. For details, see [CrossCoreWaitFlag template parameter description](CrossCoreWaitFlag_ISASI.md#template-parameter-defaults).

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| modeId | Inter-core synchronization mode. For the support for synchronization modes on different products, see [modeId Supported Value Description](#modeid-supported-value-description).<br>For the kernel function (Kernel) types supported by each mode, see [Table 3](#table3). |
| pipe | Identifies the pipeline after whose preceding instructions complete, **CrossCoreSetFlag** sends a notification to the scheduling module. The supported pipeline types are PIPE_V, PIPE_M, PIPE_MTE1, PIPE_MTE2, PIPE_MTE3, and PIPE_FIX; PIPE_S and PIPE_ALL are not supported. For the support for pipeline types on different products, see [Supported Pipeline Types for pipe](#supported-pipeline-types-for-pipe). |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| flagId | Input | Inter-core synchronization flag. For the value range description of flagId for different products, see [flagId Value Range Description](#flagId-value-range-description). |

### modeId Supported Value Description<a id="modeid-supported-value-description"></a>

The support for synchronization modes varies by product as follows:
<!-- npu="950" id8 -->
- For Ascend 950PR&950DT products, the supported synchronization modes are: mode 0, mode 1, mode 2, and mode 4.
<!-- end id8 -->
<!-- npu="A3,910b" id9 -->
- For the following products, the supported synchronization modes are: mode 0, mode 1, and mode 2.
    <!-- npu="A3" id10 -->
    - Atlas A3 products
    <!-- end id10 -->
    <!-- npu="910b" id11 -->
    - Atlas A2 products
    <!-- end id11 -->
<!-- end id9 -->

### Supported Pipeline Types for **pipe**<a id="supported-pipeline-types-for-pipe"></a>

- When the inter-core synchronization mode is mode 0, 1, or 2,
    - The supported pipeline types are **PIPE_V**, **PIPE_M**, **PIPE_MTE1**, **PIPE_MTE2**, **PIPE_MTE3**, and **PIPE_FIX**.
    - The unsupported pipeline types are **PIPE_ALL** and **PIPE_S**.
<!-- npu="950" id12 -->
- For Ascend 950PR&950DT products, when the inter-core synchronization mode is mode 4,
    - The supported pipeline types are **PIPE_V**, **PIPE_M**, **PIPE_MTE1**, **PIPE_MTE2**, **PIPE_MTE3**, **PIPE_FIX**, and **PIPE_S**.
    - The unsupported pipeline type is **PIPE_ALL**.
<!-- end id12 -->

### flagId Value Range Description<a id="flagId-value-range-description"></a>

- When the inter-core synchronization mode is mode 0, 1, or 2, the supported value range is 0-15.
<!-- npu="950" id13 -->
- For Ascend 950PR&950DT products, when the inter-core synchronization mode is mode 4, the supported value range is as follows:
    - The **CrossCoreSetFlag** operation with **flagId** 0-15 initiated by AIV0 corresponds to the operation with **flagId** 0-15 in AIC **CrossCoreWaitFlag**.
    - The **CrossCoreSetFlag** operation with **flagId** 0-15 initiated by AIV1 corresponds to the operation with **flagId** 16-31 in AIC **CrossCoreWaitFlag**.
    - The **CrossCoreSetFlag** operation with **flagId** 0-15 initiated by AIC corresponds to the operation with **flagId** 0-15 in AIV0 **CrossCoreWaitFlag**.
    - The **CrossCoreSetFlag** operation with **flagId** 16-31 initiated by AIC corresponds to the operation with **flagId** 0-15 in AIV1 **CrossCoreWaitFlag**.
<!-- end id13 -->

In addition, when this API is used together with the **Matmul** high-level API or the **SyncAll** API, developers must also ensure that the **flagId** values they use do not conflict with the **flagId** values already occupied internally by these APIs. For details, see [Inter-core Synchronization flagId Usage](key_features.md#inter_core_sync_flagid_usage).

## Return Value<a id="section640mcpsimp"></a>

None

## Constraints<a id="section633mcpsimp"></a>

- Because the hardware does not enable the scheduling module when the kernel function (Kernel) type is KERNEL_TYPE_AIC_ONLY or KERNEL_TYPE_AIV_ONLY, inter-core synchronization cannot be performed normally. Therefore, the configuration of the [kernel function (Kernel) type](../../Kernel-Tiling/set_Kernel_type.md) or the [function execution space qualifier](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#function-execution-space-qualifiers) for different synchronization modes is as follows:
    - In a pure Vector/Cube scenario (mode 0 or mode 1), it is recommended to set the kernel function (Kernel) type to KERNEL\_TYPE\_MIX\_AIV\_1\_0 or KERNEL\_TYPE\_MIX\_AIC\_1\_0. For other supported kernel function (Kernel) types, refer to Table 3.
    - For a mixed Vector and Cube scenario (mode 2 and mode 4), the kernel function (Kernel) type must be flexibly configured based on the ratio of AIC to AIV in the AI Core. For the function execution space qualifiers and kernel function (Kernel) types supported by different modes, refer to Table 3.

        **Table 3**  Modes and supported kernel function (Kernel) type configurations<a id="table3"></a>

        | Mode | Supported function execution space qualifier | Supported kernel function (Kernel) type configuration |
        | --- | --- | --- |
        | 0 | \_\_mix\_\_(0, 1), \_\_mix\_\_(1, 0), \_\_mix\_\_(1, 1), \_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIV\_1\_0, KERNEL\_TYPE\_MIX\_AIC\_1\_0, KERNEL\_TYPE\_MIX\_AIC\_1\_1, KERNEL\_TYPE\_MIX\_AIC\_1\_2 |
        | 1 | \_\_mix\_\_(1, 1), \_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIC\_1\_1, KERNEL\_TYPE\_MIX\_AIC\_1\_2 |
        | 2 | \_\_mix\_\_(1, 1), \_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIC\_1\_1, KERNEL\_TYPE\_MIX\_AIC\_1\_2 |
        | 4 | \_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIC\_1\_2 |

- The following APIs use CrossCoreSetFlag and CrossCoreWaitFlag in their internal implementation, which occupies some flagIds. When developers use CrossCoreSetFlag/CrossCoreWaitFlag together with these APIs, they must avoid flagId usage conflicts. For a complete description of the flagIds occupied by each API, see [Inter-core Synchronization flagId Usage](key_features.md#inter_core_sync_flagid_usage).

- Users must ensure that CrossCoreSetFlag and CrossCoreWaitFlag are used together; otherwise, undefined behavior may occur.
- When the inter-core synchronization mode is mode 0, 1, or 2, the template parameter pipe does not support being set to PIPE\_ALL or PIPE\_S.
<!-- npu="950" id14 -->
- For Ascend 950PR&950DT products, when the inter-core synchronization mode is mode 4, the template parameter pipe does not support being configured as PIPE\_ALL.
<!-- end id14 -->
- For multiple **CrossCoreSetFlag** calls issued consecutively by a core, the hardware does not guarantee the execution order. For example, if an AIV issues **CrossCoreSetFlag**<0, PIPE\_V\>\(0\) and **CrossCoreSetFlag**<0, PIPE\_V\>\(1\) in sequence, the actual execution order may be that flagId=1 is executed first.
- Constraints related to flagId:
    - For modes 0, 1, and 2, each AIC and each AIV has 16 flagIds, and the supported value range is 0-15. If the value of flagId exceeds this range, then truncate the low 4 bits (for example, when flagId=17, the value after truncation is 1).
    <!-- npu="950" id15 -->
    - For Ascend 950PR&950DT products, when the inter-core synchronization mode is mode 4, an AIC has 32 flagIds, and the supported value range is 0-31. If the value of flagId exceeds this range, then truncate the low 5 bits (for example, when flagId=35, the value after truncation is 3). An AIV has 16 flagIds, and the supported value range is 0-15. If the value of flagId exceeds this range, then truncate the low 4 bits (for example, when flagId=17, the value after truncation is 1).
    <!-- end id15 -->
    - Each flagId corresponds to a counter. When [CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md) is called, if the counter value is 0, the issuance of subsequent instructions is blocked, while already-issued instructions can execute normally. When the scheduling module detects that all cores participating in the synchronization (which cores are included depends on the configured inter-core synchronization mode) have completed the synchronization (by calling **CrossCoreSetFlag**), it increments the counter corresponding to the configured flagId by 1. At this point, the counter value is non-zero, the blocking is released, and the corresponding counter value is decremented by 1 to restore it. For the specific execution logic and details, refer to [Key Feature Description](key_features.md). The counting range of each counter is 0-15. If **CrossCoreSetFlag** is called more than 15 times and the counter value exceeds this range, an error is reported and the process is interrupted.
- When the inter-core synchronization mode is mode 0, 1, or 2, the constraints for using the same flagId in different inter-core synchronization modes are as follows:
    - On the same core, if the same flagId needs to be used in different inter-core synchronization modes, all synchronization operations of the previous mode must be completed before switching modes, that is, ensure that all **CrossCoreSetFlag** calls associated with this flagId and the corresponding **CrossCoreWaitFlag** calls have been executed.
    - For different cores, the same flagId can be directly used in different inter-core synchronization modes, specifically including the following two scenarios:
        - Between multiple AI Cores, use flagId=0 to synchronize all AICs (mode 0); within a single AI Core, use flagId=0 to synchronize all AIVs (mode 1).
        - Within a single AI Core, use flagId=0 to synchronize all AIVs (mode 1); within another AI Core, use flagId=0 to synchronize the AIC and all AIVs (mode 2).
- When using mode 0 of this API, it is recommended to enable **batchmode** so that the operator exclusively occupies all required core resources; otherwise, a deadlock may occur if the following conditions are met:
    - Multi-stream concurrent scenario (≥2 execution streams).
    - ≥2 operators executing concurrently.
    - The total number of cores of all concurrent operators exceeds the number of physical cores.
    - ≥2 concurrent operators use the inter-core synchronization function.

    Specifically, in a multi-stream scenario, although the inter-core synchronization operator of a stream is allocated n physical cores, only n-m cores may be scheduled and executed first, while the remaining m cores have not yet started because they are preempted by the inter-core synchronization operators of other streams. The n-m cores that start first wait for the remaining m cores to complete when they reach the inter-core synchronization point, while the remaining m cores cannot be released because they are occupied by the inter-core synchronization operators of other streams, resulting in a deadlock.
    In the kernel function (Kernel) launch scenario, the **batchmode** mode is set through the [\_\_schedmode\_\_\(mode\)](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md) qualifier; in the engineering operator development scenario, the **batchmode** mode is set through the **SetScheduleMode** API of **TilingContext**. For details, see *[Basic Data Structures and APIs](https://gitcode.com/cann/metadef/blob/9.2.0/docs/en/api/README.md)*.

## Example<a id="section837496171220"></a>

**Table 4** Sample description

| SCENARIO_NUM value | Business scenario | Used synchronization mode |
| --- | --- | --- |
| 0 | Pure Vector computation scenario (16 AIVs) | mode0 (AIV all-core synchronization) |
| 1 | Pure Vector computation scenario (2 AIVs) | mode1 |
| 2 | Cube and Vector fusion computation scenario | mode2 (AIC waits for AIV), mode2 (AIV waits for AIC), mode0 (AIC all-core synchronization) |

As shown in the table above, when SCENARIO_NUM takes different values, the specific usage of the three synchronization modes in the pure Vector computation scenario and the Cube and Vector fusion computation scenario is demonstrated respectively. The following shows part of the calling code for the pure Vector computation scenario:

```cpp
    AscendC::Muls(xLocal, xLocal, float(AscendC::GetBlockIdx()), this->blockLength);
    AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);

    // Enable atomic accumulation for Unified Buffer (UB) to GM data movement.
    AscendC::SetAtomicAdd<float>();
    // DataCopy is a PIPE_MTE3 pipeline operation.
    AscendC::DataCopy(atomicResultGm, xLocal, this->blockLength);
    // After this AIV completes the preceding PIPE_MTE3 (DataCopy) pipeline operation, notify the other AIV cores that this AIV has completed.
    AscendC::CrossCoreSetFlag<0, PIPE_MTE3>(0);
    // Block this AIV from continuing to execute subsequent instructions until all other AIVs have completed the PIPE_MTE3 pipeline operation, then unblock and continue execution.
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
