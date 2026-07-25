# L0C Buffer到Unified Buffer双目标模式

L0C Buffer到Unified Buffer双目标模式用于将同一AI Core内L0C Buffer的矩阵结果拆分后，同时写入两个Vector Core各自的Unified Buffer。一个AI Core内包含一个Cube Core和两个Vector Core，启用双目标模式后，源矩阵会按指定维度拆分为两部分，前半部分写入SUB BLOCK0，后半部分写入SUB BLOCK1。

Tensor API通过`CopyL0C2UBTrait::dualDstCtl`控制双目标模式。

`dualDstCtl`取值如下：

|取值|说明|
|--------|--------|
|`DUAL_DST_DISABLE`|单目标模式。整个矩阵写入默认目标Unified Buffer。|
|`DUAL_DST_SPLIT_M`|双目标模式，按M维度拆分。源矩阵拆分为两个形状为`M / 2 * N`的矩阵，分别写入两个Unified Buffer。|
|`DUAL_DST_SPLIT_N`|双目标模式，按N维度拆分。源矩阵拆分为两个形状为`M * N / 2`的矩阵，分别写入两个Unified Buffer。|

## 使用示例

```cpp
constexpr CopyL0C2UBTrait l0c2ubTrait = {
    RoundMode::DEFAULT,
    false,
    false,
    DUAL_DST_SPLIT_M
};

struct CopyL0C2UBTraitCustom {
    using TraitType = CopyL0C2UBTrait;
    static constexpr const TraitType value = l0c2ubTrait;
};

FixpipeParams params;
auto atom = MakeCopy(CopyL0C2UB{}, CopyL0C2UBTraitCustom{}).with(params);
Copy(atom, ub, l0c);
```

## NZ2NZ双目标模式

在NZ2NZ搬运场景下，双目标模式支持按M维度拆分和按N维度拆分。结合底层搬运接口说明如下：

N方向切分示例：

- `nSize = 32`，表示源NZ矩阵中待搬运矩阵在N方向上的大小为32个元素。
- `mSize = 48`，表示源NZ矩阵中待搬运矩阵在M方向上的大小为48个元素。
- `srcStride = 64`，表示源NZ矩阵中相邻Z排布的起始地址偏移为`64 * C0_SIZE`。
- `dstStride = 64 * C0`，表示目的NZ矩阵中相邻Z排布的起始地址偏移为`64 * 16`个元素。

M方向切分示例：

- `nSize = 32`，表示源NZ矩阵中待搬运矩阵在N方向上的大小为32个元素。
- `mSize = 24`，表示每个目标Unified Buffer接收的M方向大小，源矩阵M方向总大小为48个元素。
- `srcStride = 64`，表示源NZ矩阵中相邻Z排布的起始地址偏移为`64 * C0_SIZE`。
- `dstStride = 40 * C0`，表示目的NZ矩阵中相邻Z排布的起始地址偏移为`40 * 16`个元素。

**图1**  NZ2NZ双目标搬运模式设置示意图

![NZ2NZ双目标搬运模式设置示意图](../../../../figures/nz2nz_dual_target_movement_mode_setting.png)

## NZ2ND双目标模式

在NZ2ND搬运场景下，双目标模式也支持按M维度拆分和按N维度拆分。结合底层搬运接口说明如下：

- `ndNum = 2`，表示源NZ矩阵的数目为2。
- `nSize = 32`，表示源NZ矩阵在N方向上的大小为32个元素。
- `mSize = 48`，表示源NZ矩阵在M方向上的大小为48个元素。
- `srcStride = 64`，表示源NZ矩阵中相邻Z排布的起始地址偏移为`64 * C0_SIZE`。
- `dstStride = 64`，表示目的ND矩阵每一行中的元素个数为64。
- `srcNdStride = 240`，表示不同NZ矩阵起始地址之间的间隔为`240 * C0_SIZE`。
- `dstNdStride = 4096`，表示目的相邻ND矩阵起始地址之间的偏移为4096个元素。

**图2**  NZ2ND双目标搬运模式设置示意图

![NZ2ND双目标搬运模式设置示意图](../../../../figures/enable_nz2nd_param_dual_in_mode_setting.png)

## 约束说明

- 双目标模式仅支持L0C Buffer到Unified Buffer通路。
- 双目标模式支持普通搬运模式NZ2NZ和NZ2ND搬运场景。
- 按M维度拆分时，M必须为2的倍数。
- 按N维度拆分时，N必须为32的倍数。
