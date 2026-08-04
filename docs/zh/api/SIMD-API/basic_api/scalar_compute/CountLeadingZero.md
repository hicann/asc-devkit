# CountLeadingZero<a name="ZH-CN_TOPIC_0000001834229569"></a>

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

统计uint64\_t类型数值的二进制表示中前导0的个数（二进制表示中最高有效位到第一个'1'的'0'的数量，输入为0时返回64）。

以如下CountLeadingZero调用为例：

```cpp
int64_t leadingZeros = AscendC::CountLeadingZero(0x0100000000000020ULL); // 返回7
```

其计算结果如下图所示：

**图1** CountLeadingZero计算示意图<a name="fig1"></a>

![CountLeadingZero计算示意图](../../../figures/CountLeadingZero.png)

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline int64_t CountLeadingZero(uint64_t valueIn)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1** 参数说明

| 参数名称 | 输入/输出 | 描述 |
| ------ | ------ | ------ |
| valueIn | 输入 | 被统计的二进制数字。 |

## 返回值说明<a name="section640mcpsimp"></a>

返回valueIn的前导0的个数。

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section837496171220"></a>

```cpp
int64_t leadingZeros;
uint64_t valueIn = 0x000000000000FFFFULL;          // 二进制表示有48个前导0
leadingZeros = AscendC::CountLeadingZero(valueIn); // 返回48

valueIn = 0;                                       // 二进制表示全为0
leadingZeros = AscendC::CountLeadingZero(valueIn); // 返回64

valueIn = 0xFFFFFFFFFFFFFFFFULL;                   // 二进制表示全为1
leadingZeros = AscendC::CountLeadingZero(valueIn); // 返回0
```
