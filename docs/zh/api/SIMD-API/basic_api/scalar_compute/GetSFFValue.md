# GetSFFValue<a name="ZH-CN_TOPIC_0000001787630014"></a>

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

查找uint64\_t类型数值的二进制表示中从最低有效位开始的首个指定比特值（0或1）的位置，如果没找到则返回-1。

以如下GetSFFValue调用为例：

```cpp
int64_t sffValue = AscendC::GetSFFValue<1>(0x1000000000000100ULL); // 返回8
```

其计算结果如下图所示：

**图1** 使用GetSFFValue查找从最低有效位开始首个1的位置<a name="fig1"></a>

![使用GetSFFValue查找从最低有效位开始首个1的位置](../../../figures/GetSFFValue.png "使用GetSFFValue查找从最低有效位开始首个1的位置")

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <int countValue>
__aicore__ inline int64_t GetSFFValue(uint64_t valueIn)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
| ------ | ------ |
| countValue | 指定要查找的比特值，取值为0或1。 |

**表2** 参数说明

| 参数名称 | 输入/输出 | 描述 |
| ------ | ------ | ------ |
| valueIn | 输入 | 被查找的二进制数字。 |

## 返回值说明<a name="section640mcpsimp"></a>

返回valueIn中第一个0或1出现的位置。

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section837496171220"></a>

```cpp
int64_t sffValue;
uint64_t valueIn = 0xFFFFFFFFFFFFFFFFULL;    // 二进制表示全为1
sffValue = AscendC::GetSFFValue<0>(valueIn); // 返回-1

valueIn = 0;                                 // 二进制表示全为0
sffValue = AscendC::GetSFFValue<1>(valueIn); // 返回-1

valueIn = 0xFF00ULL;
sffValue = AscendC::GetSFFValue<0>(valueIn); // 返回0
sffValue = AscendC::GetSFFValue<1>(valueIn); // 返回8

valueIn = 0x00FFULL;
sffValue = AscendC::GetSFFValue<0>(valueIn); // 返回8
sffValue = AscendC::GetSFFValue<1>(valueIn); // 返回0
```
