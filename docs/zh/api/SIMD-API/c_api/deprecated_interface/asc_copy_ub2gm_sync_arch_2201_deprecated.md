---
okftype: reference.api
description: "已废弃的同步版 UB 到 GM 搬运接口，按字节数搬运连续数据并在返回前完成同步等待，面向 Atlas A3 与 Atlas A2，替代方案为 asc_copy_ub2gm 加 asc_sync。"
tag:
  - roles: [data_movement]
  - operations: [store]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "存储单元空间大小与对齐要求"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "替代方案中的同步部分"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md
    kind: requires
    condition: "与其他指令存在 UB 地址重叠时需插入的同步指令"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_wait.md
    kind: requires
    condition: "与其他指令存在 UB 地址重叠时需插入的同步指令"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm_arch_2201.md
    kind: replaced_by
    condition: "正文声明的替代非同步接口"
version: ">=8.5.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "9.2.0 起 include 标记 ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_copy_ub2gm and asc_sync)，正文同步给出替代方案；声明保留，属 deprecated 而非 removed，页面未设置可用上限。"
---
# asc_copy_ub2gm_sync（NPU架构版本2201，废弃）

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：不支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/composite/vector_compute_composite.h"`。

**`asc_copy_ub2gm_sync`接口已废弃，请使用[对应的非同步接口](../vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm_arch_2201.md)和同步接口[asc_sync](../sync/intra_core_sync/asc_sync.md)替代。**

将数据从Unified Buffer（UB）搬运到Global Memory (GM)。

## 函数原型

- 同步计算

```cpp
__aicore__ inline void asc_copy_ub2gm_sync(__gm__ void* dst, __ubuf__ void* src, uint32_t size)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | 目的GM地址。 |
| src | 输入 | 源UB地址。 |
| size | 输入 | 搬运数据大小（字节）。 |

## 返回值说明

无

## 流水类型

PIPE_MTE3

## 约束说明

- 各存储单元的空间大小和对齐要求请参考[存储单元说明](../general_description_and_constraints.md#存储单元说明)。
- 如果本指令与其他指令存在UB地址重叠，必须插入同步指令[asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md)和[asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md)，保证多个指令串行化，防止出现异常数据。
- 同步计算包含同步等待。
- 当采用前n个数据搬运接口时，搬运数据大小要求32字节对齐。

## 调用示例

```cpp
// total_length指参与搬运的数据总长度。dst是外部输入的half类型的GM内存。
constexpr uint64_t total_length = 128;
__ubuf__ half src[total_length];
asc_copy_ub2gm_sync(dst, src, total_length * sizeof(half));
```
