# Algorithm

<!-- md-trans-meta sourceCommit=9bfdf41ea5ec539a90d71bf0d1a3098d7cd16c53 translatedAt=2026-10-27T04:30:33.182Z pushedAt=2026-10-27T13:03:35.499Z nl=2 -->

The Algorithm layer of the Tensor API provides a unified invocation entry for Tensor-based movement and cube computation. The `copy` and `mmad` APIs are used to perform movement and cube computation, respectively, and the `make_copy` and `make_mmad` APIs are used to construct the corresponding atomic objects.

## Operation, Traits, and Atom

The Tensor API combines Trait (compile-time characteristic) and optional Params (runtime parameters) into Traits, combines an Operation and Traits into an Atom, and provides movement and cube computation capabilities through the Atom:

- An Operation is a C++ structure that encapsulates the instruction-level movement or cube computation behavior of a specific path.
- Traits is a template specialization structure associated with an Operation, providing the information required by the Operation.
- Atom is the atomic object of movement or cube computation, composed of an Operation and a Traits.

### copy_traits

`copy_traits` is used to describe the type combination formed by a movement Operation, its Trait, and the bound runtime parameters. The basic structure of `copy_traits` is defined as follows:

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

For example, `copy_gm_to_l1` represents a movement Operation from Global Memory to the L1 Buffer. You can bind the corresponding Trait to it in the following ways.

```cpp
// Bind the configurable Trait.
template <typename Trait>
struct copy_traits<copy_l0c_to_gm, Trait> : public copy_traits<copy_l0c_to_gm, Trait, copy_l0c_to_gm_with, Trait> {};

// Bind the default Trait.
template <>
struct copy_traits<copy_gm_to_l1> : public copy_traits<copy_gm_to_l1, gm_to_l1_trait_default> {};
```

#### Template Parameter Description

**Table 1** `copy_traits` Template Parameter Description

| Parameter | Description |
| :--- | :--- |
| CopyOperation | Movement Operation type, used to specify the movement path to be executed. |
| CopyOpArgs... | Variadic template parameters of `copy_traits`. In the primary template, they are used to receive different combination parameters; in partial specializations, they correspond to `CopyTrait`, `CopyOperationWith`, and `CopyTraitWith`, respectively. |
| CopyTrait | Trait type used when runtime parameters are not bound. |
| CopyOperationWith | Operation type used after runtime parameters are bound. |
| CopyTraitWith | Trait wrapper type used after runtime parameters are bound. |

#### Member Description

**Table 2** `copy_traits` Member Description

| Member | Description |
| :--- | :--- |
| `trait_type` | Specific Trait type used by the path. |
| `default_trait` | Default Trait value provided by the Trait wrapper type. |
| `with` | Binds runtime parameters and returns a `copy_traits` object containing the corresponding Operation and Trait. |
| `copy_unpack` | Unpacks the bound parameters and invokes the static `copy` member of Operation to complete movement. |

### copy_atom

`copy_atom` is the atomic object for movement. Its definition is as follows:

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

#### Template Parameter Description

**Table 3** `copy_atom` template parameter description

| Parameter | Description |
| :--- | :--- |
| Args... | Variadic template parameters of `copy_atom`. When an Operation type is passed, the corresponding `copy_traits` is generated and an Atom is constructed; when a `copy_traits<Args...>` type is passed, an Atom based on that Traits is generated. |
| CopyOperation | Movement Operation type, used to specify the movement path executed by the Atom. |

#### Member Description

**Table 4**  `copy_atom` Member Description

| Member | Description |
| :--- | :--- |
| `copy_traits_type` | The `copy_traits` type. |
| `trait_type` | Specific Trait type used by the path. |
| `default_trait` | The default Trait value used by the path. |
| `call` | Executes movement using the Operation in the Atom. |
| `with` | Binds runtime parameters and returns a new Atom without modifying the original Atom. |

### mmad_traits

`mmad_traits` is used to describe the type combination of a cube computation Operation, Trait, and runtime parameters after binding. Its definition is as follows:

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

#### Template Parameter Description

**Table 5** `mmad_traits` template parameter description

| Parameter | Description |
| :--- | :--- |
| MmadOperation | Cube computation Operation type, used to specify the cube computation to be executed. |
| MmadOpArgs... | Variadic template parameter of `mmad_traits`. In the primary template, it is used to receive different combination parameters; in partial specialization, it corresponds to `MmadTrait`, `MmadOperationWith`, and `MmadTraitWith` in sequence. |
| MmadTrait | Trait type used when runtime parameters are not bound. |
| MmadOperationWith | Operation type used after runtime parameters are bound. |
| MmadTraitWith | Trait wrapper type used after runtime parameters are bound. |

#### Member Description

**Table 6** `mmad_traits` member description

| Member | Description |
| :--- | :--- |
| `trait_type` | Specific Trait type used for cube computation. |
| `default_trait` | Default Trait value provided by the Trait wrapper type. |
| `with` | Binds runtime parameters and returns an `mmad_traits` object containing the corresponding Operation and Trait. |
| `mmad_unpack` | Unpacks the bound parameters and calls the static `mmad` member of Operation to complete the cube computation. |

### mmad_atom

`mmad_atom` is the atomic object for cube computation, and its definition is as follows:

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

#### Template Parameter Description

**Table 7** `mmad_atom` template parameter description

| Parameter | Description |
| :--- | :--- |
| Args... | Variadic template parameters of `mmad_atom`. When an Operation type is passed in, the corresponding `mmad_traits` is generated and an Atom is constructed; when a `mmad_traits<Args...>` type is passed in, an Atom based on that Traits is generated. |
| MmadOperation | Cube computation Operation type, used to specify the cube computation path executed by the Atom. |

#### Member Description

**Table 8** `mmad_atom` Member Description

| Member | Description |
| :--- | :--- |
| `mmad_traits_type` | The `mmad_traits` type. |
| `trait_type` | Specific Trait type used for cube computation. |
| `default_trait` | The default Trait value used for cube computation. |
| `call` | Executes cube computation using the Operation in the Atom. |
| `with` | Binds runtime parameters and returns a new Atom without modifying the original Atom. |

### Operation and Traits Combination List

**Table 9** Operation and Traits Combination List

| Function | Operation | Traits | Trait | Params |
| :--- | :--- | :--- | :--- | :--- |
| [Movement from Global Memory to Unified Buffer](reg_vector_compute/vector_compute_load/copy_gm_to_ub.md) | `copy_gm_to_ub` | `copy_traits<copy_gm_to_ub, ...>` | `gm_to_ub_trait` | `gm_to_ub_params` |
| [Movement from Unified Buffer to Global Memory](reg_vector_compute/vector_compute_store/copy_ub_to_gm.md) | `copy_ub_to_gm` | `copy_traits<copy_ub_to_gm, ...>` | `ub_to_gm_trait` | None |
| [Movement from Global Memory to L1 Buffer](matrix_compute/cube_compute_load/copy_gm_to_l1.md) | `copy_gm_to_l1` | `copy_traits<copy_gm_to_l1, ...>` | `gm_to_l1_trait` | None |
| [Movement from Unified Buffer to L1 Buffer](reg_vector_compute/vector_compute_load/copy_ub_to_l1.md) | `copy_ub_to_l1` | `copy_traits<copy_ub_to_l1, ...>` | `ub_to_l1_trait` | None |
| [Movement from L1 Buffer to Unified Buffer](matrix_compute/cube_compute_load/copy_l1_to_ub.md) | `copy_l1_to_ub` | `copy_traits<copy_l1_to_ub, ...>` | `l1_to_ub_trait` | None |
| [Movement from L1 Buffer to L0A Buffer](matrix_compute/cube_compute_load/copy_l1_to_l0a.md) | `copy_l1_to_l0a` | `copy_traits<copy_l1_to_l0a, ...>` | `l1_to_l0a_trait` | `img2col_params<PadType>`, used only in the Img2Col scenario |
| [Movement from L1 Buffer to L0B Buffer](matrix_compute/cube_compute_load/copy_l1_to_l0b.md) | `copy_l1_to_l0b` | `copy_traits<copy_l1_to_l0b, ...>` | `l1_to_l0b_trait` | None |
| [Movement from L1 Buffer to L0ScaleA Buffer](matrix_compute/cube_compute_load/copy_l1_to_l0scalea.md) | `copy_l1_to_l0scalea` | `copy_traits<copy_l1_to_l0scalea, ...>` | `l1_to_l0scalea_trait` | None |
| [Movement from L1 Buffer to L0ScaleB Buffer](matrix_compute/cube_compute_load/copy_l1_to_l0scaleb.md) | `copy_l1_to_l0scaleb` | `copy_traits<copy_l1_to_l0scaleb, ...>` | `l1_to_l0scaleb_trait` | None |
| [Movement from L1 Buffer to Fixpipe Buffer](matrix_compute/cube_compute_load/copy_l1_to_fixbuf.md) | `copy_l1_to_fixbuf` | `copy_traits<copy_l1_to_fixbuf, ...>` | `l1_to_fixbuf_trait` | None |
| [Movement from L1 Buffer to BiasTable Buffer](matrix_compute/cube_compute_load/copy_l1_to_biastable.md) | `copy_l1_to_biastable` | `copy_traits<copy_l1_to_biastable, ...>` | `l1_to_biastable_trait` | None |
| [Movement from L0C Buffer to Global Memory](matrix_compute/cube_compute_store/copy_l0c_to_gm.md) | `copy_l0c_to_gm` | `copy_traits<copy_l0c_to_gm, ...>` | `l0c_to_gm_trait` | `l0c_to_gm_params` |
| [Movement from L0C Buffer to Unified Buffer](matrix_compute/cube_compute_store/copy_l0c_to_ub.md) | `copy_l0c_to_ub` | `copy_traits<copy_l0c_to_ub, ...>` | `l0c_to_ub_trait` | `l0c_to_ub_params` |
| [Movement from L0C Buffer to L1 Buffer](matrix_compute/cube_compute_store/copy_l0c_to_l1.md) | `copy_l0c_to_l1` | `copy_traits<copy_l0c_to_l1, ...>` | `l0c_to_l1_trait` | `l0c_to_l1_params` |
| [Movement from Unified Buffer to Unified Buffer](reg_vector_compute/vector_compute_load/copy_ub_to_ub.md) | `copy_ub_to_ub` | `copy_traits<copy_ub_to_ub, ...>` | `ub_to_ub_trait` | None |
| [Cube multiplication computation](matrix_compute/mmad_compute/mmad.md) | `mmad_operation` | `mmad_traits<mmad_operation, ...>` | `mmad_trait` | `mmad_params` |
