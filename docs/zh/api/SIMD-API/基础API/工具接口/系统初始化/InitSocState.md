# InitSocState

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"basic_api/kernel_operator_common_intf.h"`。

本接口对AI Core的全局状态进行初始化，包括AIC（Cube Core）与AIV（Vector Core）的公共状态及各自特有状态。由于不同产品的实现存在差异，实际执行的初始化项也有所不同。

**接口涉及的全部初始化操作汇总如下：**

| 状态类别 | 初始化内容 |
| ---- | ---- |
| 公共状态 | 初始化[原子累加状态](../../原子操作/DisableDmaAtomic.md)，关闭数据搬运随路原子操作功能。 |
| 公共状态 | 初始化[Mask工作模式](../../Memory矢量计算/SIMD计算说明/掩码.md#mask-mode)为Normal模式。 |
| 公共状态 | 初始化[用于AddDeqRelu/CastDequant/Cast的s322fp16场景的scale量化参数](../../Memory矢量计算/类型转换辅助配置接口/SetDeqScale.md#func1)为1。 |
| 公共状态 | 初始化[CTRL寄存器](../../特殊寄存器访问/SetCtrlSpr(ISASI).md#tab-950)中除`CTRL[48]`以外的比特位为默认值。 |
| 公共状态 | 初始化[原子操作开启位与原子操作类型](../../原子操作/SetStoreAtomicConfig(ISASI).md)，为无效的原子操作类型。 |
| AIC | 初始化[LoadData（卷积数据搬运）接口调用时Pad填充的数值](../../矩阵计算（ISASI）/矩阵搬入辅助配置接口/SetLoadDataPaddingValue.md#功能说明)为0。 |
| AIC | 初始化[使用LoadData（卷积数据搬运）时A1/B1的边界值](../../矩阵计算（ISASI）/矩阵搬入辅助配置接口/SetLoadDataBoundary.md#功能说明)为0，表示无边界。 |
| AIV | 将[Mask](../../Memory矢量计算/SIMD计算说明/掩码.md)配置为全1，表示所有数都参与计算。 |
| AIV | [重置loop mode的参数](../../Memory矢量计算/数据搬运辅助配置接口/ResetLoopModePara.md)。 |

**各产品实际执行的初始化项如下：**

<!-- npu="950" id8 -->
**表1** 针对Ascend 950PR/Ascend 950DT本接口执行的初始化项

| 状态类别 | 初始化内容 |
| ---- | ---- |
| 公共状态 | 原子累加状态 |
| 公共状态 | Mask工作模式 |
| 公共状态 | 用于AddDeqRelu/CastDequant/Cast的s322fp16场景的scale量化参数 |
| 公共状态 | CTRL寄存器 |
| 公共状态 | 原子操作开启位与原子操作类型 |
| AIC | LoadData（卷积数据搬运）接口调用时Pad填充的数值 |
| AIV | Mask |
| AIV | loop mode的参数 |
<!-- end id8 -->

<!-- npu="A3" id9 -->
**表2** 针对Atlas A3 训练系列产品/Atlas A3 推理系列产品本接口执行的初始化项

| 状态类别 | 初始化内容 |
| ---- | ---- |
| 公共状态 | 原子累加状态 |
| 公共状态 | Mask工作模式 |
| AIC | LoadData（卷积数据搬运）接口调用时Pad填充的数值 |
| AIC | 使用LoadData（卷积数据搬运）时A1/B1的边界值 |
| AIV | Mask |
<!-- end id9 -->

<!-- npu="910b" id10 -->
**表3** 针对Atlas A2 训练系列产品/Atlas A2 推理系列产品本接口执行的初始化项

| 状态类别 | 初始化内容 |
| ---- | ---- |
| 公共状态 | 原子累加状态 |
| 公共状态 | Mask工作模式 |
| AIC | LoadData（卷积数据搬运）接口调用时Pad填充的数值 |
| AIC | 使用LoadData（卷积数据搬运）时A1/B1的边界值 |
| AIV | Mask |
<!-- end id10 -->

<!-- npu="310p" id11 -->
**表4** 针对Atlas 推理系列产品AI Core和Atlas 推理系列产品Vector Core本接口执行的初始化项

| 状态类别 | 初始化内容 |
| ---- | ---- |
| 公共状态 | 原子累加状态 |
<!-- end id11 -->

## 函数原型

```cpp
__aicore__ inline void InitSocState()
```

## 参数说明

无

## 返回值说明

无

## 约束说明

- 在实际运行中，这些值可能被前序执行的算子修改，若不调用该接口进行初始化，非预期的值可能导致计算结果出现精度错误。

  例如前序算子使用Counter模式但未重置为Normal模式，当前算子以默认的Normal模式设置Mask时，会导致Mask设置不符合预期，进而引发精度错误。
- 在[TPipe框架编程](../../../../../guide/编程指南/编程模型/AI-Core-SIMD编程/基于TPipe-TQue框架编程/TPipe-TQue框架编程原理.md)中，初始化过程由TPipe完成，无需开发者关注；在[静态Tensor编程](../../../../../guide/编程指南/编程模型/AI-Core-SIMD编程/基于Tensor的CPP编程/静态Tensor编程.md)的场景中，用户必须在Kernel入口处调用此函数来初始化AI Core状态。

## 调用示例

```cpp
__global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z)
{
    // 静态Tensor编程方式中需要开发者手动调用InitSocState()接口初始化全局状态寄存器。
    AscendC::InitSocState();
    KernelAdd op;
    op.Init(x, y, z);
    op.Process();
}
```
