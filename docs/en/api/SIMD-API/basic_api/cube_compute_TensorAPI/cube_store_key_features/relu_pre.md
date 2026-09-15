# In-line Relu

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T17:10:30.771Z -->

In-line Relu is an in-line activation capability applied when cube computation results are moved out of the L0C Buffer to Global Memory or Unified Buffer. The Tensor API controls whether to enable this capability through `CopyL0C2GMTrait::enableRelu` or `CopyL0C2UBTrait::enableRelu`.

## Description

The default value of `enableRelu` is `false`, which means the in-place Relu is disabled. When it is set to `true`, Normal Relu activation is performed on the output data during the copy-out process.

```cpp
constexpr CopyL0C2GMTrait l0c2gmTrait = {
    RoundMode::DEFAULT,
    true,
    false
};

struct CopyL0C2GMTraitCustom {
    using TraitType = CopyL0C2GMTrait;
    static constexpr const TraitType value = l0c2gmTrait;
};

FixpipeParams params;
auto atom = MakeCopy(CopyL0C2GM{}, CopyL0C2GMTraitCustom{}).with(params);
Copy(atom, gm, l0c);
```

The copy from the L0C Buffer to the Unified Buffer is also controlled by `CopyL0C2UBTrait::enableRelu`:

```cpp
constexpr CopyL0C2UBTrait l0c2ubTrait = {
    RoundMode::DEFAULT,
    true,
    false,
    DUAL_DST_DISABLE
};

struct CopyL0C2UBTraitCustom {
    using TraitType = CopyL0C2UBTrait;
    static constexpr const TraitType value = l0c2ubTrait;
};

FixpipeParams params;
auto atom = MakeCopy(CopyL0C2UB{}, CopyL0C2UBTraitCustom{}).with(params);
Copy(atom, ub, l0c);
```

## Supported Scope

Based on the capabilities of the underlying Fixpipe, the in-place Relu includes modes such as Normal Relu, Leaky Relu, and PRelu. Currently, the Tensor API exposes the Normal Relu capability through `enableRelu` for L0C Buffer-to-Global Memory/Unified Buffer copies, while in-place PRelu is not supported.

## Constraints

- `enableRelu` is a compile-time trait configuration and is disabled by default.
- The quantization and ReLU parameters cannot be `inf`, `nan`, or denormal numbers.
- The current Tensor API does not support in-path PRelu.
