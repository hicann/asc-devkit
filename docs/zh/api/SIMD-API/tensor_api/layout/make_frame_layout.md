# make_frame_layout

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

make_frame_layout用于根据标准分形排布模式构造带类型信息的layout对象。该接口根据构造参数，结合指定的LayoutPattern（排布模式）和layout trait（布局特征参数）计算shape和stride，构造layout对象。除矩阵layout外，该接口还支持构造卷积场景中的NCHW、NHWC、NC1HWC0、NCDHW和NDC1HWC0格式layout。该接口也支持在矩阵维度前传入Batch维度，用于构造多Batch矩阵的layout。

LayoutPattern决定数据在内存中的分形排列方式，不同存储位置间的搬运和计算操作要求源/目的张量满足特定的LayoutPattern组合。

## 函数原型

```cpp
template <typename LayoutPattern, typename Trait = Std::ignore_t, typename... Args>
__aicore__ inline constexpr decltype(auto) make_frame_layout(const Args&... args)

template <typename LayoutPattern, size_t C0Element, typename... Args>
__aicore__ inline constexpr decltype(auto) make_frame_layout(const Args&... args)

template <typename LayoutPattern, typename Trait = layout_trait_default<>>
struct frame_layout_format {
    template <typename... Args>
    __aicore__ inline constexpr decltype(auto) operator()(const Args&... args);
};
```

layout trait的公开定义如下。

```cpp
template <typename DataType, typename C0>
struct layout_trait {
    static constexpr auto c0_element = C0{};
};

template <typename DataType = uint16_t,
          typename C0 = Std::Int<c0_element<DataType>>>
struct layout_trait_default : layout_trait<DataType, C0> {};
```

支持的LayoutPattern类型定义如下。

```cpp
struct zn_layout_ptn {};
struct zz_layout_ptn {};
struct nn_layout_ptn {};
struct nz_layout_ptn {};
struct nd_layout_ptn {};
struct dn_layout_ptn {};
struct nd_ext_layout_ptn {};
struct dn_ext_layout_ptn {};
struct scalea_nd_layout_ptn {};
struct scalea_dn_layout_ptn {};
struct scaleb_nd_layout_ptn {};
struct scaleb_dn_layout_ptn {};
struct nchw_layout_ptn {};
struct nhwc_layout_ptn {};
struct nc1hwc0_layout_ptn {};
struct ncdhw_layout_ptn {};
struct ndc1hwc0_layout_ptn {};
```

常用调用形式如下。

```cpp
// 构造单个矩阵layout，m为行数，n为列数。
auto layout = make_frame_layout<LayoutPattern, Trait>(m, n);

// 构造Batch矩阵layout，batch为矩阵个数，m为单个矩阵的行数，n为单个矩阵的列数。
auto batch_layout = make_frame_layout<LayoutPattern, Trait>(batch, m, n);

// 构造卷积输入或输出layout。
auto nchw_layout = make_frame_layout<nchw_layout_ptn>(n, c, h, w);
auto nhwc_layout = make_frame_layout<nhwc_layout_ptn>(n, h, w, c);
auto nc1hwc0_layout = make_frame_layout<nc1hwc0_layout_ptn>(n, c1, h, w, c0);
auto ncdhw_layout = make_frame_layout<ncdhw_layout_ptn>(n, c, d, h, w);
auto ndc1hwc0_layout = make_frame_layout<ndc1hwc0_layout_ptn>(n, d, c1, h, w, c0);
```

### 卷积LayoutPattern

**表1**  卷积LayoutPattern说明

| LayoutPattern | 构造参数 | shape | stride | 描述 |
| :--- | :--- | :--- | :--- | :--- |
| `nchw_layout_ptn` | `n, c, h, w` | `(N, C, H, W)` | `(C * H * W, H * W, W, 1)` | 通道维C位于高度维H和宽度维W之前。 |
| `nhwc_layout_ptn` | `n, h, w, c` | `(N, H, W, C)` | `(H * W * C, W * C, C, 1)` | 通道维C位于最内层。 |
| `nc1hwc0_layout_ptn` | `n, c1, h, w, c0` | `(N, C1, H, W, C0)` | `(C1 * H * W * C0, H * W * C0, W * C0, C0, 1)` | 将通道维C拆分为C1和C0，其中`C = C1 * C0`。 |
| `ncdhw_layout_ptn` | `n, c, d, h, w` | `(N, C, D, H, W)` | `(C * D * H * W, D * H * W, H * W, W, 1)` | 三维卷积特征图的连续NCDHW排布。 |
| `ndc1hwc0_layout_ptn` | `n, d, c1, h, w, c0` | `(N, D, C1, H, W, C0)` | `(D * C1 * H * W * C0, C1 * H * W * C0, H * W * C0, W * C0, C0, 1)` | 三维卷积特征图的分形排布，将通道维C拆分为C1和C0。 |

上述layout均为行主序连续排布，stride的单位为元素。`nc1hwc0_layout_ptn`和`ndc1hwc0_layout_ptn`的C0由函数参数显式指定，不通过layout_trait推导。

### layout_trait的指定方式

layout_trait用于指定C0维度基数。make_frame_layout支持以下四种传递方式：

1. **`layout_trait_default<T>`**：根据数据类型T自动计算C0。T仅参与C0推导，不作为layout_trait中的数据类型信息保存。

   ```cpp
   make_frame_layout<nz_layout_ptn, layout_trait_default<float>>(m, n)
   ```

2. **C0常量类型**：直接指定C0维度的元素个数。`_8`、`_16`等类型是[Std::Int](../../../Utils-API/cpp_stdlib/type_traits/integral_constant.md)的简写形式，例如`_16`表示`Std::Int<16>`。

   ```cpp
   make_frame_layout<nz_layout_ptn, _16>(m, n)
   ```

3. **省略第二个模板参数**：在分形构造不需要用户指定C0，或C0由布局模式固定的场景下，可以只传入LayoutPattern。此时接口会根据LayoutPattern使用内置的C0取值。这样的分形有nd_layout_ptn、dn_layout_ptn、dn_ext_layout_ptn、nd_ext_layout_ptn、scalea_nd_layout_ptn、scalea_dn_layout_ptn、scaleb_nd_layout_ptn、scaleb_dn_layout_ptn。

   ```cpp
   make_frame_layout<nd_layout_ptn>(m, n)
   ```

4. **自定义Trait类型**：当根据数据类型推导C0、直接传入C0常量类型、省略第二个模板参数这三种方式都无法表达目标布局时，可以传入自定义Trait类型。自定义Trait需要提供`c0_element`成员，用于表示C0维度的元素个数。

   ```cpp
   struct my_layout_trait {
       static constexpr auto c0_element = Std::Int<16>{};
   };

   make_frame_layout<nz_layout_ptn, my_layout_trait>(m, n)
   ```

为兼容已有代码，也支持`layout_trait<T, C0>`写法，其中T仅用于保持源码兼容，实际layout_trait只使用C0作为布局特征。

## 参数说明

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| LayoutPattern | 输入 | 布局模式模板参数。支持的LayoutPattern及数据排布格式见[layout和层次化表述法](layout_hierarchical_representation.md)。 |
| Trait | 输入 | 第二个模板参数，用于描述C0取值。可传入`layout_trait_default<T>`、C0常量类型、带`c0_element`成员的自定义Trait类型或具体数据类型；省略时接口按LayoutPattern使用内置的C0取值。 |
| C0Element | 输入 | 使用非类型模板参数直接指定C0元素个数。 |
| Args | 输入 | 构造参数的类型参数包。 |
| DataType | 输入 | layout trait用于推导C0的元素类型。 |
| C0 | 输入 | 表示C0元素个数的编译期整型常量类型。 |
| args... | 输入 | 传入`make_frame_layout`或`frame_layout_format::operator()`的构造参数。矩阵场景常见为m和n，Batch矩阵场景为batch、m和n。二维卷积场景根据LayoutPattern传入n、c、h、w或n、c1、h、w、c0；三维卷积场景传入n、c、d、h、w或n、d、c1、h、w、c0。 |

## 返回值说明

返回`layout<shape, stride, Std::tuple<LayoutPattern, Trait>>`类型的对象，其中形状和步长由Layout Pattern和构造参数在编译期推导。

## 约束说明

- LayoutPattern必须是已支持的排布模式之一，不支持的模式会触发编译期报错。
- 构造参数的个数和含义由LayoutPattern决定。常见二维场景传入(m, n)即可。
- 使用`nc1hwc0_layout_ptn`时，用户需要根据数据类型和后续运算要求显式传入C0，并保证C轴数据符合`C = C1 * C0`。
- 使用`ndc1hwc0_layout_ptn`时，用户需要显式传入C0，并保证C轴数据符合`C = C1 * C0`。
- Batch模式传入(batch, m, n)，其中m和n表示单个矩阵的行数和列数，batch表示连续排布的矩阵个数。
- Trait必须为整型常量、带`c0_element`成员的Trait类型或具体数据类型，不支持的类型会触发编译期报错。传入具体数据类型时，该类型仅用于推导C0；Tensor的真实数据类型由`make_tensor`绑定的内存指针类型决定。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

// 构造GM端ND布局张量
auto layout_gm = make_frame_layout<nd_ext_layout_ptn, layout_trait_default<float>>(m, n);
auto gm_tensor = make_tensor(make_mem_ptr<location::gm>(gm_addr), layout_gm);

// 构造L1端NZ布局张量（L0A/L0B搬运的标准格式）
auto layout_l1 = make_frame_layout<nz_layout_ptn, float>(m, n);
auto l1_tensor = make_tensor(make_mem_ptr<location::l1>(l1_addr), layout_l1);

// 省略第二个模板参数构造NZ布局
auto layout_nz = make_frame_layout<nz_layout_ptn>(m, n);
auto l1_tensor_nz = make_tensor(make_mem_ptr<location::l1>(l1_addr), layout_nz);

// 指定C0元素数构造
auto layout_c0 = make_frame_layout<nz_layout_ptn, _16>(m, n);
auto l1_tensor_c0 = make_tensor(make_mem_ptr<location::l1>(l1_addr), layout_c0);

// 自定义Trait构造
struct my_layout_trait {
    static constexpr auto c0_element = Std::Int<16>{};
};
auto layout_custom = make_frame_layout<nz_layout_ptn, my_layout_trait>(m, n);

// 构造Batch矩阵布局
auto layout_batch_nz = make_frame_layout<nz_layout_ptn, float>(batch, m, n);
auto l1_batch_tensor = make_tensor(make_mem_ptr<location::l1>(l1_addr), layout_batch_nz);

// 构造MX场景的scale Batch布局
auto layout_batch_scalea = make_frame_layout<scalea_nd_layout_ptn>(batch, scale_m, scale_k);
auto gm_scale_tensor = make_tensor(make_mem_ptr<location::gm>(scale_addr), layout_batch_scalea);

// 构造卷积特征图layout
auto layout_nchw = make_frame_layout<nchw_layout_ptn>(n, c, h, w);
auto layout_nhwc = make_frame_layout<nhwc_layout_ptn>(n, h, w, c);
auto layout_nc1hwc0 = make_frame_layout<nc1hwc0_layout_ptn>(n, c1, h, w, c0);
```
