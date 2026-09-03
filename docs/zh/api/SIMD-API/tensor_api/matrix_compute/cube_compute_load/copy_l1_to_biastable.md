# copy（L1 Buffer到BiasTable Buffer数据搬运）

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

Tensor API通过`copy`接口统一执行不同通路数据搬运。该接口用于将L1 Buffer中的bias数据搬运到BiasTable Buffer，作为mmad计算中的bias输入。

接口支持完整Tensor搬运，也支持通过`dst_coord`、`src_coord`和`copy_shape`指定目的Tensor中的起始坐标、源Tensor中的起始坐标和搬运区域的形状，执行区域搬运。

接口支持batch模式。batch模式用于一次完成多块bias数据的搬运。layout在原layout最外层增加Batch维度，可通过`make_frame_layout<nd_layout_ptn>(batch, 1, n)`或`make_frame_layout<nd_ext_layout_ptn>(batch, 1, n)`构造。

## 函数原型

- 执行L1 Buffer到BiasTable Buffer的数据搬运。

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst, const SrcTensor& src)
    ```

- 根据源张量和目的张量的存储位置自动推导搬运通路，使用默认trait执行L1 Buffer到BiasTable Buffer的数据搬运。

    ```cpp
    template <typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)
    ```

- 按指定源坐标、目的坐标和搬运形状执行L1 Buffer到BiasTable Buffer搬运。

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- 根据源张量和目的张量的存储位置自动推导搬运通路，使用默认trait按指定源坐标、目的坐标和搬运形状执行L1 Buffer到BiasTable Buffer搬运。

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
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation, const CopyTrait& trait)
    ```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| Atom | `copy_atom`的模板参数，由`make_copy`接口推导得到。 |
| DstTensor | 目的张量类型，需为Tensor API Tensor类型。 |
| SrcTensor | 源张量类型，需为Tensor API Tensor类型。 |
| DstCoord | 目的张量起始坐标类型，也可为`zero_coord`。 |
| SrcCoord | 源张量起始坐标类型，也可为`zero_coord`。 |
| CopyShape | 搬运区域形状的类型。 |
| CopyOperation | 搬运操作对象类型。 |
| CopyTrait | 搬运trait对象类型。 |

**表2**  copy接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| atom | 输入 | 搬运原子对象，可由`make_copy`接口构造。 |
| dst | 输出 | 目的张量，存储位置为`location::bias`，数据格式为ND。 |
| src | 输入 | 源张量，存储位置为`location::l1`，数据格式为ND。 |
| dst_coord | 输入 | 搬运区域在目的张量中的起始坐标，也可传入`zero_coord`。 |
| src_coord | 输入 | 搬运区域在源张量中的起始坐标，也可传入`zero_coord`。 |
| copy_shape | 输入 | 搬运区域的形状，用于指定搬运区域的大小。 |

**表3**  make_copy接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| operation | 输入 | 搬运操作对象，L1 Buffer到BiasTable Buffer通路使用`copy_l1_to_biastable{}`。 |
| trait | 输入 | 搬运Trait对象，用于指定搬运特性。L1 Buffer到BiasTable Buffer通路默认Trait使用`l1_to_biastable_trait_default{}`。 |

### copy_l1_to_biastable说明

`copy_l1_to_biastable`用于标识L1 Buffer到BiasTable Buffer数据搬运通路，仅列出其public内容：

```cpp
struct copy_l1_to_biastable {
    template <typename Trait, const Trait& trait, typename... Args>
    __aicore__ inline static void copy(const Args&... args);
};
```

`copy`静态成员函数用于接收Trait和搬运参数，并分发L1 Buffer到BiasTable Buffer数据搬运。

### l1_to_biastable_trait说明

`l1_to_biastable_trait`表示该通路的静态配置。当前没有可配置成员，定义如下：

```cpp
struct l1_to_biastable_trait {};
```

`l1_to_biastable_trait_default`提供该通路的默认Trait类型和默认值：

```cpp
struct l1_to_biastable_trait_default {
    using trait_type = l1_to_biastable_trait;
    static constexpr const trait_type value = {};
};
```

## 数据类型

支持的目的张量和源张量的数据类型组合如下：

**表4**  支持的数据类型组合

| 目的张量数据类型 | 源张量数据类型 |
| :--- | :--- |
| `float` | `bfloat16_t` |
| `float` | `half` |
| `float` | `float` |
| `int32_t` | `int32_t` |

## 返回值说明

`copy`无返回值。`make_copy`返回`copy_atom`对象。

## 约束说明

- 源张量内存地址和大小需要满足32Byte对齐要求。
- 目的张量内存地址和大小需要满足64Byte对齐要求。
- 由于BiasTable Buffer大小为4KB，因此每次搬运最多4KB数据。
- batch模式要求源张量和目的张量Batch数量一致。
- batch模式下，如果Global Memory上的shape大小非对齐，由于L1 Buffer和BiasTable Buffer上有数据对齐要求，源张量和目的张量列方向shape大小必须按32Byte对齐。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_l1_to_biastable_example()
{
    using src_type = half;
    using dst_type = float;
    constexpr uint32_t n = 16;

    __cbuf__ src_type l1_buf[n];
    __biasbuf__ dst_type bt_buf[n];

    auto l1_tensor = make_tensor(make_mem_ptr(l1_buf), make_frame_layout<nd_ext_layout_ptn>(1, n));

    auto bt_tensor = make_tensor(make_mem_ptr(bt_buf), make_frame_layout<nd_ext_layout_ptn>(1, n));

    auto atom = make_copy(copy_l1_to_biastable{}, l1_to_biastable_trait_default{});
    copy(atom, bt_tensor, l1_tensor);
}
```

batch模式调用示例：

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void copy_batch_l1_to_biastable_example()
{
    using src_type = half;
    using dst_type = float;
    constexpr uint32_t batch = 2;
    constexpr uint32_t n = 15;

    // batch模式下源/目的张量列方向shape需按c0_size（32Byte）对齐，
    // 分别向上对齐到c0_element<src_type> 和c0_element<dst_type>。
    constexpr uint32_t src_n = Std::ceil_align(n, Std::Int<c0_element<src_type>>{});
    constexpr uint32_t dst_n = Std::ceil_align(n, Std::Int<c0_element<dst_type>>{});

    __cbuf__ src_type l1_buf[batch * src_n];
    __biasbuf__ dst_type bt_buf[batch * dst_n];

    auto l1_tensor = make_tensor(make_mem_ptr(l1_buf), make_frame_layout<nd_ext_layout_ptn>(batch, 1, src_n));

    auto bt_tensor = make_tensor(make_mem_ptr(bt_buf), make_frame_layout<nd_ext_layout_ptn>(batch, 1, dst_n));

    auto atom = make_copy(copy_l1_to_biastable{}, l1_to_biastable_trait_default{});
    copy(atom, bt_tensor, l1_tensor);
}
```
