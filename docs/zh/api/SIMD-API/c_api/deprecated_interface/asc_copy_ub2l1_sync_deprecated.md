# asc_copy_ub2l1_sync（废弃）

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

头文件路径为：`"c_api/composite/vector_compute_composite.h"`。

**`asc_copy_ub2l1_sync`接口已废弃，请使用[asc_copy_ub2l1](../vector_datamove/asc_copy_ub2l1.md)和同步接口[asc_sync](../sync/asc_sync.md)替代。**

将一段连续数据从Unified Buffer（UB）搬运到L1 Buffer，并在搬运完成后执行同步。

本接口仅在AIV上生效。

## 函数原型

```cpp
__aicore__ inline void asc_copy_ub2l1_sync(__cbuf__ void* dst, __ubuf__ void* src, uint32_t size)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | 目的L1 Buffer的起始地址。 |
| src | 输入 | 源UB的起始地址。 |
| size | 输入 | 搬运数据大小，单位为字节。 |

## 返回值说明

无

## 流水类型

PIPE_MTE3

## 约束说明

- 本接口在非AIV上调用直接返回。
- 各存储单元的空间大小和对齐要求请参考[存储单元说明](../general_description_and_constraints.md#存储单元说明)。
- 如果需要执行多条asc_copy_ub2l1指令，且asc_copy_ub2l1指令的目的地址存在重叠，需要插入同步指令，保证多个asc_copy_ub2l1指令的串行化，防止出现异常数据。
- 同步计算包含同步等待。

## 调用示例

```cpp
constexpr uint32_t size = 32;
__ubuf__ uint8_t src[size];
__cbuf__ uint8_t dst[size];
asc_copy_ub2l1_sync(dst, src, size);
```
