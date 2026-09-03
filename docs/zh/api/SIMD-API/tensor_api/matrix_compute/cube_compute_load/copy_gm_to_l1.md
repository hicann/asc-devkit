# copy（Global Memory到L1 Buffer数据搬运）

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

Tensor API通过`copy`接口统一执行不同通路数据搬运。该接口用于将Global Memory中的数据搬运到L1 Buffer。`copy`接口根据源张量和目的张量的存储位置、数据类型和布局选择具体搬运实现。搬运块数、搬运长度、源/目的侧步长以及格式转换相关信息由Tensor布局推导，用户不需要在`copy`调用中额外传入搬运参数。

接口支持完整Tensor搬运，也支持通过`dst_coord`、`src_coord`和`copy_shape`指定目的Tensor中的起始坐标、源Tensor中的起始坐标和搬运区域的形状，执行区域搬运。

该通路用于矩阵计算输入搬入，支持以下场景：

- 连续数据搬入：源数据从Global Memory搬入L1 Buffer后数据格式不变。
- 高维切分数据搬入：源/目的侧存在步长时，根据layout推导多段搬运。
- 随路格式转换搬入：从Global Memory搬入L1 Buffer的同时完成ND/DN等到NZ/ZN等的矩阵格式转换。
- Batch多矩阵搬入：源/目的layout带Batch轴时，一次`copy`完成多个矩阵的搬运，支持`ND2ND`、`ND2NZ`、`DN2NZ`、`ND2ZN`、`DN2ZN`、`ScaleAND2ZZ`、`ScaleADN2ZZ`、`ZZ2ZZ`、`ScaleBND2NN`、`ScaleBDN2NN`、`NN2NN`随路格式转换。
- Scale数据搬入：用于MX矩阵计算中ScaleA/ScaleB数据从Global Memory搬入L1 Buffer。
- 卷积特征图搬入：支持`NC1HWC0`到`NC1HWC0`的连续或切片搬入，以及`NHWC`、`NCHW`到`NC1HWC0`的随路格式转换。

## 函数原型

- 执行Global Memory到L1 Buffer搬运。

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const copy_atom<Atom>& atom,
        const DstTensor& dst, const SrcTensor& src)
    ```

- 根据源张量和目的张量的存储位置自动推导搬运通路，使用默认trait执行Global Memory到L1 Buffer搬运。

    ```cpp
    template <typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)
    ```

- 按指定源坐标、目的坐标和搬运形状执行Global Memory到L1 Buffer搬运。

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- 根据源张量和目的张量的存储位置自动推导搬运通路，使用默认trait按指定源坐标、目的坐标和搬运形状执行Global Memory到L1 Buffer搬运。

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
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation,
        const CopyTrait& trait)
    ```

Global Memory到L1 Buffer通路使用`copy_gm_to_l1{}`作为`operation`，使用`gm_to_l1_trait_default{}`作为默认trait对象。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| Atom | `copy_atom`的模板参数，由`make_copy`接口推导得到。 |
| DstTensor | 目的张量类型。Global Memory到L1 Buffer搬运时需为Tensor API Tensor类型，存储位置为`location::l1`。 |
| SrcTensor | 源张量类型。Global Memory到L1 Buffer搬运时需为Tensor API Tensor类型，存储位置为`location::gm`。 |
| DstCoord | 目的张量起始坐标类型，也可为`zero_coord`。 |
| SrcCoord | 源张量起始坐标类型，也可为`zero_coord`。 |
| CopyShape | 搬运区域形状的类型。 |
| CopyOperation | 搬运操作对象类型。Global Memory到L1 Buffer搬运取`copy_gm_to_l1`。 |
| CopyTrait | 搬运trait对象类型。Global Memory到L1 Buffer默认取`gm_to_l1_trait_default`。 |

**表2**  `copy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| atom | 输入 | 搬运原子对象，可由`make_copy`接口构造。 |
| dst | 输出 | 目的张量，存储位置为`location::l1`。 |
| src | 输入 | 源张量，存储位置为`location::gm`。 |
| dst_coord | 输入 | 搬运区域在目的张量中的起始坐标，也可传入`zero_coord`。 |
| src_coord | 输入 | 搬运区域在源张量中的起始坐标，也可传入`zero_coord`。 |
| copy_shape | 输入 | 搬运区域的形状，用于指定搬运区域的大小。 |

**表3**  `make_copy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| operation | 输入 | 搬运操作对象。Global Memory到L1 Buffer搬运取`copy_gm_to_l1{}`。 |
| trait | 输入 | 搬运trait对象。Global Memory到L1 Buffer默认取`gm_to_l1_trait_default{}`。 |

### copy_gm_to_l1说明

`copy_gm_to_l1`用于标识Global Memory到L1 Buffer数据搬运通路，仅列出其public内容：

```cpp
struct copy_gm_to_l1 {
    template <typename Trait, const Trait& trait, typename... Args>
    __aicore__ inline static void copy(const Args&... args);
};
```

`copy`静态成员函数用于接收Trait和搬运参数，并分发Global Memory到L1 Buffer数据搬运。

### gm_to_l1_trait说明

`gm_to_l1_trait`表示该通路的静态配置。当前没有可配置成员，定义如下：

```cpp
struct gm_to_l1_trait {};
```

`gm_to_l1_trait_default`提供该通路的默认Trait类型和默认值：

```cpp
struct gm_to_l1_trait_default {
    using trait_type = gm_to_l1_trait;
    static constexpr const trait_type value = {};
};
```


## 数据类型

源张量和目的张量的数据类型需要保持一致。

| 源操作数数据格式 | 目的操作数数据格式 | 源操作数/目的操作数数据类型 |
| -- | -- | -- |
| ND | ND | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float/int64_t/uint64_t |
| ND | NZ | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| ND | ZN | int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| DN | NZ | int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| DN | ZN | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| NZ | NZ | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float/int64_t/uint64_t |
| ZN | ZN | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float/int64_t/uint64_t |
| ScaleAND | ZZ | fp8_e8m0_t |
| ScaleADN | ZZ | fp8_e8m0_t |
| ZZ | ZZ | fp8_e8m0_t |
| ScaleBND | NN | fp8_e8m0_t |
| ScaleBDN | NN | fp8_e8m0_t |
| NN | NN | fp8_e8m0_t |
| NC1HWC0 | NC1HWC0 | int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| NHWC | NC1HWC0 | int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| NCHW | NC1HWC0 | int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |

## 返回值说明

`copy`无返回值。`make_copy`返回`copy_atom`对象。

## 约束说明

- 位于Global Memory的源地址必须1B对齐，位于L1 Buffer的目的地址必须32B对齐。
- 数据连续搬运场景中，搬运字节数需要32B对齐。若未对齐，搬运量会向下取整到32B对齐。
- 高维切分搬运场景中，搬运长度、源步长和目的步长均按32B对齐。
- 当输入数据是b4类型时，按b8类型搬运粒度处理，layout推导参数需要满足对应粒度约束。
- Batch多矩阵搬入仅支持`ND2ND`、`ND2NZ`、`DN2NZ`、`ND2ZN`、`DN2ZN`、`ScaleAND2ZZ`、`ScaleADN2ZZ`、`ZZ2ZZ`、`ScaleBND2NN`、`ScaleBDN2NN`、`NN2NN`格式转换；Batch数受搬运指令字段范围限制，需不大于4095。
- `NHWC`或`NCHW`到`NC1HWC0`的卷积特征图搬入当前仅支持N为1。源张量和目的张量的H、W、C需要一致，并保证`C = C1 * C0`。
- `NC1HWC0`到`NC1HWC0`搬入时，源张量和目的张量的N、C1、H、W、C0需要一致。

## 关键特性说明

### 随路格式转换搬入

随路格式转换搬入表示从Global Memory搬入L1 Buffer时同步完成矩阵格式转换。接口根据源layout和目的layout自动选择转换方向，支持以下格式转换：
`ND2ND`、`ND2NZ`、`ND2ZN`、`DN2NZ`、`DN2ZN`、`NZ2NZ`、`ZN2ZN`。
ND到NZ的格式转换等价于DN到ZN的格式转换，DN到NZ的格式转换等价于ND到ZN的格式转换。

#### ND2NZ连续搬运

ND2NZ连续搬运表示源矩阵按ND格式连续排布，目的矩阵按NZ格式连续排布。该场景由layout自动推导单段搬运参数，搬运过程中完成ND到NZ的格式转换，目的端按32B对齐补0。

#### ND2NZ非连续搬运

ND2NZ非连续搬运表示源侧或目的侧存在步长、切分维度等高维切分信息。该场景归属于高维切分数据搬入，`copy`会根据layout自动推导多段搬运参数，搬运过程中完成ND到NZ的格式转换，目的端按32B对齐补0。

#### DN2NZ连续搬运

DN2NZ连续搬运表示源矩阵按DN格式连续排布，目的矩阵按NZ格式连续排布。该场景由layout自动推导单段搬运参数，搬运过程中完成DN到NZ的排布转换，目的端按32B对齐补0。

#### DN2NZ非连续搬运

DN2NZ非连续搬运表示源侧或目的侧存在步长、切分维度等高维切分信息。该场景归属于高维切分数据搬入，`copy`会根据layout自动推导多段搬运参数，搬运过程中完成DN到NZ的格式转换，目的端按32B对齐补0。

### Scale数据搬入

Scale数据搬入用于矩阵计算中scale相关数据从Global Memory搬入L1 Buffer。A矩阵相关scale支持`ScaleAND`到`ZZ`、`ScaleADN`到`ZZ`、`ZZ`到`ZZ`的格式转换。b矩阵相关scale支持`ScaleBND`到`NN`、`ScaleBDN`到`NN`、`NN`到`NN`的格式转换。

### 卷积特征图搬入

卷积特征图搬入支持以下layout组合：

- 源张量和目的张量均为`NC1HWC0`时，搬入后数据格式不变。对源张量使用`slice`截取W轴局部数据后，`copy`根据切片layout完成非连续搬入。
- 源张量为`NHWC`、目的张量为`NC1HWC0`时，搬入过程中将C轴拆分为C1和C0。
- 源张量为`NCHW`、目的张量为`NC1HWC0`时，搬入过程中同时完成通道维排布转换和C轴拆分。

### Batch多矩阵搬入

Batch多矩阵搬入表示一次`copy`完成多个矩阵的搬运。当源张量和目的张量的layout带有Batch轴时，`copy`一条指令完成多个矩阵的搬运，接口调用方式与单矩阵搬运完全一致，无需额外传入Batch相关参数。

带Batch轴的layout在原矩阵layout外层增加一维Batch，Batch内存连续场景可通过`make_frame_layout`构造，Batch内存非连续场景可通过`make_layout`显式指定shape和stride构造。

例如Batch内存连续场景通过`make_frame_layout<LayoutPattern>(b, m, k)`构造，其中`b`为Batch数，`m`、`k`为单个矩阵的行、列。layout语义为：

- shape为`(b, (m, k))`。
- stride为`(m * k, (k, 1))`，即各矩阵在内存上按`m * k`连续排布，单矩阵内存按照ND格式连续排布。

Batch内存非连续场景可通过`make_layout(shape, stride)`构造，其中`shape`描述Batch数和单个矩阵的shape，`stride`描述Batch轴和矩阵内部各轴的地址间隔。以按K方向跨Batch连续排布的ND矩阵为例，layout语义为：

- shape为`(b, (m, k))`。
- stride为`(k, (b * k, 1))`，即各矩阵在内存上为非连续，内存跨Batch在K方向上连续排布。

搬运行为：

- 源（Global Memory）侧：各Batch矩阵的源地址间距由layout的Batch轴stride决定，借助Batch轴stride参数，Batch数据支持Batch间连续和非连续两种排布形式。
- 目的（L1 Buffer）侧：各Batch矩阵在L1 Buffer中连续存放，Batch间的地址间隔等于对齐后单个矩阵的占用空间。

## 调用示例

以下示例将Global Memory中的ND矩阵搬入L1 Buffer，并转换为NZ格式。

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

constexpr uint32_t m = 32;
constexpr uint32_t k = 64;

__aicore__ inline void copy_gm_to_l1_example(__gm__ half* gm_addr)
{
    __cbuf__ half l1_buf[m * k];

    // 源张量：Global Memory上的ND格式矩阵；目的张量：L1 Buffer上的NZ格式矩阵
    auto gm_a = make_tensor(make_mem_ptr(gm_addr), make_frame_layout<nd_ext_layout_ptn>(m, k));
    auto l1_a = make_tensor(make_mem_ptr(l1_buf), make_frame_layout<nz_layout_ptn, half>(m, k));

    // 构造GM到L1搬运原子对象，copy根据源/目的layout自动完成ND到NZ的随路格式转换
    auto copy_gm_to_l1_atom = make_copy(copy_gm_to_l1{}, gm_to_l1_trait_default{});
    copy(copy_gm_to_l1_atom, l1_a, gm_a);
}
```

以下示例将Global Memory中的b个ND矩阵一次性搬入L1 Buffer，并转换为NZ格式。源张量和目的张量均使用带Batch轴的layout，接口调用方式与单矩阵搬运一致。

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

constexpr uint32_t b = 4;
constexpr uint32_t m = 32;
constexpr uint32_t k = 64;

__aicore__ inline void copy_batch_gm_to_l1(__gm__ half* gm_addr)
{
    __cbuf__ half l1_buf[b * m * k];

    // 构造带Batch轴的源/目的layout（b个矩阵），make_frame_layout在原layout外层增加一维Batch
    auto gm_a = make_tensor(make_mem_ptr(gm_addr), make_frame_layout<nd_ext_layout_ptn>(b, m, k));
    auto l1_a = make_tensor(make_mem_ptr(l1_buf), make_frame_layout<nz_layout_ptn, half>(b, m, k));

    // 调用方式与单矩阵一致，copy一条指令完成b个矩阵的ND到NZ搬运
    auto copy_gm_to_l1_atom = make_copy(copy_gm_to_l1{}, gm_to_l1_trait_default{});
    copy(copy_gm_to_l1_atom, l1_a, gm_a);
}
```

以下示例将Global Memory中的NHWC卷积特征图搬入L1 Buffer，并转换为NC1HWC0格式。NCHW输入只需将源layout替换为`make_frame_layout<nchw_layout_ptn>(n, c, h, w)`。

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

constexpr uint32_t n = 1;
constexpr uint32_t h = 4;
constexpr uint32_t w = 4;
constexpr uint32_t c0 = 16;
constexpr uint32_t c = 32;
constexpr uint32_t c1 = c / c0;

__aicore__ inline void copy_conv_input_gm_to_l1(__gm__ half* gm_addr)
{
    __cbuf__ half l1_buf[n * c * h * w];

    auto gm_feature = make_tensor(
        make_mem_ptr(gm_addr), make_frame_layout<nhwc_layout_ptn>(n, h, w, c));
    auto l1_feature = make_tensor(
        make_mem_ptr(l1_buf), make_frame_layout<nc1hwc0_layout_ptn>(n, c1, h, w, c0));

    auto copy_gm_to_l1_atom = make_copy(copy_gm_to_l1{}, gm_to_l1_trait_default{});
    copy(copy_gm_to_l1_atom, l1_feature, gm_feature);
}
```
