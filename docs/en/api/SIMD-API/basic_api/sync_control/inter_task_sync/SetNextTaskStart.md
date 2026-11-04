# SetNextTaskStart<a name="ZH-CN_TOPIC_0000002337763421"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:55:02.379Z pushedAt=2026-10-26T07:41:44.812Z nl=2 -->

> [!NOTE]Description 
> This API is experimental and may be adjusted or improved in subsequent versions, with no guarantee of backward compatibility. Developers are advised to keep track of updates in subsequent versions during use.

## Applicable Products<a name="section17196114513104"></a>

<!-- npu="950" id21 -->
- Ascend 950PR&950DT products: supported, and the API takes effect.
<!-- end id21 -->
<!-- npu="A3" id22 -->
- Atlas A3 products: supported, and the API takes effect.
<!-- end id22 -->
<!-- npu="910b" id23 -->
- Atlas A2 products: supported, with guaranteed compilation compatibility only; the actual functionality does not take effect.
<!-- end id23 -->
<!-- npu="310b" id24 -->
- Atlas 200I/500 A2 inference products: supported, with guaranteed compilation compatibility only; the actual functionality does not take effect.
<!-- end id24 -->
<!-- npu="310p" id25 -->
- Atlas inference products AI Core: supported, with guaranteed compilation compatibility only; the actual functionality does not take effect.
<!-- end id25 -->
<!-- npu="310p" id26 -->
- Atlas inference products Vector Core: supported, with guaranteed compilation compatibility only; the actual functionality does not take effect.
<!-- end id26 -->
<!-- npu="910" id27 -->
- Atlas training products: supported, with guaranteed compilation compatibility only; the actual functionality does not take effect.
<!-- end id27 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_task_sync/SetNextTaskStart_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_common_intf.h"`.

This is an operator-level API that supports the Early-Start capability of the SuperKernel feature. After it is called, at the TorchAir level: 1. it is enabled by default in GE graph mode; 2. it is controlled and enabled through the early_start option in the npugraph_ex backend.

Call this API in a sub-kernel function (Kernel) of [SuperKernel](../../../../../guide/programming_guide/advanced_programming/super_kernel/principles.md). After the call, the subsequent instructions can run in parallel with other sub-kernel functions (Kernels), improving overall performance. As shown in [Figure 1](#fig37581010773), SuperKernel calls sub-kernel functions (Kernels) in sequence. To ensure that data between sub-kernel functions (Kernels) does not interfere with each other, inter-operator synchronization is inserted between them to preserve order. After sub-kernel function (Kernel)<sub>N-1</sub> calls this API, its subsequent instructions run in parallel with the subsequent sub-kernel function (Kernel)<sub>N</sub>.

SuperKernel is a binary fusion technology for operators. Unlike source code fusion, it focuses on the binary scheduling scheme of kernel functions (Kernels), performs in-depth optimization, and creates a super kernel function (Kernel) (SuperKernel) based on already-compiled binary code, calling multiple other kernel functions (Kernels) as sub-functions, that is, sub-kernel functions (Kernels). Compared with single-operator dispatch, SuperKernel technology can reduce task scheduling wait time and scheduling overhead, while further optimizing operator header overhead by utilizing idle resources between Tasks.

**Developers must ensure that the instructions after calling this API do not interfere with subsequent operators and cause precision issues. It is recommended to call this API after the last data movement instruction of the entire operator.**

**Figure 1**  Parallelism diagram implemented through SetNextTaskStart<a name="fig37581010773"></a>  
![](../../../../figures/set_next_task_start_parallel_diagram.png "Parallelism diagram implemented through SetNextTaskStart")

## Prototype<a name="section620mcpsimp"></a>

<!-- npu="950,A3,910b,310b" id10 -->
- This prototype supports the following product models:

    <!-- npu="950" id1 -->
    Ascend 950PR&950DT products 
    <!-- end id1 -->

    <!-- npu="A3" id2 -->
    Atlas A3 products 
    <!-- end id2 -->

    <!-- npu="910b" id3 -->
    Atlas A2 products 
    <!-- end id3 -->

    <!-- npu="310b" id4 -->
    Atlas 200I/500 A2 inference products 
    <!-- end id4 -->

    ```cpp
    template<pipe_t AIV_PIPE = PIPE_MTE3, pipe_t AIC_PIPE = PIPE_FIX, bool FORCE = false>
    __aicore__ inline void SetNextTaskStart()
    ```
<!-- end id10 -->

<!-- npu="310p,910" id11 -->
- This prototype supports the following product models:

    <!-- npu="310p" id7 -->
    Atlas inference products AI Core 
    <!-- end id7 -->

    <!-- npu="310p" id8 -->
    Atlas inference products Vector Core 
    <!-- end id8 -->

    <!-- npu="910" id9 -->
    Atlas training products 
    <!-- end id9 -->

    ```cpp
    template<pipe_t AIV_PIPE = PIPE_MTE3, pipe_t AIC_PIPE = PIPE_MTE3, bool FORCE = false>
    __aicore__ inline void SetNextTaskStart()
    ```
<!-- end id11 -->

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| AIV_PIPE | Instructions executed after SetNextTaskStart. If they are located on the AIV_PIPE pipeline on the AIV, they can run in parallel with subsequent operators. The value range of AIV_PIPE is PIPE_MTE2, PIPE_MTE3, PIPE_S, and PIPE_V. |
| AIC_PIPE | Instructions executed after SetNextTaskStart. If they are located on the AIC_PIPE pipeline on the AIC, they can run in parallel with subsequent operators. The value range of AIC_PIPE is PIPE_MTE1, PIPE_MTE2, PIPE_MTE3, PIPE_FIX, and PIPE_M. |
| FORCE | Whether to forcibly enable the Early-Start capability during offline compilation. This does not affect online compilation.<br>false: The Early-Start capability is not forcibly enabled during offline compilation.<br>true: The Early-Start capability is enabled during offline compilation. This applies to operators without an online compilation process (such as operators developed with <<<>>>). |

> [!NOTE]Description 
> Different hardware architectures have different specific pipelines included in each hardware pipeline type. For details about pipeline types, please refer to [Hardware Pipeline Types](../intra_core_sync/intra_core_sync_overview.md#zh-cn_topic_0000002542725361_section1272612276459).

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- This API applies to the TorchAir graph mode development scenario and takes effect only after the SuperKernel feature is enabled. For details, please refer to [Specifying the SuperKernel Scope in a Graph](https://www.hiascend.com/document/detail/zh/Pytorch/latest/devguide/TorchAir/docs/zh/ascend_ir/features/advanced/super_kernel_scope.md).
- During operator execution, ensure that this API is called on every core, and that it is called only once on each core.
- If a TilingKey branch of a sub-kernel function (Kernel) calls this API, the developer must ensure that all TilingKey branches that the current operator may run also call this API. Otherwise, the execution may hang due to a mismatch in the number of synchronization instructions.
- The internal implementation of this API occupies an inter-core synchronization flagId. If it is used together with CrossCoreSetFlag/CrossCoreWaitFlag, avoid flagId conflicts. For details, see [Inter-Core Synchronization flagId Usage](../inter_core_sync/key_features.md#inter_core_sync_flagid_usage).

## Example<a name="section837496171220"></a>

```cpp
AscendC::DataCopy(dstGlobal, dstLocal, TOTAL_LENGTH);
// Call this after the last data movement instruction so that subsequent instructions can execute in parallel with the following sub-kernel.
AscendC::SetNextTaskStart();
```

For the complete sample, please refer to [TaskSync sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/task_sync).
