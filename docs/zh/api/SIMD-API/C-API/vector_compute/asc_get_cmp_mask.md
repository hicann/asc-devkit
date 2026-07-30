# asc_get_cmp_mask

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

## 功能说明

此接口用于获取Compare操作的比较结果。

## 函数原型

```cpp
__aicore__ inline void asc_get_cmp_mask(__ubuf__ void* dst)
```

## 参数说明

|参数名|输入/输出|描述|
| ------------ | ------------ | ------------ |
|dst|输出|存放比较操作结果的地址。|

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

需和Compare操作配合使用。

## 调用示例

```cpp
constexpr uint32_t total_length = 128;
__ubuf__ int8_t dst[total_length];
...     // 进行Compare操作
asc_get_cmp_mask(dst);
```