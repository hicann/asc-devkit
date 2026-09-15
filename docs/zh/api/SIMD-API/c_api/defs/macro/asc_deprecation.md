# ASC_DEPRECATED

## 功能说明

头文件路径为：`"c_api/defs/macro.h"`。

`ASC_DEPRECATED`用于为C API声明添加C++ `[[deprecated]]`属性。开发者编译调用被标记接口的代码时，编译器会生成告警，告警信息包含接口开始废弃的版本、计划移除日期和推荐使用的替代接口。

该宏只添加废弃属性和告警信息，不会阻止被标记接口参与编译，也不会在计划移除日期后自动删除接口。

## 宏定义

```cpp
#define ASC_DEPRECATED(version, removal_date, replacement)                                                  \
    [[deprecated("Deprecated since " #version ", Will be removed after " removal_date ", Use " #replacement \
                 " instead.")]]
```

## 参数说明

|参数名|描述|
|---|---|
|version|接口开始废弃的CANN版本号。宏会将该参数转换为字符串，传参时不加双引号，例如`9.2.0`。|
|removal_date|接口的计划移除日期。该参数须使用字符串形式，格式为`"YYYY/MM/DD"`，例如`"2027/09/07"`。|
|replacement|推荐使用的替代接口或替代方案。宏会将该参数转换为字符串，传参时不加双引号；存在多个替代接口时，可传入完整的替代方案描述。|

编译告警信息的格式如下：

```text
Deprecated since <version>, Will be removed after <removal_date>, Use <replacement> instead.
```

## 约束说明

- 使用本宏时，须将其置于需要标记为废弃的声明之前。
- `version`和`replacement`会通过预处理器的`#`运算符转换为字符串，参数中不应添加双引号；`removal_date`会直接拼接到告警信息中，必须使用双引号括起。
- `removal_date`仅用于生成告警信息，不控制接口的实际删除时间。

## 调用示例

以下代码使用本宏标记[asc_muls](../../deprecated_interface/asc_muls_deprecated.md)接口声明，并推荐使用[asc_mul_scalar_float2half_rn](../../reg_compute/reg_fused/asc_mul_scalar_float2half_rn.md)接口替代。

```cpp
#include "c_api/defs/macro.h"

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_scalar_float2half_rn)
__simd_callee__ inline void asc_muls(vector_half& dst, vector_float src, float value, vector_bool mask);
```

调用`asc_muls`时，编译器生成的告警信息如下：

```text
Deprecated since 9.2.0, Will be removed after 2027/09/07, Use asc_mul_scalar_float2half_rn instead.
```
