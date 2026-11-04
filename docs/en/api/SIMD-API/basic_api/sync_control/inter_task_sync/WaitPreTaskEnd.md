# WaitPreTaskEnd<a name="ZH-CN_TOPIC_0000002337883177"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:55:21.034Z pushedAt=2026-10-26T07:41:44.815Z nl=2 -->

> [!NOTE]Note 
> This is an experimental API and may be adjusted or improved in later versions, with no guarantee of backward compatibility. Developers are advised to keep track of updates in later versions during use.

## Applicable Products<a name="section17196114513104"></a>

<!-- npu="950" id10 -->
- Ascend 950PR&950DT products: supported, and this API takes effect.
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 products: supported, and this API takes effect.
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 products: supported, but only compilation compatibility is guaranteed, and the actual functionality does not take effect.
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 inference products: supported, but only compilation compatibility is guaranteed, and the actual functionality does not take effect.
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products AI Core: supported, but only compilation compatibility is guaranteed, and the actual functionality does not take effect.
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products Vector Core: supported, but only compilation compatibility is guaranteed, and the actual functionality does not take effect.
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas training products: supported, but only compilation compatibility is guaranteed, and the actual functionality does not take effect.
<!-- end id16 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_task_sync/WaitPreTaskEnd_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_common_intf.h"`.

This API supports the Early-Start capability of the SuperKernel feature at the operator level. After it is called, at the TorchAir level: 1. it is enabled by default in GE graph mode; 2. it is controlled and enabled through the early_start option on the npugraph_ex backend.

Call this API in a sub-kernel function (Kernel) of [SuperKernel](../../../../../guide/programming_guide/advanced_programming/super_kernel/principles.md). The instructions before the call can run in parallel with the preceding sub-kernel functions (Kernels), improving overall performance. As shown in [Figure 1](#fig99271836191110), SuperKernel invokes sub-kernel functions (Kernels) in sequence. To ensure that data between sub-kernel functions (Kernels) does not interfere with each other, inter-operator synchronization is inserted between sub-kernel functions (Kernels) to preserve order. The instructions before sub-kernel function (Kernel)<sub>N+1</sub> calls this API run in parallel with the preceding sub-kernel function (Kernel)<sub>N</sub>.

SuperKernel is a binary fusion technology for operators. Unlike source-level fusion, it focuses on the scheduling scheme of the binary of kernel functions (Kernels) \( kernel functions (Kernels)\), performs in-depth optimization, and creates a super kernel function (Kernel) (SuperKernel) by fusing already-compiled binary code. It invokes multiple other kernel functions (Kernels), that is, sub-kernel functions (Kernels), in the manner of calling sub-functions. Compared with dispatching a single operator, SuperKernel technology can reduce task scheduling wait time and scheduling overhead, and further optimize the operator header overhead by utilizing the idle resources in task gaps.

**Developers must ensure that the instructions before calling this API do not interfere with the preceding operators and cause precision issues. It is recommended to call this API before the first data movement instruction of the entire operator.**

**Figure 1**  Diagram of achieving parallelism through WaitPreTaskEnd<a name="fig99271836191110"></a>  
![](../../../../figures/wait_pre_task_end_parallel_diagram.png "Diagram of achieving parallelism through WaitPreTaskEnd")

## Prototype<a name="section620mcpsimp"></a>

```cpp
template<bool FORCE = false>
__aicore__ inline void WaitPreTaskEnd()
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| FORCE | Whether to force-enable the Early-Start capability during offline compilation. This does not affect online compilation.<br>**false**: The Early-Start capability is not force-enabled during offline compilation.<br>**true**: The Early-Start capability is enabled during offline compilation. This applies to operators without an online compilation process (such as operators developed with <<<>>>). |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- This API applies to the TorchAir graph-mode development scenario and takes effect only after the SuperKernel feature is enabled. For details, see [Marking the SuperKernel Scope in a Graph](https://www.hiascend.com/document/detail/zh/Pytorch/latest/devguide/TorchAir/docs/zh/ascend_ir/features/advanced/super_kernel_scope.md).
- During operator execution, ensure that this API is called on every core, and that it is called only once on each core.
- If a TilingKey branch of a sub-kernel function (Kernel) calls this API, the developer must ensure that all TilingKeys that the current operator may run also call this API. Otherwise, the execution may hang due to a mismatch in the number of synchronization instructions.
- The internal implementation of this API occupies an inter-core synchronization flagId. If it is used together with CrossCoreSetFlag/CrossCoreWaitFlag, avoid flagId conflicts. For details, see [Inter-Core Synchronization flagId Usage](../inter_core_sync/key_features.md#inter_core_sync_flagid_usage).

## Example<a name="section837496171220"></a>

```cpp
AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
AscendC::LocalTensor<T> src0Local = ubAllocator.Alloc<T, TOTAL_LENGTH>();
AscendC::LocalTensor<T> src1Local = ubAllocator.Alloc<T, TOTAL_LENGTH>();
AscendC::LocalTensor<T> dstLocal = ubAllocator.Alloc<T, TOTAL_LENGTH>();

// Call this before the first data movement instruction so that the preceding instructions can execute in parallel with the previous sub-kernel.
AscendC::WaitPreTaskEnd();

AscendC::DataCopy(src0Local, src0Global, TOTAL_LENGTH);
AscendC::DataCopy(src1Local, src1Global, TOTAL_LENGTH);
AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

AscendC::Add(dstLocal, src0Local, src1Local, TOTAL_LENGTH);
```

For the complete sample, see [TaskSync sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/task_sync).
