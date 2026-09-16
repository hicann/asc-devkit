# asc_position_mode

`asc_position_mode`用于表示源或目的操作数在寄存器中的奇偶位置，供Reg类型转换和融合计算接口选择数据位置。常量`ASC_POSITION_EVEN`和`ASC_POSITION_ODD`可直接作为对应接口的位置参数。

头文件路径为：`"c_api/defs/enum.h"`。

## 枚举类具体定义

```cpp
enum class asc_position_mode {
    EVEN = 0,
    ODD
};

constexpr std::integral_constant<asc_position_mode, asc_position_mode::EVEN> ASC_POSITION_EVEN;
constexpr std::integral_constant<asc_position_mode, asc_position_mode::ODD> ASC_POSITION_ODD;
```

## 枚举值详解

**表1** 枚举值说明

| 枚举值 | 取值 | 说明 | 对应常量 |
| --- | --- | --- | --- |
| EVEN | 0 | 选择偶数位置。 | ASC_POSITION_EVEN |
| ODD | 1 | 选择奇数位置。 | ASC_POSITION_ODD |
