# asc_channel_pad_mode

`asc_channel_pad_mode`用于表示GM到L1 Buffer搬运时，每个32字节通道块的填充或压缩模式，作为[asc_copy_gm2l1](../../cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_highdim_split_arch_3510.md)接口的`pad_mode`参数类型。

头文件路径为：`"c_api/defs/enum.h"`。

## 枚举类具体定义

```cpp
enum class asc_channel_pad_mode : uint8_t {
    NONE = 0,
    EXPAND_1B = 1,
    EXPAND_2B = 2,
    EXPAND_4B = 3,
    EXPAND_8B = 4,
    EXPAND_16B = 5,
    COMPACT_4B = 6,
    COMPACT_8B = 7,
    COMPACT_16B = 8,
};
```

## 枚举值详解

**表1** 枚举值说明

| 枚举值 | 取值 | 说明 |
| --- | --- | --- |
| `NONE` | 0 | 不执行通道填充或压缩。 |
| `EXPAND_1B` | 1 | 每1字节源数据后插入31字节填充值。 |
| `EXPAND_2B` | 2 | 每2字节源数据后插入15个填充值。 |
| `EXPAND_4B` | 3 | 每4字节源数据后插入14个填充值。 |
| `EXPAND_8B` | 4 | 每8字节源数据后插入12个填充值。 |
| `EXPAND_16B` | 5 | 每16字节源数据后插入8个填充值。 |
| `COMPACT_4B` | 6 | 每32字节源数据保留低4字节，丢弃高28字节。 |
| `COMPACT_8B` | 7 | 每32字节源数据保留低8字节，丢弃高24字节。 |
| `COMPACT_16B` | 8 | 每32字节源数据保留低16字节，丢弃高16字节。 |
