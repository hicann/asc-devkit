# copy（L0C Buffer到Unified Buffer数据搬运）

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`tensor_api/tensor.h`。

Tensor API通过`copy`接口统一执行不同通路数据搬运。该接口用于将L0C Buffer中的矩阵计算结果搬运到Unified Buffer（UB）。L0C Buffer中的数据通常为`mmad`的输出，数据格式为`NZ`。搬运到UB时，接口会根据目的张量格式自动选择`NZ`到`ND`、`NZ`到`DN`或`NZ`到`NZ`的随路格式转换。

接口支持完整Tensor搬运，也支持通过`dst_coord`、`src_coord`和`copy_shape`指定目的Tensor中的起始坐标、源Tensor中的起始坐标和搬运区域的形状，执行区域搬运。非量化搬运和量化搬运均支持该形式。

L0C Buffer到UB搬运支持不量化输出、`float`到`half`或`bfloat16_t`的直接转换输出，以及配合标量或张量量化参数的随路量化输出。随路Relu、双目的模式和舍入方式通过`l0c_to_ub_trait`配置。`mmad`与`Fixpipe`细粒度并行相关的`unit_flag`通过`l0c_to_ub_params`配置。

接口支持batch模式。batch模式用于一次完成多块矩阵计算结果的搬运。layout在原矩阵layout最外层增加Batch维度。源张量为`NZ`格式，分形固定为16×16，可通过`make_frame_layout<nz_layout_ptn>(batch, m, n)`构造。目的张量可通过`make_frame_layout<nd_layout_ptn>(batch, m, n)`、`make_frame_layout<dn_layout_ptn>(batch, m, n)`、`make_frame_layout<nd_ext_layout_ptn>(batch, m, n)`、`make_frame_layout<dn_ext_layout_ptn>(batch, m, n)`或`make_frame_layout<nz_layout_ptn, DstType>(batch, m, n)`构造。`NZ`格式可通过模板参数`DstType`指定目的数据类型，`C0`表示NZ格式的列分形大小，默认为16。

随路量化、随路Relu、随路格式转换、随路通道拆分以及随路通道合并的有效组合、中间数据类型和数据路径如下图所示。图中的F32到F16、F32到BF16为非量化模式，仅进行cast。其余路径为不量化、随路scalar或tensor量化模式。针对Ascend 950PR/Ascend 950DT，还支持NZ2DN随路格式转换。

**图1**  L0C2UB流程图

![L0C2UB流程图](../../../../figures/L0C2UB_Function_Combination_950.png)

## 函数原型

- 执行L0C Buffer到UB的非量化搬运。

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst, const SrcTensor& src)
    ```

- 根据源张量和目的张量的存储位置自动推导搬运通路，使用默认trait执行L0C Buffer到UB的非量化搬运。

    ```cpp
    template <typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)
    ```

- 执行L0C Buffer到UB的量化搬运。

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename Quant>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst, const SrcTensor& src,
        const Quant& quant)
    ```

- 根据源张量和目的张量的存储位置自动推导搬运通路，使用默认trait执行L0C Buffer到UB的量化搬运。

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
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation, const CopyTrait& trait)
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

|参数名|输入/输出|描述|
|--------|--------|--------|
|atom|输入|搬运原子对象，可由`make_copy`接口构造。|
|dst|输出|目的张量，存储位置为`location::ub`。数据格式支持`ND`、`DN`和`NZ`。|
|src|输入|源张量，存储位置为`location::l0c`，数据格式为`NZ`，通常为`mmad`的计算结果。|
|quant|输入|可选量化参数。传入`uint64_t`时表示scalar量化参数，传入张量时表示tensor量化参数，张量位于L1 Buffer，元素类型为`uint64_t`。|
|dst_coord|输入|搬运区域在目的张量中的起始坐标，也可传入`zero_coord`。|
|src_coord|输入|搬运区域在源张量中的起始坐标，也可传入`zero_coord`。|
|copy_shape|输入|搬运区域的形状，用于指定搬运区域的大小。|

如需配置运行时搬运参数，先通过`atom.with(params)`将`l0c_to_ub_params`对象绑定到搬运原子对象；未绑定时使用默认值。

**表3**  `make_copy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| operation | 输入 | 搬运操作对象。L0C Buffer到UB搬运取`copy_l0c_to_ub{}`。 |
| trait | 输入 | 搬运trait对象。L0C Buffer到UB搬运默认取`l0c_to_ub_trait_default{}`。 |


### copy_l0c_to_ub说明

`copy_l0c_to_ub`用于标识L0C Buffer到Unified Buffer数据搬运通路，仅列出其public内容：

```cpp
struct copy_l0c_to_ub {
    template <typename Trait, const Trait& trait, typename... Args>
    __aicore__ inline static void copy(const Args&... args);
};
```

`copy`静态成员函数用于接收Trait和搬运参数，并分发L0C Buffer到Unified Buffer数据搬运。

### l0c_to_ub_trait说明

`l0c_to_ub_trait`用于配置L0C Buffer到UB搬运的静态特性。

```cpp
enum class round_mode : uint8_t { default_round = 0, hybrid = 1 };

enum class dual_dst_mode : uint8_t { disable = 0, split_m = 1, split_n = 2 };

struct l0c_to_ub_trait {
    __aicore__ constexpr l0c_to_ub_trait() = default;
    __aicore__ constexpr l0c_to_ub_trait(asc::te::round_mode round_mode,
        bool enable_relu, bool enable_channel_split, dual_dst_mode dual_dst_ctl);

    asc::te::round_mode round_mode = asc::te::round_mode::default_round;
    bool enable_relu = false;
    bool enable_channel_split = false;
    dual_dst_mode dual_dst_ctl = dual_dst_mode::disable;
};
```

**表4**  `l0c_to_ub_trait`成员说明

|成员|默认值|描述|
|--------|--------|--------|
|round_mode|`round_mode::default_round`|舍入模式。`round_mode::hybrid`仅在源类型为`float`、目的类型为`hifloat8_t`的量化输出场景支持。|
|enable_relu|`false`|是否使能随路Relu。|
|enable_channel_split|`false`|是否使能输出数据通道拆分。|
|dual_dst_ctl|`dual_dst_mode::disable`|双目标模式控制，取值包括`dual_dst_mode::disable`、`dual_dst_mode::split_m`、`dual_dst_mode::split_n`。详细说明参见[L0C Buffer到Unified Buffer双目标模式](../cube_store_key_features/l0c_to_ub_dual_dst.md)。|

使用自定义trait的示例：

```cpp
constexpr l0c_to_ub_trait l0c_to_ub_trait_value = {
    round_mode::default_round,
    false,
    false,
    dual_dst_mode::disable
};

struct copy_l0c_to_ub_trait_custom {
    using trait_type = l0c_to_ub_trait;
    static constexpr const trait_type value = l0c_to_ub_trait_value;
};
```

`l0c_to_ub_trait_default`提供该通路的默认Trait类型和默认值：

```cpp
struct l0c_to_ub_trait_default {
    using trait_type = l0c_to_ub_trait;
    static constexpr const trait_type value = {};
};
```

### l0c_to_ub_params说明

`l0c_to_ub_params`用于配置运行时搬运参数。

```cpp
enum class unit_flag_mode : uint8_t { disable = 0, enable_keep = 2, enable_update = 3 };

struct l0c_to_ub_params {
    __aicore__ constexpr l0c_to_ub_params() = default;
    __aicore__ constexpr l0c_to_ub_params(
        unit_flag_mode unit_flag, uint8_t sub_block_id = 0) :
        unit_flag(unit_flag), sub_block_id(sub_block_id)
    {}

    unit_flag_mode unit_flag = unit_flag_mode::disable;
    uint8_t sub_block_id = 0;
};
```

**表5**  `l0c_to_ub_params`成员说明

|成员|默认值|描述|
|--------|--------|--------|
|unit_flag|`unit_flag_mode::disable`|控制`mmad`指令和`Fixpipe`指令的细粒度并行。`unit_flag_mode::disable`表示不使能，`unit_flag_mode::enable_keep`表示使能且执行后不复位单元标记位，`unit_flag_mode::enable_update`表示使能且执行后复位单元标记位。|
|sub_block_id|`0`|在启用单目标模式时指示目标UB的编号。|

## 数据类型

L0C Buffer到UB搬运根据是否传入量化参数自动选择量化模式。

**表6**  数据类型说明

|源张量类型（L0C Buffer）|目的张量类型（UB）|调用形式|说明|
|--------|--------|--------|--------|
|`int32_t`|`int8_t`、`uint8_t`|`copy(atom, dst, src, quant)`|scalar或tensor量化输出。|
|`float`|`int8_t`、`uint8_t`|`copy(atom, dst, src, quant)`|scalar或tensor量化输出。|
|`float`|`fp8_e4m3fn_t`、`hifloat8_t`|`copy(atom, dst, src, quant)`|scalar或tensor量化输出。`hifloat8_t`支持`round_mode::default_round`和`round_mode::hybrid`。|
|`int32_t`|`half`、`bfloat16_t`|`copy(atom, dst, src, quant)`|scalar或tensor量化输出。|
|`float`|`half`|`copy(atom, dst, src)`|直接转换输出，对应F32到F16。|
|`float`|`bfloat16_t`|`copy(atom, dst, src)`|直接转换输出，对应F32到BF16。|
|`float`|`half`、`bfloat16_t`|`copy(atom, dst, src, quant)`|scalar或tensor量化输出。|
|`int32_t`|`int32_t`|`copy(atom, dst, src)`|不量化输出。|
|`float`|`float`|`copy(atom, dst, src)`|不量化输出。|
|`float`|`float`|`copy(atom, dst, src, quant)`|scalar或tensor量化输出。|

不传入`quant`时，接口选择不量化或直接转换模式，传入`uint64_t`时选择scalar量化模式，传入Tensor API张量时选择tensor量化模式。

## 返回值说明

`copy`无返回值。`make_copy`返回`copy_atom`对象。

## 约束说明

- 源矩阵`NZ`格式，地址要求64字节对齐。目的矩阵地址要求32字节对齐。
- 目的矩阵为`NZ`输出时，N方向大小需要为16的倍数；`b8`类型通道合并场景为32的倍数，`float`使能`enable_channel_split`通道拆分场景为8的倍数。
- tensor量化参数张量应位于L1 Buffer，元素类型为`uint64_t`，地址要求32字节对齐。详细约束参见[随路量化](../cube_store_key_features/quant_pre.md)。
- 使能`unit_flag`时，需要配合`mmad`同时使能。
- 使用`round_mode::hybrid`时，源类型必须为`float`，目的类型必须为`hifloat8_t`。
- 目的布局为`NZ`时，不支持`dual_dst_ctl`设置为`dual_dst_mode::split_n`。双目标模式详细约束参见[L0C Buffer到Unified Buffer双目标模式](../cube_store_key_features/l0c_to_ub_dual_dst.md)。
- 通道合并特性硬件自动使能，不能通过参数配置。详细约束参见[Int8 Channel Merge](../cube_store_key_features/int8_channel_merge.md)。

## 关键特性

L0C Buffer到UB搬运涉及[随路量化](../cube_store_key_features/quant_pre.md)、[随路Relu](../cube_store_key_features/relu_pre.md)、[Int8 Channel Merge](../cube_store_key_features/int8_channel_merge.md)、[L0C Buffer到Unified Buffer双目标模式](../cube_store_key_features/l0c_to_ub_dual_dst.md)和[batch搬运](../cube_store_key_features/batch_copy.md)等关键特性。

## 调用示例

### 不量化输出

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l0c_to_ub_example()
{
    auto l0c = make_tensor(make_mem_ptr(l0c_addr), make_frame_layout<nz_layout_ptn, 16>(m, n));
    auto ub = make_tensor(make_mem_ptr(ub_addr), make_frame_layout<nd_ext_layout_ptn>(m, n));

    l0c_to_ub_params params;
    auto atom = make_copy(copy_l0c_to_ub{}).with(params);
    copy(atom, ub, l0c);
}
```

### scalar量化输出

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l0c_to_ub_example()
{
    auto l0c = make_tensor(make_mem_ptr(l0c_addr), make_frame_layout<nz_layout_ptn, 16>(m, n));
    auto ub = make_tensor(make_mem_ptr(ub_addr), make_frame_layout<nd_ext_layout_ptn>(m, n));

    uint64_t quant = 1;
    auto atom = make_copy(copy_l0c_to_ub{}).with(l0c_to_ub_params{});
    copy(atom, ub, l0c, quant);
}
```

### tensor量化输出

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l0c_to_ub_example()
{
    auto l0c = make_tensor(make_mem_ptr(l0c_addr), make_frame_layout<nz_layout_ptn, 16>(m, n));
    auto ub = make_tensor(make_mem_ptr(ub_addr), make_frame_layout<nd_ext_layout_ptn>(m, n));
    auto quant = make_tensor(make_mem_ptr(l1_quant_addr), make_frame_layout<nd_ext_layout_ptn>(1, n));

    auto atom = make_copy(copy_l0c_to_ub{}).with(l0c_to_ub_params{});
    copy(atom, ub, l0c, quant);
}
```

### batch模式输出

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_batch_l0c_to_ub()
{
    constexpr uint32_t batch = 3;
    constexpr uint32_t m = 32;
    constexpr uint32_t n = 64;

    __cc__ float l0c_buf[batch * m * n];
    __ubuf__ half ub_buf[batch * m * n];

    auto l0c = make_tensor(make_mem_ptr(l0c_buf), make_frame_layout<nz_layout_ptn>(batch, m, n));
    auto ub = make_tensor(make_mem_ptr(ub_buf), make_frame_layout<nd_ext_layout_ptn>(batch, m, n));

    auto atom = make_copy(copy_l0c_to_ub{}).with(l0c_to_ub_params{});
    copy(atom, ub, l0c);
}
```
