# GetSaturationStrategy

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
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

## 功能说明

头文件路径为：`"basic_api/kernel_operator_common_intf.h"`。

获取精度转换接口当前使用的饱和模式配置策略。饱和模式用于处理精度转换过程中结果溢出或出现INF/NAN的场景，详细说明请参考[SetSaturationFlag功能说明](SetSaturationFlag_ISASI.md#功能说明)。该接口用于查询通过[SetSaturationStrategy](SetSaturationStrategy.md)选择的配置来源，主要与Memory矢量计算的[Cast](../memory_vector_compute/type_conversion/Cast.md)和Reg矢量计算的[Cast](../reg_vector_compute/type_conversion/Cast.md)配合使用：

- 使用接口自身的饱和配置时，Reg矢量计算Cast通过`CastTrait::satMode`配置单接口的饱和或非饱和模式；Memory矢量计算Cast不提供单接口饱和模式参数，其单接口配置为饱和模式。
- 使用全局饱和配置时，两类Cast均使用[SetSaturationFlag](SetSaturationFlag_ISASI.md)设置的全局饱和模式。

## 函数原型

```cpp
__aicore__ inline OverrideStrategy GetSaturationStrategy()
```

## 参数说明

无

## 返回值说明

返回值类型为`OverrideStrategy`，表示当前使用的饱和模式配置策略。

**表1**  返回值说明

| 返回值 | 描述 |
| --- | --- |
| `OverrideStrategy::USE_API` | 使用精度转换接口自身的饱和配置。 |
| `OverrideStrategy::USE_GLOBAL` | 使用[SetSaturationFlag](SetSaturationFlag_ISASI.md)设置的全局饱和配置，系统默认采用该策略。 |

## 约束说明

无

## 调用示例

如下示例中查询当前是否使用`SetSaturationFlag`设置的全局饱和模式。

```cpp
bool useGlobal = AscendC::GetSaturationStrategy() == AscendC::OverrideStrategy::USE_GLOBAL;
```
