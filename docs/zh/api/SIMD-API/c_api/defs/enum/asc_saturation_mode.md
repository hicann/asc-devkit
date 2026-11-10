---
okftype: reference.enum
description: "饱和控制所作用的模式类型枚举，供asc_set_saturation_flag与asc_get_saturation_flag按浮点、FP8、整数和类型转换场景开关饱和，其中CAST档仅在全局饱和策略下生效。"
tag:
  - roles: [configuration]
  - traits: [saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_override_strategy.md
    kind: related
    condition: "CAST档仅在饱和策略为USE_GLOBAL时生效"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_get_saturation_flag.md
    kind: related
    condition: "该接口按本枚举查询对应场景的饱和开关"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_saturation_flag.md
    kind: related
    condition: "该接口以本枚举指定要开关饱和的场景"
version: ">=9.2.0"
version_changes: []
---
# asc_saturation_mode

asc_saturation_mode用于表示饱和控制所作用的模式类型，在调用[asc_set_saturation_flag](../../spr/asc_set_saturation_flag.md)和[asc_get_saturation_flag](../../spr/asc_get_saturation_flag.md)时使用。

头文件路径为：`"c_api/defs/enum.h"`。

## 枚举类具体定义

```cpp
enum class asc_saturation_mode : uint8_t {
    FLOAT = 48,
    FLOAT8 = 50,
    INT = 53,
    CAST = 59
};
```

## 枚举值详解

**表1** 枚举值说明

| 枚举值 | 说明 |
| ------ | ----------- |
| FLOAT | 浮点计算及部分浮点精度转换场景的饱和控制，浮点数计算时支持half数据类型；浮点数精度转换时支持hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、half、bfloat16_t数据类型。 |
| FLOAT8 | fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t相关场景的饱和控制。 |
| INT | 整数计算溢出时的饱和/截断控制。 |
| CAST | 浮点转整数或整数转整数时的精度转换饱和控制。仅在饱和策略为[asc_override_strategy::USE_GLOBAL](asc_override_strategy.md)时生效。 |

各模式下开启/关闭饱和的行为差异，见[asc_set_saturation_flag](../../spr/asc_set_saturation_flag.md)中`enable_sat`参数说明。
