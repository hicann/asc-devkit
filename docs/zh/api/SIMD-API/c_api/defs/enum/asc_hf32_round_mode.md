# asc_hf32_round_mode

asc_hf32_round_mode用于Mmad计算开启HF32模式时由FP32舍入到HF32的舍入模式管理策略，在调用[asc_set_hf32_round_mode](../../cube_compute/asc_set_hf32_round_mode.md)时使用。

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
