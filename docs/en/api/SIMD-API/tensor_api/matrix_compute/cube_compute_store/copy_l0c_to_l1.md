# copy (L0C Buffer to L1 Buffer Movement)

<!-- md-trans-meta sourceCommit=2fe3a64c1bb11110e316a708c0d8e9dc67631487 translatedAt=2026-10-27T02:57:00.971Z pushedAt=2026-10-27T13:03:35.336Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- AI Core in Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Vector Core in Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is `tensor_api/tensor.h`.

This API moves cube computation results from the L0C Buffer to the L1 Buffer, supporting both non-quantized movement and movement with quantization parameters. Runtime parameters are bound through `atom.with(params)`.

The API supports complete tensor movement, and also supports regional movement by specifying the start coordinate in the destination tensor, the start coordinate in the source tensor, and the shape of the movement region through `dst_coord`, `src_coord`, and `copy_shape`. Both non-quantized movement and quantized movement support this form.

## Function Prototype

- Performs non-quantized movement from the L0C Buffer to the L1 Buffer.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const copy_atom<Atom>& atom,
        const DstTensor& dst, const SrcTensor& src)
    ```

- Automatically derives the movement path based on the storage locations of the source tensor and destination tensor, and uses the [default trait](../../algorithm.md#copy_traits) to perform non-quantized movement from the L0C Buffer to the L1 Buffer.

    ```cpp
    template <typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)
    ```

- Performs quantized movement from the L0C Buffer to the L1 Buffer.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename Quant>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const Quant& quant)
    ```

- Automatically derives the movement path based on the storage locations of the source tensor and destination tensor, and performs quantized movement from the L0C Buffer to the L1 Buffer using the default trait.

    ```cpp
    template <typename DstTensor, typename SrcTensor, typename Quant>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src, const Quant& quant)
    ```

- Executes non-quantized movement according to the specified source coordinate, destination coordinate, and movement shape.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- Automatically derives the movement path based on the storage locations of the source and destination tensors, and executes non-quantized movement according to the specified source coordinate, destination coordinate, and movement shape using the default trait.

    ```cpp
    template <typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- Executes quantized movement according to the specified source coordinate, destination coordinate, and movement shape.

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename Quant,
        typename DstCoord, typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const Quant& quant, const DstCoord& dst_coord,
        const SrcCoord& src_coord, const CopyShape& copy_shape)
    ```

- Automatically derives the movement path based on the storage locations of the source and destination tensors, and executes quantized movement according to the specified source coordinate, destination coordinate, and movement shape using the default trait.

    ```cpp
    template <typename DstTensor, typename SrcTensor, typename Quant,
        typename DstCoord, typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const DstTensor& dst,
        const SrcTensor& src, const Quant& quant, const DstCoord& dst_coord,
        const SrcCoord& src_coord, const CopyShape& copy_shape)
    ```

- Constructs a movement atomic object using the default trait.

    ```cpp
    template <typename CopyOperation>
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation)
    ```

- Constructs a movement atomic object using the specified trait.

    ```cpp
    template <typename CopyOperation, typename CopyTrait>
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation,
        const CopyTrait& trait)
    ```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| :--- | :--- |
| Atom | Template parameter of `copy_atom`, derived by the `make_copy` API. |
| DstTensor | Destination tensor type, which must be a Tensor API Tensor type. |
| SrcTensor | Source tensor type, which must be a Tensor API Tensor type. |
| Quant | Quantization parameter type, which can be `uint64_t` or a Tensor API tensor type that meets the requirements. |
| DstCoord | Start coordinate type of the destination tensor, which can also be `zero_coord`. |
| SrcCoord | Start coordinate type of the source tensor, which can also be `zero_coord`. |
| CopyShape | Type of the movement region shape. |
| CopyOperation | Type of the movement operation object. |
| CopyTrait | Type of the movement trait object. |

**Table 2**  `copy` API parameter description

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| atom | Input | Movement atomic object, which encapsulates a combination of Operation, Trait, and optional runtime Params for a specific movement path. It can be constructed by the `make_copy` API. For details, see [Algorithm](../../algorithm.md#copy_atom). |
| dst | Output | Destination tensor, whose storage location must be `location::l1`. |
| src | Input | Source tensor, whose storage location must be `location::l0c`. |
| quant | Input | Optional quantization parameter, which can be a `uint64_t` scalar or a Tensor API tensor that meets the requirements. |
| dst_coord | Input | Start coordinate of the movement region in the destination tensor. |
| src_coord | Input | Start coordinate of the movement region in the source tensor. |
| copy_shape | Input | Shape of the movement region, used to specify the size of the movement region. |

**Table 3**  `make_copy` API parameter description

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| operation | Input | Movement operation object. For L0C Buffer to L1 Buffer movement, use `copy_l0c_to_l1{}`. |
| trait | Input | Movement trait object. For L0C Buffer to L1 Buffer movement, use `l0c_to_l1_trait_default{}` by default. |

### copy_l0c_to_l1 Description

`copy_l0c_to_l1` identifies the L0C Buffer-to-L1 Buffer movement path. Only its public members are listed:

```cpp
struct copy_l0c_to_l1 {
    template <typename Trait, const Trait& trait, typename... Args>
    __aicore__ inline static void copy(const Args&... args);
};
```

The `copy` static member function receives the Trait and the movement parameters, and dispatches the L0C Buffer-to-L1 Buffer movement.

### `l0c_to_l1_trait`

`l0c_to_l1_trait` is used to configure the rounding mode, in-line ReLU, and channel split:

```cpp
enum class round_mode : uint8_t { default_round = 0, hybrid = 1 };

struct l0c_to_l1_trait {
    __aicore__ constexpr l0c_to_l1_trait() = default;
    __aicore__ constexpr l0c_to_l1_trait(asc::te::round_mode round_mode,
        bool enable_relu, bool enable_channel_split);

    asc::te::round_mode round_mode = asc::te::round_mode::default_round;
    bool enable_relu = false;
    bool enable_channel_split = false;
};
```

**Table 4**  `l0c_to_l1_trait` member description

| Member | Default Value | Description |
| :--- | :--- | :--- |
| round_mode | `round_mode::default_round` | Rounding mode used during the movement. |
| enable_relu | `false` | Whether to enable real-time ReLU. |
| enable_channel_split | `false` | Whether to enable channel split. |

`l0c_to_l1_trait_default` provides the default trait type and default value for this path:

```cpp
struct l0c_to_l1_trait_default {
    using trait_type = l0c_to_l1_trait;
    static constexpr const trait_type value = {};
};
```

### l0c_to_l1_params Description

`l0c_to_l1_params` is used to configure the runtime `unit_flag` mode:

```cpp
enum class unit_flag_mode : uint8_t {
    disable = 0,
    enable_keep = 2,
    enable_update = 3
};

struct l0c_to_l1_params {
    __aicore__ constexpr l0c_to_l1_params() = default;
    __aicore__ constexpr l0c_to_l1_params(unit_flag_mode unit_flag)
        : unit_flag(unit_flag) {}

    unit_flag_mode unit_flag = unit_flag_mode::disable;
};
```

**Table 5**  `l0c_to_l1_params` member description

| Member | Default Value | Description |
| :--- | :--- | :--- |
| unit_flag | `unit_flag_mode::disable` | Controls fine-grained parallelism between `mmad` and the move-out instruction. |


## Return Value

`copy` has no return value. `make_copy` returns a `copy_atom` object.

## Constraints

- The quantization and ReLU parameters cannot be inf/nan or denormalized numbers.
- `dst` must be located in the L1 Buffer, and `src` must be located in the L0C Buffer.
- The Trait and Params configurations must match the data type, Layout, and quantization mode.
- When coordinate-based movement is used, `coord` and `copy_shape` must match the shape structure of the corresponding tensors, and the movement region must not exceed the bounds.
- When using `unit_flag`, it must be configured in coordination with the `mmad` API that produces the L0C Buffer data.
