# Cast（float转half/int32\_t）<a name="ZH-CN_TOPIC_0000001787470378"></a>

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
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_scalar_intf.h"`。

对标量的数据类型进行转换。

在使用该接口之前，用户需要先了解[内置数据类型](../数据结构/内置数据类型.md)和[精度转换](../数据结构/precision_conversion.md)。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T, typename U, RoundMode roundMode>
__aicore__ inline U Cast(T valueIn)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| ------ | ------ |
| T | valueIn的数据类型。 |
| U | 转换后的数据类型。 |
| roundMode | 精度转换处理模式，类型是RoundMode。<br>RoundMode为枚举类型，用以控制精度转换处理模式，参考[精度舍入模式](../数据结构/precision_conversion.md#tab1)，可取值为:<pre>&bull; CAST_NONE：当转换过程存在精度损失时，按CAST_RINT模式处理；当不存在精度损失时，不进行舍入；<br>&bull; CAST_RINT：向最近的偶数舍入；<br>&bull; CAST_FLOOR：向负无穷大方向舍入；<br>&bull; CAST_CEIL：向正无穷大方向舍入；<br>&bull; CAST_ROUND：四舍五入；<br>&bull; CAST_TRUNC：截断模式；<br>&bull; CAST_ODD：向最近的奇数舍入。</pre> |

**表2**  参数说明

| 参数名称 | 输入/输出 | 描述 |
| ------ | ------ | ------ |
| valueIn | 输入 | 被转换数据类型的标量。 |

## 数据类型<a name="zh-cn_topic_0000002531121342_section4219135304818"></a>

支持的数据类型如下：

- valueIn支持float。
- 返回值支持half、int32\_t。

## 返回值说明<a name="section640mcpsimp"></a>

转换后的U类型标量数据。

## 约束说明<a name="section633mcpsimp"></a>

支持的数据类型组合及对应支持的RoundMode如下：

| valueIn | 返回值 | RoundMode |
| ------ | ------ | ------ |
| float | half | CAST_ODD |
| float | int32_t | CAST_ROUND |
| float | int32_t | CAST_CEIL |
| float | int32_t | CAST_FLOOR |
| float | int32_t | CAST_RINT |

## 调用示例<a name="section837496171220"></a>

```cpp
float valueIn = 3.14159f;
half halfOut = AscendC::Cast<float, half, AscendC::RoundMode::CAST_ODD>(valueIn);             // 返回3.142578

valueIn = 1.5f;
int32_t intRoundOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_ROUND>(valueIn); // 使用CAST_ROUND模式，四舍五入返回2
int32_t intCeilOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_CEIL>(valueIn);   // 使用CAST_CEIL模式，向上取整返回2
int32_t intFloorOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_FLOOR>(valueIn); // 使用CAST_FLOOR模式，向下取整返回1
int32_t intRintOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_RINT>(valueIn);   // 使用CAST_RINT模式，1.5向临近偶数舍入返回2

valueIn = 2.5f;
intRoundOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_ROUND>(valueIn); // 使用CAST_ROUND模式，四舍五入返回3
intCeilOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_CEIL>(valueIn);   // 使用CAST_CEIL模式，向上取整返回3
intFloorOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_FLOOR>(valueIn); // 使用CAST_FLOOR模式，向下取整返回2
intRintOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_RINT>(valueIn);   // 使用CAST_RINT模式，2.5向临近偶数舍入返回2
```
