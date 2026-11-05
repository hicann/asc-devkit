# cast_round_mode

## 功能说明

头文件路径：`"tensor_api/experimental/utils/reg_enum.h"`。

`cast_round_mode`用于指定[cast](../type_conversion/cast.md)转换时的舍入方式。

## 类型定义

```cpp
enum class cast_round_mode {
    unknown = -1,
    rint,
    round,
    floor,
    ceil,
    trunc,
    odd,
    hybrid,
};
```

## 枚举值说明

| 枚举值 | 说明 |
| :-- | :-- |
| unknown | 未指定舍入方式，预留值。 |
| rint | 舍入到最近值，正好位于中间值时舍入到偶数。 |
| round | 舍入到最近值，正好位于中间值时远离0。 |
| floor | 向负无穷方向舍入。 |
| ceil | 向正无穷方向舍入。 |
| trunc | 向0方向舍入。 |
| odd | 向最近的奇数舍入。当前仅`float`转换为`half`时支持。 |
| hybrid | 混合舍入。当前用于`float`或`half`转换为`hifloat8_t`。 |

舍入模式的具体行为和类型组合支持范围请参考[舍入模式与饱和模式](../type_conversion/rounding_mode.md)以及[cast的数据类型说明](../type_conversion/cast.md#数据类型)。
