# load_broadcast

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

`load_broadcast`用于从Unified Buffer（UB）Tensor中搬入数据，并按照`broadcast_mode`指定的方式广播到一个[reg_tensor](../reg_tensor/reg_tensor.md)。

传入`coord`时，从`tensor.engine() + tensor.layout()(coord)`对应的元素开始搬入；不传入`coord`时，从`tensor`起始位置开始搬入。

## 函数原型

- 指定坐标：

    ```cpp
    template <broadcast_mode brc_mode = broadcast_mode::elem,
              typename Tensor,
              typename Coord>
    __simd_callee__ inline auto load_broadcast(
        const Tensor& tensor, const Coord& coord)
    ```

- 从Tensor起始位置搬入：

    ```cpp
    template <broadcast_mode brc_mode = broadcast_mode::elem,
              typename Tensor>
    __simd_callee__ inline auto load_broadcast(const Tensor& tensor)
    ```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| brc_mode | 广播模式，类型为`broadcast_mode`，默认值为`broadcast_mode::elem`。具体取值请参考[表3](#表3-broadcast_mode参数说明)。 |
| Tensor | 源Tensor类型，由函数实参自动推导。Tensor必须位于UB。 |
| Coord | 源Tensor的坐标类型，由函数实参自动推导。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| tensor | 输入 | 源Tensor，必须位于UB。 |
| coord | 输入 | 搬入起始元素在`tensor`逻辑布局中的坐标。 |

<a id="表3-broadcast_mode参数说明"></a>

**表3** `broadcast_mode`参数说明

表中的VL表示一个矢量数据寄存器的长度，单位为Byte。`int4b_t`类型的数据在寄存器中以`int4x2_t`形式存储。`int4x2_t`数据类型由两个四位整型数打包为一个8比特存储单元，`fp4x2_e2m1_t`和`fp4x2_e1m2_t`数据类型由两个独立的四位浮点数打包为一个8比特存储单元，因此，对于这三种类型，下表所述的“元素”是指一个8比特对应的搬运单元，而不是单个4比特值。

| 取值 | 含义 | 搬入起始地址对齐约束（Byte） |
| --- | --- | --- |
| broadcast_mode::elem | 从UB读取一个元素，并将该元素广播到目的寄存器的所有元素位置。 | 元素的大小 |
| broadcast_mode::datablock | 从UB读取一个DataBlock（32B），并将该DataBlock重复广播到整个目的寄存器。 | 32 |
| broadcast_mode::elem2datablock | 依次读取VL/32B个元素，并将每个元素分别广播到目的寄存器的一个DataBlock内。 | 16bit类型为VL/16；32bit类型为VL/8 |

## 数据类型

| brc_mode | 支持的数据类型 |
| --- | --- |
| elem、datablock | `int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float` |
| elem2datablock | `int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float` |

## 返回值说明

接口按值返回与源Tensor元素类型对应的`reg_tensor`；当源Tensor元素类型为`int4b_t`时，返回`reg_tensor<int4x2_t>`。其`reg`成员保存广播结果，`mask`成员未初始化。

## 约束说明

- 仅支持位于UB的Tensor。
- 搬入起始地址必须满足[表3](#表3-broadcast_mode参数说明)中的对齐约束。
- 传入`coord`时，坐标维数必须与Tensor布局匹配，且坐标不能越界。
- 将广播结果用于读取`mask`的寄存器计算，或用于[store](../reg_data_store/store.md)的`direct`、`pack`、`pack_quarter`模式前，必须调用`with_mask`绑定有效掩码。`store_1st`和`intlv`模式不读取源寄存器的`mask`。
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
