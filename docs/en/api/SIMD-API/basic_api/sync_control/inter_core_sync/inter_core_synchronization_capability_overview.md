# Inter-Core Synchronization Overview<a name="ZH-CN_TOPIC_0000002555803758"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T13:29:51.438Z -->

Inter-core synchronization is typically used in the following scenario: the data that a core depends on for computation can be obtained only after all related cores have completed their computation.

Take Figure 1 as an example. AIC depends on the ReduceSum result computed by AIV. Because the overall vector is large, it must be split into multiple parts, with each AIV completing part of the computation. Each AIV writes its partial result to GM through atomic accumulation. What AIC needs to read is the final result after all AIVs have completed the accumulation. Therefore, inter-core synchronization is required to ensure the timing correctness when AIC reads the ReduceSum result.

**Figure 1** Example of an inter-core synchronization service scenario (AIC:AIV = 1:2)<a name="zh-cn_topic_0000002542605351_fig1180684585013"></a>  
![](../../../../figures/inter_core_sync_scenario_example.png "Example of an inter-core synchronization service scenario (AIC-AIV = 1-2)")

The AIC/AIV multi-core structure is shown in Figure 2. AICs and AIVs are divided into groups. Within a group, blocks and subblocks are further divided, with a block-to-subblock ratio of 1:N (N>=1). A block indicates how many "primary cores" there are, and a subblock indicates how many "secondary cores" each "primary core" carries.

Operators can be classified into three types based on their computation characteristics: Cube operators (cube computation), Vector operators (vector computation), and Mix operators (including both cube and vector computation). As shown in Table 1, the operator type determines the required inter-core synchronization method and the selectable group configuration modes. For Mix operators, the block/subblock relationship between AIC and AIV is specified through function modifiers. Cube operators and Vector operators are executed by a single type of core and do not involve block/subblock division. The configuration in which AIV is the block and AIC is the subblock (that is, the mode where the Vector core is primary and the Cube core is secondary) is currently not supported. Table 2 summarizes the inter-core synchronization APIs and the synchronization scenarios they support.

**Figure 2** Relationship between blocks and subblocks (the gray part indicates one group, that is, one block and N subblocks)<a name="zh-cn_topic_0000002542605351_fig52727114018"></a>
![](../../../../figures/block_subblock_relationship_3510.png "(NPU architecture version 3510) Relationship between blocks and subblocks (the gray part indicates one group, that is, one block and N subblocks)")

**Table 1** Group configuration<a name="zh-cn_topic_0000002542605351_table17238184964217"></a>

| Operator Type | Function Modifier | Kernel Type | block/subblock | 1:N |
| --- | --- | --- | --- | --- |
| Cube operator | \_\_cube\_\_ | KERNEL_TYPE_AIC_ONLY | AIC is the block, with no subblock | Not applicable |
| Vector operator | \_\_vector\_\_ | KERNEL_TYPE_AIV_ONLY | AIV is the block, with no subblock | Not applicable |
| Mix operator | \_\_mix\_\_(1, 0) | KERNEL_TYPE_MIX_AIC_1_0 | AIC is the block, with no subblock | Not applicable |
| Mix operator | \_\_mix\_\_(0, 1) | KERNEL_TYPE_MIX_AIV_1_0 | AIV is the block, with no subblock | Not applicable |
| Mix operator | \_\_mix\_\_(1, 1) | KERNEL_TYPE_MIX_AIC_1_1 | AIC is the block, AIV is the subblock | 1:1 |
| Mix operator | \_\_mix\_\_(1, 2) | KERNEL_TYPE_MIX_AIC_1_2 | AIC is the block, AIV is the subblock | 1:2 |

**Table 2** Supported inter-core synchronization scenarios

| API Name | Overview of Inter-core Synchronization Scenarios | Synchronization Category |
| --- | --- | --- |
| [CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md)/[CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md) | Synchronization control among multiple AICs or AIVs. CrossCoreSetFlag sends notifications, and CrossCoreWaitFlag blocks until CrossCoreSetFlag sends a notification.<br>Supports the following synchronization scenarios:<br>&bull; (Mode 0) Synchronously wait for all AICs or all AIVs to finish execution.<br>&bull; (Mode 1) Synchronization among all AIVs within a single AI Core.<br>&bull; (Mode 2) Synchronization between the AIC and all AIVs within a single AI Core.<br>&bull; (Mode 4) Synchronization between the AIC core and a single AIV within a single AI Core. | Hardware synchronization |
| [IBSet](IBSet.md)/[IBWait](IBWait.md) | IBSet and IBWait are used together for synchronization between a pair of AIVs: the waiting core calls IBWait to block the execution of its subsequent instructions, and the waited core calls IBSet to notify the waiting core to unblock. The principle of the preceding synchronization process is as follows: the waited core calls IBSet to notify the waiting core by writing the value 1 to a block of addresses in global memory; the waiting core calls IBWait to continuously read the value of the corresponding address block in global memory until the value becomes 1. | Software synchronization |
| [SyncAll](SyncAll.md) | Used for synchronization control among multiple cores. Supports the following synchronization scenarios:<br>&bull; Full-core synchronization of pure Vector operators. For details, see [Mode 0 of CrossCoreSetFlag](key_feature_description.md).<br>&bull; Full-core synchronization of fused operators, which implements three inter-core synchronization capabilities at once (see [Key Feature Description of CrossCoreSetFlag](key_feature_description.md)): first, synchronization between the AIC and all AIVs within a single AI Core (the AIC waits for the AIVs); then, synchronization among all AICs across multiple AI Cores; finally, synchronization between all AIVs and the AIC within a single AI Core (the AIVs wait for the AIC). | Hardware synchronization |
| [SyncAll](SyncAll.md) | Supports only multi-core synchronization of pure Vector operators (the number of cores participating in synchronization can be specified through an input parameter). Each AIV writes the signal value 1 to the corresponding location in global memory, and then each AIV continuously polls the signal values corresponding to all AIVs in global memory until the signal values corresponding to all AIV cores become 1. | Software synchronization |
| [InitDetermineComputeWorkspace](InitDetermineComputeWorkspace.md)/[NotifyNextBlock](NotifyNextBlock.md)/[WaitPreBlock](WaitPreBlock.md) | &bull; InitDetermineComputeWorkspace, NotifyNextBlock, and WaitPreBlock must be used in combination. They ensure that multiple AIV cores execute strictly in ascending order of blockIdx, which is suitable for scenarios requiring deterministic computation.<br>&bull; InitDetermineComputeWorkspace initializes the value of the GM shared memory. Only after initialization is complete can NotifyNextBlock and WaitPreBlock be called.<br>&bull; NotifyNextBlock notifies other cores that the current core has finished execution by writing to global memory, so that other cores can continue execution; WaitPreBlock determines whether the current core can continue execution by reading global memory. | Software synchronization |
