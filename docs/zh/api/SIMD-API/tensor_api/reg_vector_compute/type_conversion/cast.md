# cast

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

头文件路径：`"tensor_api/experimental/arch/vector/type_conversion.h"`。

命名空间：`asc::te::experimental`。

`cast`用于寄存器数据类型转换，将源操作数`reg_tensor<SrcType>`中的有效元素转换为`DstType`，支持浮点转整数、浮点转浮点、整数转浮点和整数转整数。转换过程中可能发生位宽或精度变化，可通过编译期`cast_options`配置以下行为：

- [cast_layout](../aux_data_types/cast_layout.md)：源类型和目的类型位宽不同时，单条指令的有效计算量以位宽较大的类型为准。该配置指定位宽较小的元素在同组元素中的位置。
- [cast_round_mode](../aux_data_types/cast_round_mode.md)：指定发生精度损失时的舍入方式。
- [cast_sat_mode](../aux_data_types/cast_sat_mode.md)：指定转换结果超出目的类型表示范围时采用饱和还是非饱和处理。

接口使用`src.mask`筛选源操作数中的有效元素，底层转换采用zeroing模式，mask未选中的目的元素置0。返回值保存转换结果，并原样传递`src.mask`。发生位宽变化时，搬出前需根据目的数据排布处理结果和mask，具体请参考[cast_layout与位宽变化](#cast_layout与位宽变化)。

## 函数原型

```cpp
template <typename DstType,
          const cast_options& opt,
          typename SrcType>
__simd_callee__ inline reg_tensor<DstType> cast(const reg_tensor<SrcType>& src)
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| `DstType` | 目的元素类型，必须与`SrcType`不同。支持的数据类型组合请参考[数据类型](#数据类型)。 |
| `opt` | 需显式指定的`cast_options`类型编译期常量。不同转换支持的配置组合请参考[数据类型](#数据类型)。 |
| `SrcType` | 源元素类型，由函数实参自动推导，无需显式指定。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| `src` | 输入 | 源寄存器。`src.reg`保存待转换数据，`src.mask`指示参与转换的有效元素。调用前必须完成数据和mask初始化。 |

`cast_options`配置项说明如下：

| 配置项 | 取值和说明 |
| --- | --- |
| `layout` | [cast_layout](../aux_data_types/cast_layout.md)类型，`zero`、`one`、`two`、`three`分别表示位宽较小的元素位于同组元素中的第0、1、2、3个位置。可用位置由位宽比决定；不需要选择位置的同位宽转换使用`unknown`。 |
| `round` | [cast_round_mode](../aux_data_types/cast_round_mode.md)类型，`rint`表示舍入到最近偶数；`round`表示舍入到最近且中间值远离0；`floor`、`ceil`、`trunc`分别表示向负无穷、正无穷、0方向舍入；部分转换还支持`odd`或`hybrid`。无需选择舍入模式时使用`unknown`。 |
| `sat` | [cast_sat_mode](../aux_data_types/cast_sat_mode.md)类型，`not_sat`表示非饱和模式，`sat`表示饱和模式；无需选择饱和模式时使用`unknown`。具体行为请参考[饱和与非饱和模式](#饱和与非饱和模式)。 |

> [!NOTE]
> `cast_options{}`的三个字段默认为`unknown`，当前支持的转换均不接受三项同时为`unknown`，请根据类型组合设置对应字段。

## 数据类型

下表为当前支持的完整配置范围。每行`layout`、`round`和`sat`列中的取值可以任意组合，未列出的类型或配置组合会产生编译期错误。

| 源类型 → 目的类型 | `layout` | `round` | `sat` |
| --- | --- | --- | --- |
| `bfloat16_t → float`、`half → float`、`int16_t → float`、`int8_t → half`、`uint8_t → half`、`int16_t → int32_t`、`int32_t → int64_t`、`int8_t → int16_t`、`int16_t → uint32_t`、`uint16_t → uint32_t`、`uint8_t → uint16_t`、`hifloat8_t → half` | `zero`、`one` | `unknown` | `unknown` |
| `float → bfloat16_t`、`bfloat16_t → int32_t`、`float → int16_t`、`float → int64_t`、`half → int8_t`、`half → uint8_t` | `zero`、`one` | `floor`、`rint`、`round`、`ceil`、`trunc` | `not_sat`、`sat` |
| `bfloat16_t → half`、`float → int32_t`、`half → int16_t` | `unknown` | `floor`、`rint`、`round`、`ceil`、`trunc` | `not_sat`、`sat` |
| `half → bfloat16_t`、`int32_t → float`、`int16_t → half` | `unknown` | `floor`、`rint`、`round`、`ceil`、`trunc` | `unknown` |
| `float → half` | `zero`、`one` | `floor`、`rint`、`round`、`ceil`、`trunc`、`odd` | `not_sat`、`sat` |
| `int64_t → float`、`half → int32_t` | `zero`、`one` | `floor`、`rint`、`round`、`ceil`、`trunc` | `unknown` |
| `int32_t → int16_t`、`int64_t → int32_t`、`int16_t → uint8_t`、`int32_t → uint16_t`、`uint32_t → int16_t`、`uint16_t → uint8_t`、`uint32_t → uint16_t` | `zero`、`one` | `unknown` | `not_sat`、`sat` |
| `int8_t → int32_t`、`uint8_t → uint32_t`、`hifloat8_t → float`、`int4x2_t → half`、`int4x2_t → int16_t`、`fp4x2_e1m2_t → bfloat16_t`、`fp4x2_e2m1_t → bfloat16_t`、`int4x2_t → bfloat16_t`、`fp8_e4m3fn_t → float`、`fp8_e5m2_t → float` | `zero`、`one`、`two`、`three` | `unknown` | `unknown` |
| `int32_t → uint8_t`、`uint32_t → uint8_t` | `zero`、`one`、`two`、`three` | `unknown` | `not_sat`、`sat` |
| `float → hifloat8_t` | `zero`、`one`、`two`、`three` | `round`、`hybrid` | `not_sat`、`sat` |
| `half → hifloat8_t` | `zero`、`one` | `round`、`hybrid` | `not_sat`、`sat` |
| `half → int4x2_t` | `zero`、`one`、`two`、`three` | `floor`、`rint`、`round`、`ceil`、`trunc` | `not_sat`、`sat` |
| `float → fp8_e4m3fn_t`、`float → fp8_e5m2_t` | `zero`、`one`、`two`、`three` | `rint` | `not_sat`、`sat` |
| `bfloat16_t → fp4x2_e1m2_t`、`bfloat16_t → fp4x2_e2m1_t` | `zero`、`one`、`two`、`three` | `floor`、`rint`、`round`、`ceil`、`trunc` | `unknown` |

`cast`支持的类型为寄存器元素类型，对于`int4x2_t`相关的转换，可以通过从UB中的元素类型为`int4b_t`的Tensor搬入后，使用返回的`reg_tensor<int4x2_t>`参与转换；`cast<int4x2_t>`的结果可通过`store`写入`int4b_t` Tensor。

## 返回值说明

返回`reg_tensor<DstType>`。返回值的`reg`保存转换结果，`mask`与`src.mask`相同。

## 约束说明

- `DstType`和`SrcType`必须不同；相同类型转换会产生编译期错误。
- 调用前必须初始化`src.reg`和`src.mask`。`cast`根据源操作数类型和数据排布解释mask，不会检查mask是否与源数据匹配。
- 返回值仅原样传递`src.mask`，不会根据`DstType`重新生成mask。发生位宽变化时，调用方必须保证mask、`layout`以及搬入/搬出sideband模式相互匹配。
- 接口只支持[数据类型](#数据类型)表中列出的完整配置组合。其他类型或选项组合会产生编译期错误。
- 寄存器计算应在`__simd_vf__`函数或`__simd_callee__`函数中完成。

## 关键特性说明


### 饱和与非饱和模式

不同转换场景下，`cast_sat_mode::not_sat`和`cast_sat_mode::sat`的含义如下。

| 转换场景 | `not_sat` | `sat` |
| --- | --- | --- |
| 浮点转整数 | 输入超出目的类型范围时，结果截断为目的格式的数据宽度，即保留低位。例如，`half`值257转换为`uint8_t`时结果为1。输入为正负无穷时，返回目的类型的对应边界值；输入为NaN时返回0。 | 输入超出目的类型范围时，结果钳位到目的类型边界。例如，`half`值257转换为`uint8_t`时结果为255，负无穷转换为`uint8_t`时结果为0；输入为NaN时返回0。 |
| 浮点转浮点 | 目的类型能够表示NaN或正负无穷时，保留对应特殊值；有限输入超出目的类型范围时，按目的浮点格式的非饱和规则转换。 | 输入超出目的类型范围时，结果钳位到目的类型边界；NaN默认转换为0。 |
| 整数转浮点 | 该场景不提供`sat`配置。转换精度由目的浮点格式以及对应行支持的`round`配置决定。 | 不支持。 |
| 整数转整数 | 窄化转换超出目的类型范围时，结果截断为目的格式的数据宽度。例如，`int32_t`值256转换为`uint8_t`时结果为0。一般的扩宽转换保持数值。特别地，`int16_t`转`uint32_t`的`opt.sat`必须配置为表中固定的`unknown`，但转换本身采用饱和语义，负数结果为0。 | 输入超出目的类型范围时，结果钳位到目的类型边界。 |

### 舍入模式

舍入模式只在转换可能损失精度时影响结果。例如，浮点转整数、宽浮点转窄浮点以及`int32_t`或`int64_t`转`float`可能需要舍入；精确扩宽转换不会额外改变精确结果。必须使用[数据类型](#数据类型)表中对应转换支持的模式，不支持的模式会在编译期报错。

`float`或`half`转换为`hifloat8_t`支持`cast_round_mode::round`和`cast_round_mode::hybrid`。

### cast_layout与位宽变化

当源、目的类型位宽不同时，一条转换指令处理的逻辑元素数由位宽较大的类型决定。`layout`描述位宽较小的元素在每组数据中的位置：

- 位宽比为2时，可选择`zero`或`one`，对应每组中的第0或第1个位置。
- 位宽比为4时，可选择`zero`、`one`、`two`或`three`，对应每组中的第0至第3个位置。
- 源、目的类型位宽相同时，使用`unknown`。具体取值以[数据类型](#数据类型)表为准。

`cast`只完成寄存器计算，不会隐式调整Unified Buffer（UB）中的数据排布。连续处理Tensor数据时，应使`asc::te::experimental::load`、`cast_options::layout`和搬出时的数据排布保持一致。源类型比目的类型窄2倍或4倍时，可分别使用`load_sideband_mode::unpack`或`load_sideband_mode::unpack4`搬入，再按目的类型直接搬出。源类型比目的类型宽时，转换后的有效结果分散在目的寄存器中；若需得到连续数组，应先提取有效结果，再重新生成目的类型mask并直接搬出。

对于尾块，建议按照源、目的类型中位宽较大的类型计算单寄存器逻辑元素数，并用相同类型调用`update_mask`生成转换mask。缩窄转换在提取有效结果后，还需按照目的类型和本轮有效元素数重新生成搬出mask，不能直接沿用转换mask。

`store`只按mask将寄存器数据写到对应的UB位置，不会把分散的有效结果自动收拢。如果为目的类型设置连续全有效或前缀有效的mask后直接搬出，会把组内的0也写出；如果仅选中有效结果的位置，则输出中仍然留有间隔，而不是连续数组。原样沿用转换mask也不保证能选中目的组内的结果位置。

使用[deinterleave](../data_reorder/deinterleave.md)的目的是去掉这些组内间隔，将有效结果集中到寄存器前部，再按目的类型直接搬出。例如，四倍缩窄使用`layout::two`时，结果提取过程如下，`a`、`b`、`c`表示转换后的值：

```text
转换后：                 [0, 0, a, 0, 0, 0, b, 0, 0, 0, c, 0, ...]
第一次解交织取first：    [0, a, 0, b, 0, c, ...]
第二次解交织取second：   [a, b, c, ...]
```

`deinterleave`将两个输入寄存器的偶数索引元素放入`first`，奇数索引元素放入`second`。这里将同一个寄存器作为两个输入，是为了只利用偶数/奇数索引筛选功能：第一次筛选出目标位置所在的奇偶集合；四倍缩窄时，再对筛选结果执行一次相同操作，选择该集合中目标位置的第二级奇偶索引。下表中每次解交织的两个输入相同，第二次以第一次选出的结果作为两个输入：

| 缩窄位宽比 | `layout` | 第一次解交织选择 | 第二次解交织选择 |
| --- | --- | --- | --- |
| 2:1 | `zero` | `first` | 不需要 |
| 2:1 | `one` | `second` | 不需要 |
| 4:1 | `zero` | `first` | `first` |
| 4:1 | `one` | `second` | `first` |
| 4:1 | `two` | `first` | `second` |
| 4:1 | `three` | `second` | `second` |

该表适用于普通类型及完整块或连续前缀有效的尾块。同一个寄存器作为两个输入时，筛选后的数据会重复出现在输出中，本轮转换结果仅占最终寄存器的前二分之一或前四分之一。当前`deinterleave`为输出设置全mask，因此搬出前必须使用本轮有效元素数另行生成目的类型mask，避免写出重复数据。应使用独立的搬出计数，不要再次更新已经由转换mask消耗过的`remain`。对于不连续的有效元素，还需要匹配的数据与mask重排。

### 4 bit打包类型

int4b_t在寄存器上以两个逻辑元素打包为一个字节，内存Tensor使用`int4b_t`，寄存器使用`int4x2_t`。`int4x2_t`、`fp4x2_e2m1_t`和`fp4x2_e1m2_t`每个存储单元包含两个4 bit元素。它们与16 bit类型转换时，指令按两个元素一组读写；从16 bit类型转换到4 bit打包类型时，每组mask以偶数位为有效控制位。

## 调用示例

以下示例每次处理一个转换块，`validCount`表示本轮有效元素数，范围为1至64。以256 Byte寄存器为例，一次最多处理64个32 bit逻辑元素。源、目的Tensor分别使用对应示例中的类型，起始地址满足搬运对齐要求。即使`validCount`小于64，源缓冲区仍须容纳完整搬入范围：第一个示例为64个`half`，后两个示例分别为64个`float`和64个`int32_t`。下列输入输出只列出有效元素，缓冲区中的其余源元素可以补0，目的有效范围之外保持原值。

### 位宽由小变大

以下示例将`half`转换为`int32_t`，使用向负无穷方向舍入。源类型位宽为目的类型的一半，因此使用`unpack`模式搬入，并以位宽较大的`int32_t`生成mask。

| `validCount` | 有效输入（`half`） | 预期有效输出（`int32_t`） |
| --- | --- | --- |
| 4 | `[1.5, -2.25, 3.0, 4.75]` | `[1, -3, 3, 4]` |

```cpp
constexpr asc::te::experimental::cast_options f16ToS32Options = {
    asc::te::experimental::cast_layout::zero,
    asc::te::experimental::cast_round_mode::floor,
    asc::te::experimental::cast_sat_mode::unknown
};

template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void cast_f16_to_s32(const SrcTensor src, DstTensor dst, uint32_t validCount)
{
    auto srcReg = asc::te::experimental::load<asc::te::experimental::load_sideband_mode::unpack>(
        src, asc::te::make_coord(0));
    uint32_t remain = validCount;
    srcReg.with_mask(asc::te::experimental::update_mask<int32_t>(remain));

    auto dstReg = asc::te::experimental::cast<int32_t, f16ToS32Options>(srcReg);
    asc::te::experimental::store(dst, asc::te::make_coord(0), dstReg);
}
```

本示例的搬入范围为64个`half`，mask按32 bit粒度选择其中的`validCount`个逻辑元素，转换结果可使用同一mask直接写出。处理完整的64个元素时，也可使用`all_mask<int32_t>()`。

### 位宽由大变小

以下示例将`float`转换为`int16_t`，使用向最近偶数舍入和饱和模式。`layout::zero`将结果写在每组两个`int16_t`槽位的第0个位置，通过一次解交织取出连续结果，重新设置16 bit粒度的mask后直接搬出。

| `validCount` | 有效输入（`float`） | 预期有效输出（`int16_t`） |
| --- | --- | --- |
| 4 | `[1.5, 2.5, -1.5, 40000.0]` | `[2, 2, -2, 32767]` |

```cpp
constexpr asc::te::experimental::cast_options f32ToS16Options = {
    asc::te::experimental::cast_layout::zero,
    asc::te::experimental::cast_round_mode::rint,
    asc::te::experimental::cast_sat_mode::sat
};

template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void cast_f32_to_s16(const SrcTensor src, DstTensor dst, uint32_t validCount)
{
    auto srcReg = asc::te::experimental::load(src, asc::te::make_coord(0));
    uint32_t castRemain = validCount;
    srcReg.with_mask(asc::te::experimental::update_mask<float>(castRemain));

    auto dstReg = asc::te::experimental::cast<int16_t, f32ToS16Options>(srcReg);
    auto compact = asc::te::experimental::deinterleave(dstReg, dstReg).first;
    uint32_t storeRemain = validCount;
    compact.with_mask(asc::te::experimental::update_mask<int16_t>(storeRemain));
    asc::te::experimental::store(dst, asc::te::make_coord(0), compact);
}
```

### 四倍缩窄并选择two位置

以下示例将`int32_t`饱和转换为`uint8_t`，通过`layout::two`将结果写到每组4个字节的第2个位置。两次解交织分别选择`first`、`second`，将结果提取到寄存器前四分之一，重新设置8 bit粒度的mask后直接搬出。若改用`layout::three`，第一次应选择`second`，第二次仍选择`second`。

| `validCount` | 有效输入（`int32_t`） | 预期有效输出（`uint8_t`） |
| --- | --- | --- |
| 4 | `[-1, 0, 255, 256]` | `[0, 0, 255, 255]` |

```cpp
constexpr asc::te::experimental::cast_options s32ToU8TwoOptions = {
    asc::te::experimental::cast_layout::two,
    asc::te::experimental::cast_round_mode::unknown,
    asc::te::experimental::cast_sat_mode::sat
};

template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void cast_s32_to_u8_two(const SrcTensor src, DstTensor dst, uint32_t validCount)
{
    auto srcReg = asc::te::experimental::load(src, asc::te::make_coord(0));
    uint32_t remain = validCount;
    srcReg.with_mask(asc::te::experimental::update_mask<int32_t>(remain));

    auto dstReg = asc::te::experimental::cast<uint8_t, s32ToU8TwoOptions>(srcReg);
    auto stage1 = asc::te::experimental::deinterleave(dstReg, dstReg).first;
    auto compact = asc::te::experimental::deinterleave(stage1, stage1).second;
    uint32_t storeRemain = validCount;
    compact.with_mask(asc::te::experimental::update_mask<uint8_t>(storeRemain));
    asc::te::experimental::store(dst, asc::te::make_coord(0), compact);
}
```
