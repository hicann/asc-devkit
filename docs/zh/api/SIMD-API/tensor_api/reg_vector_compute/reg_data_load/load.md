# load

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

头文件路径为：`"tensor_api/experimental/arch/vector/reg_data_load.h"`，命名空间为`asc::te::experimental`。

`load`用于将Unified Buffer（UB）中的数据搬入[reg_tensor](../reg_tensor/reg_tensor.md)。接口支持以下两类目的寄存器：

- [矢量数据寄存器搬入](#矢量数据寄存器搬入)：根据源Tensor的元素类型推导寄存器类型，通过`load_sideband_mode`选择正常搬入、上采样、下采样、展开或解交织。`deintlv`模式返回`reg_pair`，其他模式返回`reg_tensor`。
- [掩码寄存器搬入](#掩码寄存器搬入)：通过`load<bool>`从`uint32_t`类型的UB Tensor按比特搬入掩码数据，支持`direct`、`upsample`和`downsample`模式，返回`reg_tensor<bool>`。

接口支持以下两种寻址方式：

- 传入`coord`时，从`tensor.engine() + tensor.layout()(coord)`对应的元素开始搬入。
- 不传入`coord`时，从`tensor`起始位置开始搬入。

## 函数原型

### 矢量数据寄存器搬入

- 指定坐标：

    ```cpp
    template <load_sideband_mode sideband_mode = load_sideband_mode::direct,
              typename Tensor,
              typename Coord>
    __simd_callee__ inline auto load(const Tensor& tensor, const Coord& coord)
    ```

- 从Tensor起始位置搬入：

    ```cpp
    template <load_sideband_mode sideband_mode = load_sideband_mode::direct,
              typename Tensor>
    __simd_callee__ inline auto load(const Tensor& tensor)
    ```

### 掩码寄存器搬入

- 指定坐标：

    ```cpp
    template <typename RegDataType,
              load_sideband_mode sideband_mode = load_sideband_mode::direct,
              typename Tensor,
              typename Coord>
    __simd_callee__ inline auto load(const Tensor& tensor, const Coord& coord)
    ```

- 从Tensor起始位置搬入：

    ```cpp
    template <typename RegDataType,
              load_sideband_mode sideband_mode = load_sideband_mode::direct,
              typename Tensor>
    __simd_callee__ inline auto load(const Tensor& tensor)
    ```

## 参数说明

### 矢量数据寄存器搬入

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| sideband_mode | 搬入模式，类型为`load_sideband_mode`，默认值为`load_sideband_mode::direct`。支持的模式请参考[表3](#表3-load_sideband_mode参数说明)。 |
| Tensor | 源Tensor类型，由函数实参自动推导。Tensor必须位于UB。 |
| Coord | 源Tensor的坐标类型，由函数实参自动推导。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| tensor | 输入 | 源Tensor，必须位于UB。 |
| coord | 输入 | 搬入起始元素在`tensor`逻辑布局中的坐标。 |

<a id="表3-load_sideband_mode参数说明"></a>

**表3** 矢量数据寄存器搬入模式

表中的VL表示一个矢量数据寄存器的长度，单位为Byte。`int4b_t`类型的数据在寄存器中以`int4x2_t`形式存储。`int4x2_t`数据类型由两个四位整型数打包为一个8比特存储单元，`fp4x2_e2m1_t`和`fp4x2_e1m2_t`数据类型由两个独立的四位浮点数打包为一个8比特存储单元，因此，对于这三种类型，下表所述的“元素”是指一个8比特对应的搬运单元，而不是单个4比特值；补零、重复、采样和交织等操作均以该搬运单元为粒度。

| 取值 | 含义 | 搬入起始地址对齐约束（Byte） |
| --- | --- | --- |
| load_sideband_mode::direct | 正常模式，从UB连续读取VL数据量并搬入一个`reg_tensor`。 | 32 |
| load_sideband_mode::deintlv | 解交织模式，从UB连续读取2\*VL数据量，将偶数索引和奇数索引的元素分别搬入返回值的`first`和`second`成员。 | 32 |
| load_sideband_mode::downsample | 2倍下采样模式，从UB读取2\*VL数据量，每隔一个元素保留一个元素，得到VL数据量。 | 32 |
| load_sideband_mode::unpack | 2倍展开模式，从UB读取VL/2数据量，在每个源元素后补一个0元素，展开为VL数据量。 | min(32, VL/2) |
| load_sideband_mode::unpack4 | 4倍展开模式，从UB读取VL/4数据量，在每个源元素后补三个0元素，展开为VL数据量。 | min(32, VL/4) |
| load_sideband_mode::upsample | 2倍上采样模式，从UB读取VL/2数据量，每个源元素在目的寄存器中重复两次。 | min(32, VL/2) |

### 掩码寄存器搬入

**表4** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| RegDataType | 目的寄存器元素类型，必须显式指定为`bool`。 |
| sideband_mode | 搬入模式，类型为`load_sideband_mode`，默认值为`load_sideband_mode::direct`。支持的模式请参考[表6](#表6-掩码寄存器搬入模式)。 |
| Tensor | 源Tensor类型，由函数实参自动推导。Tensor必须位于UB，元素类型必须为`uint32_t`。 |
| Coord | 源Tensor的坐标类型，由函数实参自动推导。 |

**表5** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| tensor | 输入 | 存放掩码比特的`uint32_t`类型UB Tensor。 |
| coord | 输入 | 搬入起始`uint32_t`元素在`tensor`逻辑布局中的坐标。 |

<a id="表6-掩码寄存器搬入模式"></a>

**表6** 掩码寄存器搬入模式

掩码寄存器搬入以比特为单位处理数据，不以`uint32_t`元素为单位进行采样。VL为256字节时，掩码寄存器的长度为VL/8，即32字节。

| 取值 | 含义 | UB读取量（Byte） | 搬入起始地址对齐约束（Byte） |
| --- | --- | --- | --- |
| load_sideband_mode::direct | 按原样搬入源比特。 | VL/8（32） | 32 |
| load_sideband_mode::upsample | 将每个源比特重复两次。 | VL/16（16） | 16 |
| load_sideband_mode::downsample | 保留偶数索引（从0开始）的源比特。 | VL/4（64） | 32 |

## 数据类型

### 矢量数据寄存器搬入

源Tensor支持的元素类型如下。未列出的类型或模式组合会产生编译期错误。

| sideband_mode | 支持的数据类型 |
| --- | --- |
| direct | `int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`、`int64_t` |
| deintlv | `int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float` |
| downsample、upsample | `int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t` |
| unpack | `int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float` |
| unpack4 | `int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t` |

### 掩码寄存器搬入

源Tensor的元素类型仅支持`uint32_t`，目的寄存器的元素类型仅支持`bool`。支持的模式请参考[表6](#表6-掩码寄存器搬入模式)。

## 返回值说明

接口按值返回寄存器对象，具体类型由源Tensor元素类型、搬入模式及是否显式指定`RegDataType`决定。

### 矢量数据寄存器搬入

- 对于非`int4b_t`类型，`deintlv`模式返回`reg_pair<typename Tensor::data_type>`，其`first.reg`和`second.reg`分别保存偶数索引和奇数索引元素的搬入结果。
- 对于非`int4b_t`类型，其他模式返回`reg_tensor<typename Tensor::data_type>`，其`reg`成员保存搬入结果。
- 当源Tensor元素类型为`int4b_t`时，`deintlv`模式返回`reg_pair<int4x2_t>`，其他模式返回`reg_tensor<int4x2_t>`。

### 掩码寄存器搬入

返回`reg_tensor<bool>`，其`reg`成员保存搬入的掩码比特。返回对象可直接作为`with_mask`的实参。

## 约束说明

- 仅支持位于UB的Tensor。
- 实际搬入起始地址必须满足[表3](#表3-load_sideband_mode参数说明)或[表6](#表6-掩码寄存器搬入模式)中的对齐约束，整个读取范围不得超出源Tensor的UB缓冲区。
- 传入`coord`时，坐标维数必须与Tensor布局匹配，且坐标不能越界。
- 搬入接口只初始化返回对象的`reg`成员，不初始化`mask`成员。将矢量数据寄存器结果用于读取`mask`的寄存器计算或搬出前，必须调用`with_mask`绑定有效掩码。
- 寄存器数据搬入应在`__simd_vf__`函数或`__simd_callee__`函数中完成。

## 调用示例

### 正常搬入

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/reg_data_load.h"

template <typename T, typename SrcTensor>
__simd_vf__ inline void LoadData(const SrcTensor& src)
{
    auto value = asc::te::experimental::load(src, asc::te::make_coord(0));
    value.with_mask(asc::te::experimental::all_mask<T>());
    // 使用value完成后续寄存器计算。
}
```

### 解交织搬入

```cpp
template <typename T, typename SrcTensor>
__simd_vf__ inline void LoadDeinterleaved(const SrcTensor& src)
{
    auto value = asc::te::experimental::load<asc::te::experimental::load_sideband_mode::deintlv>(
        src, asc::te::make_coord(0));

    auto mask = asc::te::experimental::all_mask<T>();
    value.first.with_mask(mask);
    value.second.with_mask(mask);
    // value.first和value.second分别保存偶数索引和奇数索引的搬运单元。
}
```

### 掩码寄存器搬入

`src`为`uint32_t`类型的UB Tensor。以下示例从其起始位置搬入原始掩码，并演示上采样和下采样。要同时满足三个调用的要求，`src`起始地址须按32字节对齐，缓冲区至少包含64字节。

```cpp
template <typename SrcTensor>
__simd_vf__ inline void LoadMasks(SrcTensor src)
{
    using namespace asc::te::experimental;
    auto direct_mask = load<bool>(src);
    auto upsampled_mask = load<bool, load_sideband_mode::upsample>(src);
    auto downsampled_mask = load<bool, load_sideband_mode::downsample>(src, asc::te::make_coord(0));
    // 各返回值的reg成员保存对应的掩码比特。
}
```
