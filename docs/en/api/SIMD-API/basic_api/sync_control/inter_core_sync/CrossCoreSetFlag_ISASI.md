<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:23:46.819Z -->
# CrossCoreSetFlag\(ISASI\)<a name="ZH-CN_TOPIC_0000001834069637"></a>


## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not Supported
<!-- end id7 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_block_sync_intf.h"`.

In inter-core synchronization scenarios, the **CrossCoreSetFlag** API works in pair with the [CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md) API. When using them, you need to pass in the flag ID (**flagId**) for inter-core synchronization, where each ID corresponds to a counter used for synchronization control.

- **CrossCoreSetFlag**: After the AIC/AIV in the AI Core completes the preceding pipe pipeline tasks of the current core, it calls this API to report to the scheduling module that the tasks of the current core are complete. The scheduling module collects the report information from all participating cores as the global completion flag.
- **CrossCoreWaitFlag**: Used to block the issuance of subsequent instructions of the current core (already-issued instructions can execute normally) until the scheduling module confirms that all cores that should call **CrossCoreSetFlag** have reported completion, at which point the blocking is released and the core is allowed to continue issuing instructions.

As shown in [Figure 1](#fig37581010773) (in the figure, block represents AIC and sub_block represents AIV), synchronization control is divided into the following modes:

- Mode 0: Synchronization control between AI Cores. For the AIC scenario, all AIC cores are synchronized, and the instructions following **CrossCoreWaitFlag** are executed only after all AIC cores have executed **CrossCoreSetFlag**. For the AIV scenario, all AIV cores are synchronized, and the instructions following **CrossCoreWaitFlag** are executed only after all AIV cores have executed **CrossCoreSetFlag**.
- Mode 1: Synchronization control between AIV cores within an AI Core. The instructions following **CrossCoreWaitFlag** are executed only after both AIV cores have executed **CrossCoreSetFlag**.
- Mode 2: Synchronization control between the AIC and all AIVs within an AI Core. After the AIC core executes **CrossCoreSetFlag**, the instructions following **CrossCoreWaitFlag** on the two AIVs continue to execute. After both AIVs execute **CrossCoreSetFlag**, the instructions following **CrossCoreWaitFlag** on the AIC can execute.
- Mode 4: Synchronization control between the AIC and a single AIV within an AI Core. After the AIC core executes **CrossCoreSetFlag**, the instructions following **CrossCoreWaitFlag** on AIV0 or AIV1 continue to execute. After AIV0 or AIV1 executes **CrossCoreSetFlag**, the instructions following **CrossCoreWaitFlag** on the AIC can execute.

> [!NOTE]
> - Different models support different modes. For details about the supported modes, see [Supported Value Description of modeId](#supported-value-description-of-modeid).
> - For the specific execution logic and details of each mode, see [Key Features Description](key_feature_description.md#ZH-CN_TOPIC_0000002586300741).

**Figure 1**  Synchronization Control Mode Diagram<a name="fig37581010773"></a>  
![](../../../../figures/block_subblock_relationship_3510.png "Synchronization Control Mode Diagram")

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <uint8_t modeId, pipe_t pipe>
__aicore__ inline void CrossCoreSetFlag(uint16_t flagId)
```

## Parameter Description<a name="section622mcpsimp"></a>

### Template Parameters and Input Parameters

**Table 1** Template Parameters

| Parameter | Description |
| --- | --- |
| modeId | Inter-core synchronization mode. For the synchronization modes supported by different products, see [Supported Value Description of modeId](#supported-value-description-of-modeid).<br>For the Kernel types supported by each mode, see [Table 3](#table3). |
| pipe | Pipeline type in which this instruction resides. Supported pipeline types are PIPE_V, PIPE_M, PIPE_MTE1, PIPE_MTE2, PIPE_MTE3, and PIPE_FIX. PIPE_S and PIPE_ALL are not supported. For the pipeline types supported by different products, see [Supported Pipeline Type Description for pipe](#supported-pipeline-type-description-for-pipe). |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| flagId | Input | Inter-core synchronization flag. For the value range of flagId on different products, see [flagId Value Range Description](#flagid-value-range-description). |

### Supported Value Description of modeId<a name="supported-value-description-of-modeid"></a>

The support for synchronization modes varies by product as follows:
<!-- npu="950" id8 -->
- For Ascend 950PR/Ascend 950DT, the supported synchronization modes are **Mode 0**, **Mode 1**, **Mode 2**, and **Mode 4**.
<!-- end id8 -->
<!-- npu="A3,910b" id9 -->
- For the following products, the supported synchronization modes are **Mode 0**, **Mode 1**, and **Mode 2**.
    <!-- npu="A3" id10 -->
    - Atlas A3 training products/Atlas A3 inference products
    <!-- end id10 -->
    <!-- npu="910b" id11 -->
    - Atlas A2 training products/Atlas A2 inference products
    <!-- end id11 -->
<!-- end id9 -->

### Supported Pipeline Type Description for pipe<a name="supported-pipeline-type-description-for-pipe"></a>

- When the inter-core synchronization mode is mode 0, 1, or 2,
    - The supported pipeline types are **PIPE_V**, **PIPE_M**, **PIPE_MTE1**, **PIPE_MTE2**, **PIPE_MTE3**, and **PIPE_FIX**.
    - The unsupported pipeline types are **PIPE_ALL** and **PIPE_S**.
<!-- npu="950" id12 -->
- For Ascend 950PR/Ascend 950DT, when the inter-core synchronization mode is mode 4,
    - The supported pipeline types are **PIPE_V**, **PIPE_M**, **PIPE_MTE1**, **PIPE_MTE2**, **PIPE_MTE3**, **PIPE_FIX**, and **PIPE_S**.
    - The unsupported pipeline type is **PIPE_ALL**.
<!-- end id12 -->

### flagId Value Range Description<a name="flagId-value-range-description"></a>

- When the inter-core synchronization mode is Mode 0, 1, or 2, the supported value range is 0-15.
<!-- npu="950" id13 -->
- For Ascend 950PR/Ascend 950DT, when the inter-core synchronization mode is Mode 4, the supported value ranges are as follows:
    - The **CrossCoreSetFlag** operation with **flagId** 0-10 initiated by **AIV0** corresponds to the operation with **flagId** 0-10 in **AIC** **CrossCoreWaitFlag**.
    - The **CrossCoreSetFlag** operation with **flagId** 0-10 initiated by **AIV1** corresponds to the operation with **flagId** 16-26 in **AIC** **CrossCoreWaitFlag**.
    - The **CrossCoreSetFlag** operation with **flagId** 0-10 initiated by **AIC** corresponds to the operation with **flagId** 0-10 in **AIV0** **CrossCoreWaitFlag**.
    - The **CrossCoreSetFlag** operation with **flagId** 16-26 initiated by **AIC** corresponds to the operation with **flagId** 0-10 in **AIV1** **CrossCoreWaitFlag**.
<!-- end id13 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- When the Kernel type is **KERNEL_TYPE_AIC_ONLY** or **KERNEL_TYPE_AIV_ONLY**, the hardware does not enable the scheduling module, so inter-core synchronization cannot be performed normally. Therefore, the [Kernel type](../../Kernel-Tiling/set_kernel_type.md) or [function modifier](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMT-BuiltIn_keyword.md#section1074418132518) configured for different synchronization modes is as follows:
    - In pure Vector/Cube scenarios (mode 0 or mode 1), it is recommended to set the Kernel type to **KERNEL_TYPE_MIX_AIV_1_0** or **KERNEL_TYPE_MIX_AIC_1_0**. For other supported Kernel types, see Table 3.
    - For mixed Vector and Cube scenarios (mode 2 and mode 4), configure the Kernel type flexibly based on the ratio of AIC to AIV in the AI Core. For the function modifiers and Kernel types supported by different modes, see Table 3.

        **Table 3**  Modes and supported Kernel type configurations<a name="table3"></a>

        | Mode | Supported Function Modifier | Supported Kernel Type Configuration |
        | --- | --- | --- |
        | 0 | \_\_mix\_\_(0, 1), \_\_mix\_\_(1, 0), \_\_mix\_\_(1, 1), \_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIV\_1\_0, KERNEL\_TYPE\_MIX\_AIC\_1\_0, KERNEL\_TYPE\_MIX\_AIC\_1\_1, KERNEL\_TYPE\_MIX\_AIC\_1\_2 |
        | 1 | \_\_mix\_\_(1, 1), \_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIC\_1\_1, KERNEL\_TYPE\_MIX\_AIC\_1\_2 |
        | 2 | \_\_mix\_\_(1, 1), \_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIC\_1\_1, KERNEL\_TYPE\_MIX\_AIC\_1\_2 |
        | 4 | \_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIC\_1\_2 |

- When the API uses mode 0, 1, or 2, avoid flagId usage conflicts:
    - The internal implementation of the Matmul high-level API uses **CrossCoreSetFlag** for inter-core synchronization control. Therefore, it is not recommended to use **CrossCoreSetFlag** together with the Matmul high-level API; otherwise, there is a risk of flagId conflicts. The flagId range occupied internally by the Matmul high-level API is related to the number of defined Matmul objects. Assuming N Matmul objects are defined, the flagId range occupied internally by the Matmul high-level API is [0, 2 * N - 1]. Matmul supports defining up to 4 objects, in which case the occupied flagId range is [0, 7].
    - The internal implementation of the SyncAll hardware synchronization API uses **CrossCoreSetFlag** for inter-core synchronization control. Therefore, it is not recommended to use **CrossCoreSetFlag** together with the SyncAll hardware synchronization API; otherwise, there is a risk of flagId conflicts. The flagId range occupied by the SyncAll hardware synchronization API is [11-14].
- Users must ensure that **CrossCoreSetFlag** and **CrossCoreWaitFlag** are used together; otherwise, undefined behavior may occur.
- When the inter-core synchronization mode is mode 0, 1, or 2, the template parameter **pipe** does not support being set to `PIPE_ALL` or `PIPE_S`.
<!-- npu="950" id14 -->
- For Ascend 950PR/Ascend 950DT, when the inter-core synchronization mode is mode 4, the template parameter **pipe** does not support being configured as `PIPE_ALL`.
<!-- end id14 -->
- For **CrossCoreSetFlag** issued consecutively by a single core, the hardware does not guarantee the execution order. For example, if the AIV sequentially issues **CrossCoreSetFlag**<0, `PIPE_V`>(0) and **CrossCoreSetFlag**<0, `PIPE_V`>(1), the actual execution order may be that flagId=1 is executed first.
- Constraints related to **flagId**:
    - For modes 0, 1, and 2, each AIC and each AIV has 16 **flagId** values respectively, with a supported value range of 0 to 15. If the value of **flagId** exceeds this range, it is truncated to the lowest 4 bits.
    - Each **flagId** corresponds to a counter. When [CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md) is called, if the counter value is 0, the issuance of subsequent instructions is blocked, while already-issued instructions can execute normally. After the scheduling module detects that all cores participating in the synchronization (the specific cores involved depend on the configured inter-core synchronization mode) have completed the synchronization (by calling **CrossCoreSetFlag**), it increments the counter corresponding to the configured **flagId** by 1. At this point, the counter value is non-zero, the blocking is released, and the corresponding counter value is decremented by 1 to restore it. For the specific execution logic and details, see [Key Features](key_feature_description.md#ZH-CN_TOPIC_0000002586300741). Each counter has a counting range of 0 to 15. If **CrossCoreWaitFlag** is called too many times and the counter value exceeds this range, an error is reported and the process is interrupted.
- When the inter-core synchronization mode is mode 0, 1, or 2, the constraints on using the same **flagId** for different inter-core synchronization modes are as follows:
    - On the same core, if the same **flagId** needs to be used for different inter-core synchronization modes, all synchronization operations of the previous mode must be completed before switching modes, that is, ensure that all **CrossCoreSetFlag** calls and their matching **CrossCoreWaitFlag** calls associated with this **flagId** have been fully executed.
    - For different cores, the same **flagId** can be directly used for different inter-core synchronization modes, specifically including the following two scenarios:
        - Across multiple AI Cores, use **flagId**=0 to synchronize all AICs (mode 0); within a single AI Core, use **flagId**=0 to synchronize all AIVs (mode 1).
        - Within a single AI Core, use flagId=0 to synchronize all AIVs (mode 1); within another AI Core, use flagId=0 to synchronize the AIC and all AIVs (mode 2).
- When using mode 0 of this API, it is recommended to enable batchmode so that the operator exclusively occupies all required core resources. Otherwise, a deadlock may occur if the following conditions are met:
    - Multi-stream concurrent scenario (≥2 execution streams).
    - ≥2 operators executing concurrently.
    - The total number of cores of all concurrent operators exceeds the number of physical cores.
    - ≥2 concurrent operators use the inter-core synchronization feature.

    Specifically, in a multi-stream scenario, although an inter-core synchronization operator of a certain stream is allocated n physical cores, only n-m cores may be scheduled and executed first, while the remaining m cores have not yet started because they are preempted by inter-core synchronization operators of other streams. The n-m cores that start first wait for the remaining m cores to complete when they reach the inter-core synchronization point, while the remaining m cores cannot be released because they are occupied by inter-core synchronization operators of other streams, resulting in a deadlock.
    In the Kernel direct invocation scenario, set batchmode through the [\_\_schedmode\_\_\(mode\)](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMT-BuiltIn_keyword.md) qualifier; in the engineering operator development scenario, set batchmode through the **SetScheduleMode** API of **TilingContext**. For details, see *Basic Data Structures and APIs*.

## Example<a name="section837496171220"></a>

**Table 4** Sample description

| SCENARIO_NUM Value | Business Scenario | Synchronization Mode Used |
| --- | --- | --- |
| 0 | Pure Vector computing scenario (16 AIVs) | mode0 (full-core synchronization of AIVs) |
| 1 | Pure Vector computing scenario (2 AIVs) | mode1 |
| 2 | Cube and Vector fusion computing scenario | mode2 (AIC waits for AIV), mode2 (AIV waits for AIC), mode0 (full-core synchronization of AICs) |

As shown in the table above, when SCENARIO_NUM takes different values, the specific usage of the three synchronization modes in the pure Vector computing scenario and the Cube and Vector fusion computing scenario is demonstrated respectively. The following shows part of the calling code for the pure Vector computing scenario:

```cpp
    AscendC::Muls(xLocal, xLocal, float(AscendC::GetBlockIdx()), this->blockLength);
    AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);

    // Enable atomic accumulation for UB-to-GM data movement.
    AscendC::SetAtomicAdd<float>();
    // DataCopy is a PIPE_MTE3 pipeline operation.
    AscendC::DataCopy(atomicResultGm, xLocal, this->blockLength);
    // After this AIV completes the preceding PIPE_MTE3 (DataCopy) pipeline operation, notify the other AIV cores that this AIV has completed.
    AscendC::CrossCoreSetFlag<0, PIPE_MTE3>(0);
    // Block this AIV from continuing to execute instructions until all other AIVs have completed the PIPE_MTE3 pipeline operation, then release the block and continue execution.
    AscendC::CrossCoreWaitFlag(0);
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

For the complete sample, see [CrossCoreSetFlag and CrossCoreWaitFlag inter-core synchronization sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/05_sync_control/cross_core_set_wait_flag).
