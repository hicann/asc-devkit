# store

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
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

头文件路径为：`"tensor_api/experimental/arch/vector/reg_data_store.h"`，命名空间为`asc::te::experimental`。

`store`用于将[reg_tensor](../reg_tensor/reg_tensor.md)中的数据搬出到Unified Buffer（UB）Tensor。接口支持以下两类源寄存器：

- [矢量数据寄存器搬出](#矢量数据寄存器搬出)：通过`store_sideband_mode`选择正常搬出、首元素搬出、交织搬出或压缩搬出。
- [掩码寄存器搬出](#掩码寄存器搬出)：使用`reg_tensor<bool>`作为单源操作数，通过`direct`按原样搬出掩码比特，或通过`pack`隔位压缩搬出。两种模式均不读取`src.mask`。

接口支持以下两种寻址方式：

- 传入`coord`时，从`tensor.engine() + tensor.layout()(coord)`对应的元素开始写入。
- 不传入`coord`时，从`tensor`起始位置开始写入。

`intlv`模式使用双源操作数重载，其他模式使用单源操作数重载。

## 函数原型

### 矢量数据寄存器搬出

<a id="store-single-source"></a>

- 单源操作数，指定坐标：

    ```cpp
    template <store_sideband_mode sideband_mode = store_sideband_mode::direct,
              typename Tensor,
              typename Coord,
              typename RegDataType>
    __simd_callee__ inline void store(
        Tensor& tensor,
        const Coord& coord,
        const reg_tensor<RegDataType>& src)
    ```

- 单源操作数，从Tensor起始位置搬出：

    ```cpp
    template <store_sideband_mode sideband_mode = store_sideband_mode::direct,
              typename Tensor,
              typename RegDataType>
    __simd_callee__ inline void store(
        Tensor& tensor, const reg_tensor<RegDataType>& src)
    ```

- 双源操作数，指定坐标：

    ```cpp
    template <store_sideband_mode sideband_mode = store_sideband_mode::intlv,
              typename Tensor,
              typename Coord,
              typename RegDataType>
    __simd_callee__ inline void store(
        Tensor& tensor,
        const Coord& coord,
        const reg_tensor<RegDataType>& src0,
        const reg_tensor<RegDataType>& src1)
    ```

- 双源操作数，从Tensor起始位置搬出：

    ```cpp
    template <store_sideband_mode sideband_mode = store_sideband_mode::intlv,
              typename Tensor,
              typename RegDataType>
    __simd_callee__ inline void store(
        Tensor& tensor,
        const reg_tensor<RegDataType>& src0,
        const reg_tensor<RegDataType>& src1)
    ```

### 掩码寄存器搬出

复用上述[两个单源操作数原型](#store-single-source)，分别支持指定`coord`和从Tensor起始位置搬出。传入`reg_tensor<bool>`，由`src`推导出`RegDataType`为`bool`。不支持双源操作数原型。

## 参数说明

### 矢量数据寄存器搬出

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| sideband_mode | 搬出模式，类型为`store_sideband_mode`。单源操作数重载的默认值为`store_sideband_mode::direct`；双源操作数重载的默认值为`store_sideband_mode::intlv`。支持的模式请参考[表3](#表3-store_sideband_mode参数说明)。 |
| Tensor | 目的Tensor类型，由函数实参自动推导。Tensor必须位于UB。 |
| Coord | 目的Tensor的坐标类型，由函数实参自动推导。 |
| RegDataType | 源寄存器元素类型，由函数实参自动推导。与目的Tensor的元素类型一致，但目的Tensor的元素类型为`int4b_t`时，源寄存器的元素类型为`int4x2_t`。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| tensor | 输出 | 目的Tensor，必须位于UB。 |
| coord | 输入 | 搬出起始元素在`tensor`逻辑布局中的坐标。 |
| src | 输入 | 单源操作数重载的源寄存器。`direct`、`pack`和`pack_quarter`使用`src.mask`筛选有效元素，`store_1st`忽略`src.mask`。 |
| src0 | 输入 | `intlv`模式的第一个源寄存器。该模式忽略`src0.mask`。 |
| src1 | 输入 | `intlv`模式的第二个源寄存器。该模式忽略`src1.mask`。 |

<a id="表3-store_sideband_mode参数说明"></a>

**表3** 矢量数据寄存器搬出模式

表中的VL表示一个矢量数据寄存器的长度，单位为Byte。`int4b_t`类型的数据在寄存器中以`int4x2_t`形式存储。`int4x2_t`数据类型由两个四位整型数打包为一个8比特存储单元，`fp4x2_e2m1_t`和`fp4x2_e1m2_t`数据类型由两个独立的四位浮点数打包为一个8比特存储单元，因此，对于这三种类型，下表所述的“元素”是指一个8比特对应的搬运单元，而不是单个4比特值。

| 取值 | 含义 | 搬出起始地址对齐约束（Byte） |
| --- | --- | --- |
| store_sideband_mode::direct | 正常模式，根据`src.mask`将源寄存器中的有效元素连续写入UB中的对应位置。 | 32 |
| store_sideband_mode::store_1st | 仅将`src`的第一个元素写入UB，忽略`src.mask`。 | `sizeof(RegDataType)` |
| store_sideband_mode::intlv | 交织模式，将`src0`和`src1`中的元素交替写入UB，得到2\*VL数据量，忽略两个源操作数的`mask`。仅支持双源操作数重载。 | 32 |
| store_sideband_mode::pack | 2倍压缩模式，根据`src.mask`筛选有效元素，将每个有效元素的低半部分bit写入压缩后的位置。 | min(32, VL/2) |
| store_sideband_mode::pack_quarter | 4倍压缩模式，根据`src.mask`筛选有效元素，将每个有效32bit元素的低8bit写入压缩后的位置。 | min(32, VL/4) |

### 掩码寄存器搬出

**表4** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| sideband_mode | 搬出模式，类型为`store_sideband_mode`，默认值为`store_sideband_mode::direct`。支持的模式请参考[表6](#表6-掩码寄存器搬出模式)。 |
| Tensor | 目的Tensor类型，由函数实参自动推导。Tensor必须位于UB，支持的元素类型请参考[数据类型](#掩码寄存器搬出-2)。 |
| Coord | 目的Tensor的坐标类型，由函数实参自动推导。 |
| RegDataType | 源寄存器元素类型，从`reg_tensor<bool>`实参推导为`bool`。 |

**表5** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| tensor | 输出 | 用于存放掩码比特的UB Tensor。 |
| coord | 输入 | 搬出起始元素在`tensor`逻辑布局中的坐标。 |
| src | 输入 | 类型为`reg_tensor<bool>`。仅搬出`src.reg`中的掩码比特，不读取`src.mask`。 |

<a id="表6-掩码寄存器搬出模式"></a>

**表6** 掩码寄存器搬出模式

掩码比特连续写入UB，不会转换为逐元素的布尔值。VL为256字节时，掩码寄存器的长度为VL/8，即32字节。

| 取值 | 含义 | UB写入量（Byte） | 搬出起始地址对齐约束（Byte） |
| --- | --- | --- | --- |
| store_sideband_mode::direct | 按原样写入`src.reg`中的全部比特。 | VL/8（32） | 32 |
| store_sideband_mode::pack | 保留`src.reg`中偶数索引（从0开始）的比特，按顺序连续写入。 | VL/16（16） | 16 |

## 数据类型

### 矢量数据寄存器搬出

支持的目的Tensor元素类型如下。未列出的类型或模式组合会产生编译期错误。其中，源寄存器类型为`reg_tensor<int4x2_t>`时，目的Tensor对应的数据类型为`int4b_t`。

| sideband_mode | 支持的数据类型 |
| --- | --- |
| direct、store_1st | `int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float` |
| intlv | `int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t` |
| pack | `int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`、`int64_t` |
| pack_quarter | `int32_t`、`uint32_t`、`float` |

### 掩码寄存器搬出

源寄存器元素类型为`bool`，目的Tensor的元素类型仅支持`uint8_t`、`uint16_t`和`uint32_t`。支持的模式请参考[表6](#表6-掩码寄存器搬出模式)。

## 返回值说明

两类搬出均无返回值。

## 约束说明

- 仅支持位于UB的Tensor。
- 实际搬出起始地址必须满足[表3](#表3-store_sideband_mode参数说明)或[表6](#表6-掩码寄存器搬出模式)中的对齐约束。
- 传入`coord`时，坐标维数必须与Tensor布局匹配，且坐标不能越界。
- 单源操作数重载不支持`store_sideband_mode::intlv`；双源操作数重载仅支持`store_sideband_mode::intlv`。
- 矢量数据寄存器搬出时，`direct`、`pack`和`pack_quarter`模式要求`src.reg`和`src.mask`均已初始化；`store_1st`和`intlv`模式仅要求参与搬出的`reg`成员已初始化。
- 矢量数据寄存器以`direct`模式搬出时，如果目的UB的剩余空间不足一个VL，必须通过`src.mask`关闭越界位置，并保证目的UB至少剩余一个DataBlock（32B）空间。
- 掩码寄存器搬出时，仅需初始化`src.reg`。目的Tensor的UB缓冲区必须能容纳[表6](#表6-掩码寄存器搬出模式)所列的完整写入量，不能通过`src.mask`缩减写入范围。
- 寄存器数据搬出应在`__simd_vf__`函数或`__simd_callee__`函数中完成。

## 调用示例

### 正常搬出

以下示例将源Tensor搬入寄存器，绑定全有效mask后写入目的Tensor。

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/reg_data_load.h"
#include "tensor_api/experimental/arch/vector/reg_data_store.h"

template <typename T, typename SrcTensor, typename DstTensor>
__simd_vf__ inline void LoadAndStore(const SrcTensor& src, DstTensor& dst)
{
    auto coord = asc::te::make_coord(0);
    auto value = asc::te::experimental::load(src, coord);
    value.with_mask(asc::te::experimental::all_mask<T>());
    asc::te::experimental::store(dst, coord, value);
}
```

### 交织搬出

```cpp
template <typename T, typename DstTensor>
__simd_vf__ inline void StoreInterleaved(
    DstTensor& dst,
    const asc::te::experimental::reg_tensor<T>& even,
    const asc::te::experimental::reg_tensor<T>& odd)
{
    asc::te::experimental::store<asc::te::experimental::store_sideband_mode::intlv>(
        dst, asc::te::make_coord(0), even, odd);
}
```

### 掩码寄存器搬出

以下示例将同一个掩码分别搬出到两个UB Tensor。两个Tensor的元素类型均可为`uint8_t`、`uint16_t`或`uint32_t`。`direct_dst`须按32字节对齐且至少包含32字节，`packed_dst`须按16字节对齐且至少包含16字节。

```cpp
template <typename DirectTensor, typename PackedTensor>
__simd_vf__ inline void StoreMask(DirectTensor direct_dst, PackedTensor packed_dst)
{
    using namespace asc::te::experimental;
    auto mask = all_mask<uint16_t>();
    store(direct_dst, mask);
    store<store_sideband_mode::pack>(packed_dst, asc::te::make_coord(0), mask);
}
```
