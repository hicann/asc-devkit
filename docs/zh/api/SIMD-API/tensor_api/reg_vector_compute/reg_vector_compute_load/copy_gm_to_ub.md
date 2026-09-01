# copy（Global Memory到Unified Buffer的数据搬运）

> [!NOTE]说明
> 本接口为试验接口，在后续版本中可能会调整或改进。请开发者在使用过程中关注后续版本更新。

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

头文件路径为：`tensor_api/tensor.h`，命名空间为`asc::te`。

Tensor API通过`copy`接口统一执行不同通路的数据搬运。该接口用于将Global Memory中的数据搬运到Unified Buffer（UB）。接口根据源Tensor和目的Tensor的存储位置自动选择`copy_gm_to_ub`通路，并根据Layout推导搬运块数、单块长度、源步长和目的步长，实现连续或非连续的高维切分数据搬运。

当前支持ND2ND、DN2DN、NZ2NZ和ZN2ZN搬运。源、目的Layout Pattern必须相同，搬运过程中不执行格式转换，数据格式和内容保持不变。

除整Tensor搬运外，接口还支持通过目的坐标、源坐标和搬运Shape指定局部区域。需要在目的侧增加左右Padding时，可通过`gm_to_ub_params`配置，并使用`copy_atom::with`绑定到搬运原子对象。

## 函数原型

- 根据源、目的Tensor的存储位置自动推导通路。

    ```cpp
    template <typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)
    ```

- 使用搬运原子对象执行搬运。

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const copy_atom<Atom>& atom,
        const DstTensor& dst, const SrcTensor& src)
    ```

- 搬运指定区域。

    ```cpp
    template <typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src,
        const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- 构造搬运原子对象。

    ```cpp
    template <typename CopyOperation>
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation)

    template <typename CopyOperation, typename CopyTrait>
    __aicore__ inline constexpr auto make_copy(
        const CopyOperation& operation, const CopyTrait& trait)
    ```

Global Memory到UB通路使用`copy_gm_to_ub{}`作为搬运操作对象，默认trait为`gm_to_ub_trait_default{}`。

## 参数说明

**表1** `copy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| atom | 输入 | 搬运原子对象，可通过`make_copy(copy_gm_to_ub{})`或`make_copy(copy_gm_to_ub{}, gm_to_ub_trait_default{})`构造。 |
| dst | 输出 | 目的Tensor，存储位置必须为`location::ub`。 |
| src | 输入 | 源Tensor，存储位置必须为`location::gm`。 |
| dst_coord | 输入 | 目的区域起始坐标。显式坐标的结构应与`copy_shape`一致，也可以使用`zero_coord`。 |
| src_coord | 输入 | 源区域起始坐标。显式坐标的结构应与`copy_shape`一致，也可以使用`zero_coord`。 |
| copy_shape | 输入 | 搬运区域的逻辑Shape。接口根据该Shape及源/目的Layout推导地址和搬运参数。 |

### 参数推导

`copy`根据源、目的Tensor的Layout自动推导底层多块搬运参数。下表中的单块长度和步长单位均为字节；源、目的步长分别表示相邻数据块起始地址的间隔。

| Layout模式 | 块数推导 | 单块长度推导 | 源、目的步长推导 |
| :--- | :--- | :--- | :--- |
| ND | Layout的总行数 | Layout的总列数 × 元素字节数 | 对应Tensor的行步长 × 元素字节数 |
| DN | Layout的总列数 | Layout的总行数 × 元素字节数 | 对应Tensor的列步长 × 元素字节数 |
| NZ | 分形格式的外层列数 | 总行数 × 内层列数 × 元素字节数 | 对应Tensor的外层列步长 × 元素字节数 |
| ZN | 分形格式的外层行数 | 总列数 × 内层行数 × 元素字节数 | 对应Tensor的外层行步长 × 元素字节数 |

- 源、目的步长均等于单块长度时，数据块在相应存储空间中连续排列。
- 源或目的步长大于单块长度时，接口执行非连续搬运；间隔由对应Tensor的Layout步长描述，无需单独配置。

### gm_to_ub_params说明

```cpp
struct gm_to_ub_params {
    uint8_t left_padding_count = 0;
    uint8_t right_padding_count = 0;
    bool enable_constant_pad = true;
};
```

| 成员 | 默认值 | 描述 |
| :--- | :---: | :--- |
| left_padding_count | `0` | 每个搬运块左侧的Padding元素个数。 |
| right_padding_count | `0` | 每个搬运块右侧的Padding元素个数。 |
| enable_constant_pad | `true` | 是否使能常量Padding控制。 |

通过以下方式绑定运行时搬运参数：

```cpp
asc::te::gm_to_ub_params params{left_padding, right_padding, true};
auto atom = asc::te::make_copy(asc::te::copy_gm_to_ub{}).with(params);
asc::te::copy(atom, dst, src);
```

## 数据类型

源Tensor和目的Tensor的数据类型必须相同。

| 源/目的Layout | 支持的数据类型 |
| :--- | :--- |
| ND、DN、NZ、ZN | `fp4x2_e1m2_t`、`fp4x2_e2m1_t`、`int8_t`、`uint8_t`、`hifloat8_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`、`int64_t` |

## 返回值说明

`copy`无返回值。`make_copy`返回`copy_atom`对象。

## 约束说明

- 源Tensor必须位于Global Memory，目的Tensor必须位于UB。
- 源地址支持1字节对齐，目的UB地址需满足32字节对齐要求。
- 源、目的Layout Pattern必须相同。支持ND2ND、DN2DN、NZ2NZ和ZN2ZN，不支持随路格式转换。
- 目的Tensor或目的区域的容量必须不小于待搬运数据及Padding所需空间。
- Layout推导出的块数、块长度和步长必须满足底层`asc_copy_gm2ub_align`指令的字段范围及对齐要求。有效搬运的块数取值范围为[1, 4095]，单块长度取值范围为[1, 2097151]字节。与部分产品上的`DataCopyParams`接口不同，Tensor API要求源、目的Layout以及区域搬运Shape合法，不能依赖块数或单块长度为0表示空操作。
- 对于多个有效数据块，目的步长应等于单块长度或为32字节的正整数倍；源步长和目的步长均表示相邻数据块起始地址的间隔，而不是`DataCopyParams`中的块间Gap。
- 左、右单侧Padding均不能超过32字节。对于8Byte元素类型，Padding计数按4Byte粒度分发。
- 当目的步长等于单块搬运长度，即处于紧凑搬运模式时，左右Padding必须均为0。
- b4类型按b8搬运粒度调整块长度和步长。

## 关键特性说明

### Layout自动推导

ND通路沿行方向生成搬运块，DN通路沿列方向生成搬运块。NZ和ZN通路根据分形Layout的内外轴推导块数、块长度和步长。源、目的Layout的对应步长决定数据块是连续排列还是带间隔排列。用户只需保证源、目的Tensor的Layout正确匹配。

### 连续和非连续搬运

当块数为1时，源、目的步长不影响搬运结果。当块数大于1时，源、目的步长均等于单块长度表示相邻数据块首尾相接，属于连续搬运；任一步长大于单块长度表示对应存储空间中的数据块之间存在间隔，属于非连续搬运。

以元素类型为`half`、搬运形状为`(2, 32)`的ND Tensor为例，单块长度为`32 × sizeof(half) = 64`字节。

| 场景 | 源Tensor行步长 | 目的Tensor行步长 | 推导结果 |
| :--- | :--- | :--- | :--- |
| 连续搬运 | 32个元素 | 32个元素 | 块数为2，单块长度为64字节，源、目的步长均为64字节。 |
| 非连续搬运 | 48个元素 | 64个元素 | 块数为2，单块长度为64字节，源步长为96字节，目的步长为128字节。每个数据块之间的源数据跳过32字节，目的数据保留64字节间隔。 |

搬运时，`copy`仅写入每个数据块覆盖的目的地址范围；由目的Tensor步长产生的间隔不会被覆盖。Padding参数同样按每个数据块生效，目的Tensor的Layout步长应包含Padding占用的空间。

### 局部区域搬运

区域重载根据`dst_coord`、`src_coord`和`copy_shape`解析起始地址。该方式无需先构造Tensor切片，适合从大Tensor中搬运矩形局部区域。

## 调用示例

以下示例将Global Memory中的ND数据搬入UB：

```cpp
#include "tensor_api/tensor.h"

constexpr uint32_t m = 8;
constexpr uint32_t n = 64;

__aicore__ inline void copy_gm_to_ub_example(__gm__ half* gm_addr)
{
    __ubuf__ half ub_buf[m * n];

    auto layout = asc::te::make_frame_layout<asc::te::nd_ext_layout_ptn,
        asc::te::layout_trait_default<half>>(m, n);
    auto src = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::gm>(gm_addr), layout);
    auto dst = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(ub_buf), layout);

    asc::te::copy(dst, src);
}
```

该示例的ND Layout包含8行，每行64个`half`元素，因此接口推导出块数为8、单块长度为128字节，源步长和目的步长均为128字节。各数据块首尾相接，属于连续搬运。

以下示例搬运局部区域，并显式构造搬运原子对象：

```cpp
auto atom = asc::te::make_copy(asc::te::copy_gm_to_ub{}, asc::te::gm_to_ub_trait_default{});
asc::te::copy(atom, dst, src, asc::te::make_coord(1, 0), asc::te::make_coord(2, 0), asc::te::make_shape(2, 32));
```

该区域包含2行，每行搬运32个`half`元素，因此接口推导出块数为2、单块长度为64字节。源、目的Tensor的行步长均为128字节，所以源步长和目的步长均为128字节，相邻数据块之间各有64字节间隔。
