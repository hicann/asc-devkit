# Inter-core Synchronization Overview<a name="ZH-CN_TOPIC_0000002555803758"></a>

<!-- md-trans-meta sourceCommit=8ea00a2b723c5d344a12f874846135651b6b80c3 translatedAt=2026-11-05T05:07:08.979Z pushedAt=2026-11-18T01:25:31.147Z nl=2 -->

Inter-core synchronization is typically used in the following scenario: a core that depends on data computed by other cores must wait until all the related cores finish their computation before it can obtain the data.  

Take Figure 1 as an example. AIC depends on the ReduceSum result computed by AIV. Because the overall vector is large, it must be split into multiple parts, with each AIV completing part of the computation. Each AIV writes its partial result to GM through atomic accumulation. What AIC needs to read must be the final result after all AIVs have completed the accumulation. Therefore, inter-core synchronization is required to ensure the timing correctness when AIC reads the ReduceSum result.

**Figure 1** Example of an inter-core synchronization service scenario (AIC:AIV = 1:2)<a name="zh-cn_topic_0000002542605351_fig1180684585013"></a>  
![](../../../../figures/inter_core_sync_scenario_example.png "Example of an inter-core synchronization service scenario (AIC-AIV = 1-2)")

The AIC/AIV multi-core structure is shown in Figure 2. AICs/AIVs are divided into groups. A group contains one block and N subblocks (N≥1), where the block represents the "primary core" and each subblock represents a "secondary core".

Operators can be classified into three types by computation characteristics: Cube operators (matrix computation), Vector operators (vector computation), and Mix operators (containing both matrix and vector computation). As shown in Table 1, the operator type determines the required inter-core synchronization method and the selectable group configuration mode. Among them, Mix operators specify the block/subblock relationship between AIC and AIV through the function execution space qualifier; Cube operators and Vector operators are executed by a single type of core and do not involve block/subblock division. The configuration where AIV is the block and AIC is the subblock (that is, the mode where the Vector core is primary and the Cube core is secondary) is currently not supported. Table 2 summarizes the inter-core synchronization APIs and the synchronization scenarios they support.

**Figure 2** Relationship between block and subblock (the gray part represents a group, that is, one block and N subblocks)<a name="zh-cn_topic_0000002542605351_fig52727114018"></a>
![](../../../../figures/block_subblock_relationship_3510.png "(NPU architecture version 3510) Relationship between block and subblock (the gray part represents a group, that is, one block and N subblocks)")

**Table 1** Group configuration<a name="zh-cn_topic_0000002542605351_table17238184964217"></a>

| Operator Type | Function Execution Space Qualifier | Kernel Type | block/subblock | 1:N |
| --- | --- | --- | --- | --- |
| Cube operator | \_\_cube\_\_ | KERNEL_TYPE_AIC_ONLY | AIC is the block, with no subblock | Not involved |
| Vector operator | \_\_vector\_\_ | KERNEL_TYPE_AIV_ONLY | AIV is the block, with no subblock | Not involved |
| Mix operator | \_\_mix\_\_(1, 0) | KERNEL_TYPE_MIX_AIC_1_0 | AIC is the block, with no subblock | Not involved |
| Mix operator | \_\_mix\_\_(0, 1) | KERNEL_TYPE_MIX_AIV_1_0 | AIV is the block, with no subblock | Not involved |
| Mix operator | \_\_mix\_\_(1, 1) | KERNEL_TYPE_MIX_AIC_1_1 | AIC is the block, AIV is the subblock | 1:1 |
| Mix operator | \_\_mix\_\_(1, 2) | KERNEL_TYPE_MIX_AIC_1_2 | AIC is the block, AIV is the subblock | 1:2 |

**Table 2** Supported inter-core synchronization scenarios

| API name | Inter-core Synchronization Scenario Overview | Synchronization Category |
| --- | --- | --- |
| [CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md)/[CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md) | Synchronization control among multiple AICs or AIVs. CrossCoreSetFlag sends a notification, and CrossCoreWaitFlag blocks until CrossCoreSetFlag sends the notification.<br>Supports the following synchronization scenarios:<br>&bull; (Mode 0) Synchronously wait for all AICs or all AIVs to finish execution.<br>&bull; (Mode 1) Synchronization among all AIVs within a single AI Core.<br>&bull; (Mode 2) Synchronization between the AIC and all AIVs within a single AI Core.<br>&bull; (Mode 4) Synchronization between the AIC core and a single AIV within a single AI Core. | Hardware Synchronization |
| [IBSet](IBSet.md)/[IBWait](IBWait.md) | IBSet and IBWait are used together for synchronization between a pair of AIVs: the waiting core calls IBWait to block the execution of its subsequent instructions, and the waited core calls IBSet to notify the waiting core to unblock. The principle of the preceding synchronization process is as follows: the waited core calls IBSet to notify the waiting core by writing the value 1 to a block of addresses in global memory; the waiting core calls IBWait to continuously read the value of the corresponding address block in global memory until the value becomes 1. | Software Synchronization |
| [SyncAll](SyncAll.md) | Used for synchronization control among multiple cores. Supports the following synchronization scenarios:<br>&bull; Full-core synchronization of pure Vector operators. For details, see [Mode 0 of CrossCoreSetFlag](key_features.md).<br>&bull; Full-core synchronization of fusion operators, which implements three inter-core synchronization capabilities at once (see [Key Feature Description of CrossCoreSetFlag](key_features.md)): first, synchronization between the AIC and all AIVs within a single AI Core (AIC waits for AIVs); then, synchronization of all AICs among multiple AI Cores; finally, synchronization between all AIVs and the AIC within a single AI Core (AIVs wait for the AIC). | Hardware Synchronization |
| [SyncAll](SyncAll.md) | Supports only multi-core synchronization of pure Vector operators (the number of cores participating in synchronization can be specified through an input parameter). Each AIV writes the signal value 1 to the corresponding location in global memory, and then each AIV continuously polls the signal values corresponding to all AIVs in global memory until the signal values corresponding to all AIV cores become 1. | Software Synchronization |
| [InitDetermineComputeWorkspace](InitDetermineComputeWorkspace.md)/[NotifyNextBlock](NotifyNextBlock.md)/[WaitPreBlock](WaitPreBlock.md) | &bull; InitDetermineComputeWorkspace, NotifyNextBlock, and WaitPreBlock must be used in combination. They ensure that multiple AIV cores execute strictly in ascending order of blockIdx, which is suitable for scenarios requiring deterministic computation.<br>&bull; InitDetermineComputeWorkspace initializes the value of the GM shared memory. Only after initialization is complete can NotifyNextBlock and WaitPreBlock be called.<br>&bull; NotifyNextBlock notifies other cores by writing to global memory that the current core has finished executing and that other cores can continue; WaitPreBlock determines whether the current core can continue by reading global memory. | Software Synchronization |
