# F32 Channel Split

F32 Channel Split是矩阵计算结果搬出过程中的通道拆分能力。当源类型和目的类型均为`float`、目的格式为NZ，且`CopyL0C2GMTrait::enableChannelSplit`或`CopyL0C2UBTrait::enableChannelSplit`设置为`true`时，硬件会将16×16的分形矩阵拆分为两个独立的16×8小Z分形矩阵。

启用该能力后，目的NZ输出的C0按8处理，N方向大小必须为8的倍数。如下图所示，当源操作数shape为`[64, 32]`时，搬出结果会被拆分为16个独立的16×8分形矩阵。当源操作数shape为`[64, 24]`时，搬出结果会被拆分为3列16×8分形矩阵。

**图1**  F32 Channel Split示意图1

![F32 Channel Split示意图1](../../../../figures/Fixpipe_Channel_Split.png)

**图2**  F32 Channel Split示意图2

![F32 Channel Split示意图2](../../../../figures/Fixpipe_Channel_Split_with_dirty.png)

## 调用形式

```cpp
using namespace AscendC::Te;

auto l0c = MakeTensor(MakeMemPtr(l0cAddr), MakeFrameLayout<NZLayoutPtn, 16>(m, n));
auto gm = MakeTensor(MakeMemPtr(gmAddr), MakeFrameLayout<NZLayoutPtn, float>(m, n));

constexpr CopyL0C2GMTrait l0c2gmTrait = {
    RoundMode::DEFAULT,
    false,
    true
};

struct CopyL0C2GMTraitCustom {
    using TraitType = CopyL0C2GMTrait;
    static constexpr const TraitType value = l0c2gmTrait;
};

FixpipeParams params;
auto atom = MakeCopy(CopyL0C2GM{}, CopyL0C2GMTraitCustom{}).with(params);
Copy(atom, gm, l0c);
```

## 约束说明

- F32 Channel Split仅支持源类型和目的类型均为`float`，目的格式为NZ。
- 启用F32 Channel Split时，不能同时使能`unitFlag`，N方向大小必须为8的倍数。
