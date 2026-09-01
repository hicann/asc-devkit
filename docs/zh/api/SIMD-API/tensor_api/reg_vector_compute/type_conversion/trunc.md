# trunc

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

头文件路径：`"tensor_api/arch/vector/experimental/type_conversion.h"`。

命名空间：`asc::te::experimental`。

`trunc`将源寄存器中mask选中的浮点元素向0取整，即截去小数部分并保留原数据类型，计算关系如下：

```text
dst[i] = trunc(src[i])
```

对于有限浮点数，计算规则可表示为：

```text
src[i] >= 0：dst[i] = floor(src[i])
src[i] <  0：dst[i] = ceil(src[i])
```

例如，输入`3.75`时输出`3.0`，输入`-3.75`时输出`-3.0`；输入本身为整数值时，数值保持不变。输出元素类型与输入元素类型相同。

接口使用`src.mask`筛选参与计算的元素，底层固定采用向0舍入和zeroing模式：mask未选中的返回值数据元素置0。返回值的mask与`src.mask`保持一致，可继续用于后续寄存器计算或`local_tensor::store`写回。

## 函数原型

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> trunc(reg_tensor<T> src)
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| `T` | 源、目的元素类型，由函数实参自动推导，无需显式指定。支持的数据类型请参考[数据类型](#数据类型)。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| `src` | 输入 | 源寄存器，类型为`reg_tensor<T>`。`src.reg`保存待处理数据，`src.mask`指示参与计算的有效元素；调用前必须完成数据和mask初始化。 |

## 数据类型

目的元素类型与源元素类型保持一致，支持的数据类型如下。

| `T` | 源操作数类型 | 目的操作数类型 |
| --- | --- | --- |
| `half` | `reg_tensor<half>` | `reg_tensor<half>` |
| `bfloat16_t` | `reg_tensor<bfloat16_t>` | `reg_tensor<bfloat16_t>` |
| `float` | `reg_tensor<float>` | `reg_tensor<float>` |

该接口不执行浮点与整数类型之间的转换。若需要改变元素类型，请使用[cast](cast.md)接口。

## 返回值说明

返回`reg_tensor<T>`：

- `reg`保存向0取整后的结果，mask未选中的元素置0。
- `mask`与`src.mask`相同，接口不会重新生成或扩展有效范围。

## 约束说明

- 调用前必须初始化`src.reg`和`src.mask`。`local_tensor::load`只初始化返回对象的`reg`，需通过`with_mask`绑定与元素类型匹配的mask。
- 舍入模式固定为向0舍入，不支持参考Reg矢量计算`Truncate`接口中的其他舍入模式，也不提供舍入模式模板参数。
- 接口不改变元素类型，也不提供饱和或非饱和模式配置。
- mask合并模式固定为zeroing，不支持merging模式。mask未选中的返回值数据元素置0；后续使用单源`local_tensor::store`时，写回仍由返回值中传递的mask控制。
- 寄存器计算应在`__simd_vf__`函数或`__simd_callee__`函数中完成。

## 调用示例

以下示例对UB中的一段浮点数据执行向0取整。[update_mask](../mask_reg_compute/update_mask.md)`<T>(remain)`根据剩余待处理元素数生成当前循环的mask，并通过引用参数自动更新`remain`，因此完整寄存器块和尾块可以使用同一计算流程。

```cpp
template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void TruncRegLoop(
    const SrcTensor src, DstTensor dst, uint32_t dataSize, uint32_t columnCount)
{
    using T = typename SrcTensor::data_type;
    using RegType = typename asc::te::experimental::reg_tensor<T>::reg_type;
    static_assert(AscendC::Std::is_same_v<T, typename DstTensor::data_type>,
        "trunc requires matching source and destination data types");

    constexpr uint32_t ONE_REPEAT = sizeof(RegType) / sizeof(T);
    const uint32_t repeatTimes = (dataSize + ONE_REPEAT - 1) / ONE_REPEAT;
    uint32_t remain = dataSize;

    for (uint16_t i = 0; i < repeatTimes; ++i) {
        const uint32_t offset = i * ONE_REPEAT;
        const auto coord = asc::te::make_coord(offset / columnCount, offset % columnCount);

        auto srcReg = asc::te::experimental::load(src, coord);
        srcReg.with_mask(asc::te::experimental::update_mask<T>(remain));

        auto dstReg = asc::te::experimental::trunc(srcReg);
        asc::te::experimental::store(dst, coord, dstReg);
    }
}
```

> [!NOTE]  
> `local_tensor::load`每次搬入一个完整寄存器块。使用上述尾块写法时，源UB Tensor在逻辑数据末尾需预留足够的可读空间；`update_mask`只控制参与计算和写回的有效元素，不会缩小实际搬入宽度。
