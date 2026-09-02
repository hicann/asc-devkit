# load

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

头文件路径为：`"tensor_api/experimental/arch/vector/reg_data_load.h"`，命名空间为`asc::te::experimental`。

`load`用于将Unified Buffer（UB）中的数据搬入[reg_tensor](../reg_tensor/reg_tensor.md)。接口通过`load_sideband_mode`配置正常搬入、上采样、下采样、解压缩或解交织等数据排列方式。`deintlv`模式返回`reg_pair`，其他模式返回`reg_tensor`。

接口支持以下两种寻址方式：

- 传入`coord`时，从`tensor.engine() + tensor.layout()(coord)`对应的元素开始搬入。
- 不传入`coord`时，从`tensor`起始位置开始搬入。

## 函数原型

- 指定坐标：

    ```cpp
    template <load_sideband_mode sideband_mode = load_sideband_mode::direct,
              typename Tensor,
              typename Coord>
    __simd_callee__ inline decltype(auto) load(const Tensor& tensor, const Coord& coord)
    ```

- 从Tensor起始位置搬入：

    ```cpp
    template <load_sideband_mode sideband_mode = load_sideband_mode::direct,
              typename Tensor>
    __simd_callee__ inline decltype(auto) load(const Tensor& tensor)
    ```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| `sideband_mode` | 搬入模式，类型为`load_sideband_mode`，默认值为`load_sideband_mode::direct`。具体取值请参考[表3](#表3-load_sideband_mode参数说明)。 |
| `Tensor` | 源Tensor类型，由函数实参自动推导。Tensor必须位于UB。 |
| `Coord` | 源Tensor的坐标类型，由函数实参自动推导。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| `tensor` | 输入 | 源Tensor，必须位于UB。 |
| `coord` | 输入 | 搬入起始元素在`tensor`逻辑布局中的坐标。 |

<a id="表3-load_sideband_mode参数说明"></a>

**表3** `load_sideband_mode`参数说明

| 取值 | 含义 | 搬入起始地址对齐约束（Byte） |
| --- | --- | --- |
| `load_sideband_mode::direct` | 正常模式，从UB连续读取VL数据量并搬入一个`reg_tensor`。 | 32 |
| `load_sideband_mode::deintlv` | 解交织模式，从UB连续读取2\*VL数据量，将偶数索引和奇数索引的元素分别搬入返回值的`first`和`second`成员。 | 32 |
| `load_sideband_mode::downsample` | 2倍下采样模式，从UB读取2\*VL数据量，每隔一个元素保留一个元素，得到VL数据量。 | 32 |
| `load_sideband_mode::unpack` | 2倍展开模式，从UB读取VL/2数据量，在每个源元素后补一个0，展开为VL数据量。 | min(32, VL/2) |
| `load_sideband_mode::unpack4` | 4倍展开模式，从UB读取VL/4数据量，在每个源元素后补三个0，展开为VL数据量。 | min(32, VL/4) |
| `load_sideband_mode::upsample` | 2倍上采样模式，从UB读取VL/2数据量，每个源元素在目的寄存器中重复两次。 | min(32, VL/2) |

## 数据类型

Tensor元素类型与寄存器元素类型保持一致。不同模式支持的数据类型如下，未列出的类型或模式组合会产生编译期错误。

| `sideband_mode` | 支持的数据类型 |
| --- | --- |
| `direct` | `int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`int32_t`、`uint32_t`、`int64_t`、`uint64_t`、`half`、`bfloat16_t`、`float`、`hifloat8_t`、`fp8_e4m3fn_t`、`fp8_e5m2_t`、`fp8_e8m0_t`、`int4x2_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t` |
| `deintlv` | `int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`int32_t`、`uint32_t`、`half`、`bfloat16_t`、`float`、`hifloat8_t`、`fp8_e4m3fn_t`、`fp8_e5m2_t`、`fp8_e8m0_t`、`int4x2_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t` |
| `downsample`、`upsample` | 8bit和16bit元素类型，包括对应位宽的整数、浮点和打包4bit类型 |
| `unpack` | 8bit、16bit和32bit元素类型，包括对应位宽的整数、浮点和打包4bit类型 |
| `unpack4` | 8bit元素类型，包括`int8_t`、`uint8_t`、`hifloat8_t`、三种FP8类型和三种打包4bit类型 |

## 返回值说明

- `deintlv`模式返回`reg_pair<typename Tensor::data_type>`，其`first.reg`和`second.reg`分别保存偶数索引和奇数索引元素的搬入结果。
- 其他模式返回`reg_tensor<typename Tensor::data_type>`，其`reg`成员保存搬入结果。

## 约束说明

- 仅支持位于UB的Tensor。
- 搬入起始地址必须满足[表3](#表3-load_sideband_mode参数说明)中的对齐约束。
- 传入`coord`时，坐标维数必须与Tensor布局匹配，且坐标不能越界。
- 搬入接口只初始化返回对象或目的对象的`reg`成员，不初始化`mask`成员。将结果用于读取`mask`的寄存器计算或`store`模式前，必须调用`with_mask`绑定有效mask。
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
    // value.first和value.second分别保存偶数索引和奇数索引的元素。
}
```
