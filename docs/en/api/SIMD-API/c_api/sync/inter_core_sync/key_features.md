---
okftype: reference.concept
description: "Key features of inter-core synchronization: describes the counter increment/decrement and blocking release principles for AI Core-to-AI Core, AI Vector-to-AI Vector, AI Cube-to-AI Vector, and AI Cube-to-single AI Vector synchronization in modes 0, 1, 2, and 4 respectively, provides the arrive and wait pairing constraints, and explains the effective scope of the pipe parameter on the 2201 and 3510 architectures and the architectural limitations of mode 4."
---
# Key Feature Description<a id="inter_core_sync_key_features"></a>

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.526296+00:00 pushedAt=2026-11-16T02:10:57.743Z nl=2 -->

As shown in Figure 1, this section uses timing diagrams to introduce the principles of the four synchronization control modes supported by the four pairs of inter-core synchronization APIs.

- Mode 0: Synchronization control among AI Cores. For the all-AIC scenario, all AIC cores are synchronized. Instructions in all subsequent pipelines of `asc_sync_inter_wait` or in the pipeline specified by the `pipe` parameter (depending on the NPU architecture) are executed only after all AIC cores have executed `asc_sync_inter_arrive`. For the all-AIV scenario, all AIV cores are synchronized. Instructions in all subsequent pipelines of `asc_sync_inter_wait` or in the pipeline specified by the `pipe` parameter (depending on the NPU architecture) are executed only after all AIV cores have executed `asc_sync_inter_arrive`.
- Mode 1: Synchronization control among AIV cores within an AI Core. Only after both AIV cores have executed `asc_sync_subblock_arrive` are instructions in all subsequent pipelines of `asc_sync_subblock_wait` or in the pipeline specified by the `pipe` parameter (depending on the NPU architecture) executed.
- Mode 2: Synchronization control between the AIC and AIVs within an AI Core. Only after the AIC core executes `asc_sync_block_arrive` can instructions in all subsequent pipelines of `asc_sync_block_wait` on the two AIVs or in the pipeline specified by the `pipe` parameter (depending on the NPU architecture) continue to execute. Only after both AIVs execute `asc_sync_block_arrive` can instructions in all subsequent pipelines of `asc_sync_block_wait` on the AIC or in the pipeline specified by the `pipe` parameter (depending on the NPU architecture) execute.
- Mode 4: Synchronization control between the AIC and a single AIV within an AI Core. Only after the AIC executes `asc_sync_intra_arrive` can instructions following `asc_sync_intra_wait` in the pipeline specified by `pipe` on the single AIV continue to execute. Only after the single AIV executes `asc_sync_intra_arrive` can instructions following `asc_sync_intra_wait` in the pipeline specified by `pipe` on the AIC execute. This mode is supported only on [NPU Architecture Version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md).

> [!NOTE]
> The pipeline types blocked by the `asc_sync_inter_wait`, `asc_sync_subblock_wait`, and `asc_sync_block_wait` APIs vary by NPU architecture:
> - On [NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md), the `pipe` parameter passed to the preceding APIs is invalid. Regardless of its value, subsequent instructions in all pipelines are blocked.
> - On [NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md), the `pipe` parameter passed to the preceding APIs takes effect and blocks subsequent instructions in the pipeline specified by the `pipe` parameter.

**Figure 1** Synchronization control mode diagram<a id="sync_control_mode_diagram"></a>    
![](../../../../figures/3510_sync_control_mode_diagram.png "Synchronization control mode diagram")

The following synchronization features use the following scenario configuration as an example: the kernel function (Kernel) is decorated with `__mix__(1, 2)`, that is, each AI Core contains 1 AIC and 2 AIVs, and the logical core count `numBlocks=2` is set, that is, 2 AI Cores are started in total, so there are 2 AICs and 4 AIVs. For ease of description, the 2 AICs are numbered AIC0 and AIC1 respectively; the 2 AIVs in AI Core0 are numbered AIV0-0 and AIV0-1 respectively, and the 2 AIVs in AI Core1 are numbered AIV1-0 and AIV1-1 respectively. **The initial value of the counter corresponding to `flag_id` or `sync_id` in each core is 0.**

Mode 0, mode 1, and mode 2 are supported on both NPU architecture versions 2201 and 3510. The synchronization flows in the following sections apply to both architectures. The architectural differences are only reflected in whether the `pipe` parameter passed to the `wait` API takes effect and the scope of the blocked pipeline. For details, see the description in each section.

<!-- npu="950" id1 -->

> [!NOTE]
> The following description of mode 0 is valid only for [NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md). When calling the related APIs, you must explicitly pass the `pipe` parameter. The `pipe` parameter passed to both the `arrive` and `wait` APIs takes effect. The `wait` API blocks subsequent instructions of the pipeline specified by the `pipe` parameter.

## Full-Core Synchronization of AICs or AIVs Across Multiple AI Cores (Mode 0)<a id="multi_ai_core_aic_aiv_full_sync"></a>

Full-core synchronization of AICs or AIVs across multiple AI Cores requires the use of the mode 0 [asc_sync_inter_arrive](asc_sync_inter_arrive.md) and [asc_sync_inter_wait](asc_sync_inter_wait.md) APIs. This scenario can be divided into two types:

- Full-core synchronization of the 2 AICs in the 2 AI Cores.

    When both AICs have executed `asc_sync_inter_arrive`, the counter for `flag_id` on each AIC increases by 1. Only if the counter for `flag_id` on the AIC is non-zero can the instructions in the specified pipeline blocked by `asc_sync_inter_wait` continue to execute, and the counter for `flag_id` decreases by 1.

    For each AIC, `asc_sync_inter_arrive` and `asc_sync_inter_wait` must be used in pairs.

- Full-core synchronization of the 4 AIVs in the 2 AI Cores.

    When all 4 AIVs have executed `asc_sync_inter_arrive`, the counter for `flag_id` on each AIV core increases by 1. Only if the counter for `flag_id` on the AIV is non-zero can the instructions in the specified pipeline blocked by `asc_sync_inter_wait` continue to execute, and the counter for `flag_id` decreases by 1.

    For each AIV, `asc_sync_inter_arrive` and `asc_sync_inter_wait` must be used in pairs.

Using Figure 2 as an example, the following code snippet demonstrates full-core synchronization of the 2 AICs (AIC0 and AIC1) in the 2 AI Cores:

```cpp
// When performing inter-core synchronization, even if only AICs participate in the synchronization, the kernel function (Kernel) cannot be decorated with __cube__. For details, see the constraints of asc_sync_inter_arrive.
if ASC_IS_AIC {
    // Each core should have paired calls to asc_sync_inter_arrive and asc_sync_inter_wait similar to the following.
    // The flag_id of the two APIs must be the same. The pipe parameter must be explicitly passed to both APIs. asc_sync_inter_wait blocks the pipeline specified by the pipe parameter.
    asc_sync_inter_arrive(PIPE_M, 0);  // After the preceding PIPE_M pipeline tasks are complete, notify the scheduling module.
    asc_sync_inter_wait(PIPE_FIX, 0);  // After all AICs complete their preceding PIPE_M instructions, the scheduling module updates the counter for flag_id=0 and then unblocks subsequent instructions of the PIPE_FIX pipeline.
}
```

After AIC0 executes `asc_sync_inter_wait`, the counter for `flag_id=0` on AIC0 is 0, and subsequent instructions in the PIPE_FIX pipeline are blocked until both AIC cores have executed `asc_sync_inter_arrive`.

After AIC1 finishes executing `asc_sync_inter_arrive`, the scheduling module detects that both AICs have executed `asc_sync_inter_arrive`, so the counter value for `flag_id=0` on each AIC increases to 1. AIC0 and AIC1 detect that their respective counters for `flag_id=0` become 1, unblock subsequent instructions of the PIPE_FIX pipeline, continue to execute subsequent instructions, and decrease the counter value by 1.

**Figure 2** Mode 0: Full-core synchronization of AICs across multiple AI Cores<a id="multi_ai_core_aic_full_sync"></a>    
![](../../../../figures/inter_core_aic_all_sync_c_api.png "Synchronization control mode diagram")
<!-- end id1 -->

<!-- npu="A3,910b" id2 -->

> [!NOTE]
> The following description of mode 1 is valid only for [NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md). When calling the related APIs, you must explicitly pass the `pipe` parameter. The `pipe` parameter passed to the `arrive` API takes effect, while the `pipe` parameter passed to the `wait` API does not take effect. The `wait` API blocks subsequent instructions of all pipelines.

## Full-Core Synchronization of AIVs in a Single AI Core (Mode 1)<a id="single_ai_core_aiv_full_sync"></a>

This scenario requires the use of the mode 1 [asc_sync_subblock_arrive](asc_sync_subblock_arrive.md) and [asc_sync_subblock_wait](asc_sync_subblock_wait.md) APIs.

Full-core synchronization of the 2 AIVs in 1 AI Core. When both AIVs have executed `asc_sync_subblock_arrive`, the counter for `flag_id` on each AIV increases by 1. Only if the counter for `flag_id` on the AIV is non-zero can the instructions in all pipelines blocked by `asc_sync_subblock_wait` continue to execute, and the counter for `flag_id` decreases by 1. For each AIV, `asc_sync_subblock_arrive` and `asc_sync_subblock_wait` must be used in pairs.

> [!CAUTION] Caution
> In this mode, AIV cores of different AI Cores do not affect each other's synchronization.

Using Figure 3 as an example, the following code snippet demonstrates full-core synchronization of the 2 AIVs (AIV0-0 and AIV0-1) in AI Core 0:

```cpp
// When performing inter-core synchronization, even if only AIVs participate in the synchronization, the kernel function (Kernel) cannot be decorated with __vector__. For details, see the constraints of asc_sync_subblock_arrive.
if ASC_IS_AIV {
    if (block_idx == 0) {
        // The 2 AIVs participating in the synchronization belong to AI Core 0.
        // Each AIV should have paired calls to asc_sync_subblock_arrive and asc_sync_subblock_wait similar to the following.
        // The flag_id of the two APIs must be the same; the pipe parameter must be explicitly passed to both APIs. The pipe parameter of asc_sync_subblock_wait does not take effect on the 2201 architecture.
        asc_sync_subblock_arrive(PIPE_MTE3, 0);  // Notify the scheduling module after the preceding PIPE_MTE3 pipeline tasks are complete.
        asc_sync_subblock_wait(PIPE_MTE2, 0);    // After all AIVs in the AI Core complete their preceding PIPE_MTE3 instructions, the scheduling module updates the counter for flag_id=0 and then unblocks subsequent instructions in all pipelines.
    }
}
```

After AIV0-0 executes `asc_sync_subblock_wait`, the counter for `flag_id=0` on AIV0-0 is 0, and subsequent instructions in all pipelines are blocked until both AIVs have executed `asc_sync_subblock_arrive`.

After AIV0-1 finishes executing `asc_sync_subblock_arrive`, the scheduling module detects that both AIVs have executed `asc_sync_subblock_arrive`, so it increments the counter for `flag_id=0` on AIV0-0 and AIV0-1 to 1. AIV0-0 and AIV0-1 detect that their respective counters for `flag_id=0` become 1, unblock subsequent instructions in all pipelines, continue executing subsequent instructions, and decrement the counter by 1.

**Figure 3** Mode 1: Full-Core synchronization of AIVs within a single AI Core<a id="single_ai_core_aiv_full_sync_diagram"></a>    
![](../../../../figures/single_core_aiv_all_sync_c_api.png "Synchronization control mode diagram")
<!-- end id2 -->

<!-- npu="950" id3 -->

> [!NOTE]
> The following description of mode 2 is valid only for [NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md). When calling the related APIs, the `pipe` parameter must be explicitly passed. The `pipe` parameter passed to both the `arrive` and `wait` APIs takes effect, and the `wait` API blocks subsequent instructions in the pipeline specified by the `pipe` parameter.

## Full-Core Synchronization Between AIC and AIVs Within a Single AI Core (Mode 2)<a id="single_ai_core_aic_aiv_full_sync"></a>

Full-core synchronization between AIC and AIVs within a single AI Core requires the paired use of the mode 2 [asc_sync_block_arrive](asc_sync_block_arrive.md) and [asc_sync_block_wait](asc_sync_block_wait.md) APIs. This scenario can be divided into two cases:

- The AIC in 1 AI Core executes `asc_sync_block_arrive`, and the corresponding 2 AIVs both execute `asc_sync_block_wait`.

    When the AIC finishes executing `asc_sync_block_arrive`, the counters for `flag_id` on the 2 AIVs are incremented by 1. If the counter for `flag_id` on an AIV is non-zero, subsequent instructions in the pipeline specified by the AIV's `pipe` parameter are unblocked, instructions after `asc_sync_block_wait` continue to be issued, and the counter for the `flag_id` is decremented by 1.

    In total, 1 `asc_sync_block_arrive` call by the AIC and 1 `asc_sync_block_wait` call by each of the 2 AIVs constitute a paired use.

- The 2 AIVs in 1 AI Core execute `asc_sync_block_arrive`, and the corresponding AIC executes `asc_sync_block_wait`.

    When both AIVs have finished executing `asc_sync_block_arrive`, the counter for `flag_id` on the AIC is incremented by 1. If the counter for `flag_id` on the AIC is non-zero, subsequent instructions in the pipeline specified by the AIC's `pipe` parameter are unblocked, instructions after `asc_sync_block_wait` continue to be issued, and the counter for the `flag_id` is decremented by 1.

    In total, 1 `asc_sync_block_arrive` call by each of the 2 AIVs and 1 `asc_sync_block_wait` call by the AIC constitute a paired use.

Using Figure 4 as an example, the synchronization between AIC0 and AIV0-0 and AIV0-1 in the 0th AI Core is demonstrated, where the AIVs initiate `asc_sync_block_arrive`. The code snippet is as follows:

```cpp
// Both the AIC and AIVs participate in inter-core synchronization and must use the corresponding macros to wrap their code segments. For details, see the constraints of asc_sync_block_arrive.
if ASC_IS_AIV {
    if (block_idx == 0) {
        // The AIVs participating in synchronization belong to the 0th AI Core.
        // Each AIV should have an asc_sync_block_arrive call similar to the following, and the flag_id must be the same.
        asc_sync_block_arrive(PIPE_MTE3, 0);  // Notify the scheduling module after the preceding PIPE_MTE3 pipeline tasks are complete. Must be executed by all AIVs.
    }
}
if ASC_IS_AIC {
    if (block_idx == 0) {
        // The AIC participating in synchronization belongs to the 0th AI Core.
        // The pipe parameter must be explicitly passed to asc_sync_block_wait, which blocks the pipeline specified by that parameter.
        asc_sync_block_wait(PIPE_MTE2, 0);  // After all AIVs in the AI Core complete their preceding PIPE_MTE3 instructions, the scheduling module updates the counter for flag_id=0, and then the AIC unblocks subsequent instructions in the PIPE_MTE2 pipeline.
    }
}
```

After AIC0 executes `asc_sync_block_wait`, the counter for `flag_id=0` on AIC0 is 0, and subsequent instructions in the PIPE_MTE2 pipeline are blocked until both AIVs have executed `asc_sync_block_arrive`.

After AIV0-1 executes `asc_sync_block_arrive` following Vector instruction 1, all preceding PIPE\_V instructions are complete, and `asc_sync_block_arrive` finishes execution. However, AIV0-0 has not executed `asc_sync_block_arrive` at this time. Therefore, the counter for `flag_id=0` on AIC0 remains 0, and instructions in the PIPE_MTE2 pipeline of the AIC are still blocked.

After Vector instructions 1 and 2 of AIV0-0 have all been executed, all preceding PIPE\_V instructions are complete, and `asc_sync_block_arrive` takes effect. At this time, the scheduling module detects that both AIVs have executed `asc_sync_block_arrive`. It then increments the counter for `flag_id=0` on AIC0 to 1. AIC0 detects that the corresponding counter for `flag_id=0` becomes 1. It then unblocks subsequent instructions in the PIPE_MTE2 pipeline, continues executing the subsequent Matrix instruction 2, and decrements the counter by 1.

**Figure 4** Mode 2: Full-Core synchronization between AIC and AIVs within a single AI Core (AIV performs asc_sync_block_arrive)<a id="single_ai_core_aic_aiv_full_sync_diagram"></a>    
![](../../../../figures/single_core_aic_aiv_sync_aiv_setflag_c_api.png "Synchronization control mode diagram")
<!-- end id3 -->

<!-- npu="950" id4 -->
> [!NOTE]
> Mode 4 is supported only on [NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md). The `pipe` parameter passed to `asc_sync_intra_wait` takes effect and blocks subsequent instructions in the pipeline specified by the `pipe` parameter.

## Synchronization Between AIC and a Single AIV Within a Single AI Core (Mode 4)<a id="single_ai_core_aic_single_aiv_sync"></a>

Synchronization between the AIC and a single AIV within a single AI Core requires the paired use of the mode 4 [asc_sync_intra_arrive](asc_sync_intra_arrive.md) and [asc_sync_intra_wait](asc_sync_intra_wait.md) APIs. This scenario can be divided into two cases:

- The AIC in 1 AI Core executes `asc_sync_intra_arrive`, and the corresponding AIV executes `asc_sync_intra_wait`.

    When the AIC finishes executing `asc_sync_intra_arrive`, the counter for the corresponding `sync_id` on the AIV increases by 1. If the counter for that `sync_id` on the AIV is non-zero, the pipeline specified by the `pipe` parameter on the AIV is unblocked, the instructions after `asc_sync_intra_wait` continue to be issued, and the counter for that `sync_id` decreases by 1.

    In total, 1 `asc_sync_intra_arrive` call by the AIC and one completed `asc_sync_intra_wait` by a single AIV constitute a paired use.

- One AIV in 1 AI Core executes `asc_sync_intra_arrive`, and the corresponding AIC executes `asc_sync_intra_wait`.

    When the AIV finishes executing `asc_sync_intra_arrive`, the counter for the corresponding `sync_id` on the AIC increases by 1. If the counter for that `sync_id` on the AIC is non-zero, the pipeline specified by the `pipe` parameter on the AIC is unblocked, the instructions after `asc_sync_intra_wait` continue to be issued, and the counter for that `sync_id` decreases by 1.

    In total, 1 `asc_sync_intra_arrive` by a single AIV and 1 `asc_sync_intra_wait` call by the corresponding AIC constitute a paired use.

Using Figure 5 as an example, the following code snippet demonstrates synchronization between AIV0-1 and AIC0 in AI Core 0, where AIV0-1 initiates `asc_sync_intra_arrive`:

```cpp
// Both the AIC and AIV participate in inter-core synchronization and must wrap their code segments with the corresponding macros. For details, see the constraints of asc_sync_intra_arrive.
if ASC_IS_AIV {
    if ((block_idx == 0) && (asc_get_sub_block_id() == 1)) {
        // sync_id 0 to 15 on AIV0-1 correspond to sync_id 16 to 31 on AIC0.
        asc_sync_intra_arrive(PIPE_MTE3, 0);  // After the preceding PIPE_MTE3 pipeline tasks on AIV0-1 are complete, notify the scheduling module.
    }
}
if ASC_IS_AIC {
    if (block_idx == 0) {
        // The pipe parameter of asc_sync_intra_wait takes effect and blocks the PIPE_FIX pipeline here.
        asc_sync_intra_wait(PIPE_FIX, 16);  // AIC0 remains blocked until AIV0-1 in the same AI Core completes the preceding PIPE_MTE3 pipeline and the scheduling module updates the counter for sync_id=16.
    }
}
```

After AIC0 executes `asc_sync_intra_wait`, the counter for `sync_id=16` on AIC0 is 0, so all subsequent instructions of the pipeline specified by the `pipe` parameter (PIPE_FIX here) are blocked until 1 AIV finishes executing `asc_sync_intra_arrive`.

- AIV0-0 does not need to execute `asc_sync_intra_arrive` and does not need to participate in synchronization.
- After all PIPE_MTE3 instructions of AIV0-1 are complete, `asc_sync_intra_arrive` takes effect. At this point, the scheduling module detects that 1 AIV has finished executing `asc_sync_intra_arrive`, so it increases the counter value for `sync_id=16` on AIC0 to 1. When AIC0 detects that the counter for the corresponding `sync_id=16` becomes 1, AIC0 unblocks subsequent instructions of the pipeline specified by the `pipe` parameter, continues executing the subsequent PIPE_FIX instructions, and decreases the counter value by 1.

**Figure 5** Mode 4: Synchronization between the AIC and a single AIV in one AI Core (the AIV performs asc_sync_intra_arrive) <a id="single_ai_core_aic_single_aiv_sync_diagram"></a>    
![](../../../../figures/single_ai_core_aic_single_aiv_sync_c_api.png "Synchronization control mode diagram")
<!-- end id4 -->
