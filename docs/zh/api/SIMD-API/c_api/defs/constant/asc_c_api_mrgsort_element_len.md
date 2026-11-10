---
okftype: reference.constant
description: "定义常量 ASC_C_API_MRGSORT_ELEMENT_LEN 的取值与用途：该常量固定为4，表示 MrgSort4 支持的队列数量，用作 asc_mrgsort4 源队列地址数组和 asc_get_vms4_sr 结果数组的长度。"
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_sort/asc_get_vms4_sr.md
    kind: related
    condition: "该常量为 asc_get_vms4_sr 的 sorted_num 结果数组提供长度"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_sort/asc_mrgsort4.md
    kind: related
    condition: "该常量为 asc_mrgsort4 的 src 队列地址数组提供长度"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "定义位置由实现侧 impl/c_api/instr_impl/npu_arch_2201/utils_impl/utils_impl.h 迁入公共头文件 include/c_api/defs/constant.h（第16行），取值仍为4；CANN 9.2.0 Release Notes 公共头文件节明确记载 ASC_C_API_MRGSORT_ELEMENT_LEN 由实现侧定义迁入 c_api/defs/constant.h 且已有接口原型中的用途不变。"
---
# ASC_C_API_MRGSORT_ELEMENT_LEN

ASC_C_API_MRGSORT_ELEMENT_LEN表示MrgSort4操作支持的队列数量，值为4。该常量用于[asc_mrgsort4](../../vector_compute/vector_sort/asc_mrgsort4.md)接口的src数组长度，以及[asc_get_vms4_sr](../../vector_compute/vector_sort/asc_get_vms4_sr.md)接口的sorted_num数组长度。

头文件路径：`c_api/defs/constant.h`。

## 常量定义

```cpp
constexpr uint8_t ASC_C_API_MRGSORT_ELEMENT_LEN = 4;
```
