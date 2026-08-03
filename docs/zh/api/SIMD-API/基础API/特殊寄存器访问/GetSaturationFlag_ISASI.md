# GetSaturationFlag(ISASI)

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
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

## 功能说明

头文件路径为：`"basic_api/kernel_operator_common_intf.h"`。

获取计算/精度转换时饱和模式的开启状态。

## 函数原型

```cpp
template <SaturationMode mode>
__aicore__ inline bool GetSaturationFlag()
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| -------- | -------- |
| mode | 配置的对应饱和模式。<br>当mode配置为FLOAT时，开启饱和模式，inf输出会被饱和为±MAX，NaN输出会被饱和为0；关闭饱和模式，inf/NaN保持原输出。<br>当mode配置为CAST时，开启饱和模式，溢出值会被饱和为±MAX；关闭饱和模式，溢出值会按目标数据类型位数截断，保留低位，舍弃高位。<br><pre>enum class SaturationMode : uint8_t {<br>   FLOAT,     // 控制浮点数计算和浮点数精度转换时的饱和模式,浮点数数据类型仅支持half、bfloat16_t;<br>   CAST       // 控制浮点数转整数或整数转整数时的精度转换饱和模式;<br>};<br></pre> |

## 返回值说明

返回true表示开启饱和模式，false表示未开启饱和模式。

## 约束说明

无

## 调用示例

如下示例中查询是否开启整数转整数时精度转换的饱和模式。

```cpp
bool res = AscendC::GetSaturationFlag<AscendC::SaturationMode::CAST>();
```
