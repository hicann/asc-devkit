# cast

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

`cast`用于寄存器数据类型转换，将源操作数`reg_tensor<SrcType>`中的有效元素转换为`DstType`，支持浮点转整数、浮点转浮点、整数转浮点和整数转整数。转换过程中可能发生位宽或精度变化，可通过编译期`cast_options`配置以下行为：

- `cast_layout`：源类型和目的类型位宽不同时，单条指令的有效计算量以位宽较大的类型为准。该配置指定位宽较小的元素在同组元素中的位置。
- `cast_round_mode`：指定发生精度损失时的舍入方式。
- `cast_sat_mode`：指定转换结果超出目的类型表示范围时采用饱和还是非饱和处理。

接口使用`src.mask`筛选源操作数中的有效元素，底层转换采用zeroing模式，mask未选中的目的元素置0。返回值保存转换结果，并原样传递`src.mask`，便于后续计算或`local_tensor::store`写回。

## 函数原型

```cpp
template <typename DstType,
          const cast_options& opt = default_cast_ops,
          typename SrcType>
__simd_callee__ inline reg_tensor<DstType> cast(reg_tensor<SrcType> src)
```

默认配置为：

```cpp
constexpr cast_options default_cast_ops = {
    cast_layout::zero,
    cast_round_mode::rint,
    cast_sat_mode::not_sat
};
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| `DstType` | 目的元素类型，必须与`SrcType`不同。支持的数据类型组合请参考[数据类型](#数据类型)。 |
| `opt` | `cast_options`类型的编译期常量，默认值为`default_cast_ops`。不同转换支持的配置组合请参考[数据类型](#数据类型)。 |
| `SrcType` | 源元素类型，由函数实参自动推导，无需显式指定。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| `src` | 输入 | 源寄存器。`src.reg`保存待转换数据，`src.mask`指示参与转换的有效元素。调用前必须完成数据和mask初始化。 |

`cast_options`配置项说明如下：

| 配置项 | 取值和说明 |
| --- | --- |
| `layout` | `cast_layout::zero`、`cast_layout::one`、`cast_layout::two`、`cast_layout::three`，分别表示位宽较小的元素位于同组元素中的第0、1、2、3个位置。可用位置由源、目的类型的位宽比决定；同位宽转换仅支持`zero`。 |
| `round` | `cast_round_mode::rint`：舍入到最近值，正好位于中间值时舍入到偶数；`cast_round_mode::round`：舍入到最近值，正好位于中间值时远离0；`cast_round_mode::floor`：向负无穷方向舍入；`cast_round_mode::ceil`：向正无穷方向舍入；`cast_round_mode::trunc`：向0方向舍入。仅在转换可能损失精度且对应组合支持该模式时生效。 |
| `sat` | `cast_sat_mode::not_sat`：非饱和模式，超出范围时按目标格式截断或保留该浮点格式支持的特殊值；`cast_sat_mode::sat`：饱和模式，超出范围时钳位到目的类型边界。具体行为请参考[饱和与非饱和模式](#饱和与非饱和模式)。 |

> [!NOTE]  
> `default_cast_ops`仅表示接口的默认参数，不表示所有类型组合均支持该配置。若默认配置不在对应转换行的支持范围内，必须显式传入`cast_options`。

## 数据类型

下表为当前支持的完整配置范围。每行`layout`、`round`和`sat`列中的取值可以任意组合，未列出的类型或配置组合会产生编译期错误。

| 源类型 → 目的类型 | `layout` | `round` | `sat` |
| --- | --- | --- | --- |
| `bfloat16_t → float`、`half → float`、`int16_t → float`、`int8_t → half`、`uint8_t → half`、`int16_t → int32_t`、`int32_t → int64_t`、`int8_t → int16_t`、`int16_t → uint32_t`、`uint16_t → uint32_t`、`uint8_t → uint16_t`、`hifloat8_t → half` | `zero`、`one` | `rint` | `not_sat` |
| `int8_t → int32_t`、`uint8_t → uint32_t`、`hifloat8_t → float`、`int4x2_t → half`、`int4x2_t → int16_t`、`int4x2_t → bfloat16_t`、`fp4x2_e1m2_t → bfloat16_t`、`fp4x2_e2m1_t → bfloat16_t`、`fp8_e4m3fn_t → float`、`fp8_e5m2_t → float` | `zero`、`one`、`two`、`three` | `rint` | `not_sat` |
| `int32_t → int16_t`、`int64_t → int32_t`、`int16_t → uint8_t`、`int32_t → uint16_t`、`uint32_t → int16_t`、`uint16_t → uint8_t`、`uint32_t → uint16_t` | `zero`、`one` | `rint` | `not_sat`、`sat` |
| `float → bfloat16_t`、`bfloat16_t → int32_t`、`float → half`、`float → int16_t`、`float → int64_t`、`half → int8_t`、`half → uint8_t` | `zero`、`one` | `rint`、`round`、`floor`、`ceil`、`trunc` | `not_sat`、`sat` |
| `int32_t → uint8_t`、`uint32_t → uint8_t`、`float → fp8_e4m3fn_t`、`float → fp8_e5m2_t` | `zero`、`one`、`two`、`three` | `rint` | `not_sat`、`sat` |
| `half → bfloat16_t`、`int32_t → float`、`int16_t → half` | `zero` | `rint`、`round`、`floor`、`ceil`、`trunc` | `not_sat` |
| `bfloat16_t → half`、`float → int32_t`、`half → int16_t` | `zero` | `rint`、`round`、`floor`、`ceil`、`trunc` | `not_sat`、`sat` |
| `bfloat16_t → fp4x2_e1m2_t`、`bfloat16_t → fp4x2_e2m1_t` | `zero`、`one`、`two`、`three` | `rint`、`round`、`floor`、`ceil`、`trunc` | `not_sat` |
| `int64_t → float`、`half → int32_t` | `zero`、`one` | `rint`、`round`、`floor`、`ceil`、`trunc` | `not_sat` |
| `float → hifloat8_t` | `zero`、`one`、`two`、`three` | `round` | `not_sat`、`sat` |
| `half → int4x2_t` | `zero`、`one`、`two`、`three` | `rint`、`round`、`floor`、`ceil`、`trunc` | `not_sat`、`sat` |
| `half → hifloat8_t` | `zero`、`one` | `round` | `not_sat`、`sat` |

## 关键特性说明

### 饱和与非饱和模式<a name="饱和与非饱和模式"></a>

不同转换场景下，`cast_sat_mode::not_sat`和`cast_sat_mode::sat`的含义如下。

| 转换场景 | `not_sat` | `sat` |
| --- | --- | --- |
| 浮点转整数 | 输入超出目的类型范围时，结果截断为目的格式的数据宽度，即保留低位。例如，`half`值257转换为`uint8_t`时结果为1。输入为正负无穷时，返回目的类型的对应边界值；输入为NaN时返回0。 | 输入超出目的类型范围时，结果钳位到目的类型边界。例如，`half`值257转换为`uint8_t`时结果为255，负无穷转换为`uint8_t`时结果为0；输入为NaN时返回0。 |
| 浮点转浮点 | 目的类型能够表示NaN或正负无穷时，保留对应特殊值；有限输入超出目的类型范围时，按目的浮点格式的非饱和规则转换。 | 输入超出目的类型范围时，结果钳位到目的类型边界；NaN默认转换为0。 |
| 整数转浮点 | 该场景不提供`sat`配置。转换精度由目的浮点格式以及对应行支持的`round`配置决定。 | 不支持。 |
| 整数转整数 | 窄化转换超出目的类型范围时，结果截断为目的格式的数据宽度。例如，`int32_t`值256转换为`uint8_t`时结果为0。一般的扩宽转换保持数值。特别地，`int16_t`转`uint32_t`的`opt.sat`必须配置为表中固定的`not_sat`，但转换本身采用饱和语义，负数结果为0。 | 输入超出目的类型范围时，结果钳位到目的类型边界。 |

### 舍入模式<a name="舍入模式"></a>

舍入模式只在转换可能损失精度时影响结果。例如，浮点转整数、宽浮点转窄浮点以及`int32_t`或`int64_t`转`float`可能需要舍入；精确扩宽转换虽然在类型表中以`rint`表示固定配置，但不会额外改变精确结果。必须使用[数据类型](#数据类型)表中对应转换支持的模式，不支持的模式会在编译期报错。

`float`或`half`转换为`hifloat8_t`仅支持`cast_round_mode::round`，不支持其他舍入配置。

### cast_layout与位宽变化<a name="cast_layout与位宽变化"></a>

当源、目的类型位宽不同时，一条转换指令处理的逻辑元素数由位宽较大的类型决定。`layout`描述位宽较小的元素在每组数据中的位置：

- 位宽比为2时，可选择`zero`或`one`，对应每组中的第0或第1个位置。
- 位宽比为4时，可选择`zero`、`one`、`two`或`three`，对应每组中的第0至第3个位置。
- 源、目的类型位宽相同时，使用`zero`。

`cast`只完成寄存器计算，不会隐式调整UB中的数据排布。连续处理Tensor数据时，应使`local_tensor::load`、`cast_options::layout`和`local_tensor::store`的排布保持一致。常见用法如下：

| 转换方向 | 搬入 | 搬出 |
| --- | --- | --- |
| 源类型比目的类型窄2倍 | `load_sideband_mode::unpack` | `store_sideband_mode::direct` |
| 源类型比目的类型窄4倍 | `load_sideband_mode::unpack4` | `store_sideband_mode::direct` |
| 源类型比目的类型宽2倍 | `load_sideband_mode::direct` | `store_sideband_mode::pack` |
| 源类型比目的类型宽4倍 | `load_sideband_mode::direct` | `store_sideband_mode::pack_quarter` |
| 源、目的类型位宽相同 | `load_sideband_mode::direct` | `store_sideband_mode::direct` |

对于尾块，建议按照源、目的类型中位宽较大的类型计算单寄存器逻辑元素数，并用相同类型调用`update_mask`。这样可使搬入/搬出sideband模式、`layout`和mask对应同一组逻辑元素。

`int4x2_t`、`fp4x2_e2m1_t`和`fp4x2_e1m2_t`每个存储单元包含两个4 bit元素。它们与16 bit类型转换时，指令按两个元素一组读写；从16 bit类型转换到4 bit打包类型时，每组mask以偶数位为有效控制位。

## 返回值说明

返回`reg_tensor<DstType>`。返回值的`reg`保存转换结果，`mask`与`src.mask`相同。

## 约束说明

- `DstType`和`SrcType`必须不同；相同类型转换会产生编译期错误。
- 调用前必须初始化`src.reg`和`src.mask`。`cast`根据源操作数类型和数据排布解释mask，不会检查mask是否与源数据匹配。
- 返回值仅原样传递`src.mask`，不会根据`DstType`重新生成mask。发生位宽变化时，调用方必须保证mask、`layout`以及搬入/搬出sideband模式相互匹配。
- 接口只支持[数据类型](#数据类型)表中列出的完整配置组合。其他类型或选项组合会产生编译期错误。
- 寄存器计算应在`__simd_vf__`函数或`__simd_callee__`函数中完成。

## 调用示例

### 位宽由小变大

以下示例将`half`转换为`int32_t`。源类型位宽为目的类型的一半，因此使用`unpack`模式搬入，并以位宽较大的`int32_t`生成mask。

```cpp
constexpr asc::te::experimental::cast_options f16ToS32Options = {
    asc::te::experimental::cast_layout::zero,
    asc::te::experimental::cast_round_mode::floor,
    asc::te::experimental::cast_sat_mode::not_sat
};

template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void cast_f16_to_s32(const SrcTensor src, DstTensor dst)
{
    auto srcReg = asc::te::experimental::load<asc::te::experimental::load_sideband_mode::unpack>(
        src, asc::te::make_coord(0));
    srcReg.with_mask(asc::te::experimental::all_mask<int32_t>());

    auto dstReg = asc::te::experimental::cast<int32_t, f16ToS32Options>(srcReg);
    asc::te::experimental::store(dst, asc::te::make_coord(0), dstReg);
}
```

### 位宽由大变小

以下示例将`float`转换为`int16_t`，使用向最近偶数舍入和饱和模式。目的类型位宽为源类型的一半，因此使用`pack`模式搬出。

```cpp
constexpr asc::te::experimental::cast_options f32ToS16Options = {
    asc::te::experimental::cast_layout::zero,
    asc::te::experimental::cast_round_mode::rint,
    asc::te::experimental::cast_sat_mode::sat
};

template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void CastF32ToS16(const SrcTensor src, DstTensor dst)
{
    auto srcReg = asc::te::experimental::load(src, asc::te::make_coord(0));
    srcReg.with_mask(asc::te::experimental::all_mask<float>());

    auto dstReg = asc::te::experimental::cast<int16_t, f32ToS16Options>(srcReg);
    asc::te::experimental::store<asc::te::experimental::store_sideband_mode::pack>(
        dst, asc::te::make_coord(0), dstReg);
}
```
