# asc_channel_pad_mode

头文件路径为：`"c_api/defs/enum.h"`。

asc_channel_pad_mode用于表示从GM搬运到L1 Buffer时，每个32字节通道块的填充或压缩模式，可用于[asc_copy_gm2l1](../../cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_highdim_split_arch_3510.md)高维切分数据搬运接口。该模式为硬件`pad_func_mode`的类型化定义，对每个32字节通道数据块生效，取值范围为[0, 8]。

`EXPAND_*`为填充模式，在目的数据中插入padding值。通道填充值通过[asc_set_gm2l1_padding](../../cube_datamove/asc_set_gm2l1_padding.md)设置。

`COMPACT_*`为删除模式，从每个32字节源数据块中移除高位部分数据，仅保留低位有效区域。

该枚举控制的是通道块内部的填充或删除，区别于通过[asc_set_gm2l1_pad](../../cube_datamove/asc_set_gm2l1_pad.md)配置的左右填充。

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
    COMPACT_16B = 8
};
```

## 枚举值详解

**表1** 枚举值说明

| 枚举值 | 取值 | 说明 |
| --- | --- | --- |
| NONE | 0 | 不执行通道填充或压缩。 |
| EXPAND_1B | 1 | 每1字节源数据后插入31字节填充值，其中插入的每个padding的位宽为b8。 |
| EXPAND_2B | 2 | 每2字节源数据后插入15个填充值，其中插入的每个padding的位宽为b16。 |
| EXPAND_4B | 3 | 每4字节源数据后插入14个填充值，其中插入的每个padding的位宽为b16。 |
| EXPAND_8B | 4 | 每8字节源数据后插入12个填充值，其中插入的每个padding的位宽为b16。 |
| EXPAND_16B | 5 | 每16字节源数据后插入8个填充值，其中插入的每个padding的位宽为b16。 |
| COMPACT_4B | 6 | 每32字节源数据保留低4字节，丢弃高28字节，只保留4个最低有效位字节。 |
| COMPACT_8B | 7 | 每32字节源数据保留低8字节，丢弃高24字节，只保留8个最低有效位字节。 |
| COMPACT_16B | 8 | 每32字节源数据保留低16字节，丢弃高16字节，只保留16个最低有效位字节。 |
