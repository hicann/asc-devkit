# WaitPreTaskEnd<a name="ZH-CN_TOPIC_0000002337883177"></a>

> [!NOTE]说明 
>本接口为试验接口，在后续版本中可能会调整或改进，不保证后续兼容性。请开发者在使用过程中关注后续版本更新。

## 产品支持情况<a name="section17196114513104"></a>

<!-- npu="950" id10 -->
- Ascend 950PR/Ascend 950DT：支持，该接口生效
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持，该接口生效
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持，仅保证编译兼容，实际功能不生效。
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 推理产品：支持，仅保证编译兼容，实际功能不生效。
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas 推理系列产品AI Core：支持，仅保证编译兼容，实际功能不生效。
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas 推理系列产品Vector Core：支持，仅保证编译兼容，实际功能不生效。
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas 训练系列产品：支持，仅保证编译兼容，实际功能不生效。
<!-- end id16 -->
<!-- npu="x90" id17 -->
- Kirin X90：支持，仅保证编译兼容，实际功能不生效。
<!-- end id17 -->
<!-- npu="9030" id18 -->
- Kirin 9030：支持，仅保证编译兼容，实际功能不生效。
<!-- end id18 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_task_sync/WaitPreTaskEnd_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_common_intf.h"`。

算子层面支持SuperKernel特性Early-Start能力的接口，调用后在TorchAir层面：1.GE图模式下进行默认启用; 2.npugraph_ex后端通过early_start选项进行控制启用。

在[SuperKernel](../../../../../guide/programming_guide/advanced_programming/super_kernel/principles.md)的子核函数（Kernel）中调用，调用前的指令可以和前序其他的子核函数（Kernel）实现并行，提升整体性能。如[图1](#fig99271836191110)所示，SuperKernel按序调用子核函数（Kernel），为保证子核函数（Kernel）之间数据互不干扰，会在子核函数（Kernel）间插入算子间同步进行保序，子核函数（Kernel）<sub>N+1</sub>调用该接口之前的指令会和前序子核函数（Kernel）<sub>N</sub>实现并行。

SuperKernel是一种算子的二进制融合技术，与源码融合不同，它聚焦于核函数（Kernel） \(核函数（Kernel）\)的二进制的调度方案，展开深度优化，于已编译的二进制代码基础上融合创建一个超级核函数（Kernel）（SuperKernel），以调用子函数的方式调用多个其他核函数（Kernel），也就是子核函数（Kernel）。相对于单算子下发，SuperKernel技术可以减少任务调度等待时间和调度开销，同时利用Task间隙资源进一步优化算子头开销。

**开发者需要自行保证调用此接口前的指令不会与前序算子互相干扰而导致精度问题，推荐在整个算子第一条搬运指令前调用此接口。**

**图1**  通过WaitPreTaskEnd实现并行示意图<a name="fig99271836191110"></a>  
![](../../../../figures/wait_pre_task_end_parallel_diagram.png "通过WaitPreTaskEnd实现并行示意图")

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template<bool FORCE = false>
__aicore__ inline void WaitPreTaskEnd()
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| FORCE | 离线编译时是否强制启用Early-Start能力，不影响在线编译。<br>false：离线编译时不强制启用Early-Start能力；<br>true：离线编译时启用Early-Start能力，适用于无在线编译流程的算子（如<<<>>>开发的算子）。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 该接口适用于TorchAir图模式开发场景，且需在启用SuperKernel特性后方可生效。具体内容请参考[图内标定SuperKernel范围](https://www.hiascend.com/document/detail/zh/Pytorch/latest/devguide/TorchAir/docs/zh/ascend_ir/features/advanced/super_kernel_scope.md)。
- 在算子运行过程中，需要保证此接口在每个核上都被调用，且每个核上仅被调用一次。
- 若子核函数（Kernel）某个TilingKey分支调用了此接口，则开发者需要保证当前算子可能会运行的所有TilingKey均调用了此接口，否则会出现因同步指令数量不匹配而卡住的现象。

## 调用示例<a name="section837496171220"></a>

```cpp
AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
AscendC::LocalTensor<T> src0Local = ubAllocator.Alloc<T, TOTAL_LENGTH>();
AscendC::LocalTensor<T> src1Local = ubAllocator.Alloc<T, TOTAL_LENGTH>();
AscendC::LocalTensor<T> dstLocal = ubAllocator.Alloc<T, TOTAL_LENGTH>();

// 在第一条搬运指令前调用，使之前的指令可与前序子kernel并行执行。
AscendC::WaitPreTaskEnd();

AscendC::DataCopy(src0Local, src0Global, TOTAL_LENGTH);
AscendC::DataCopy(src1Local, src1Global, TOTAL_LENGTH);
AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

AscendC::Add(dstLocal, src0Local, src1Local, TOTAL_LENGTH);
```

完整样例请参考[TaskSync样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/task_sync)。
