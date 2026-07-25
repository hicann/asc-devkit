# 随路Relu

随路Relu是矩阵计算结果从L0C Buffer搬出到Global Memory或Unified Buffer过程中的随路激活能力。Tensor API通过`CopyL0C2GMTrait::enableRelu`或`CopyL0C2UBTrait::enableRelu`控制是否使能该能力。

## 功能说明

`enableRelu`默认值为`false`，表示不使能随路Relu。设置为`true`时，搬出过程中对输出数据执行Normal Relu激活。

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

L0C Buffer到Unified Buffer搬运同样通过`CopyL0C2UBTrait::enableRelu`控制：

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

## 支持范围

根据底层Fixpipe的能力，随路Relu包含Normal Relu、Leaky Relu和PRelu等模式。当前Tensor API的L0C Buffer到Global Memory/Unified Buffer搬运通过`enableRelu`暴露Normal Relu能力，当前不支持随路PRelu。

## 约束说明

- `enableRelu`为编译期trait配置，默认不使能。
- 量化和Relu参数不能为`inf`、`nan`或非规格化数。
- 当前Tensor API不支持随路PRelu。
