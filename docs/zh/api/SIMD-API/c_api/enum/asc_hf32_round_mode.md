# asc_hf32_round_mode

asc_hf32_round_mode用于Mmad计算开启HF32模式时由FP32舍入到HF32的舍入模式管理策略，在调用[asc_set_hf32_round_mode](../cube_compute/asc_set_hf32_round_mode.md)时使用。

## 枚举类具体定义

```cpp
enum class asc_hf32_round_mode : uint8_t{
    NEAREST_ZERO,
    NEAREST_EVEN
};
```

## 枚举值详解

|枚举值|HF32 舍入模式管理策略|
|------|-----------|
| NEAREST_ZERO | FP32将以向零靠近的方式四舍五入为HF32。 |
| NEAREST_EVEN | FP32将以最接近偶数的方式四舍五入为HF32。 |
