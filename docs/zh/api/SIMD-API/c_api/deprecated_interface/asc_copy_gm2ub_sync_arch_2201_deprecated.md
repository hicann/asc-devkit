# asc_copy_gm2ub_sync_arch_2201（废弃）

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

头文件路径为：`"c_api/composite/vector_compute_composite.h"`。

**`asc_copy_gm2ub_sync`接口已废弃，请使用[对应的非同步接口](../vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_2201.md)和同步接口[asc_sync](../sync/asc_sync.md)替代。**

将数据从Global Memory (GM)搬运到Unified Buffer（UB）。

## 函数原型

- 同步计算

```cpp
__aicore__ inline void asc_copy_gm2ub_sync(__ubuf__ void* dst, __gm__ void* src, uint32_t size)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | 目的UB地址。 |
| src | 输入 | 源GM地址。 |
| size | 输入 | 搬运数据大小（字节）。 |

## 返回值说明

无

## 流水类型

PIPE_MTE2

## 约束说明

- 各存储单元的空间大小和对齐要求请参考[存储单元说明](../general_description_and_constraints.md#存储单元说明)。
- 如果本指令与其他指令存在UB地址重叠，必须插入同步指令[asc_sync_notify](../sync/asc_sync_notify.md)和[asc_sync_wait](../sync/asc_sync_wait.md)，保证多个指令串行化，防止出现异常数据。
- 同步计算包含同步等待。
- 当采用前n个数据搬运接口时，搬运数据大小要求32字节对齐。

## 调用示例

```cpp
//total_length指参与搬运的数据总长度。src是外部输入的half类型的GM内存。
constexpr uint64_t total_length = 128;
__ubuf__ half dst[total_length];
asc_copy_gm2ub_sync(dst, src, total_length * sizeof(half));
```
