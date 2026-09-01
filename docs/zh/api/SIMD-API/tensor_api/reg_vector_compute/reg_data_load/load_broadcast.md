# load_broadcast

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

`load_broadcast`用于从Unified Buffer（UB）Tensor中搬入数据，并按照`broadcast_mode`指定的方式广播到一个[reg_tensor](../reg_tensor/reg_tensor.md)。

传入`coord`时，从`tensor.engine() + tensor.layout()(coord)`对应的元素开始搬入；不传入`coord`时，从`tensor`起始位置开始搬入。

## 函数原型

- 指定坐标：

    ```cpp
    template <broadcast_mode brc_mode = broadcast_mode::elem,
              typename Tensor,
              typename Coord>
    __simd_callee__ inline decltype(auto) load_broadcast(
        const Tensor& tensor, const Coord& coord)
    ```

- 从Tensor起始位置搬入：

    ```cpp
    template <broadcast_mode brc_mode = broadcast_mode::elem,
              typename Tensor>
    __simd_callee__ inline decltype(auto) load_broadcast(const Tensor& tensor)
    ```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| `brc_mode` | 广播模式，类型为`broadcast_mode`，默认值为`broadcast_mode::elem`。具体取值请参考[表3](#表3-broadcast_mode参数说明)。 |
| `Tensor` | 源Tensor类型，由函数实参自动推导。Tensor必须位于UB。 |
| `Coord` | 源Tensor的坐标类型，由函数实参自动推导。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| `tensor` | 输入 | 源Tensor，必须位于UB。 |
| `coord` | 输入 | 搬入起始元素在`tensor`逻辑布局中的坐标。 |

<a id="表3-broadcast_mode参数说明"></a>

**表3** `broadcast_mode`参数说明

| 取值 | 含义 | 搬入起始地址对齐约束（Byte） |
| --- | --- | --- |
| `broadcast_mode::elem` | 从UB读取一个元素，并将该元素广播到目的寄存器的所有元素位置。 | `sizeof(Tensor::data_type)` |
| `broadcast_mode::datablock` | 从UB读取一个DataBlock（32B），并将该DataBlock重复广播到整个目的寄存器。 | 32 |
| `broadcast_mode::elem2datablock` | 依次读取VL/32B个元素，并将每个元素分别广播到目的寄存器的一个DataBlock内。 | 16bit类型为VL/16；32bit类型为VL/8 |

## 数据类型

| `brc_mode` | 支持的数据类型 |
| --- | --- |
| `elem`、`datablock` | `int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`int32_t`、`uint32_t`、`half`、`bfloat16_t`、`float`、`hifloat8_t`、`fp8_e4m3fn_t`、`fp8_e5m2_t`、`fp8_e8m0_t`、`int4x2_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t` |
| `elem2datablock` | `int16_t`、`uint16_t`、`int32_t`、`uint32_t`、`half`、`bfloat16_t`、`float` |

未列出的类型或模式组合会产生编译期错误。

## 返回值说明

返回`reg_tensor<typename Tensor::data_type>`，其`reg`成员保存广播结果。

## 约束说明

- 仅支持位于UB的Tensor。
- 搬入起始地址必须满足[表3](#表3-broadcast_mode参数说明)中的对齐约束。
- 传入`coord`时，坐标维数必须与Tensor布局匹配，且坐标不能越界。
- 接口只初始化返回对象的`reg`成员，不初始化`mask`成员。将结果用于读取`mask`的寄存器计算或`store`模式前，必须调用`with_mask`绑定有效mask。
- 寄存器数据搬入应在`__simd_vf__`函数或`__simd_callee__`函数中完成。

## 调用示例

以下示例从UB读取一个`float`元素，并将其广播到整个寄存器。

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/reg_data_load.h"

template <typename SrcTensor>
__simd_vf__ inline void LoadScalarBroadcast(const SrcTensor& src)
{
    auto value = asc::te::experimental::load_broadcast<asc::te::experimental::broadcast_mode::elem>(
        src, asc::te::make_coord(0));
    value.with_mask(asc::te::experimental::all_mask<float>());
    // 使用value完成后续寄存器计算。
}
```
