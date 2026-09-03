# copy（L1 Buffer到L0A Buffer数据搬运）

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

Tensor API通过`copy`接口统一执行不同通路数据搬运。该接口用于将L1 Buffer中的左矩阵数据搬运到L0A Buffer。

接口支持完整Tensor搬运，也支持通过`dst_coord`、`src_coord`和`copy_shape`指定目的张量中的起始坐标、源张量中的起始坐标和搬运区域的形状，执行区域搬运。

该通路以512Byte数据分形为基本搬运单位。不同数据类型对应的分形矩阵形态如下：

**表1**  数据位宽与分形矩阵形态

| 数据位宽 | 分形矩阵形态 |
| :--- | :--- |
| b4 | 16 * 64 |
| b8 | 16 * 32 |
| b16 | 16 * 16 |
| b32 | 16 * 8 |

接口支持非转置搬运、转置搬运和Img2Col卷积数据搬运。源张量和目的张量的Layout需要匹配当前通路支持的格式组合，具体如表2所示。

**表2**  搬运模式与格式组合

| 搬运模式 | 源Layout -> 目的Layout |
| :--- | :--- |
| 非转置搬运 | `Nz -> Nz` |
| 转置搬运 | `Zn -> Nz` |
| Img2Col搬运 | `NC1HWC0 -> Nz`或`NDC1HWC0 -> Nz` |

Img2Col场景中，搬运过程中会将卷积特征图展开为矩阵。

接口支持Batch模式。Batch模式下，源张量和目的张量的Layout需要在原有分形Layout最前面增加Batch维，形状结构为`(b, 单矩阵形状)`，其中`b`表示Batch数量。源张量和目的张量的Batch数量必须一致，Batch维Stride需要与单Batch数据的物理占用空间一致，即`Batch维Stride = 单Batch Layout`的[capacity()](../../layout/capacity.md)。用户可使用`make_frame_layout<nz_layout_ptn, DataType>(b, m, k)`或`make_frame_layout<zn_layout_ptn, DataType>(b, m, k)`构造连续的Batch Layout。

## 函数原型

- 执行L1 Buffer到L0A Buffer的数据搬运。

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst, const SrcTensor& src)
    ```

- 根据源张量和目的张量的存储位置自动推导搬运通路，使用默认trait执行L1 Buffer到L0A Buffer的数据搬运。

    ```cpp
    template <typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)
    ```

- 按指定源坐标、目的坐标和搬运形状执行L1 Buffer到L0A Buffer搬运。

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- 根据源张量和目的张量的存储位置自动推导搬运通路，使用默认trait按指定源坐标、目的坐标和搬运形状执行L1 Buffer到L0A Buffer搬运。

    ```cpp
    template <typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- 使用默认trait构造搬运原子对象。

    ```cpp
    template <typename CopyOperation>
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation)
    ```

- 使用指定trait构造搬运原子对象。

    ```cpp
    template <typename CopyOperation, typename CopyTrait>
    __aicore__ inline constexpr auto make_copy(
        const CopyOperation& operation, const CopyTrait& trait)
    ```

Img2Col搬运通过`copy_atom::with`绑定`img2col_params`后，使用相同的`copy(atom, dst, src)`函数原型执行。

## 参数说明

**表3**  `copy`接口模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| Atom | `copy_atom`的模板参数，由`make_copy`接口推导得到。 |
| DstTensor | 目的张量类型。 |
| SrcTensor | 源张量类型。 |
| DstCoord | 目的张量起始坐标类型，也可为`zero_coord`。 |
| SrcCoord | 源张量起始坐标类型，也可为`zero_coord`。 |
| CopyShape | 搬运区域形状的类型。 |

**表4**  `make_copy`接口模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| CopyOperation | 搬运操作对象类型。 |
| CopyTrait | 搬运trait对象类型。 |

**表5**  `copy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| atom | 输入 | 搬运原子对象，可由`make_copy`接口构造。 |
| dst | 输出 | 目的张量，存储位置为`location::l0a`。 |
| src | 输入 | 源张量，存储位置为`location::l1`。 |
| dst_coord | 输入 | 搬运区域在目的张量中的起始坐标，也可传入`zero_coord`。 |
| src_coord | 输入 | 搬运区域在源张量中的起始坐标，也可传入`zero_coord`。 |
| copy_shape | 输入 | 搬运区域的形状，用于指定搬运区域的大小。 |

**表6**  `make_copy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| operation | 输入 | 搬运操作对象。L1 Buffer到L0A Buffer通路使用`copy_l1_to_l0a{}`。 |
| trait | 输入 | 搬运Trait对象，用于指定搬运特性。L1 Buffer到L0A Buffer默认Trait使用`l1_to_l0a_trait_default{}`。 |

### copy_l1_to_l0a说明

`copy_l1_to_l0a`用于标识L1 Buffer到L0A Buffer数据搬运通路，仅列出其public内容：

```cpp
struct copy_l1_to_l0a {
    template <typename Trait, const Trait& trait, typename... Args>
    __aicore__ inline static void copy(const Args&... args);
};
```

`copy`静态成员函数用于接收Trait和搬运参数，并分发L1 Buffer到L0A Buffer数据搬运。

### l1_to_l0a_trait说明

`l1_to_l0a_trait`表示该通路的静态配置。当前没有可配置成员，定义如下：

```cpp
struct l1_to_l0a_trait {};
```

`l1_to_l0a_trait_default`提供该通路的默认Trait类型和默认值：

```cpp
struct l1_to_l0a_trait_default {
    using trait_type = l1_to_l0a_trait;
    static constexpr const trait_type value = {};
};
```

### img2col_params说明

`img2col_params<PadType>`用于配置Img2Col搬运的目的矩阵范围、卷积核大小、滑动步长、膨胀系数、padding大小和padding值。

```cpp
template <typename PadType>
struct img2col_params {
    uint16_t m_extension = 0;
    uint16_t k_extension = 0;
    uint16_t m_start_pos = 0;
    uint16_t k_start_pos = 0;
    uint8_t pad_list[4] = {0, 0, 0, 0};
    uint8_t stride_w = 1;
    uint8_t stride_h = 1;
    uint8_t filter_w = 1;
    uint8_t filter_h = 1;
    uint8_t dilation_filter_w = 1;
    uint8_t dilation_filter_h = 1;
    bool enable_filter_w_extend = false;
    bool enable_filter_h_extend = false;
    bool enable_transpose = false;
    bool enable_f_matrix_ctrl = false;
    PadType pad_value = 0;
};
```

**表7**  `img2col_params`成员说明

| 成员 | 默认值 | 描述 |
| :--- | :--- | :--- |
| m_extension | `0` | 目的矩阵M轴的搬运元素数，取值范围为[0, 65535]，为0时不执行搬运。搬运范围未覆盖目的矩阵最下侧分形时，b4、b8和b16数据类型要求取值为16的倍数，b32数据类型无倍数要求；覆盖最下侧分形时无倍数要求。 |
| k_extension | `0` | 目的矩阵K轴的搬运元素数，取值范围为[0, 65535]，为0时不执行搬运。搬运范围未覆盖目的矩阵最右侧分形时，b4、b8、b16和b32数据类型分别要求取值为64、32、16和8的倍数；覆盖最右侧分形时无倍数要求。 |
| m_start_pos | `0` | 目的矩阵M轴的搬运起点，取值范围[0, 32767]。 |
| k_start_pos | `0` | 该指令在目的操作数width维度的起点，对于b32类型，应为8的倍数；对于b16类型，应为16的倍数；对于b8类型，应为32的倍数；对于b4类型，应为64的倍数。取值范围[0, 65535]。 |
| pad_list | `{0, 0, 0, 0}` | padding大小，依次为左、右、上、下，每个值的取值范围为[0, 255]。 |
| stride_w | `1` | 卷积核在源特征图W轴的滑动步长，取值范围为[0, 63]。 |
| stride_h | `1` | 卷积核在源特征图H轴的滑动步长，取值范围为[0, 63]。 |
| filter_w | `1` | 卷积核的宽度，取值范围为[0, 255]。 |
| filter_h | `1` | 卷积核的高度，取值范围为[0, 255]。 |
| dilation_filter_w | `1` | 卷积核W轴的膨胀系数，取值范围为[0, 255]。 |
| dilation_filter_h | `1` | 卷积核H轴的膨胀系数，取值范围为[0, 255]。 |
| enable_filter_w_extend | `false` | 是否在`filter_w`基础上将卷积核宽度增加256。 |
| enable_filter_h_extend | `false` | 是否在`filter_h`基础上将卷积核高度增加256。 |
| enable_transpose | `false` | 是否对展开后的目的矩阵进行转置。 |
| enable_f_matrix_ctrl | `false` | FeatureMap属性选择标识，当前仅支持`false`。 |
| pad_value | `0` | padding区域的填充值。模板参数PadType通常需要与目的张量的元素类型一致。目的张量为`fp8_e4m3fn_t`、`fp8_e5m2_t`或`hifloat8_t`类型时，不支持直接使用对应的8位浮点类型作为PadType，需要先将padding值转换为接口支持的整型编码后传入。 |


## 数据类型

支持的数据类型包括：

`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`int8_t`、`uint8_t`、`hifloat8_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`half`、`bfloat16_t`、`int16_t`、`uint16_t`、`int32_t`、`uint32_t`、`float`。

源张量和目的张量的数据类型需要保持一致。

## 返回值说明

- `copy`无返回值。
- `make_copy`返回`copy_atom`对象。

## 约束说明

- 目的地址位于L0A Buffer，起始地址需要满足512Byte对齐要求。
- 源地址位于L1 Buffer，起始地址需要满足32Byte对齐要求。
- 特殊数据类型约束：L1 Buffer到L0A Buffer通路使能转置时，即`ZN2NZ`，支持的数据类型约束如下：
  - b32数据类型要求源矩阵形状在K轴方向16对齐。
  - b8数据类型要求源矩阵形状在M轴方向32对齐。
  - b4数据类型要求源矩阵形状在M轴方向64对齐。
- Batch模式要求源张量和目的张量的Batch数量一致，且在L1 Buffer和L0A Buffer上张量数据连续排布。
- Img2Col搬运当前仅支持N为1的`NC1HWC0`源张量，目的张量需要为`NZ`格式。
- `img2col_params<PadType>`的PadType通常需要与L0A Buffer目的张量的元素类型一致。目的张量为`fp8_e4m3fn_t`、`fp8_e5m2_t`或`hifloat8_t`类型时，不支持直接使用对应的8位浮点类型作为PadType，需要先将padding值转换为接口支持的整型编码后传入。

## 关键特性说明

### 非转置搬运

当源张量和目的张量数据格式为`NZ`时，接口按非转置方式从L1 Buffer搬运到L0A Buffer。

**图1**  非转置搬运示意图

![非转置搬运示意图](../../../../figures/load2dv2_start_addr_demo.png)

### 转置搬运

当源张量数据格式为`ZN`、目的张量数据格式为`NZ`时，接口按转置方式搬运。转置搬运会对源张量中每个分形矩阵进行转置。

不同数据位宽的转置搬运示意如下。

#### b4转置搬运

**图2**  b4转置搬运示意图

![b4转置搬运示意图](../../../../figures/load2dv2_l12l0_trans_b4.png)

#### b8转置搬运

**图3**  b8转置搬运示意图

![b8转置搬运示意图](../../../../figures/load2dv2_l12l0_trans_b8.png)

#### b16转置搬运

**图4**  b16转置搬运示意图

![b16转置搬运示意图](../../../../figures/load2dv2_l12l0_trans_b16.png)

#### b32转置搬运

**图5**  b32转置搬运示意图

![b32转置搬运示意图](../../../../figures/load2dv2_l12l0_trans_b32.png)

### Batch搬运

当源张量和目的张量均使用带Batch维的Layout时，`copy`接口会按照Batch维描述的数据范围完成L1 Buffer到L0A Buffer的数据搬运。非转置搬运时源张量、目的张量均为`NZ`格式，转置搬运时源张量为`ZN`格式，目的张量为`NZ`格式。

### Img2Col搬运

Img2Col搬运将`NC1HWC0`格式特征图按卷积核窗口展开为`NZ`格式矩阵。展开后矩阵的M轴对应输出特征图的空间位置，K轴对应卷积核的H、W和输入通道。用户通过`img2col_params`配置展开范围和卷积参数，再通过`atom.with(params)`将参数绑定到搬运原子对象。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l1_to_l0a_example()
{
    using a_type = half;
    constexpr uint32_t m = 16;
    constexpr uint32_t k = 16;

    __cbuf__ a_type l1_buf[m * k];
    __ca__ a_type l0a_buf[m * k];

    auto l1_tensor = make_tensor(make_mem_ptr(l1_buf), make_frame_layout<nz_layout_ptn, a_type>(m, k));

    auto l0a_tensor = make_tensor(make_mem_ptr(l0a_buf), make_frame_layout<nz_layout_ptn, a_type>(m, k));

    auto atom = make_copy(copy_l1_to_l0a{}, l1_to_l0a_trait_default{});
    copy(atom, l0a_tensor, l1_tensor);
}
```

Batch模式示例如下。

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l1_to_l0a_batch_example()
{
    using a_type = half;
    constexpr uint32_t batch = 2;
    constexpr uint32_t m = 16;
    constexpr uint32_t k = 16;

    __cbuf__ a_type l1_buf[batch * m * k];
    __ca__ a_type l0a_buf[batch * m * k];

    auto layout = make_frame_layout<nz_layout_ptn, a_type>(batch, m, k);
    auto l1_tensor = make_tensor(make_mem_ptr(l1_buf), layout);
    auto l0a_tensor = make_tensor(make_mem_ptr(l0a_buf), layout);

    auto atom = make_copy(copy_l1_to_l0a{}, l1_to_l0a_trait_default{});
    copy(atom, l0a_tensor, l1_tensor);
}
```

Img2Col搬运示例如下。输入特征图形状为`(1, 2, 5, 5, 16)`，卷积核为3×3，stride为1，padding为1，展开后矩阵的逻辑形状为`(25, 288)`。

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_img2col_l1_to_l0a()
{
    constexpr uint32_t n = 1;
    constexpr uint32_t c1 = 2;
    constexpr uint32_t h = 5;
    constexpr uint32_t w = 5;
    constexpr uint32_t c0 = 16;
    constexpr uint32_t m = 25;
    constexpr uint32_t k = 3 * 3 * c1 * c0;
    constexpr uint32_t m_align = 32;

    __cbuf__ half l1_buf[n * c1 * h * w * c0];
    __ca__ half l0a_buf[m_align * k];

    auto l1_feature = make_tensor(
        make_mem_ptr(l1_buf), make_frame_layout<nc1hwc0_layout_ptn>(n, c1, h, w, c0));
    auto l0a_matrix = make_tensor(
        make_mem_ptr(l0a_buf), make_frame_layout<nz_layout_ptn, half>(m, k));

    img2col_params<half> params;
    params.m_extension = m;
    params.k_extension = k;
    params.m_start_pos = 0;
    params.k_start_pos = 0;
    params.filter_w = 3;
    params.filter_h = 3;
    params.stride_w = 1;
    params.stride_h = 1;
    params.pad_list[0] = 1;
    params.pad_list[1] = 1;
    params.pad_list[2] = 1;
    params.pad_list[3] = 1;

    auto copy_l1_to_l0a_atom = make_copy(copy_l1_to_l0a{}, l1_to_l0a_trait_default{});
    copy(copy_l1_to_l0a_atom.with(params), l0a_matrix, l1_feature);
}
```
