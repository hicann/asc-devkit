# asc_clear_nthbit

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

位操作函数，用于将一个uint64_t整数bits的第idx位设置为0。

## 函数原型

```cpp
__aicore__ inline uint64_t asc_clear_nthbit(uint64_t bits, int64_t idx)
```

## 参数说明

|参数名|输入/输出|描述|
|------------|------------|-----------|
| bits    | 输入     | 表示需要修改的值。   |
| idx     | 输入     | 位索引，表示需要设置为0的位的位置。|

## 返回值说明

修改后的uint64_t整数。

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

```cpp
uint64_t bits = 0x7;
int64_t idx = 0x2;
uint64_t res = asc_clear_nthbit(bits, idx);
```