# GetSaturationFlag(ISASI)

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
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：不支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：不支持
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/special_register_access/GetSaturationFlag_ISASI_res.md#id1 -->

## 功能说明

头文件路径为：`"basic_api/kernel_operator_common_intf.h"`。

获取`mode`指定的全局饱和模式的开启状态，该状态通过[SetSaturationFlag](SetSaturationFlag_ISASI.md)设置。饱和模式用于处理计算或精度转换过程中结果溢出或出现INF/NAN的场景，详细说明请参考[SetSaturationFlag功能说明](SetSaturationFlag_ISASI.md#功能说明)。

## 函数原型

```cpp
template <SaturationMode mode>
__aicore__ inline bool GetSaturationFlag()
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| mode | 饱和模式，取值如下：<br>&bull;`SaturationMode::FLOAT`：浮点数计算和浮点数精度转换的饱和模式，系统默认开启。<br>&bull;`SaturationMode::FLOAT8`：浮点数精度转换时，控制FP8类型输出为NAN时是否饱和为0，系统默认开启。该模式仅在精度转换接口处于饱和模式时生效，具体请参考[约束说明](#约束说明)。<br>&bull;`SaturationMode::INT`：整数计算的饱和模式，系统默认关闭。<br>&bull;`SaturationMode::CAST`：浮点数转整数或整数转整数时的精度转换饱和模式，系统默认开启。<br>各模式影响的数据类型请参考[SetSaturationFlag的数据类型](SetSaturationFlag_ISASI.md#数据类型)。模板参数无默认值，调用本接口时必须显式指定。 |

## 返回值说明

返回`true`表示`mode`指定的全局饱和模式已开启，返回`false`表示该模式已关闭。

## 约束说明

<!-- npu="950" id10 -->
- Ascend 950PR/Ascend 950DT支持`SaturationMode::FLOAT`、`SaturationMode::FLOAT8`、`SaturationMode::INT`和`SaturationMode::CAST`。
- 查询`SaturationMode::FLOAT8`时，返回值仅表示该模式的开启状态。该模式仅在精度转换接口处于饱和模式时生效，可通过全局`SaturationMode::FLOAT`或单接口`satMode`进入饱和模式。
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品支持`SaturationMode::FLOAT`、`SaturationMode::INT`和`SaturationMode::CAST`，不支持`SaturationMode::FLOAT8`。
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品支持`SaturationMode::FLOAT`、`SaturationMode::INT`和`SaturationMode::CAST`，不支持`SaturationMode::FLOAT8`。
<!-- end id12 -->

## 调用示例

如下示例中查询整数计算的饱和模式是否开启。

```cpp
bool enableSat = AscendC::GetSaturationFlag<AscendC::SaturationMode::INT>();
```
