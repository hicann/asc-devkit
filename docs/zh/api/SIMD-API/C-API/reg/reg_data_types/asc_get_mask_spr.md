# asc_get_mask_spr

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

## 功能说明

从特殊寄存器SPR{MASK1, MASK0}读取mask值并根据数据类型格式返回对应的mask数据，MASK0、MASK1均为64bit的寄存器。

- 对于b16类型，会读取完整的128bit{MASK1, MASK0}数据，并将每bit复制为2bit并返回。

- 对于b32类型，会读取64bit MASK0数据，并将每bit复制为4bit并返回。

## 函数原型

```cpp
__simd_callee__ inline vector_bool asc_get_mask_spr_b16()
__simd_callee__ inline vector_bool asc_get_mask_spr_b32()
```

## 参数说明

无

## 返回值说明

特殊寄存器中读取的mask值。

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

```cpp
vector_bool mask_b16 = asc_get_mask_spr_b16();
vector_bool mask_b32 = asc_get_mask_spr_b32();
```