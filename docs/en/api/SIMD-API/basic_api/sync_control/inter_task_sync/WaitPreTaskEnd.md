# WaitPreTaskEnd<a name="ZH-CN_TOPIC_0000002337883177"></a>

<!-- md-trans-meta sourceCommit=dc56b6592be9a41ee18c3bfdf6bd8116a9020128 translatedAt=2026-08-27T14:02:19.458Z -->

> [!NOTE]Note
> This API is experimental and may be adjusted or improved in later versions, with no guarantee of backward compatibility. Developers are advised to keep track of updates in subsequent versions.

## Applicable Products<a name="section17196114513104"></a>

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: Supported, and the API takes effect.
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 training products/Atlas A3 inference products: Supported, and the API takes effect.
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 training products/Atlas A2 inference products: Supported, but only compilation compatibility is guaranteed, and the actual function does not take effect.
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: Supported, but only compilation compatibility is guaranteed, and the actual function does not take effect.
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: Supported, but only compilation compatibility is guaranteed, and the actual function does not take effect.
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Supported, but only compilation compatibility is guaranteed, and the actual function does not take effect.
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Supported, but only compilation compatibility is guaranteed, and the actual function does not take effect.
<!-- end id14 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_common_intf.h"`.

This API supports the Early-Start capability of the SuperKernel feature at the operator level. After it is called, at the TorchAir level: 1. It is enabled by default in GE graph mode; 2. It is enabled through the early_start option on the npugraph_ex backend.

Call this API in a sub-Kernel of [SuperKernel](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/advanced_programming/super_kernel/principle_introduction.md). The instructions before the call can run in parallel with the preceding sub-Kernel, improving overall performance. As shown in [Figure 1](#fig99271836191110), SuperKernel invokes sub-Kernels in sequence. To ensure that data between sub-Kernels does not interfere with each other, inter-operator synchronization is inserted between sub-Kernels to preserve order. The instructions before sub-Kernel<sub>N+1</sub> calls this API run in parallel with the preceding sub-Kernel<sub>N</sub>.

SuperKernel is a binary fusion technology for operators. Unlike source-level fusion, it focuses on the binary scheduling scheme of the kernel function \(Kernel\) and performs in-depth optimization. Based on the compiled binary code, it fuses and creates a super kernel function (SuperKernel), which invokes multiple other kernel functions, that is, sub-Kernels, in the manner of calling sub-functions. Compared with dispatching individual operators, SuperKernel reduces task scheduling wait time and scheduling overhead, and further optimizes the operator header overhead by utilizing the idle resources between tasks.

**Developers must ensure that the instructions before calling this API do not interfere with the preceding operators and cause precision issues. It is recommended to call this API before the first data movement instruction of the entire operator.**

**Figure 1**  Parallelization diagram implemented through WaitPreTaskEnd<a name="fig99271836191110"></a>  
![](../../../../figures/wait_pre_task_end_parallel_diagram.png "Parallelization diagram implemented through WaitPreTaskEnd")

## Prototype<a name="section620mcpsimp"></a>

```cpp
template<bool FORCE = false>
__aicore__ inline void WaitPreTaskEnd()
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| FORCE | Whether to forcibly enable the Early-Start capability during offline compilation. This does not affect online compilation.<br>**false**: The Early-Start capability is not forcibly enabled during offline compilation.<br>**true**: The Early-Start capability is enabled during offline compilation. This applies to operators without an online compilation process (such as operators developed with <<<>>>). |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- This API applies to the TorchAir graph mode development scenario and takes effect only after the **SuperKernel** feature is enabled. For details, see [Marking the SuperKernel Scope Within a Graph](https://www.hiascend.com/document/detail/zh/Pytorch/2610/devguide/TorchAir/docs/zh/ascend_ir/features/advanced/super_kernel_scope.md).
- During operator execution, ensure that this API is called on every core, and that it is called only once on each core.
- If a **TilingKey** branch of a sub-kernel calls this API, the developer must ensure that all **TilingKey** values that the current operator may run also call this API. Otherwise, the operator may hang due to a mismatch in the number of synchronization instructions.

## Example<a name="section837496171220"></a>

```cpp
AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
AscendC::LocalTensor<T> src0Local = ubAllocator.Alloc<T, TOTAL_LENGTH>();
AscendC::LocalTensor<T> src1Local = ubAllocator.Alloc<T, TOTAL_LENGTH>();
AscendC::LocalTensor<T> dstLocal = ubAllocator.Alloc<T, TOTAL_LENGTH>();

// Call before the first data movement instruction so that the preceding instructions can execute in parallel with the previous sub-kernel.
AscendC::WaitPreTaskEnd();

AscendC::DataCopy(src0Local, src0Global, TOTAL_LENGTH);
AscendC::DataCopy(src1Local, src1Global, TOTAL_LENGTH);
AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

AscendC::Add(dstLocal, src0Local, src1Local, TOTAL_LENGTH);
```

For the complete sample, see [TaskSync sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/05_sync_control/task_sync).
