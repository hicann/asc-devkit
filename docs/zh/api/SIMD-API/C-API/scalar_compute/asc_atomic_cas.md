# asc_atomic_cas

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

对Global Memory中address指向的元素执行原子比较赋值操作，如果address上的数值等于指定数值compare，则对address赋值为指定数值val，否则address的数值不变。

## 函数原型

```cpp
__aicore__ inline int32_t  asc_atomic_cas(__gm__ int32_t  *address, int32_t  compare, int32_t  val)

__aicore__ inline uint32_t asc_atomic_cas(__gm__ uint32_t *address, uint32_t compare, uint32_t val)

__aicore__ inline float    asc_atomic_cas(__gm__ float    *address, float    compare, float    val)

__aicore__ inline int64_t  asc_atomic_cas(__gm__ int64_t  *address, int64_t  compare, int64_t  val)

__aicore__ inline uint64_t asc_atomic_cas(__gm__ uint64_t *address, uint64_t compare, uint64_t val)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| address | 输出 | Global Memory的地址。 |
| compare | 输入 | 源操作数，做比较的值。 |
| val | 输入 | 源操作数，用于赋值的值。 |

## 返回值说明

address地址中计算前的原始数据。

## 流水类型

PIPE_S

## 约束说明

- 在开启编译器自动同步功能的前提下，编译器能够自动在PIPE_MTE2/PIPE_MTE3与PIPE_S之间插入同步。但是，asc_atomic_cas为标量计算，在读写GM时如果与搬运单元（MTE2/MTE3）存在数据依赖，编译器却无法自动插入同步，开发者需要根据实际情况手动插入同步。
- Scalar原子操作会绕过DCache，需要调用[asc_dcci](../cache_ctrl/asc_dcci.md)接口确保GM与DCache的一致性。

## 调用示例

```cpp
// dst为外部输入的int32_t类型的GM内存，此示例的初始值为10
asc_dcci_entire_all();            // 手动同步
int32_t old = asc_atomic_cas(dst, 10, 20);  // 计算后dst = 20, old = 10
```
