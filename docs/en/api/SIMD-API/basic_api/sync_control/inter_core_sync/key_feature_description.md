# Key Feature Description<a name="ZH-CN_TOPIC_0000002586300741"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T13:25:42.077Z -->

As shown in Figure 1, this chapter introduces, with the aid of timing diagrams, the principles of the four synchronization control modes supported when **CrossCoreSetFlag** and **CrossCoreWaitFlag** are used together.

- Mode 0: Synchronization control between AI Cores. For the AIC full-core scenario, all AIC cores are synchronized, and the instructions following **CrossCoreWaitFlag** are executed only after all AIC cores have executed **CrossCoreSetFlag**. For the AIV full-core scenario, all AIV cores are synchronized, and the instructions following **CrossCoreWaitFlag** are executed only after all AIV cores have executed **CrossCoreSetFlag**.
- Mode 1: Synchronization control between AIV cores within an AI Core. The instructions following **CrossCoreWaitFlag** are executed only after both AIV cores have executed **CrossCoreSetFlag**.
- Mode 2: Synchronization control between the AIC and the AIVs within an AI Core. After the AIC core executes **CrossCoreSetFlag**, the instructions following **CrossCoreWaitFlag** on the two AIVs continue to execute. After both AIVs execute **CrossCoreSetFlag**, the instructions following **CrossCoreWaitFlag** on the AIC can execute.
- Mode 4: Synchronization control between the AIC and a single AIV within an AI Core. AIV0 and AIV1 can independently trigger the AIC to wait.

**Figure 1**  Synchronization control mode diagram<a name="fig37581010773"></a>  
![](../../../../figures/3510_sync_control_mode_diagram.png "Synchronization control mode diagram")

The following synchronization features are described using this scenario configuration as an example: each AI Core contains one AIC and M AIVs, and N AI Cores are started in total, that is, there are N AICs and M \* N AIVs.

## Full-Core Synchronization of AIC or AIV Across Multiple AI Cores<a name="ZH-CN_TOPIC_0000002586180795"></a>

Full-core synchronization of AIC or AIV across multiple AI Cores requires the paired use of the mode-0 [CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md) and [CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md) APIs. This scenario can be divided into two categories:

- Full-core synchronization of N AICs across N AI Cores.

    When all N AICs have finished executing CrossCoreSetFlag, the counter corresponding to each AIC's flagId is incremented by 1. If the counter of that flagId on the AIC is non-zero, all instructions blocked by CrossCoreWaitFlag are allowed to proceed, and the counter of that flagId is decremented by 1.

    For each AIC, CrossCoreSetFlag and CrossCoreWaitFlag must be used in pairs.

- Full-core synchronization of M \* N AIVs across N AI Cores.

    When all M \* N AIVs have finished executing CrossCoreSetFlag, the counter corresponding to each AIV core's flagId is incremented by 1. If the counter of that flagId on the AIV is non-zero, all instructions blocked by CrossCoreWaitFlag are allowed to proceed, and the counter of that flagId is decremented by 1.

    For each AIV, CrossCoreSetFlag and CrossCoreWaitFlag must be used in pairs.

    ```cpp
    // Each core should have paired calls to CrossCoreSetFlag and CrossCoreWaitFlag similar to the following.
    // modeId must be configured as 0, pipe must be consistent, and flagId must be consistent.
    AscendC::CrossCoreSetFlag<0, PIPE_M>(0);       // After the preceding PIPE_M pipeline tasks are complete, notify the scheduling module.
    AscendC::CrossCoreWaitFlag(0);                 // Block until all AICs have completed the preceding PIPE_M pipeline and the scheduling module updates the counter of flagId=0, then release the block.
    ```

Using Figure 1 as an example, this demonstrates full-core synchronization of two AICs in two AI Cores when N=2.

After AIC 1 executes CrossCoreWaitFlag, the counter of flagId=0 in AIC 1 is 0, and all subsequent instructions are blocked until both AIC cores have executed CrossCoreSetFlag.

After AIC 2 completes CrossCoreSetFlag, the scheduling module detects that both AICs have executed CrossCoreSetFlag, so the counter value of flagId=0 in each AIC increases to 1. AIC 1 and AIC 2 detect that their respective flagId=0 counters have become 1, release the block, continue executing subsequent instructions, and decrement the counter value by 1.

**Figure 1**  Mode 0: Full-core synchronization of AICs across multiple AI Cores<a name="zh-cn_topic_0000002517901268_fig288716464319"></a>  
![](../../../../figures/inter_core_aic_all_sync.png "Full-core synchronization of AICs across multiple AI Cores")

## Full-Core Synchronization of AIVs Within a Single AI Core<a name="ZH-CN_TOPIC_0000002555621176"></a>

This scenario requires the use of the mode-1 [CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md) and [CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md) APIs together.

M AIVs within one AI Core perform full-core synchronization. When all M AIVs have finished executing CrossCoreSetFlag, the counter of each AIV's corresponding flagId is incremented by 1. If the counter of an AIV's flagId is non-zero, all instructions blocked by CrossCoreWaitFlag are allowed to proceed, and the counter of that flagId is decremented by 1. For each AIV, CrossCoreSetFlag and CrossCoreWaitFlag must be used in pairs.

>[!CAUTION]Note
>In this mode, AIV cores of different AI Cores do not affect each other's synchronization.

```cpp
// Each AIV should have paired calls to CrossCoreSetFlag and CrossCoreWaitFlag similar to the following.
// modeId must be set to 1, and pipe and flagId must be consistent.
AscendC::CrossCoreSetFlag<1, PIPE_MTE3>(0);       // After the preceding PIPE_MTE3 pipeline task completes, notify the scheduling module.
AscendC::CrossCoreWaitFlag(0);                 // Until all AIVs in this AI Core have completed the preceding PIPE_MTE3 pipeline and the scheduling module updates the counter of flagId=0, the blocking is released.
```

Taking Figure 2 as an example, it demonstrates full-core synchronization of two AIVs within one AI Core when M=2.

After AIV 1-1 executes CrossCoreWaitFlag, the counter of AIV 1-1's flagId=0 is 0, so all subsequent instructions are blocked until both AIVs have finished executing CrossCoreSetFlag.

After **CrossCoreSetFlag** of AIV 1-2 is executed, the scheduling module becomes aware that both AIVs have executed **CrossCoreSetFlag**, and therefore increments the counter value of **flagId**=0 for AIV 1-1 and AIV 1-2 to 1. AIV 1-1 and AIV 1-2 detect that their respective **flagId**=0 counters have changed to 1, both release the block, continue executing subsequent instructions, and decrement the counter value by 1.

**Figure 2** Mode 1: Full-core synchronization of AIVs in a single AI Core<a name="zh-cn_topic_0000002518061186_fig15383112914156"></a>  
![](../../../../figures/single_core_aiv_all_sync.png "Full-core synchronization of AIVs in a single AI Core")

## Full-Core Synchronization Between AIC and AIV in a Single AI Core<a name="ZH-CN_TOPIC_0000002555780808"></a>

Full-core synchronization between AIC and AIV in a single AI Core requires the paired use of the mode 2 [CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md) and [CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md) APIs. This scenario can be further divided into two cases:

- The AIC in one AI Core executes **CrossCoreSetFlag**, and the corresponding M AIVs all execute **CrossCoreWaitFlag**.

    When the AIC finishes executing **CrossCoreSetFlag**, the counter of the corresponding **flagId** on the M AIVs is incremented by 1. If the counter of this **flagId** on an AIV is non-zero, the AIV is unblocked, the instructions following **CrossCoreWaitFlag** continue to be issued, and the counter of this **flagId** is decremented by 1.

    In total, one **CrossCoreSetFlag** call by the AIC paired with one **CrossCoreWaitFlag** call by each of the M AIVs constitutes a matched pair.

- The M AIVs in one AI Core all execute **CrossCoreSetFlag**, and the corresponding AIC executes **CrossCoreWaitFlag**.

    When all M AIVs have finished executing **CrossCoreSetFlag**, the counter of the corresponding **flagId** on the AIC is incremented by 1. If the counter of this **flagId** on the AIC is non-zero, the AIC is unblocked, the instructions following **CrossCoreWaitFlag** continue to be issued, and the counter of this **flagId** is decremented by 1.

    In total, one **CrossCoreSetFlag** call by each of the M AIVs paired with one **CrossCoreWaitFlag** call by the AIC constitutes a matched pair.

```cpp
// Both AIC and AIV should have paired calls to CrossCoreSetFlag and CrossCoreWaitFlag similar to the following.
// modeId must be set to 2, pipe must be consistent, and flagId must be consistent.
if ASCEND_IS_AIV {
    AscendC::CrossCoreSetFlag<2, PIPE_MTE3>(0);       // After the preceding PIPE_MTE3 pipeline tasks are complete, notify the scheduler. All AIVs must execute this.
}
if ASCEND_IS_AIC {
    AscendC::CrossCoreWaitFlag(0);                    // Until all AIVs in this AI Core have completed the preceding PIPE_MTE3 pipeline and the scheduler updates the counter for flagId=0, the AIC remains blocked.
}
```

Using Figure 3 as an example, this demonstrates synchronization between AIVs and the AIC within a single AI Core when M=2 (the AIVs initiate CrossCoreSetFlag).

After AIC 1 executes CrossCoreWaitFlag, the counter for flagId=0 of AIC 1 is 0, and all subsequent instructions are blocked until both AIVs have executed CrossCoreSetFlag.

When the CrossCoreSetFlag of AIV 1-2 is executed after Vector instruction 1, all preceding PIPE\_V instructions are complete and CrossCoreSetFlag finishes execution. However, AIV 1-1 has not yet executed CrossCoreSetFlag. Therefore, the counter for flagId=0 of AIC 1 remains 0, and all instructions of the AIC are still blocked.

After both Vector instructions 1 and 2 of AIV 1-1 are complete, all preceding PIPE\_V instructions are complete and CrossCoreSetFlag takes effect. At this point, the scheduler detects that both AIVs have executed CrossCoreSetFlag, so it increments the counter for flagId=0 of AIC 1 to 1. When AIC 1 detects that the counter for flagId=0 has become 1, AIC 1 is unblocked and continues to execute the subsequent instruction, Cube instruction 2, and decrements the counter by 1.

**Figure 3**  Mode 2: Full-core synchronization between the AIC and AIVs within a single AI Core (AIVs perform CrossCoreSetFlag)<a name="zh-cn_topic_0000002549581037_fig164171141103813"></a>  
![](../../../../figures/single_core_aic_aiv_sync_aiv_setflag.png "Full-core synchronization between the AIC and AIVs within a single AI Core (AIVs perform CrossCoreSetFlag)")

## Synchronization Between AIC and a Single AIV in a Single AI Core<a name="ZH-CN_TOPIC_0000002586300742"></a>

Synchronization between the AIC and a single AIV in a single AI Core requires the paired use of the [CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md) and [CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md) APIs in mode 4. This scenario can be divided into two cases:

- The AIC in one AI Core executes CrossCoreSetFlag, and the corresponding AIV executes CrossCoreWaitFlag.

    When the AIC finishes executing CrossCoreSetFlag, the counter of the corresponding flagId on the AIV increases by 1. If the counter of this flagId on the AIV is non-zero, the AIV is unblocked, the instructions following CrossCoreWaitFlag continue to be issued, and the counter of this flagId decreases by 1.

    In total, one CrossCoreSetFlag call by the AIC paired with one CrossCoreWaitFlag completion by a single AIV constitutes a matched pair.

- One AIV in one AI Core executes CrossCoreSetFlag, and the corresponding AIC executes CrossCoreWaitFlag.

    When the AIV finishes executing CrossCoreSetFlag, the counter of the corresponding flagId on the AIC increases by 1. If the counter of this flagId on the AIC is non-zero, the AIC is unblocked, the instructions following CrossCoreWaitFlag continue to be issued, and the counter of this flagId decreases by 1.

    In total, one CrossCoreSetFlag call by a single AIV paired with one CrossCoreWaitFlag call by the corresponding AIC constitutes a matched pair.

```cpp
// Both the AIC and the AIV should have paired calls to CrossCoreSetFlag and CrossCoreWaitFlag similar to the following.
// modeId must be configured to 4, and flagId must be consistent.
if ASCEND_IS_AIV { 
    if (blockIdx % 2 == 0) { 
        AscendC::CrossCoreSetFlag<4, PIPE_MTE3>(0);   // After the preceding PIPE_MTE3 pipeline tasks on AIV0 are complete, notify the scheduler. For flagId, 0-15 on AIV0 correspond to 0-15 on AIC.
    } 
    if (blockIdx % 2 == 1) { 
        AscendC::CrossCoreSetFlag<4, PIPE_MTE3>(0);   // After the preceding PIPE_MTE3 pipeline tasks on AIV1 are complete, notify the scheduler. For flagId, 0-15 on AIV1 correspond to 16-31 on AIC.
    } 
} 
if ASCEND_IS_AIC { 
    AscendC::CrossCoreWaitFlag<4, PIPE_FIX>(0);        // Block subsequent PIPE_FIX pipeline instructions until all AIV0 in this AI Core have completed the preceding PIPE_MTE3 pipeline and the scheduler updates the counter for flagId=0, after which AIC is unblocked.
    ... 
    AscendC::CrossCoreWaitFlag<4, PIPE_FIX>(16);       // Block subsequent PIPE_FIX pipeline instructions until all AIV1 in this AI Core have completed the preceding PIPE_MTE3 pipeline and the scheduler updates the counter for flagId=16, after which AIC is unblocked.
    ... 
}
```

Using Figure 4 as an example, this demonstrates the synchronization between AIV1 and AIC within a single AI Core when M=2 (AIV1-2 initiates CrossCoreSetFlag).

After AIC 1 executes CrossCoreWaitFlag, the counter for flagId=0 on AIC 1 is 0, and all subsequent instructions are blocked until one AIV completes CrossCoreSetFlag.

- AIV 1-1 does not need to execute CrossCoreSetFlag.
- After all PIPE_MTE3 instructions of AIV 1-2 are complete, CrossCoreSetFlag takes effect. At this point, the scheduling module detects that one AIV has completed CrossCoreSetFlag, and therefore increments the counter for flagId=0 on AIC 1 to 1. When AIC 1 detects that the counter for the corresponding flagId=0 has become 1, AIC 1 is unblocked, continues executing subsequent PIPE_FIX instructions, and decrements the counter by 1.

**Figure 4** Mode 4: Synchronization between AIC and a single AIV within a single AI Core (AIV performs CrossCoreSetFlag)<a name="zh-cn_topic_0000002549581037_fig164171141103814"></a>  
![](../../../../figures/single_ai_core_aic_single_aiv_sync.png "Synchronization between AIC and a single AIV within a single AI Core (AIV performs CrossCoreSetFlag)")
