---
okftype: reference.constant
description: "定义常量 ASC_VDEQ_SIZE 的取值与用途：该常量固定为16，表示 asc_set_deq_scale 中 scale_arr、offset_arr 和 sign_mode_arr 三个数组的元素个数。"
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_set_deq_scale.md
    kind: related
    condition: "该常量为 asc_set_deq_scale 的 quant 相关三个数组参数提供长度"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "定义位置由实现侧 impl/c_api/instr_impl/npu_arch_2201/utils_impl/utils_impl.h 迁入公共头文件 include/c_api/defs/constant.h（第17行），取值仍为16；CANN 9.2.0 Release Notes 公共头文件节明确记载 ASC_VDEQ_SIZE 由实现侧定义迁入 c_api/defs/constant.h 且已有接口原型中的用途不变。"
---
# ASC_VDEQ_SIZE

ASC_VDEQ_SIZE表示[asc_set_deq_scale](../../vector_compute/vector_fused/asc_set_deq_scale.md)接口中scale_arr、offset_arr和sign_mode_arr数组的元素个数，值为16。

头文件路径：`c_api/defs/constant.h`。

## 常量定义

```cpp
constexpr uint8_t ASC_VDEQ_SIZE = 16;
```
