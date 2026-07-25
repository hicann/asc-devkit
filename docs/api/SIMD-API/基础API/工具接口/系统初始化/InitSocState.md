# InitSocState

## 产品支持情况

<!-- npu="950" id12 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id12 -->
<!-- npu="A3" id13 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id13 -->
<!-- npu="910b" id14 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id14 -->
<!-- npu="310b" id15 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id15 -->
<!-- npu="310p" id16 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id16 -->
<!-- npu="310p" id17 -->
- Atlas 推理系列产品Vector Core：支持
<!-- end id17 -->
<!-- npu="910" id18 -->
- Atlas 训练系列产品：不支持
<!-- end id18 -->

## 功能说明

头文件路径为：`"basic_api/kernel_operator_common_intf.h"`。

本接口对AI Core的全局状态进行初始化，包括AIC（Cube Core）与AIV（Vector Core）的公共状态及各自特有状态。由于不同产品的实现存在差异，实际执行的初始化项也有所不同。

**接口涉及的全部初始化操作汇总如下：**

| 状态类别 | 初始化内容 |
| ---- | ---- |
| 公共状态 | 初始化[原子累加状态](../../原子操作/DisableDmaAtomic.md)，关闭数据搬运随路原子操作功能。 |
| 公共状态 | 初始化[Mask工作模式](../../Memory矢量计算/SIMD计算说明/掩码.md#mask-mode)为Normal模式。 |
| 公共状态 | 初始化[用于AddDeqRelu/CastDequant/Cast的s322fp16场景的scale量化参数](../../Memory矢量计算/类型转换辅助配置接口/SetDeqScale.md#func1)为1。 |
| 公共状态 | 初始化[CTRL寄存器](../../特殊寄存器访问/SetCtrlSpr_ISASI.md#tab-950)中除`CTRL[48]`以外的比特位为默认值。 |
| 公共状态 | 初始化[原子操作开启位与原子操作类型](../../原子操作/SetStoreAtomicConfig_ISASI.md)，为无效的原子操作类型。 |
| AIC | 初始化[Load3D接口调用时Pad填充的数值](../../cube_compute_ISASI/矩阵计算的搬入/辅助配置接口/SetLoadDataPaddingValue.md#功能说明)为0。 |
| AIC | 初始化[使用Load3D时A1/B1的边界值](../../cube_compute_ISASI/矩阵计算的搬入/辅助配置接口/SetLoadDataBoundary.md#功能说明)为0，表示无边界。 |
| AIV | 将[Mask](../../Memory矢量计算/SIMD计算说明/掩码.md)配置为全1，表示所有数都参与计算。 |
| AIV | [重置loop mode的参数](../../Memory矢量计算/数据搬运辅助配置接口/ResetLoopModePara.md)。 |

**各产品实际执行的初始化项如下：**

<!-- npu="950" id1 -->
**表1** 针对Ascend 950PR/Ascend 950DT本接口执行的初始化项

| 状态类别 | 初始化内容 |
| ---- | ---- |
| 公共状态 | 原子累加状态 |
| 公共状态 | Mask工作模式 |
| 公共状态 | 用于AddDeqRelu/CastDequant/Cast的s322fp16场景的scale量化参数 |
| 公共状态 | CTRL寄存器 |
| 公共状态 | 原子操作开启位与原子操作类型 |
| AIC | Load3D接口调用时Pad填充的数值 |
| AIV | Mask |
| AIV | loop mode的参数 |
<!-- end id1 -->

<!-- npu="A3" id2 -->
**表2** 针对Atlas A3 训练系列产品/Atlas A3 推理系列产品本接口执行的初始化项

| 状态类别 | 初始化内容 |
| ---- | ---- |
| 公共状态 | 原子累加状态 |
| 公共状态 | Mask工作模式 |
| AIC | Load3D接口调用时Pad填充的数值 |
| AIC | 使用Load3D时A1/B1的边界值 |
| AIV | Mask |
<!-- end id2 -->

<!-- npu="910b" id3 -->
**表3** 针对Atlas A2 训练系列产品/Atlas A2 推理系列产品本接口执行的初始化项

| 状态类别 | 初始化内容 |
| ---- | ---- |
| 公共状态 | 原子累加状态 |
| 公共状态 | Mask工作模式 |
| AIC | Load3D接口调用时Pad填充的数值 |
| AIC | 使用Load3D时A1/B1的边界值 |
| AIV | Mask |
<!-- end id3 -->

<!-- npu="310p" id4 -->
**表4** 针对Atlas 推理系列产品AI Core和Atlas 推理系列产品Vector Core本接口执行的初始化项

| 状态类别 | 初始化内容 |
| ---- | ---- |
| 公共状态 | 原子累加状态 |
<!-- end id4 -->

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
- 在[TPipe框架编程](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/guide/编程指南/编程模型/AI-Core-SIMD编程/基于TPipe-TQue框架编程/TPipe-TQue框架编程原理.md)中，初始化过程由TPipe完成，无需开发者关注；在[静态Tensor编程](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/guide/编程指南/编程模型/AI-Core-SIMD编程/基于Tensor的CPP编程/静态Tensor编程.md)的场景中，用户必须在Kernel入口处调用此函数来初始化AI Core状态。

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
