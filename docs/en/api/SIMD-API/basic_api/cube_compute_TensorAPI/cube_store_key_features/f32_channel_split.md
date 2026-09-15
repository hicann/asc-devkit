# F32 Channel Split

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T17:08:29.506Z -->

F32 Channel Split is a channel splitting capability used when moving cube computation results out. When both the source type and destination type are `float`, the destination format is NZ, and `CopyL0C2GMTrait::enableChannelSplit` or `CopyL0C2UBTrait::enableChannelSplit` is set to `true`, the hardware splits a 16×16 fractal cube into two independent 16×8 small-Z fractal matrices.

After this capability is enabled, C0 of the destination NZ output is processed as 8, and the size in the N direction must be a multiple of 8. As shown in the following figures, when the source operand shape is `[64, 32]`, the moved-out result is split into 16 independent 16×8 fractal matrices. When the source operand shape is `[64, 24]`, the moved-out result is split into 3 columns of 16×8 fractal matrices.

**Figure 1**  F32 Channel Split diagram 1

![F32 Channel Split diagram 1](../../../../figures/Fixpipe_Channel_Split.png)

**Figure 2**  F32 Channel Split diagram 2

![F32 Channel Split diagram 2](../../../../figures/Fixpipe_Channel_Split_with_dirty.png)

## Call Form

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

## Constraints

- F32 Channel Split supports only `float` for both the source type and the destination type, with the destination format being NZ.
- When F32 Channel Split is enabled, `unitFlag` cannot be enabled at the same time, and the size in the **N** direction must be a multiple of 8.
