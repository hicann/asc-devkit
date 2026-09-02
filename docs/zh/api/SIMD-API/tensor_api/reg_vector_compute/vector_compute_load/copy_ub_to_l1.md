# copy（Unified Buffer到L1 Buffer的数据搬运）

> [!NOTE]说明
> 本接口为试验接口，在后续版本中可能会调整或改进，不保证后续兼容性。请开发者在使用过程中关注后续版本更新。

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

该接口用于将Unified Buffer（UB）中的数据搬运到L1 Buffer。接口根据源、目的Tensor的存储位置自动选择`copy_ub_to_l1`通路，并根据Layout推导数据块个数、单块长度以及源、目的块间间隔，同时支持连续和非连续的高维切分数据搬运。

当前支持ND2ND、DN2DN、NZ2NZ和ZN2ZN搬运。源、目的Layout Pattern必须相同，搬运过程中不执行格式转换和数据类型转换，数据格式和内容保持不变。该通路可用于将矢量计算生成或预处理的数据转移到L1 Buffer，供后续矩阵计算或其他片上流程使用。

## 函数原型

```cpp
template <typename DstTensor, typename SrcTensor>
__aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src)

template <typename Atom, typename DstTensor, typename SrcTensor>
__aicore__ inline void copy(const copy_atom<Atom>& atom,
    const DstTensor& dst, const SrcTensor& src)

template <typename DstTensor, typename SrcTensor, typename DstCoord,
    typename SrcCoord, typename CopyShape>
__aicore__ inline void copy(const DstTensor& dst, const SrcTensor& src,
    const DstCoord& dst_coord, const SrcCoord& src_coord,
    const CopyShape& copy_shape)

template <typename CopyOperation>
__aicore__ inline constexpr auto make_copy(const CopyOperation& operation)

template <typename CopyOperation, typename CopyTrait>
__aicore__ inline constexpr auto make_copy(
    const CopyOperation& operation, const CopyTrait& trait)
```

UB到L1 Buffer通路使用`copy_ub_to_l1{}`作为搬运操作对象，默认trait为`ub_to_l1_trait_default{}`。

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| atom | 输入 | 搬运原子对象，可通过`make_copy(copy_ub_to_l1{})`或`make_copy(copy_ub_to_l1{}, ub_to_l1_trait_default{})`构造。 |
| dst | 输出 | 目的Tensor，存储位置必须为`location::l1`。 |
| src | 输入 | 源Tensor，存储位置必须为`location::ub`。 |
| dst_coord | 输入 | 目的区域起始坐标。显式坐标的结构应与`copy_shape`一致，也可以使用`zero_coord`。 |
| src_coord | 输入 | 源区域起始坐标。显式坐标的结构应与`copy_shape`一致，也可以使用`zero_coord`。 |
| copy_shape | 输入 | 搬运区域的逻辑Shape。接口根据该Shape及源、目的Layout推导起始地址和搬运参数。 |

### 搬运参数推导

`copy`根据源、目的Tensor的Layout自动推导这些参数。单块长度和源、目的块间间隔均以DataBlock为单位，1个DataBlock为32字节。

对于整Tensor搬运，各Layout模式的推导规则如下。表中的“每个DataBlock的元素数”等于`32 / sizeof(T)`；对于b4类型，按对应的32字节搬运粒度计算。

| Layout模式 | 数据块个数 | 单块长度 | 源、目的块间间隔 |
| :--- | :--- | :--- | :--- |
| ND | 源Layout的总行数 | 总列数除以每个DataBlock的元素数，向上取整 | 对应Tensor的行步长减去总列数，再除以每个DataBlock的元素数并向上取整 |
| DN | 源Layout的总列数 | 总行数除以每个DataBlock的元素数，向上取整 | 对应Tensor的列步长减去总行数，再除以每个DataBlock的元素数并向上取整 |
| NZ | 分形格式的外层列数 | Layout的总行数 | 对应Tensor的外层列步长除以每个DataBlock的元素数，再减去单块长度 |
| ZN | 分形格式的外层行数 | Layout的总列数 | 对应Tensor的外层行步长除以每个DataBlock的元素数，再减去单块长度 |

推导结果与`DataCopyParams`直接对应：数据块个数对应`blockCount`，单块长度对应`blockLen`，源、目的块间间隔分别对应`srcGap`和`dstGap`。调用`copy`时无需构造或传入`DataCopyParams`。

使用区域搬运重载时，接口根据`src_coord`、`dst_coord`和`copy_shape`解析起始地址和搬运区域。ND、DN模式按区域行列数推导参数；NZ、ZN模式还会结合分形内轴和Batch维度推导数据块个数。

## 数据类型

源Tensor和目的Tensor的数据类型必须相同。

| 源/目的Layout | 支持的数据类型 |
| :--- | :--- |
| ND、DN、NZ、ZN | `fp4x2_e1m2_t`、`fp4x2_e2m1_t`、`int8_t`、`uint8_t`、`hifloat8_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`、`int64_t` |

## 返回值说明

`copy`无返回值。`make_copy`返回`copy_atom`对象。

## 约束说明

- 源Tensor必须位于UB，目的Tensor必须位于L1 Buffer。
- 源UB地址和目的L1 Buffer地址均需满足32字节对齐要求。
- 源、目的Layout Pattern必须相同。支持ND2ND、DN2DN、NZ2NZ和ZN2ZN，不支持随路格式转换。
- 源、目的数据类型必须相同，目的Tensor或目的区域容量必须不小于搬运区域。
- 搬运长度和源/目的间隔以32字节DataBlock为粒度。ND和DN区域大小不能整除DataBlock元素数时，接口按DataBlock粒度向上取整，调用方必须保证源、目的缓冲区具备相应空间。
- Layout推导出的参数必须满足底层`asc_copy_ub2l1`指令的字段范围。有效搬运的数据块个数取值范围为[1, 4095]，单块长度取值范围为[1, 65535]个DataBlock，源、目的块间间隔取值范围均为[0, 65535]个DataBlock。
- Tensor API要求源、目的Layout以及区域搬运Shape合法，不能依赖数据块个数或单块长度为0表示空操作。
- 连续执行多个`copy`且目的L1 Buffer区域存在重叠时，必须在两次搬运之间插入MTE2流水同步，例如调用`PipeBarrier<PIPE_MTE2>()`，避免后一次搬运覆盖前一次尚未完成的数据。

## 关键特性说明

### DataBlock粒度搬运

ND通路按行搬运，DN通路按列搬运。接口根据数据类型计算一个32字节DataBlock可容纳的元素数，并将逻辑搬运长度向上取整为DataBlock数量。NZ和ZN通路根据分形内轴直接推导DataBlock参数。

例如，`half`类型的一个DataBlock包含16个元素。搬运一行20个`half`元素时，单块长度向上取整为2个DataBlock，底层实际按32个`half`元素的空间处理。因此源、目的缓冲区及Layout步长必须覆盖向上取整后的范围。

### 连续和非连续搬运

源、目的块间间隔均为0时，相邻数据块首尾相接，属于连续搬运。任一块间间隔大于0时，属于非连续搬运；接口只搬运数据块覆盖的地址范围，不读取源间隔，也不写入目的间隔。当数据块个数为1时，块间间隔不影响搬运结果。

以官方`DataCopy`示例中的`half`数据为例，每个数据块包含8个DataBlock，即256字节：

| 数据块个数 | 单块长度 | 源块间间隔 | 目的块间间隔 | 搬运效果 |
| :--- | :--- | :--- | :--- | :--- |
| 2 | 8个DataBlock | 0 | 1个DataBlock | UB中的两个数据块连续读取；写入L1 Buffer时，两个数据块之间保留32字节间隔。 |

### 局部区域搬运

区域重载支持在源、目的Tensor中指定不同的起始坐标。接口根据`copy_shape`和Layout Stride生成多块搬运，可用于将UB中的局部矩形区域写入L1 Buffer的指定位置。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

constexpr uint32_t M = 8;
constexpr uint32_t N = 64;

__aicore__ inline void copy_ub_to_l1_example()
{
    __ubuf__ half ub_buf[M * N];
    __cbuf__ half l1_buf[M * N];

    auto layout = asc::te::make_frame_layout<asc::te::nd_ext_layout_ptn,
        asc::te::layout_trait_default<half>>(M, N);
    auto src = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::ub>(ub_buf), layout);
    auto dst = asc::te::make_tensor(asc::te::make_mem_ptr<asc::te::location::l1>(l1_buf), layout);

    asc::te::copy(dst, src);
}
```

该示例的ND Layout包含8行，每行64个`half`元素。一个DataBlock包含16个`half`元素，因此接口推导出`blockCount = 8`、`blockLen = 4`、`srcGap = 0`、`dstGap = 0`，属于连续搬运。

以下示例显式构造搬运原子对象，并搬运局部区域：

```cpp
auto atom = asc::te::make_copy(asc::te::copy_ub_to_l1{}, asc::te::ub_to_l1_trait_default{});
asc::te::copy(atom, dst, src, asc::te::make_coord(1, 0), asc::te::make_coord(2, 0),
    asc::te::make_shape(2, 32));
```

该区域包含2行，每行搬运32个`half`元素，单块长度为2个DataBlock。源、目的Tensor的行步长均为64个元素，每行有效区域之后均保留32个元素，即2个DataBlock，因此接口推导出`blockCount = 2`、`blockLen = 2`、`srcGap = 2`、`dstGap = 2`。
