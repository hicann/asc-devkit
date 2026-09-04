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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/system_init/InitSocState_res.md#id1 -->

## 功能说明

头文件路径为：`"basic_api/kernel_operator_common_intf.h"`。

本接口对AI Core的全局状态进行初始化，包括AIC（Cube Core）与AIV（Vector Core）的公共状态及各自特有状态。由于不同产品的实现存在差异，实际执行的初始化项也有所不同。

**接口涉及的全部初始化操作汇总如下：**

| 状态类别 | 初始化内容 |
| ---- | ---- |
| 公共状态 | 初始化[原子累加状态](../../atomic_operations/DisableDmaAtomic.md)，关闭数据搬运随路原子操作功能。 |
| 公共状态 | 初始化[Mask工作模式](../../memory_vector_compute/SIMD_compute/mask.md#mask-mode)为Normal模式。 |
| 公共状态 | 初始化[用于AddDeqRelu/CastDequant/Cast的s322fp16场景的scale量化参数](../../memory_vector_compute/type_conversion_aux_config/SetDeqScale.md#func1)为1。 |
| 公共状态 | 初始化[CTRL寄存器](../../special_register_access/SetCtrlSpr_ISASI.md#tab-950)中除`CTRL[48]`以外的比特位为默认值。 |
| 公共状态 | 初始化[原子操作开启位与原子操作类型](../../atomic_operations/SetStoreAtomicConfig_ISASI.md)，为无效的原子操作类型。 |
| AIC | 初始化[LoadData（卷积数据搬运）接口调用时Pad填充的数值](../../cube_compute_ISASI/cube_load_aux_config/SetLoadDataPaddingValue.md#功能说明)为0。 |
| AIC | 初始化[使用LoadData（卷积数据搬运）时L1 Buffer（A1/B1）的边界值](../../cube_compute_ISASI/cube_load_aux_config/SetLoadDataBoundary.md#功能说明)为0，表示无边界。 |
| AIV | 将[Mask](../../memory_vector_compute/SIMD_compute/mask.md)配置为全1，表示所有数都参与计算。 |
| AIV | [重置loop mode的参数](../../memory_vector_compute/data_move_aux_config/ResetLoopModePara.md)。 |

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
| AIC | 使用LoadData（卷积数据搬运）时L1 Buffer（A1/B1）的边界值 |
| AIV | Mask |

<!-- end id9 -->

<!-- npu="910b" id10 -->
**表3** 针对Atlas A2 训练系列产品/Atlas A2 推理系列产品本接口执行的初始化项

| 状态类别 | 初始化内容 |
| ---- | ---- |
| 公共状态 | 原子累加状态 |
| 公共状态 | Mask工作模式 |
| AIC | LoadData（卷积数据搬运）接口调用时Pad填充的数值 |
| AIC | 使用LoadData（卷积数据搬运）时L1 Buffer（A1/B1）的边界值 |
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
- 在[TPipe框架编程](../../../../../guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_principles.md)中，初始化过程由TPipe完成，无需开发者关注；在[静态Tensor编程](../../../../../guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md)的场景中，用户必须在核函数（Kernel）入口处调用此函数来初始化AI Core状态。

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
