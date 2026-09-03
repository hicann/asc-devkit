# copy（L1 Buffer到L0B Buffer数据搬运）

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

Tensor API通过`copy`接口统一执行不同通路数据搬运。该接口用于将L1 Buffer中的右矩阵数据搬运到L0B Buffer。

接口支持完整Tensor搬运，也支持通过`dst_coord`、`src_coord`和`copy_shape`指定目的张量中的起始坐标、源张量中的起始坐标和搬运区域的形状，执行区域搬运。

该通路以512Byte数据分形为基本搬运单位。不同数据类型对应的分形矩阵形态如下：

**表1**  数据位宽与分形矩阵形态

| 数据位宽 | 分形矩阵形态 |
| :--- | :--- |
| b4 | 64 * 16 |
| b8 | 32 * 16 |
| b16 | 16 * 16 |
| b32 | 8 * 16 |

接口支持非转置搬运和转置搬运。源张量和目的张量的Layout需要匹配当前通路支持的格式组合，具体如表2所示。

**表2**  搬运模式与格式组合

| 搬运模式 | 源Layout -> 目的Layout |
| :--- | :--- |
| 非转置搬运 | `Zn -> Zn` |
| 转置搬运 | `Nz -> Zn` |

接口支持Batch模式。Batch模式下，源张量和目的张量的Layout需要在原有分形Layout最前面增加Batch维，形状结构为`(b, 单矩阵形状)`，其中`b`表示Batch数量。源张量和目的张量的Batch数量必须一致，Batch维stride需要与单Batch数据的物理占用空间一致，即`Batch维stride = 单Batch layout`的[capacity()](../../layout/capacity.md)；按字节计算时，`Batch维stride * sizeof(元素类型) = 单Batch数据的物理占用字节数`。用户可使用`make_frame_layout<zn_layout_ptn, DataType>(b, k, n)`或`make_frame_layout<nz_layout_ptn, DataType>(b, k, n)`构造连续的Batch Layout。

## 函数原型

- 执行L1 Buffer到L0B Buffer的数据搬运。

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst, const SrcTensor& src)
    ```

- 根据源张量和目的张量的存储位置自动推导搬运通路，使用默认trait执行L1 Buffer到L0B Buffer的数据搬运。

    ```cpp
    template <typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)
    ```

- 按指定源坐标、目的坐标和搬运形状执行L1 Buffer到L0B Buffer搬运。

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- 根据源张量和目的张量的存储位置自动推导搬运通路，使用默认trait按指定源坐标、目的坐标和搬运形状执行L1 Buffer到L0B Buffer搬运。

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
| dst | 输出 | 目的张量，存储位置必须为`location::l0b`。 |
| src | 输入 | 源张量，存储位置必须为`location::l1`。 |
| dst_coord | 输入 | 搬运区域在目的张量中的起始坐标，也可传入`zero_coord`。 |
| src_coord | 输入 | 搬运区域在源张量中的起始坐标，也可传入`zero_coord`。 |
| copy_shape | 输入 | 搬运区域的形状，用于指定搬运区域的大小。 |

**表6**  `make_copy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| operation | 输入 | 搬运操作对象。L1 Buffer到L0B Buffer通路使用`copy_l1_to_l0b{}`。 |
| trait | 输入 | 搬运Trait对象，用于指定搬运特性。L1 Buffer到L0B Buffer默认Trait使用`l1_to_l0b_trait_default{}`。 |

### copy_l1_to_l0b说明

`copy_l1_to_l0b`用于标识L1 Buffer到L0B Buffer数据搬运通路，仅列出其public内容：

```cpp
struct copy_l1_to_l0b {
    template <typename Trait, const Trait& trait, typename... Args>
    __aicore__ inline static void copy(const Args&... args);
};
```

`copy`静态成员函数用于接收Trait和搬运参数，并分发L1 Buffer到L0B Buffer数据搬运。

### l1_to_l0b_trait说明

`l1_to_l0b_trait`表示该通路的静态配置。当前没有可配置成员，定义如下：

```cpp
struct l1_to_l0b_trait {};
```

`l1_to_l0b_trait_default`提供该通路的默认Trait类型和默认值：

```cpp
struct l1_to_l0b_trait_default {
    using trait_type = l1_to_l0b_trait;
    static constexpr const trait_type value = {};
};
```


## 数据类型

支持的数据类型包括：

`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`int8_t`、`uint8_t`、`hifloat8_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`half`、`bfloat16_t`、`int16_t`、`uint16_t`、`int32_t`、`uint32_t`、`float`。

源张量和目的张量的数据类型需要保持一致。

## 返回值说明

- `copy`无返回值。
- `make_copy`返回`copy_atom`对象。

## 约束说明

- 目的地址位于L0B Buffer，起始地址需要满足512Byte对齐要求。
- 源地址位于L1 Buffer，起始地址需要满足32Byte对齐要求。
- 特殊数据类型约束：L1 Buffer到L0B Buffer通路使能转置时，即`NZ2ZN`，支持的数据类型约束如下：
  - b32数据类型要求源矩阵形状在K轴方向16对齐。
  - b8数据类型要求源矩阵形状在M轴方向32对齐。
  - b4数据类型要求源矩阵形状在M轴方向64对齐。
- Batch模式要求源张量和目的张量的Batch数量一致，且在L1 Buffer和L0B Buffer上张量数据连续排布。

## 关键特性说明

### 非转置搬运

当源张量和目的张量数据格式均为`ZN`时，接口按非转置方式从L1 Buffer搬运到L0B Buffer。

**图1**  非转置搬运示意图

![非转置搬运示意图](../../../../figures/load2dv2_start_addr_demo.png)

### 转置搬运

当源张量数据格式为`NZ`、目的张量数据格式为`ZN`时，接口按转置方式搬运。转置搬运会对源张量每个分形矩阵执行转置。

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

当源张量和目的张量均使用带Batch维的Layout时，`copy`接口会按照Batch维描述的数据范围完成L1 Buffer到L0B Buffer的数据搬运。非转置搬运时源张量、目的张量均为`ZN`格式，转置搬运时源张量为`NZ`格式，目的张量为`ZN`格式。Batch维位于最外层，源和目的Batch数量必须一致，并且每个Batch的Batch Stride应等于对应单Batch Layout的物理占用空间。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l1_to_l0b_example()
{
    using b_type = half;
    constexpr uint32_t k = 16;
    constexpr uint32_t n = 16;

    __cbuf__ b_type l1_buf[k * n];
    __cb__ b_type l0b_buf[k * n];

    auto l1_tensor = make_tensor(make_mem_ptr(l1_buf), make_frame_layout<zn_layout_ptn, b_type>(k, n));

    auto l0b_tensor = make_tensor(make_mem_ptr(l0b_buf), make_frame_layout<zn_layout_ptn, b_type>(k, n));

    auto atom = make_copy(copy_l1_to_l0b{}, l1_to_l0b_trait_default{});
    copy(atom, l0b_tensor, l1_tensor);
}
```

Batch模式示例如下。

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l1_to_l0b_batch_example()
{
    using b_type = half;
    constexpr uint32_t batch = 2;
    constexpr uint32_t k = 16;
    constexpr uint32_t n = 16;

    __cbuf__ b_type l1_buf[batch * k * n];
    __cb__ b_type l0b_buf[batch * k * n];

    auto layout = make_frame_layout<zn_layout_ptn, b_type>(batch, k, n);
    auto l1_tensor = make_tensor(make_mem_ptr(l1_buf), layout);
    auto l0b_tensor = make_tensor(make_mem_ptr(l0b_buf), layout);

    auto atom = make_copy(copy_l1_to_l0b{}, l1_to_l0b_trait_default{});
    copy(atom, l0b_tensor, l1_tensor);
}
```
