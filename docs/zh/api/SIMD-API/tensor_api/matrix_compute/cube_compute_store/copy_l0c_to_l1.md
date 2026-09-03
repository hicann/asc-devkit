# copy（L0C Buffer到L1 Buffer数据搬运）

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3训练系列产品/Atlas A3推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2训练系列产品/Atlas A2推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`tensor_api/tensor.h`。

本接口将矩阵计算结果从L0C Buffer搬运到L1 Buffer，支持非量化搬运和带量化参数的搬运。运行时参数通过`atom.with(params)`绑定。

接口支持完整Tensor搬运，也支持通过`dst_coord`、`src_coord`和`copy_shape`指定目的Tensor中的起始坐标、源Tensor中的起始坐标和搬运区域的形状，执行区域搬运。非量化搬运和量化搬运均支持该形式。

## 函数原型

- 执行L0C Buffer到L1 Buffer的非量化搬运。

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const copy_atom<Atom>& atom,
        const DstTensor& dst, const SrcTensor& src)
    ```

- 根据源张量和目的张量的存储位置自动推导搬运通路，使用默认trait执行L0C Buffer到L1 Buffer的非量化搬运。

    ```cpp
    template <typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)
    ```

- 执行L0C Buffer到L1 Buffer的量化搬运。

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename Quant>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const Quant& quant)
    ```

- 根据源张量和目的张量的存储位置自动推导搬运通路，使用默认trait执行L0C Buffer到L1 Buffer的量化搬运。

    ```cpp
    template <typename DstTensor, typename SrcTensor, typename Quant>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src, const Quant& quant)
    ```

- 按指定源坐标、目的坐标和搬运形状执行非量化搬运。

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- 根据源张量和目的张量的存储位置自动推导搬运通路，使用默认trait按指定源坐标、目的坐标和搬运形状执行非量化搬运。

    ```cpp
    template <typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- 按指定源坐标、目的坐标和搬运形状执行量化搬运。

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename Quant,
        typename DstCoord, typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const Quant& quant, const DstCoord& dst_coord,
        const SrcCoord& src_coord, const CopyShape& copy_shape)
    ```

- 根据源张量和目的张量的存储位置自动推导搬运通路，使用默认trait按指定源坐标、目的坐标和搬运形状执行量化搬运。

    ```cpp
    template <typename DstTensor, typename SrcTensor, typename Quant,
        typename DstCoord, typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const DstTensor& dst,
        const SrcTensor& src, const Quant& quant, const DstCoord& dst_coord,
        const SrcCoord& src_coord, const CopyShape& copy_shape)
    ```

- 使用默认trait构造搬运原子对象。

    ```cpp
    template <typename CopyOperation>
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation)
    ```

- 使用指定trait构造搬运原子对象。

    ```cpp
    template <typename CopyOperation, typename CopyTrait>
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation,
        const CopyTrait& trait)
    ```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| Atom | `copy_atom`的模板参数，由`make_copy`接口推导得到。 |
| DstTensor | 目的张量类型，需为Tensor API Tensor类型。 |
| SrcTensor | 源张量类型，需为Tensor API Tensor类型。 |
| Quant | 量化参数类型，可以为`uint64_t`或符合要求的Tensor API张量类型。 |
| DstCoord | 目的张量起始坐标类型，也可为`zero_coord`。 |
| SrcCoord | 源张量起始坐标类型，也可为`zero_coord`。 |
| CopyShape | 搬运区域形状的类型。 |
| CopyOperation | 搬运操作对象类型。 |
| CopyTrait | 搬运trait对象类型。 |

**表2**  `copy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| atom | 输入 | 搬运原子对象，可由`make_copy`接口构造。 |
| dst | 输出 | 目的张量，存储位置必须为`location::l1`。 |
| src | 输入 | 源张量，存储位置必须为`location::l0c`。 |
| quant | 输入 | 可选量化参数，可以是`uint64_t`标量或符合要求的Tensor API张量。 |
| dst_coord | 输入 | 搬运区域在目的张量中的起始坐标。 |
| src_coord | 输入 | 搬运区域在源张量中的起始坐标。 |
| copy_shape | 输入 | 搬运区域的形状，用于指定搬运区域的大小。 |

**表3**  `make_copy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| operation | 输入 | 搬运操作对象。L0C Buffer到L1 Buffer搬运取`copy_l0c_to_l1{}`。 |
| trait | 输入 | 搬运trait对象。L0C Buffer到L1 Buffer默认取`l0c_to_l1_trait_default{}`。 |

### copy_l0c_to_l1说明

`copy_l0c_to_l1`用于标识L0C Buffer到L1 Buffer数据搬运通路，仅列出其public内容：

```cpp
struct copy_l0c_to_l1 {
    template <typename Trait, const Trait& trait, typename... Args>
    __aicore__ inline static void copy(const Args&... args);
};
```

`copy`静态成员函数用于接收Trait和搬运参数，并分发L0C Buffer到L1 Buffer数据搬运。

### l0c_to_l1_trait说明

`l0c_to_l1_trait`用于配置舍入模式、随路Relu和通道拆分：

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

**表4**  `l0c_to_l1_trait`成员说明

| 成员 | 默认值 | 描述 |
| :--- | :--- | :--- |
| round_mode | `round_mode::default_round` | 搬运过程中的舍入模式。 |
| enable_relu | `false` | 是否使能随路Relu。 |
| enable_channel_split | `false` | 是否使能通道拆分。 |

`l0c_to_l1_trait_default`提供该通路的默认Trait类型和默认值：

```cpp
struct l0c_to_l1_trait_default {
    using trait_type = l0c_to_l1_trait;
    static constexpr const trait_type value = {};
};
```

### l0c_to_l1_params说明

`l0c_to_l1_params`用于配置运行时的unit_flag模式：

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

**表5**  `l0c_to_l1_params`成员说明

| 成员 | 默认值 | 描述 |
| :--- | :--- | :--- |
| unit_flag | `unit_flag_mode::disable` | 控制`mmad`与搬出指令之间的细粒度并行。 |


## 返回值说明

`copy`无返回值。`make_copy`返回`copy_atom`对象。

## 约束说明

- `dst`必须位于L1 Buffer，`src`必须位于L0C Buffer。
- Trait和Params的配置需要与数据类型、Layout和量化模式匹配。
- 使用坐标搬运时，coord和copy_shape需要与对应张量的形状结构匹配，且搬运范围不能越界。
- 使用`unit_flag`时，需要与产生L0C Buffer数据的`mmad`接口配合设置。
