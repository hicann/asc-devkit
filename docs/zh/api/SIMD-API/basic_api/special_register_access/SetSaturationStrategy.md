# SetSaturationStrategy

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

设置精度转换接口使用的饱和模式配置策略。该配置主要与Memory矢量计算的[Cast](../memory_vector_compute/type_conversion/Cast.md)和Reg矢量计算的[Cast](../reg_vector_compute/type_conversion/Cast.md)配合使用，两类Cast的配置方式如下：

- 使用单接口饱和配置时，Reg矢量计算Cast通过`CastTrait::satMode`配置单接口的饱和或非饱和模式；Memory矢量计算Cast不提供单接口饱和模式参数，其单接口配置为饱和模式。
- 使用全局饱和配置时，两类Cast均使用[SetSaturationFlag](SetSaturationFlag_ISASI.md)设置的全局饱和模式。

具体支持的数据类型和转换组合以各Cast接口资料为准，可通过[GetSaturationStrategy](GetSaturationStrategy.md)查询当前使用的配置策略。

## 函数原型

```cpp
template <OverrideStrategy strategy>
__aicore__ inline void SetSaturationStrategy()
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| strategy | 饱和模式配置策略，取值如下：<br>&bull;`OverrideStrategy::USE_API`：使用精度转换接口自身的饱和配置。<br>&bull;`OverrideStrategy::USE_GLOBAL`：使用[SetSaturationFlag](SetSaturationFlag_ISASI.md)设置的全局饱和配置，系统默认采用该策略。<br>模板参数无默认值，调用本接口时必须显式指定。 |

## 返回值说明

无

## 约束说明

无

## 调用示例

以下示例通过Memory矢量计算[Cast](../memory_vector_compute/type_conversion/Cast.md)体现两种配置策略的效果。假设`int16_t`类型的`srcLocal`中各元素均为257，两个输出Tensor的数据类型均为`uint8_t`，`count`表示参与转换的元素个数。示例关闭全局精度转换饱和模式，使接口自身配置与全局配置产生不同结果。

```cpp
// 关闭整数精度转换的全局饱和模式。
AscendC::SetSaturationFlag<AscendC::SaturationMode::CAST>(false);

// 使用Memory矢量计算Cast自身的饱和配置，转换结果为255。
AscendC::SetSaturationStrategy<AscendC::OverrideStrategy::USE_API>();
AscendC::PipeBarrier<PIPE_ALL>();
AscendC::Cast(apiDstLocal, srcLocal, AscendC::RoundMode::CAST_NONE, count);

// 使用全局非饱和配置，转换结果为1。
AscendC::SetSaturationStrategy<AscendC::OverrideStrategy::USE_GLOBAL>();
AscendC::PipeBarrier<PIPE_ALL>();
AscendC::Cast(globalDstLocal, srcLocal, AscendC::RoundMode::CAST_NONE, count);
```
