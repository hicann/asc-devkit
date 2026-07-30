# asc_sflbits

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

## 功能说明

计算一个int64_t类型数字的二进制中，从最高数值位开始与符号位相同的连续比特位的个数。

- 例：int64_t value = 0x0f00000000000000;

  符号位为0，从最高数值位开始往后（不包含符号位）与符号位相同的连续比特数为3，故返回3。

## 函数原型

```cpp
__aicore__ inline int64_t asc_sflbits(int64_t value)
```

## 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| value | 输入 | 输入数据。 |

## 返回值说明

返回从最高数值位开始和符号位相同的连续比特位的个数，数据类型为int64_t。

## 流水类型

PIPE_S

## 约束说明

当输入是-1（比特位全1）或者0（比特位全0）时，返回-1。

## 调用示例

```cpp
int64_t val = 0x000f00ff0ff0ffff;
int64_t res = 0;
res = asc_sflbits(val);    // 返回11
```