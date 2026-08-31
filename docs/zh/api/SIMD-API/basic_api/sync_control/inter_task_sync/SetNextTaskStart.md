# SetNextTaskStart<a name="ZH-CN_TOPIC_0000002337763421"></a>

> [!NOTE]说明 
>本接口为试验接口，在后续版本中可能会调整或改进，不保证后续兼容性。请开发者在使用过程中关注后续版本更新。

## 产品支持情况<a name="section17196114513104"></a>

<!-- npu="950" id21 -->
- Ascend 950PR/Ascend 950DT：支持，该接口生效
<!-- end id21 -->
<!-- npu="A3" id22 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持，该接口生效
<!-- end id22 -->
<!-- npu="910b" id23 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持，仅保证编译兼容，实际功能不生效。
<!-- end id23 -->
<!-- npu="310b" id24 -->
- Atlas 200I/500 A2 推理产品：支持，仅保证编译兼容，实际功能不生效。
<!-- end id24 -->
<!-- npu="310p" id25 -->
- Atlas 推理系列产品AI Core：支持，仅保证编译兼容，实际功能不生效。
<!-- end id25 -->
<!-- npu="310p" id26 -->
- Atlas 推理系列产品Vector Core：支持，仅保证编译兼容，实际功能不生效。
<!-- end id26 -->
<!-- npu="910" id27 -->
- Atlas 训练系列产品：支持，仅保证编译兼容，实际功能不生效。
<!-- end id27 -->
<!-- npu="x90" id28 -->
- Kirin X90：支持，仅保证编译兼容，实际功能不生效。
<!-- end id28 -->
<!-- npu="9030" id29 -->
- Kirin 9030：支持，仅保证编译兼容，实际功能不生效。
<!-- end id29 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_task_sync/SetNextTaskStart_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_common_intf.h"`。

算子层面支持SuperKernel特性Early-Start能力的接口，调用后在TorchAir层面：1.GE图模式下进行默认启用; 2.npugraph_ex后端通过early_start选项进行控制启用。

在[SuperKernel](../../../../../guide/programming_guide/advanced_programming/super_kernel/principles.md)的子核函数（Kernel）中调用，调用后的指令可以和后续其他的子核函数（Kernel）实现并行，提升整体性能。如[图1](#fig37581010773)所示，SuperKernel按序调用子核函数（Kernel），为保证子核函数（Kernel）之间数据互不干扰，会在子核函数（Kernel）间插入算子间同步进行保序，子核函数（Kernel）<sub>N-1</sub>调用该接口后，之后的指令会和后续子核函数（Kernel）<sub>N</sub>实现并行。

SuperKernel是一种算子的二进制融合技术，与源码融合不同，它聚焦于核函数（Kernel） \(核函数（Kernel）\)的二进制的调度方案，展开深度优化，于已编译的二进制代码基础上融合创建一个超级核函数（Kernel）（SuperKernel），以调用子函数的方式调用多个其他核函数（Kernel），也就是子核函数（Kernel）。相对于单算子下发，SuperKernel技术可以减少任务调度等待时间和调度开销，同时利用Task间隙资源进一步优化算子头开销。

**开发者需要自行保证调用此接口后的指令不会与后序算子互相干扰而导致精度问题，推荐在整个算子最后一条搬运指令后调用此接口。**

**图1**  通过SetNextTaskStart实现并行示意图<a name="fig37581010773"></a>  
![](../../../../figures/set_next_task_start_parallel_diagram.png "通过SetNextTaskStart实现并行示意图")

## 函数原型<a name="section620mcpsimp"></a>

<!-- npu="950,A3,910b,310b,x90,9030" id10 -->
- 该原型支持如下产品型号：

    <!-- npu="950" id1 -->
    Ascend 950PR/Ascend 950DT 
    <!-- end id1 -->

    <!-- npu="A3" id2 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品 
    <!-- end id2 -->

    <!-- npu="910b" id3 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品 
    <!-- end id3 -->

    <!-- npu="310b" id4 -->
    Atlas 200I/500 A2 推理产品 
    <!-- end id4 -->

    <!-- npu="x90" id5 -->
    Kirin X90 
    <!-- end id5 -->

    <!-- npu="9030" id6 -->
    Kirin 9030 
    <!-- end id6 -->

    ```cpp
    template<pipe_t AIV_PIPE = PIPE_MTE3, pipe_t AIC_PIPE = PIPE_FIX, bool FORCE = false>
    __aicore__ inline void SetNextTaskStart()
    ```
<!-- end id10 -->

<!-- npu="310p,910" id11 -->
- 该原型支持如下产品型号：

    <!-- npu="310p" id7 -->
    Atlas 推理系列产品AI Core 
    <!-- end id7 -->

    <!-- npu="310p" id8 -->
    Atlas 推理系列产品Vector Core 
    <!-- end id8 -->

    <!-- npu="910" id9 -->
    Atlas 训练系列产品 
    <!-- end id9 -->

    ```cpp
    template<pipe_t AIV_PIPE = PIPE_MTE3, pipe_t AIC_PIPE = PIPE_MTE3, bool FORCE = false>
    __aicore__ inline void SetNextTaskStart()
    ```
<!-- end id11 -->

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| AIV_PIPE | SetNextTaskStart之后运行的指令，如果位于AIV上的AIV_PIPE流水，可以与后序算子并行。AIV_PIPE的取值范围为PIPE_MTE2、PIPE_MTE3、PIPE_S、PIPE_V。 |
| AIC_PIPE | SetNextTaskStart之后运行的指令，如果位于AIC上的AIC_PIPE流水，可以与后序算子并行。AIC_PIPE的取值范围为PIPE_MTE1、PIPE_MTE2、PIPE_MTE3、PIPE_FIX、PIPE_M。 |
| FORCE | 离线编译时是否强制启用Early-Start能力，不影响在线编译。<br>false：离线编译时不强制启用Early-Start能力；<br>true：离线编译时启用Early-Start能力，适用于无在线编译流程的算子（如<<<>>>开发的算子）。 |

> [!NOTE]说明 
>不同的硬件架构，每一种硬件流水类型包含的具体流水会有所差异，流水类型详细介绍请参考[硬件流水类型](../intra_core_sync/intra_core_sync_overview.md#zh-cn_topic_0000002542725361_section1272612276459)。

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 该接口适用于TorchAir图模式开发场景，且需在启用SuperKernel特性后方可生效。具体内容请参考[图内标定SuperKernel范围](https://www.hiascend.com/document/detail/zh/Pytorch/latest/devguide/TorchAir/docs/zh/ascend_ir/features/advanced/super_kernel_scope.md)。
- 在算子运行过程中，需要保证此接口在每个核上都被调用，且每个核上仅被调用一次。
- 若子核函数（Kernel）某个TilingKey分支调用了此接口，则开发者需要保证当前算子可能会运行的所有TilingKey均调用了此接口，否则会出现因同步指令数量不匹配而卡住的现象。

## 调用示例<a name="section837496171220"></a>

```cpp
AscendC::DataCopy(dstGlobal, dstLocal, TOTAL_LENGTH);
// 在最后一条搬运指令后调用，使之后的指令可与后续子kernel并行执行。
AscendC::SetNextTaskStart();
```

完整样例请参考[TaskSync样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/task_sync)。
