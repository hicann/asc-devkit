---
okftype: reference.enum
description: "Mmad开启HF32模式后FP32舍入到HF32的舍入模式管理策略枚举，供asc_set_hf32_round_mode配置，含平局远离零的NEAREST_AWAY、平局向偶数的NEAREST_EVEN和兼容别名NEAREST_ZERO。"
tag:
  - roles: [configuration]
  - traits: [rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute_aux_config/asc_set_hf32_round_mode.md
    kind: related
    condition: "开启HF32模式后由该接口消费本枚举"
version: ">=9.2.0"
version_changes:
  - since: "9.2.0"
    kind: enum_value_changed
    scope: "enum_value:asc_hf32_round_mode.NEAREST_ZERO"
    scope_basis: resolved
    note: "NEAREST_ZERO 由向最接近值舍入且平局向零方向的独立取值改为等同 NEAREST_AWAY 的兼容别名（写作 NEAREST_ZERO = NEAREST_AWAY，同值保留），不再表示独立舍入行为；旧写法行为随之变化，新代码应使用 NEAREST_AWAY 或 NEAREST_EVEN。"
---
# asc_hf32_round_mode

asc_hf32_round_mode用于Mmad计算开启HF32模式时由FP32舍入到HF32的舍入模式管理策略，在调用[asc_set_hf32_round_mode](../../cube_compute/mmad_compute_aux_config/asc_set_hf32_round_mode.md)时使用。

头文件路径为：`"c_api/defs/enum.h"`。

## 枚举类具体定义

```cpp
enum class asc_hf32_round_mode : uint8_t {
    NEAREST_AWAY = 0,
    NEAREST_EVEN = 1,
    NEAREST_ZERO = NEAREST_AWAY  // Compatible with the old name; the actual behavior is equivalent to NEAREST_AWAY.
};
```

## 枚举值详解

|枚举值|HF32 舍入模式管理策略|
|------|-----------|
| NEAREST_AWAY | FP32将以向最接近的值舍入，平局时远离零的方式舍入为HF32。 |
| NEAREST_EVEN | FP32将以向最接近的值舍入，平局时向偶数舍入的方式舍入为HF32。 |
| NEAREST_ZERO | 用于兼容旧版实现，不推荐使用，实际行为等同于`NEAREST_AWAY`。 |
