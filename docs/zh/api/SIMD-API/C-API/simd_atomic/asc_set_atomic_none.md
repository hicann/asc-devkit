# asc_set_atomic_none

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

清空原子操作的状态。一般和[asc_set_atomic_add](asc_set_atomic_add.md)，[asc_set_atomic_max](asc_set_atomic_max.md)，[asc_set_atomic_min](asc_set_atomic_min.md)接口配合使用，用于在完成原子操作后关闭原子操作，避免影响后续功能。

## 函数原型

```c++
__aicore__ inline void asc_set_atomic_none()
```

## 参数说明

无
## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

```c++
//total_length指参与搬运的数据总长度。dst是外部输入的int16_t类型的GM内存。
constexpr uint32_t total_length = 256;
__ubuf__ int16_t src0[total_length];
__ubuf__ int16_t src1[total_length];
asc_set_atomic_add_int16();
asc_copy_ub2gm_sync(dst, src0, total_length * sizeof(int16_t));
asc_copy_ub2gm_sync(dst, src1, total_length * sizeof(int16_t));
asc_set_atomic_none();
```

结果示例：

```
输入数据src0：[1, 1, 1, ..., 1]  // int16_t类型
输入数据src1：[2, 2, 2, ..., 2]  // int16_t类型
输出数据dst：[3, 3, 3, ..., 3]   // int16_t类型
```