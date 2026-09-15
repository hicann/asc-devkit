# SetNextTaskStart<a name="ZH-CN_TOPIC_0000002337763421"></a>

<!-- md-trans-meta sourceCommit=dc56b6592be9a41ee18c3bfdf6bd8116a9020128 translatedAt=2026-08-27T14:00:48.803Z -->

> [!NOTE] Description
> This API is experimental and may be adjusted or improved in later versions, without guaranteeing backward compatibility. Developers should pay attention to updates in later versions during use.

## Applicable Products<a name="section17196114513104"></a>

<!-- npu="950" id17 -->
- Ascend 950PR/Ascend 950DT: Supported, and the API takes effect.
<!-- end id17 -->
<!-- npu="A3" id18 -->
- Atlas A3 training products/Atlas A3 inference products: Supported, and the API takes effect.
<!-- end id18 -->
<!-- npu="910b" id19 -->
- Atlas A2 training products/Atlas A2 inference products: Supported, only guarantee compilation compatibility, and the actual functionality does not take effect.
<!-- end id19 -->
<!-- npu="310b" id20 -->
- Atlas 200I/500 A2 inference products: Supported, only guarantee compilation compatibility, and the actual functionality does not take effect.
<!-- end id20 -->
<!-- npu="310p" id21 -->
- Atlas inference products AI Core: Supported, only guarantee compilation compatibility, and the actual functionality does not take effect.
<!-- end id21 -->
<!-- npu="310p" id22 -->
- Atlas inference products Vector Core: Supported, only guarantee compilation compatibility, and the actual functionality does not take effect.
<!-- end id22 -->
<!-- npu="910" id23 -->
- Atlas training products: Supported, only guarantee compilation compatibility, and the actual functionality does not take effect.
<!-- end id23 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_common_intf.h"`.

This API supports the Early-Start capability of the SuperKernel feature at the operator level. After it is called, at the TorchAir level: 1. it is enabled by default in GE graph mode; 2. it is controlled and enabled through the `early_start` option on the npugraph_ex backend.

Call this API in a sub-Kernel of [SuperKernel](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/advanced_programming/super_kernel/principle_introduction.md). After the call, the subsequent instructions can achieve parallelism with other subsequent sub-Kernels, improving overall performance. As shown in [Figure 1](#fig37581010773), SuperKernel invokes sub-Kernels in sequence. To ensure that data between sub-Kernels does not interfere with each other, inter-operator synchronization is inserted between sub-Kernels to preserve ordering. After sub-Kernel<sub>N-1</sub> calls this API, its subsequent instructions achieve parallelism with the subsequent sub-Kernel<sub>N</sub>.

SuperKernel is a binary fusion technology for operators. Unlike source-code fusion, it focuses on the binary scheduling scheme of kernel functions \(Kernel\), performs in-depth optimization, and creates a super Kernel function (SuperKernel) by fusing already-compiled binary code. It invokes multiple other kernel functions, that is, sub-Kernels, in the manner of calling sub-functions. Compared with dispatching individual operators, the SuperKernel technology reduces task scheduling wait time and scheduling overhead, and further optimizes the operator header overhead by utilizing the idle resources between tasks.

**Developers must ensure that the instructions following the call to this API do not interfere with subsequent operators and cause precision issues. It is recommended to call this API after the last data movement instruction of the entire operator.**

**Figure 1**  Parallelism achieved through SetNextTaskStart<a name="fig37581010773"></a>  
![](../../../../figures/set_next_task_start_parallel_diagram.png "Parallelism achieved through SetNextTaskStart")

## Prototype<a name="section620mcpsimp"></a>

<!-- npu="950,A3,910b,310b" id8 -->
- This prototype supports the following products:

    <!-- npu="950" id1 -->
    - Ascend 950PR/Ascend 950DT
    <!-- end id1 -->

    <!-- npu="A3" id2 -->
    - Atlas A3 training products/Atlas A3 inference products
    <!-- end id2 -->

    <!-- npu="910b" id3 -->
    - Atlas A2 training products/Atlas A2 inference products
    <!-- end id3 -->

    <!-- npu="310b" id4 -->
    - Atlas 200I/500 A2 inference products
    <!-- end id4 -->

    ```cpp
    template<pipe_t AIV_PIPE = PIPE_MTE3, pipe_t AIC_PIPE = PIPE_FIX, bool FORCE = false>
    __aicore__ inline void SetNextTaskStart()
    ```
<!-- end id8 -->

<!-- npu="310p,910" id9 -->
- This prototype supports the following products:

    <!-- npu="310p" id5 -->
    - Atlas inference products AI Core
    <!-- end id5 -->

    <!-- npu="310p" id6 -->
    - Atlas inference products Vector Core
    <!-- end id6 -->

    <!-- npu="910" id7 -->
    - Atlas training products
    <!-- end id7 -->

    ```cpp
    template<pipe_t AIV_PIPE = PIPE_MTE3, pipe_t AIC_PIPE = PIPE_MTE3, bool FORCE = false>
    __aicore__ inline void SetNextTaskStart()
    ```
<!-- end id9 -->

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| AIV_PIPE | Instructions executed after SetNextTaskStart. If they are located on the AIV_PIPE pipeline on the AIV, they can run in parallel with subsequent operators. The value range of AIV_PIPE is PIPE_MTE2, PIPE_MTE3, PIPE_S, and PIPE_V. |
| AIC_PIPE | Instructions executed after SetNextTaskStart. If they are located on the AIC_PIPE pipeline on the AIC, they can run in parallel with subsequent operators. The value range of AIC_PIPE is PIPE_MTE1, PIPE_MTE2, PIPE_MTE3, PIPE_FIX, and PIPE_M. |
| FORCE | Whether to forcibly enable the Early-Start capability during offline compilation. This does not affect online compilation.<br>false: The Early-Start capability is not forcibly enabled during offline compilation.<br>true: The Early-Start capability is enabled during offline compilation. This applies to operators without an online compilation process (such as operators developed with <<<>>>). |

> [!NOTE] Description
> Different hardware architectures may have different specific pipelines included in each hardware pipeline type. For details about pipeline types, please refer to [Hardware Pipeline Types](../intra_core_sync/intra_core_synchronization_capability_overview.md#zh-cn_topic_0000002542725361_section1272612276459).

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- This API applies to the TorchAir graph mode development scenario and takes effect only after the SuperKernel feature is enabled. For details, see [Marking the SuperKernel Scope in a Graph](https://www.hiascend.com/document/detail/zh/Pytorch/2610/devguide/TorchAir/docs/zh/ascend_ir/features/advanced/super_kernel_scope.md).
- During operator execution, ensure that this API is called on every core and is called only once on each core.
- If a TilingKey branch of a sub-kernel calls this API, the developer must ensure that all TilingKeys that the current operator may run also call this API. Otherwise, the operator may hang due to a mismatch in the number of synchronization instructions.

## Example<a name="section837496171220"></a>

```cpp
AscendC::DataCopy(dstGlobal, dstLocal, TOTAL_LENGTH);
// Call after the last data copy instruction so that subsequent instructions can execute in parallel with the next sub-kernel.
AscendC::SetNextTaskStart();
```

For the complete sample, please refer to [TaskSync sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/05_sync_control/task_sync).
