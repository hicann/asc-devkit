# CrossCoreWaitFlag(ISASI)<a name="ZH-CN_TOPIC_0000001787470374"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-09-01T06:48:16.501Z -->

## Product Support<a name="section1550532418810"></a>

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
- Atlas inference products: Not Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas Inference Series AI Core: Not Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas Inference Series Vector Core: Not Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas Training Series: Not Supported
<!-- end id7 -->

## Function Description<a name="section618mcpsimp"></a>

The header file path is: `"basic_api/kernel_operator_block_sync_intf.h"`.

In the inter-core synchronization scenario, the CrossCoreSetFlag API and the CrossCoreWaitFlag API work in pairs. For details about the specific functions, see [CrossCoreSetFlag Function Description](CrossCoreSetFlag_ISASI.md#section618mcpsimp).

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
template <uint8_t modeId = 0, pipe_t pipe = PIPE_S>
__aicore__ inline void CrossCoreWaitFlag(uint16_t flagId)
```

## Parameter Description<a name="section622mcpsimp"></a>

### Template Parameter and Input Parameter Description

**Table 1**  Template Parameter Description

| Parameter Name | Description |
| --- | --- |
| modeId | The mode of inter-core synchronization. For the support of synchronization modes on different products, see [modeId Supported Value Description](#modeid-supported-value-description).<br>For the Kernel types supported by each mode, see [Table 3](#table3). |
| pipe | Sets the pipeline type where this instruction resides. The supported pipeline types are PIPE_V, PIPE_M, PIPE_MTE1, PIPE_MTE2, PIPE_MTE3, and PIPE_FIX. PIPE_S and PIPE_ALL are not supported. For the support of pipeline types on different products, see [pipe Supported Pipeline Type Description](#supported-pipeline-type-description-for-pipe). |

The effectiveness of template parameters modeId and pipe on different products is as follows:
<!-- npu="950" id8 -->
- On Ascend 950PR/Ascend 950DT, the hardware supports configuring the inter-core synchronization mode and pipeline type, and template parameters modeId and pipe are **effective**. At this time, CrossCoreWaitFlag blocks the subsequent instructions of the **specified pipeline**.
<!-- end id8 -->
<!-- npu="A3,910b" id9 -->
- For the following products, the hardware does not support configuring the inter-core synchronization mode and pipeline type, and template parameters modeId and pipe are **not effective**. At this time, CrossCoreWaitFlag blocks the subsequent instructions of **all pipelines**.
    <!-- npu="A3" id10 -->
    - Atlas A3 training products/Atlas A3 inference products
    <!-- end id10 -->
    <!-- npu="910b" id11 -->
    - Atlas A2 training products/Atlas A2 inference products
    <!-- end id11 -->
<!-- end id9 -->

**Table 2**  Parameter Description

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| flagId | Input | The flag of inter-core synchronization. For the value range description of flagId on different products, see [flagId Value Range Description](#flagid-value-range-description). |

### modeId Supported Value Description<a name="modeId-Supported-Value-Description"></a>

The support for synchronization modes on different products is as follows:
<!-- npu="950" id12 -->
- For Ascend 950PR/Ascend 950DT, the synchronization modes used are: Mode 0, Mode 1, Mode 2, Mode 4.
<!-- end id12 -->
<!-- npu="A3,910b" id13 -->
- For the following products, the synchronization modes used are: Mode 0, Mode 1, Mode 2.
    <!-- npu="A3" id14 -->
    - Atlas A3 training products/Atlas A3 inference products
    <!-- end id14 -->
    <!-- npu="910b" id15 -->
    - Atlas A2 training products/Atlas A2 inference products
    <!-- end id15 -->
<!-- end id13 -->

### Supported Pipeline Type Description for pipe<a name="Supported Pipeline Type Description for pipe"></a>

- When the inter-core synchronization mode is mode 0, 1, or 2,
    - The supported pipeline types are PIPE_V, PIPE_M, PIPE_MTE1, PIPE_MTE2, PIPE_MTE3, and PIPE_FIX.
    - The not supported pipeline types are PIPE_ALL and PIPE_S.
<!-- npu="950" id16 -->
- For Ascend 950PR/Ascend 950DT, when the inter-core synchronization mode is mode 4,
    - The supported pipeline types are PIPE_V, PIPE_M, PIPE_MTE1, PIPE_MTE2, PIPE_MTE3, PIPE_FIX, and PIPE_S.
    - The not supported pipeline type is PIPE_ALL.
<!-- end id16 -->

### flagId Value Range Description<a name="flagId-value-range-description"></a>

- When the inter-core synchronization mode is mode 0, 1, or 2, the supported value range is 0-15.
<!-- npu="950" id17 -->
- For Ascend 950PR/Ascend 950DT, when the inter-core synchronization mode is mode 4, the supported value range is as follows:
    - The CrossCoreSetFlag operation with flagId 0-10 initiated by AIV0 corresponds to the operation with flagId 0-10 in AIC CrossCoreWaitFlag.
    - The CrossCoreSetFlag operation with flagId 0-10 initiated by AIV1 corresponds to the operation with flagId 16-26 in AIC CrossCoreWaitFlag.
    - The CrossCoreSetFlag operation with flagId 0-10 initiated by AIC corresponds to the operation with flagId 0-10 in AIV0 CrossCoreWaitFlag.
    - The CrossCoreSetFlag operation with flagId 16-26 initiated by AIC corresponds to the operation with flagId 0-10 in AIV1 CrossCoreWaitFlag.
<!-- end id17 -->

## Return Value Description<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- Because when the Kernel type is KERNEL_TYPE_AIC_ONLY or KERNEL_TYPE_AIV_ONLY, the hardware does not enable the scheduling module, and thus inter-core synchronization cannot be performed normally, the configurations of [Kernel Type](../../Kernel-Tiling/Setting Kernel Type.md) or [Function Modifier](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md#section1074418132518) for different synchronization modes are as follows:
    - In pure Vector/Cube scenarios (Mode 0 or Mode 1), it is recommended to set the Kernel type to KERNEL\_TYPE\_MIX\_AIV\_1\_0 or KERNEL\_TYPE\_MIX\_AIC\_1\_0. For other supported Kernel types, see Table 3.
    - For mixed Vector and Cube scenarios (Mode 2 and Mode 4), the Kernel type must be flexibly configured based on the ratio of AIC to AIV in the AI Core. For the function modifiers and Kernel types supported by different modes, see Table 3.

        **Table 3**  Mode and Supported Kernel Type Configuration<a name="table3"></a>

        | Mode | Supported Function Modifier | Supported Kernel Type Configuration |
        | --- | --- | --- |
        | 0 | \_\_mix\_\_(0, 1), \_\_mix\_\_(1, 0), \_\_mix\_\_(1, 1), \_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIV\_1\_0, KERNEL\_TYPE\_MIX\_AIC\_1\_0, KERNEL\_TYPE\_MIX\_AIC\_1\_1, KERNEL\_TYPE\_MIX\_AIC\_1\_2 |
        | 1 | \_\_mix\_\_(1, 1), \_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIC\_1\_1, KERNEL\_TYPE\_MIX\_AIC\_1\_2 |
        | 2 | \_\_mix\_\_(1, 1), \_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIC\_1\_1, KERNEL\_TYPE\_MIX\_AIC\_1\_2 |
        | 4 | \_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIC\_1\_2 |

- CrossCoreWaitFlag must be used together with the [CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md) API to prevent the compute core from remaining in the blocking phase.

- When the API uses Modes 0, 1, and 2, flagId usage conflicts must be avoided:
    - The internal implementation of the Matmul high-level API uses CrossCoreSetFlag for inter-core synchronization control. Therefore, developers are not advised to use CrossCoreSetFlag and the Matmul high-level API at the same time; otherwise, there is a risk of flagId conflicts. The flagId range occupied internally by the Matmul high-level API is related to the number of defined Matmul objects. Assuming N Matmul objects are defined, the flagId range occupied internally by the Matmul high-level API is \[0, 2 \* N - 1\]. Matmul supports defining up to 4 objects. At this time, the flagId occupation range is \[0, 7\].

    - The internal implementation of the SyncAll hardware synchronization API uses CrossCoreSetFlag for inter-core synchronization control. Therefore, developers are not advised to use CrossCoreSetFlag and the SyncAll hardware synchronization API at the same time; otherwise, there is a risk of flagId conflicts. The flagId occupation range of the SyncAll hardware synchronization API is \[11-14\].

- Constraints related to flagId:
    - For Modes 0, 1, and 2, each AIC and each AIV has 16 flagIds respectively, with a supported value range of 0-15. If the value of flagId exceeds this range, the lowest 4 bits are taken as the effective value.
    - Each flagId corresponds to a counter. When [CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md) is called, if the counter value is 0, the issuance of subsequent instructions is blocked, while already-issued instructions can execute normally. When the scheduling module detects that the inter-core synchronization (CrossCoreSetFlag) is fully completed, it increments the counter corresponding to CrossCoreWaitFlag by 1. At this time, the counter value is non-zero, the blocking is released, and the corresponding counter value is decremented by 1 to restore it. For specific execution logic and details, see [Key Feature Description](key_feature_description.md#ZH-CN_TOPIC_0000002586300741). The counting range of the counter corresponding to flagId is 0-15. If the counter value exceeds this range, an error is reported and the process is interrupted.
    <!-- npu="950" id18 -->
    - For Ascend 950PR/Ascend 950DT, when the inter-core synchronization mode is Mode 4, each AIC has 32 flagIds with a supported value range of 0-31; each AIV has 16 flagIds with a supported value range of 0-15. Among them, AIC flagId 0-15 corresponds to AIV0 flagId 0-15, and AIC flagId 16-31 corresponds to AIV1 flagId 0-15.
    - For Ascend 950PR/Ascend 950DT, when the inter-core synchronization mode is Mode 4, the flagId of Mode 4 is independent of Modes 0, 1, and 2 and does not affect each other.
    <!-- end id18 -->
- Constraints on using the same flagId for different inter-core synchronization modes under Modes 0, 1, and 2:
    - On the same core, if the same flagId needs to be used for different inter-core synchronization modes, all synchronization operations of the previous mode must be completed before the mode switch - that is, ensure that all CrossCoreSetFlag calls associated with this flagId and their matching CrossCoreWaitFlag calls have been fully executed.
    - For different cores, the same flagId can be directly used for different inter-core synchronization modes, specifically including the following 2 scenarios:
        - Among multiple AI Cores, use flagId=0 to synchronize all AICs (Mode 0); within a single AI Core, use flagId=0 to synchronize all AIVs (Mode 1).
        - Within a single AI Core, use flagId=0 to synchronize all AIVs (Mode 1); within another AI Core, use flagId=0 to synchronize the AIC and all AIVs (Mode 2).
- When using mode 0 of this API, it is recommended to enable batchmode so that the operator exclusively occupies all required core resources. Otherwise, a deadlock may occur if the following conditions are met:
    - Multi-stream concurrency scenario (≥2 execution streams).
    - ≥2 operators are executed concurrently.
    - The total number of cores of all concurrent operators exceeds the number of physical cores.
    - ≥2 concurrent operators use the inter-core synchronization function.

    Specifically, in a multi-stream scenario, although the inter-core synchronization operator of a stream is allocated to n physical cores, only n-m cores may be scheduled and executed first, while the remaining m cores have not yet started because they are preempted by inter-core synchronization operators of other streams. The n-m cores that start first wait for the remaining m cores to complete when executing inter-core synchronization, while the remaining m cores cannot be released because they are occupied by inter-core synchronization operators of other streams, resulting in a deadlock.

    In the Kernel direct invocation scenario, the batchmode mode is set through the [\_\_schedmode\_\_\(mode\)](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md) qualifier; in the engineering operator development scenario, the batchmode mode is set through the SetScheduleMode API of TilingContext. For details, see *Basic Data Structures and APIs*.

## Calling Example<a name="section837496171220"></a>

**Table 4** Sample description

| SCENARIO_NUM Value | Business Scenario | Synchronization Mode Used |
| --- | --- | --- |
| 0 | Pure Vector computation scenario (16 AIVs) | mode0 (AIV all-core synchronization) |
| 1 | Pure Vector computation scenario (2 AIVs) | mode1 |
| 2 | Cube and Vector fused computation scenario | mode2 (AIC waits for AIV), mode2 (AIV waits for AIC), mode0 (AIC all-core synchronization) |

As shown in the table above, when SCENARIO_NUM takes different values, the specific usage of the three synchronization modes in the pure Vector computation scenario and the Cube and Vector fused computation scenario is demonstrated respectively. The following shows part of the calling code for the pure Vector computation scenario:

```cpp
    AscendC::Muls(xLocal, xLocal, float(AscendC::GetBlockIdx()), this->blockLength);
    AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);

    // Enable atomic accumulation for UB-to-GM copy.
    AscendC::SetAtomicAdd<float>();
    // DataCopy is a PIPE_MTE3 pipeline operation.
    AscendC::DataCopy(atomicResultGm, xLocal, this->blockLength);
    // After this AIV completes the preceding PIPE_MTE3 (DataCopy) pipeline operation, notify other AIV cores that this AIV has completed.
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

For the complete sample, see [CrossCoreSetFlag and CrossCoreWaitFlag Inter-core Synchronization Sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/05_sync_control/cross_core_set_wait_flag).
