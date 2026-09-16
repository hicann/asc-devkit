# asc_dual_dst_mode

`asc_dual_dst_mode`用于表示L0C Buffer数据搬出时的双目标拆分模式，作为矩阵搬出接口中`dual_dst_ctrl`参数的类型。

头文件路径为：`"c_api/defs/enum.h"`。

## 枚举类具体定义

```cpp
enum class asc_dual_dst_mode : uint8_t {
    DUAL_DST_DISABLE = 0,
    DUAL_DST_SPLIT_M = 1,
    DUAL_DST_SPLIT_N = 2
};
```

## 枚举值详解

**表1** 枚举值说明

| 枚举值 | 取值 | 说明 |
| --- | --- | --- |
| DUAL_DST_DISABLE | 0 | 关闭双目标模式。 |
| DUAL_DST_SPLIT_M | 1 | 开启双目标模式，沿M方向拆分数据。 |
| DUAL_DST_SPLIT_N | 2 | 开启双目标模式，沿N方向拆分数据。 |
