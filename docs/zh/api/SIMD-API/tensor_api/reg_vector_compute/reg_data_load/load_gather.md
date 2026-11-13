# load_gather

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

`load_gather`根据索引寄存器，从Unified Buffer（UB）Tensor的离散位置收集数据到[reg_tensor](../reg_tensor/reg_tensor.md)。`gather_mode::element`表示按元素收集，`gather_mode::data_block`表示按DataBlock（32B）收集。

传入`coord`时，以`tensor.engine() + tensor.layout()(coord)`为基地址；不传入`coord`时，以`tensor`起始地址为基地址。

## 函数原型

```cpp
template <gather_mode mode = gather_mode::element,
          typename Tensor, typename Coord, typename Index>
__simd_callee__ inline auto load_gather(
    const Tensor& tensor,
    const Coord& coord,
    const reg_tensor<Index> index,
    const reg_tensor<bool>& gather_mask)

template <gather_mode mode = gather_mode::element,
          typename Tensor, typename Index>
__simd_callee__ inline auto load_gather(
    const Tensor& tensor,
    const reg_tensor<Index> index,
    const reg_tensor<bool>& gather_mask)
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| `mode` | 收集模式，类型为`gather_mode`，默认值为`gather_mode::element`。具体取值请参考[表3](#表3-gather_mode参数说明)。 |
| `Tensor` | 源Tensor类型，由函数实参自动推导。Tensor必须位于UB。 |
| `Coord` | 源Tensor的坐标类型，由函数实参自动推导。 |
| `Index` | 索引的数据类型，由函数实参自动推导。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| `tensor` | 输入 | 位于UB的源Tensor, 基地址必须32B对齐。 |
| `coord` | 输入 | 基地址在`tensor`逻辑布局中的坐标。 |
| `index` | 输入 | 相对于基地址的索引。元素模式下单位为元素；DataBlock模式下单位为字节, index仅前8个元素有效。索引可以存在相同的值。 |
| `gather_mask` | 输入 | 收集操作的有效位Mask。元素模式下逐元素生效；DataBlock模式下按b32格式解释，一个DataBlock对应4bit，仅每4bit中的最低位有效，对应的仅bit 0、4、8、12、16、20、24、28有效。 |

<a id="表3-gather_mode参数说明"></a>

**表3** `gather_mode`参数说明

| 取值 | 含义 | 索引单位 |
| --- | --- | --- |
| `gather_mode::element` | 根据`index`逐元素从UB收集数据到目的寄存器。 | 元素 |
| `gather_mode::data_block` | 根据`index`以DataBlock（32B）为单位从UB收集数据到目的寄存器。 | 字节 |

## 数据类型

| `mode` | Tensor元素类型 | `Index`类型 |
| --- | --- | --- |
| `element` | `int8_t` | `uint16_t` |
| `element` | `uint8_t` | `uint16_t` |
| `element` | `int16_t` | `uint16_t` |
| `element` | `int16_t` | `uint32_t` |
| `element` | `uint16_t` | `uint16_t` |
| `element` | `uint16_t` | `uint32_t` |
| `element` | `half` | `uint16_t` |
| `element` | `half` | `uint32_t` |
| `element` | `bfloat16_t` | `uint16_t` |
| `element` | `bfloat16_t` | `uint32_t` |
| `element` | `int32_t` | `uint32_t` |
| `element` | `uint32_t` | `uint32_t` |
| `element` | `float` | `uint32_t` |
| `element` | `hifloat8_t` | `uint16_t` |
| `element` | `fp8_e4m3fn_t` | `uint16_t` |
| `element` | `fp8_e5m2_t` | `uint16_t` |
| `element` | `fp8_e8m0_t` | `uint16_t` |
| `data_block` | `int4b_t`、`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`int32_t`、`uint32_t`、`int64_t`、`uint64_t`、`half`、`bfloat16_t`、`float`、`hifloat8_t`、`fp8_e4m3fn_t`、`fp8_e5m2_t`、`fp8_e8m0_t`、`fp4x2_e1m2_t`、`fp4x2_e2m1_t` | `uint32_t` |

未列出的类型或模式组合会产生编译期错误。

## 返回值说明

接口按值返回`reg_tensor<typename Tensor::data_type>`；当Tensor元素类型为`int4b_t`时，返回`reg_tensor<int4x2_t>`。返回对象的`reg`成员保存收集结果，`mask`成员与输入参数`gather_mask.reg`一致。

## 约束说明

- 传入`coord`时，坐标维数必须与Tensor布局匹配，且坐标不能越界。
- 基地址必须32B对齐。传入`coord`时，基地址为`tensor.engine() + tensor.layout()(coord)`；不传入`coord`时，基地址为`tensor`起始地址。
- 索引对应的数据必须在UB有效地址范围内。
- `gather_mode::data_block`仅使用`index`的前8个元素，且每个索引必须大于等于0且32B对齐。
- 索引可以存在相同的值。
- 在`gather_mode::element`模式下，`gather_mask`未选中的结果寄存器元素置0。
- 在`gather_mode::element`模式下，当`index`的元素位宽大于`tensor`的元素位宽时，数据写入结果寄存器的偶数索引位置，奇数索引位置置零。
- 寄存器数据搬入应在`__simd_vf__`函数或`__simd_callee__`函数中完成。

## 调用示例

```cpp
template <typename SrcTensor, typename IndexTensor, typename DstTensor>
__simd_vf__ inline void GatherData(SrcTensor src, IndexTensor index,
    DstTensor dst, uint32_t repeat_times, uint32_t total_length,
    uint32_t one_repeat_size)
{
    using data_type = typename SrcTensor::data_type;
    uint32_t count = total_length;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        auto mask = asc::te::experimental::update_mask<data_type>(count);
        uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto index_reg = asc::te::experimental::load(index);
        auto src_reg = asc::te::experimental::load_gather<
            asc::te::experimental::gather_mode::element>(src, coord, index_reg, mask);
        asc::te::experimental::store(dst, coord, src_reg);
    }
}
```
