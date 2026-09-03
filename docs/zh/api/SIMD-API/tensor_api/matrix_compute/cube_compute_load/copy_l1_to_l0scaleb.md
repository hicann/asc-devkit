# copy（L1 Buffer到L0B_MX Buffer数据搬运）

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

Tensor API通过`copy`接口统一执行不同通路数据搬运。该接口用于将L1 Buffer中的右矩阵缩放数据搬运到L0B_MX Buffer。右矩阵缩放数据在L0B_MX Buffer上的首地址由右矩阵在L0B Buffer的首地址的1/16推导出来。

接口支持完整Tensor搬运，也支持通过`dst_coord`、`src_coord`和`copy_shape`指定目的Tensor中的起始坐标、源Tensor中的起始坐标和搬运区域的形状，执行区域搬运。

该通路的数据类型固定为`fp8_e8m0_t`。ScaleB的逻辑矩阵形状为`(K / 32, N)`，物理存储使用`NN`分形布局，单个分形大小为2 * 16。构造layout时需要根据物理分形对逻辑形状进行补齐。左矩阵和右矩阵的缩放数据的分形排布和缩放计算关系可参考下图，其中右矩阵缩放数据是`NN`的数据格式：

**图1**  左矩阵和右矩阵缩放数据缩放示意图

![左矩阵和右矩阵缩放数据缩放示意图](../../../../figures/zh-cn_image_0000002549011155.png)

## 函数原型

- 执行L1 Buffer到L0B_MX Buffer的数据搬运。

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst, const SrcTensor& src)
    ```

- 根据源张量和目的张量的存储位置自动推导搬运通路，使用默认trait执行L1 Buffer到L0B_MX Buffer的数据搬运。

    ```cpp
    template <typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)
    ```

- 按指定源坐标、目的坐标和搬运形状执行L1 Buffer到L0B_MX Buffer搬运。

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- 根据源张量和目的张量的存储位置自动推导搬运通路，使用默认trait按指定源坐标、目的坐标和搬运形状执行L1 Buffer到L0B_MX Buffer搬运。

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

**表1**  `copy`接口模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| Atom | `copy_atom`的模板参数，由`make_copy`接口推导得到。 |
| DstTensor | 目的张量类型，需为Tensor API Tensor类型。 |
| SrcTensor | 源张量类型，需为Tensor API Tensor类型。 |
| DstCoord | 目的张量起始坐标类型，也可为`zero_coord`。 |
| SrcCoord | 源张量起始坐标类型，也可为`zero_coord`。 |
| CopyShape | 搬运区域形状的类型。 |

**表2**  `make_copy`接口模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| CopyOperation | 搬运操作对象类型。 |
| CopyTrait | 搬运trait对象类型。 |

**表3**  `copy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| atom | 输入 | 搬运原子对象，可由`make_copy`接口构造。 |
| dst | 输出 | 目的张量，存储位置为`location::l0scaleb`，数据格式为`NN`。 |
| src | 输入 | 源张量，存储位置为`location::l1`，数据格式为`NN`。 |
| dst_coord | 输入 | 搬运区域在目的张量中的起始坐标，也可传入`zero_coord`。 |
| src_coord | 输入 | 搬运区域在源张量中的起始坐标，也可传入`zero_coord`。 |
| copy_shape | 输入 | 搬运区域的形状，用于指定搬运区域的大小。 |

**表4**  `make_copy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| operation | 输入 | 搬运操作对象。L1 Buffer到L0B_MX Buffer通路使用`copy_l1_to_l0scaleb{}`。 |
| trait | 输入 | 搬运Trait对象，用于指定搬运特性。L1 Buffer到L0B_MX Buffer默认Trait使用`l1_to_l0scaleb_trait_default{}`。 |

### copy_l1_to_l0scaleb说明

`copy_l1_to_l0scaleb`用于标识L1 Buffer到L0B_MX Buffer数据搬运通路，仅列出其public内容：

```cpp
struct copy_l1_to_l0scaleb {
    template <typename Trait, const Trait& trait, typename... Args>
    __aicore__ inline static void copy(const Args&... args);
};
```

`copy`静态成员函数用于接收Trait和搬运参数，并分发L1 Buffer到L0B_MX Buffer数据搬运。

### l1_to_l0scaleb_trait说明

`l1_to_l0scaleb_trait`表示该通路的静态配置。当前没有可配置成员，定义如下：

```cpp
struct l1_to_l0scaleb_trait {};
```

`l1_to_l0scaleb_trait_default`提供该通路的默认Trait类型和默认值：

```cpp
struct l1_to_l0scaleb_trait_default {
    using trait_type = l1_to_l0scaleb_trait;
    static constexpr const trait_type value = {};
};
```


## 数据类型

数据类型仅支持`fp8_e8m0_t`。

源张量和目的张量的基础数据类型需要保持一致。

## 返回值说明

- `copy`无返回值。
- `make_copy`返回`copy_atom`对象。

## 约束说明

- 源张量和目的张量数据格式为`NN`，使用`make_frame_layout<nn_layout_ptn, _2>(...)`构造。
- 源地址和目的地址需要满足32Byte对齐要求。
- ScaleB的逻辑形状为`(K / 32, N)`；传入`make_frame_layout`的行列参数需要满足`NN`布局的物理分形要求，物理存储空间可能包含补齐数据。
- ScaleB的目的地址由对应L0B矩阵地址按1/16地址编码关系推导得到，不能将普通L0B地址直接作为L0ScaleB地址使用。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l1_to_l0scaleb_example()
{
    constexpr uint32_t k = 32;
    constexpr uint32_t n = 16;
    constexpr uint32_t scale_k = k / 16;

    __cbuf__ fp8_e8m0_t l1_buf[scale_k * n];
    __cb__ fp8_e5m2_t l0b_buf[k * n];

    auto l1_tensor = make_tensor(make_mem_ptr(l1_buf), make_frame_layout<nn_layout_ptn, _2>(scale_k, n));

    // ScaleB地址由右矩阵的L0B Buffer地址按1/16地址编码换算得到。
    auto l0scaleb_ptr = make_mem_ptr<location::l0scaleb, fp8_e8m0_t>(reinterpret_cast<uint64_t>(l0b_buf) / 16);
    auto l0scaleb_tensor = make_tensor(l0scaleb_ptr, make_frame_layout<nn_layout_ptn, _2>(scale_k, n));

    auto atom = make_copy(copy_l1_to_l0scaleb{}, l1_to_l0scaleb_trait_default{});
    copy(atom, l0scaleb_tensor, l1_tensor);
}
```
