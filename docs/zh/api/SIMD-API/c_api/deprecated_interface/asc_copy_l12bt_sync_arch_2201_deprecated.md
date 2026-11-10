---
okftype: reference.api
description: "废弃接口页：2201 架构下 asc_copy_l12bt_sync 以目的地址、源地址与字节数同步把 L1 Buffer 的 Bias 数据搬到 BiasTable Buffer，前 n 个数据搬运要求 32 字节对齐，建议改用非同步 asc_copy_l12bt 加 asc_sync。"
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [aligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12bt_arch_2201.md
    kind: replaced_by
    condition: "建议改用对应的非同步接口并显式调用 asc_sync"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "空间、对齐与地址重叠约束按其存储单元说明与通用地址重叠约束"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "替代方案中承担同步等待"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "include 头以 ASC_DEPRECATED(9.2.0) 标记该 2201 同步形式，替代路径是非同步 asc_copy_l12bt 加 asc_sync；接口未移除。"
---
# asc_copy_l12bt_sync（NPU架构版本2201，废弃）

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

头文件路径为：`"c_api/composite/cube_datamove_composite.h"`。

**`asc_copy_l12bt_sync`接口已废弃，请使用[对应的非同步接口](../cube_datamove/cube_compute_load/asc_copy_l12bt_arch_2201.md)和同步接口[asc_sync](../sync/intra_core_sync/asc_sync.md)替代。**

将数据从L1 Buffer搬运到BiasTable Buffer中，BiasTable Buffer用于存放矩阵计算中的Bias。

## 函数原型

- 同步搬运

    ```cpp
    __aicore__ inline void asc_copy_l12bt_sync(uint64_t dst, __cbuf__ void* src, uint32_t size)
    ```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | 目的操作数起始地址。 |
| src | 输入 | 源操作数起始地址。 |
| size | 输入 | 搬运数据大小（字节）。|

## 返回值说明

无

## 流水类型

PIPE_MTE1

## 约束说明

- 各存储单元的空间大小和对齐要求请参考[存储单元说明](../general_description_and_constraints.md#存储单元说明)。
- 操作数地址重叠约束请参考[通用地址重叠约束](../general_description_and_constraints.md#通用地址重叠约束)。
- 当采用前n个数据搬运接口时，搬运数据大小要求32字节对齐。

## 调用示例

```cpp
// 同步接口按字节数连续搬运，此处搬运一个32字节数据块。
constexpr uint32_t copy_size = 32;
__cbuf__ half src[256];
__biasbuf__ half dst[256];
uint64_t dst_addr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst)) / 16;
asc_copy_l12bt_sync(dst_addr, src, copy_size);
```
