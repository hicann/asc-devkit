# Algorithm

Tensor API的Algorithm层为基于Tensor的数据搬运和矩阵计算提供统一的调用入口。其中，`copy`和`mmad`接口分别用于执行数据搬运和矩阵计算，`make_copy`和`make_mmad`接口用于构造对应的Atom原子对象。

## Operation、Traits和Atom

Tensor API将Trait（编译期特性）和可选Params（运行时参数）组合为Traits，Operation和Traits组合为Atom，并通过Atom提供数据搬运和矩阵计算能力：

- Operation是一个C++结构体，用于封装特定通路的指令级数据搬运或矩阵计算行为。
- Traits是一个与Operation关联的模板特化结构体，为Operation提供所必须的信息。
- Atom是数据搬运或矩阵计算的原子对象，由Operation和Traits组成。

### copy_traits

`copy_traits`用于描述数据搬运Operation、Trait以及绑定运行时参数后形成的类型组合。`copy_traits`的基础结构定义如下：

```cpp
template <typename CopyOperation, typename... CopyOpArgs>
struct copy_traits {};

template <typename CopyOperation, typename CopyTrait,
    typename CopyOperationWith, typename CopyTraitWith>
struct copy_traits<CopyOperation, CopyTrait, CopyOperationWith, CopyTraitWith> {
    using trait_type = typename CopyTrait::trait_type;
    static constexpr const trait_type default_trait = CopyTrait::value;

    template <typename Params>
    __aicore__ inline constexpr copy_traits<CopyOperationWith, CopyTraitWith>
        with(const Params& params) const;

    template <const trait_type& trait = default_trait, typename... Args>
    __aicore__ inline void copy_unpack(const Args&... args) const;
};
```

例如，`copy_gm_to_l1`表示从Global Memory到L1 Buffer的数据搬运Operation，可通过以下方式为其绑定对应的Trait。

```cpp
// 绑定可配置Trait
template <typename Trait>
struct copy_traits<copy_l0c_to_gm, Trait> : public copy_traits<copy_l0c_to_gm, Trait, copy_l0c_to_gm_with, Trait> {};

// 绑定默认Trait
template <>
struct copy_traits<copy_gm_to_l1> : public copy_traits<copy_gm_to_l1, gm_to_l1_trait_default> {};
```

#### 模板参数说明

**表1**  `copy_traits`模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| CopyOperation | 数据搬运Operation类型，用于指定执行的搬运通路。 |
| CopyOpArgs... | `copy_traits`的可变模板参数。基础模板中用于接收不同组合参数；在偏特化中依次对应`CopyTrait`、`CopyOperationWith`和`CopyTraitWith`。 |
| CopyTrait | 未绑定运行时参数时使用的Trait类型。 |
| CopyOperationWith | 绑定运行时参数后使用的Operation类型。 |
| CopyTraitWith | 绑定运行时参数后使用的Trait包装类型。 |

#### 成员说明

**表2**  `copy_traits`成员说明

| 成员 | 描述 |
| :--- | :--- |
| `trait_type` | 通路使用的具体Trait类型。 |
| `default_trait` | Trait包装类型提供的默认Trait值。 |
| `with` | 绑定运行时参数，返回包含对应Operation和Trait的`copy_traits`对象。 |
| `copy_unpack` | 展开已绑定参数并调用Operation的静态`copy`成员，完成数据搬运。 |

### copy_atom

`copy_atom`是数据搬运的原子对象，其定义如下：

```cpp
template <typename... Args>
struct copy_atom;

template <typename CopyOperation>
struct copy_atom<CopyOperation> : copy_atom<copy_traits<CopyOperation>> {};

template <typename... Args>
struct copy_atom<copy_traits<Args...>> : copy_traits<Args...> {
    using copy_traits_type = copy_traits<Args...>;
    using trait_type = typename copy_traits_type::trait_type;
    static constexpr const trait_type default_trait = copy_traits_type::default_trait;

    template <const trait_type& traits = default_trait, typename... Params>
    __aicore__ inline void call(const Params&... params) const;

    template <typename... TraitArgs>
    __aicore__ inline auto with(TraitArgs&&... args) const;
};
```

#### 模板参数说明

**表3**  `copy_atom`模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| Args... | `copy_atom`的可变模板参数。当传入Operation类型时，生成对应的`copy_traits`并构造Atom；当传入`copy_traits<Args...>`类型时，生成基于该Traits的Atom。 |
| CopyOperation | 数据搬运Operation类型，用于指定Atom执行的搬运通路。 |

#### 成员说明

**表4**  `copy_atom`成员说明

| 成员 | 描述 |
| :--- | :--- |
| `copy_traits_type` | `copy_traits`类型。 |
| `trait_type` | 通路使用的具体Trait类型。 |
| `default_trait` | 通路使用的默认Trait值。 |
| `call` | 使用Atom中的Operation执行数据搬运。 |
| `with` | 绑定运行时参数并返回新的Atom，不修改原Atom。 |

### mmad_traits

`mmad_traits`用于描述矩阵计算Operation、Trait及运行时参数绑定后的类型组合，其定义如下：

```cpp
template <typename MmadOperation, typename... MmadOpArgs>
struct mmad_traits {};

template <typename MmadOperation, typename MmadTrait,
    typename MmadOperationWith, typename MmadTraitWith>
struct mmad_traits<MmadOperation, MmadTrait, MmadOperationWith, MmadTraitWith> {
    using trait_type = typename MmadTrait::trait_type;
    static constexpr const trait_type default_trait = MmadTrait::value;

    template <typename Params>
    __aicore__ inline constexpr mmad_traits<MmadOperationWith, MmadTraitWith>
        with(const Params& params) const;

    template <const trait_type& trait = default_trait, typename... Args>
    __aicore__ inline void mmad_unpack(const Args&... args) const;
};
```

#### 模板参数说明

**表5**  `mmad_traits`模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| MmadOperation | 矩阵计算Operation类型，用于指定执行的矩阵计算。 |
| MmadOpArgs... | `mmad_traits`的可变模板参数。基础模板中用于接收不同组合参数；在偏特化中依次对应`MmadTrait`、`MmadOperationWith`和`MmadTraitWith`。 |
| MmadTrait | 未绑定运行时参数时使用的Trait类型。 |
| MmadOperationWith | 绑定运行时参数后使用的Operation类型。 |
| MmadTraitWith | 绑定运行时参数后使用的Trait包装类型。 |

#### 成员说明

**表6**  `mmad_traits`成员说明

| 成员 | 描述 |
| :--- | :--- |
| `trait_type` | 矩阵计算使用的具体Trait类型。 |
| `default_trait` | Trait包装类型提供的默认Trait值。 |
| `with` | 绑定运行时参数，返回包含对应Operation和Trait的`mmad_traits`对象。 |
| `mmad_unpack` | 展开已绑定参数并调用Operation的静态`mmad`成员，完成矩阵计算。 |

### mmad_atom

`mmad_atom`是矩阵计算的原子对象，其定义如下：

```cpp
template <typename... Args>
struct mmad_atom;

template <typename MmadOperation>
struct mmad_atom<MmadOperation> : mmad_atom<mmad_traits<MmadOperation>> {};

template <typename MmadOperation, typename... Args>
struct mmad_atom<mmad_traits<MmadOperation, Args...>> : mmad_traits<MmadOperation, Args...> {
    using mmad_traits_type = mmad_traits<MmadOperation, Args...>;
    using trait_type = typename mmad_traits_type::trait_type;
    static constexpr const trait_type default_trait = mmad_traits_type::default_trait;

    template <const trait_type& traits = default_trait, typename... Params>
    __aicore__ inline void call(const Params&... params) const;

    template <typename... TraitArgs>
    __aicore__ inline auto with(TraitArgs&&... args) const;
};
```

#### 模板参数说明

**表7**  `mmad_atom`模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| Args... | `mmad_atom`的可变模板参数。当传入Operation类型时，生成对应的`mmad_traits`并构造Atom；当传入`mmad_traits<Args...>`类型时，生成基于该Traits的Atom。 |
| MmadOperation | 矩阵计算Operation类型，用于指定Atom执行的矩阵计算通路。 |

#### 成员说明

**表8**  `mmad_atom`成员说明

| 成员 | 描述 |
| :--- | :--- |
| `mmad_traits_type` | `mmad_traits`类型。 |
| `trait_type` | 矩阵计算使用的具体Trait类型。 |
| `default_trait` | 矩阵计算使用的默认Trait值。 |
| `call` | 使用Atom中的Operation执行矩阵计算。 |
| `with` | 绑定运行时参数并返回新的Atom，不修改原Atom。 |

### Operation和Traits组合列表

**表9**  Operation和Traits组合列表

| 功能 | Operation | Traits | Trait | Params |
| :--- | :--- | :--- | :--- | :--- |
| [Global Memory到Unified Buffer的数据搬运](reg_vector_compute/vector_compute_load/copy_gm_to_ub.md) | `copy_gm_to_ub` | `copy_traits<copy_gm_to_ub, ...>` | `gm_to_ub_trait` | `gm_to_ub_params` |
| [Unified Buffer到Global Memory的数据搬运](reg_vector_compute/vector_compute_store/copy_ub_to_gm.md) | `copy_ub_to_gm` | `copy_traits<copy_ub_to_gm, ...>` | `ub_to_gm_trait` | 无 |
| [Global Memory到L1 Buffer的数据搬运](matrix_compute/cube_compute_load/copy_gm_to_l1.md) | `copy_gm_to_l1` | `copy_traits<copy_gm_to_l1, ...>` | `gm_to_l1_trait` | 无 |
| [Unified Buffer到L1 Buffer的数据搬运](reg_vector_compute/vector_compute_load/copy_ub_to_l1.md) | `copy_ub_to_l1` | `copy_traits<copy_ub_to_l1, ...>` | `ub_to_l1_trait` | 无 |
| [L1 Buffer到Unified Buffer的数据搬运](matrix_compute/cube_compute_load/copy_l1_to_ub.md) | `copy_l1_to_ub` | `copy_traits<copy_l1_to_ub, ...>` | `l1_to_ub_trait` | 无 |
| [L1 Buffer到L0A Buffer的数据搬运](matrix_compute/cube_compute_load/copy_l1_to_l0a.md) | `copy_l1_to_l0a` | `copy_traits<copy_l1_to_l0a, ...>` | `l1_to_l0a_trait` | `img2col_params<PadType>`，仅Img2Col场景使用 |
| [L1 Buffer到L0B Buffer的数据搬运](matrix_compute/cube_compute_load/copy_l1_to_l0b.md) | `copy_l1_to_l0b` | `copy_traits<copy_l1_to_l0b, ...>` | `l1_to_l0b_trait` | 无 |
| [L1 Buffer到L0ScaleA Buffer的数据搬运](matrix_compute/cube_compute_load/copy_l1_to_l0scalea.md) | `copy_l1_to_l0scalea` | `copy_traits<copy_l1_to_l0scalea, ...>` | `l1_to_l0scalea_trait` | 无 |
| [L1 Buffer到L0ScaleB Buffer的数据搬运](matrix_compute/cube_compute_load/copy_l1_to_l0scaleb.md) | `copy_l1_to_l0scaleb` | `copy_traits<copy_l1_to_l0scaleb, ...>` | `l1_to_l0scaleb_trait` | 无 |
| [L1 Buffer到Fixpipe Buffer的数据搬运](matrix_compute/cube_compute_load/copy_l1_to_fixbuf.md) | `copy_l1_to_fixbuf` | `copy_traits<copy_l1_to_fixbuf, ...>` | `l1_to_fixbuf_trait` | 无 |
| [L1 Buffer到BiasTable Buffer的数据搬运](matrix_compute/cube_compute_load/copy_l1_to_biastable.md) | `copy_l1_to_biastable` | `copy_traits<copy_l1_to_biastable, ...>` | `l1_to_biastable_trait` | 无 |
| [L0C Buffer到Global Memory的数据搬运](matrix_compute/cube_compute_store/copy_l0c_to_gm.md) | `copy_l0c_to_gm` | `copy_traits<copy_l0c_to_gm, ...>` | `l0c_to_gm_trait` | `l0c_to_gm_params` |
| [L0C Buffer到Unified Buffer的数据搬运](matrix_compute/cube_compute_store/copy_l0c_to_ub.md) | `copy_l0c_to_ub` | `copy_traits<copy_l0c_to_ub, ...>` | `l0c_to_ub_trait` | `l0c_to_ub_params` |
| [L0C Buffer到L1 Buffer的数据搬运](matrix_compute/cube_compute_store/copy_l0c_to_l1.md) | `copy_l0c_to_l1` | `copy_traits<copy_l0c_to_l1, ...>` | `l0c_to_l1_trait` | `l0c_to_l1_params` |
| [Unified Buffer到Unified Buffer的数据搬运](reg_vector_compute/vector_compute_load/copy_ub_to_ub.md) | `copy_ub_to_ub` | `copy_traits<copy_ub_to_ub, ...>` | `ub_to_ub_trait` | 无 |
| [矩阵乘计算](matrix_compute/mmad_compute/mmad.md) | `mmad_operation` | `mmad_traits<mmad_operation, ...>` | `mmad_trait` | `mmad_params` |
