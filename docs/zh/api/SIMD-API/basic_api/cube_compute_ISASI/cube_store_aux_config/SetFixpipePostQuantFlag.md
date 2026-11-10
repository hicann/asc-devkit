# SetFixpipePostQuantFlag<a name="ZH-CN_TOPIC_0000002738071285"></a>

## 产品支持情况<a name="section18204144912492"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：不支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- KirinX90处理器系列产品：不支持
<!-- end id8 -->
<!-- npu="9020" id9 -->
- Kirin9020处理器系列产品：不支持
<!-- end id9 -->
<!-- npu="9030" id10 -->
- Kirin9030处理器系列产品：不支持
<!-- end id10 -->
<!-- npu="9050" id11 -->
- Kirin9050处理器系列产品：支持
<!-- end id11 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_store_aux_config/SetFixpipePostQuantFlag_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_macro_intf.h"`。

MacroCube计算（如[MmadFix](../mmad_compute/MmadFix.md)、ConvFix）过程中，开启后量化（PostQuant）后处理功能时，通过调用该接口设置后量化的scalar参数。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetFixpipePostQuantFlag(uint64_t config)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| config | 输入 | 后量化过程中使用到的scalar量化参数。 |

## 数据类型

参数的数据类型为uint64\_t。

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 量化参数不能为inf/nan和非规格化数。
- 该接口需配合MacroCube系列计算接口（如[MmadFix](../mmad_compute/MmadFix.md)、ConvFix）一起使用，需在MacroCube计算接口之前调用。
- 本章节涉及寄存器在相同核函数（Kernel）内部会维持状态直到被人为改变，而在不同核函数（Kernel）内部不会继承之前的状态。

## 调用示例<a name="section6461234123118"></a>

```cpp
float tmp = (float)0.5;
// 将float的tmp转换成uint64_t的postQuantScalar
uint64_t postQuantScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&tmp));
AscendC::SetFixpipePostQuantFlag(postQuantScalar);  // 设置后量化参数
AscendC::PipeBarrier<PIPE_FIX>();
```
