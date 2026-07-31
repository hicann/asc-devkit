# ShiftRight（右移位数为Tensor）<a name="ZH-CN_TOPIC_0000002193989473"></a>

## 产品支持情况<a name="section1550532418810"></a>

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
- Atlas 推理系列产品 AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品 Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

源操作数内每个元素做右移。

对无符号数据类型的源操作数做逻辑右移，对有符号数据类型的源操作数做算术右移。

逻辑右移为去掉最低位，最高位填充为0。

算术右移为去掉最低位，最高位复制符号位。

例：数据类型uint16\_t，二进制数1010101010101010，逻辑右移一位结果为0101010101010101；

数据类型int16\_t，二进制数1010101010101010，算术右移一位结果为1101010101010101；

数据类型int16\_t，二进制数1010101010101010，算术右移三位结果为1111010101010101。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T, typename U>
__aicore__ inline void ShiftRight(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<U>& src1, const int32_t& count)
```

## 参数说明<a name="section1436019411811"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| T | 源/目的操作数数据类型。 |
| U | 源操作数数据类型。 |

**表2**  参数说明

| **参数名称** | 输入/输出 | **说明** |
| :--- | :--- | :--- |
| dst | 输出 | 目的操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。 |
| src0 | 输入 | 源操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。<br>数据类型需要与目的操作数保持一致。 |
| src1 | 输入 | 存放右移位数的LocalTensor，数据类型的字节数需要与源src0操作数Tensor中的元素数据类型的字节数相匹配，不支持设置为负数。 |
| count | 输入 | 参与计算的元素个数。 |


## 数据类型
<!-- npu="950" id8 -->
Ascend 950PR/Ascend 950DT，T支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t。U支持的数据类型为：int8_t、int16_t、int32_t、int64_t。
<!-- end id8 -->

## 返回值说明<a name="section194321251175110"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

-   操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。

-   对于逻辑位移（无符号数据类型），如果位移量大于数据类型位宽，则输出为0。
-   对于算数位移（有符号数据类型），如果src0小于0，src1大于0，并且位移量大于数据类型位宽，则输出-1；如果src0大于0，并且位移量大于数据类型位宽，则输出0。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
AscendC::ShiftRight(dstLocal, srcLocal0, srcLocal1, 512);
```

结果示例如下：

```plain
输入数据(srcLocal0): [1 2 3 ... 512]
输入数据(srcLocal1): [2 2 2 ... 2]
输出数据(dstLocal): [0 0 0 1 1 1 1 ... 128]
```
