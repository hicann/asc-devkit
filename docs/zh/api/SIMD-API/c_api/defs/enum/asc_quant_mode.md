# asc_quant_mode

`asc_quant_mode`用于表示L0C Buffer数据搬出时的类型转换或量化模式，作为矩阵搬出接口中`quant_pre_mode`参数的类型。

头文件路径为：`"c_api/defs/enum.h"`。

## 类型别名具体定义

```cpp
using asc_quant_mode = QuantMode_t;
```

`asc_quant_mode`与`QuantMode_t`具有相同的枚举值。`QuantMode_t`由编译器环境提供，枚举项未显式指定底层整数值。

## 枚举值详解

不同产品型号支持的量化模式不同，具体接口对枚举值的支持范围也可能不同，使用时请以对应接口中`quant_pre_mode`参数的说明为准。当前产品支持情况如下：

<!-- npu="910b" id1 -->
- 针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的枚举值：`NoQuant`、`F322F16`、`F322BF16`、`DEQF16`、`VDEQF16`、`QF322B8_PRE`、`VQF322B8_PRE`、`REQ8`、`VREQ8`。部分矩阵搬出接口还支持`REQ4`、`VREQ4`、`QF322S4_PRE`和`VQF322S4_PRE`。
<!-- end id1 -->
<!-- npu="A3" id2 -->
- 针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的枚举值：`NoQuant`、`F322F16`、`F322BF16`、`DEQF16`、`VDEQF16`、`QF322B8_PRE`、`VQF322B8_PRE`、`REQ8`、`VREQ8`。部分矩阵搬出接口还支持`REQ4`、`VREQ4`、`QF322S4_PRE`和`VQF322S4_PRE`。
<!-- end id2 -->
<!-- npu="950" id3 -->
- 针对Ascend 950PR/Ascend 950DT，下表列出的全部枚举值。

    **表1** 枚举值说明

    | 枚举值 | 说明 |
    | --- | --- |
    | NoQuant | 不开启量化功能。 |
    | F322F16 | 将`float`转换为`half`。 |
    | F322BF16 | 将`float`转换为`bfloat16_t`。 |
    | DEQF16 | 将`int32_t`按Scalar量化参数量化为`half`。 |
    | VDEQF16 | 将`int32_t`按Tensor量化参数量化为`half`。 |
    | REQ4 | 将`int32_t`按Scalar量化参数量化为`int4b_t`。 |
    | VREQ4 | 将`int32_t`按Tensor量化参数量化为`int4b_t`。 |
    | QF322B8_PRE | 将`float`按Scalar量化参数量化为`int8_t`或`uint8_t`。 |
    | VQF322B8_PRE | 将`float`按Tensor量化参数量化为`int8_t`或`uint8_t`。 |
    | REQ8 | 将`int32_t`按Scalar量化参数量化为`int8_t`或`uint8_t`。 |
    | VREQ8 | 将`int32_t`按Tensor量化参数量化为`int8_t`或`uint8_t`。 |
    | QF322S4_PRE | 将`float`按Scalar量化参数量化为`int4b_t`。 |
    | VQF322S4_PRE | 将`float`按Tensor量化参数量化为`int4b_t`。 |
    | QF322FP8_PRE | 将`float`按Scalar量化参数量化为`fp8_e4m3fn_t`。 |
    | VQF322FP8_PRE | 将`float`按Tensor量化参数量化为`fp8_e4m3fn_t`。 |
    | QF322HIF8_PRE | 将`float`按Scalar量化参数量化为`hifloat8_t`，采用Half to Away舍入。 |
    | VQF322HIF8_PRE | 将`float`按Tensor量化参数量化为`hifloat8_t`，采用Half to Away舍入。 |
    | QF322HIF8_PRE_HYBRID | 将`float`按Scalar量化参数量化为`hifloat8_t`，采用Hybrid舍入。 |
    | VQF322HIF8_PRE_HYBRID | 将`float`按Tensor量化参数量化为`hifloat8_t`，采用Hybrid舍入。 |
    | QS322BF16_PRE | 将`int32_t`按Scalar量化参数量化为`bfloat16_t`。 |
    | VQS322BF16_PRE | 将`int32_t`按Tensor量化参数量化为`bfloat16_t`。 |
    | QF322F16_PRE | 将`float`按Scalar量化参数量化为`half`。 |
    | VQF322F16_PRE | 将`float`按Tensor量化参数量化为`half`。 |
    | QF322BF16_PRE | 将`float`按Scalar量化参数量化为`bfloat16_t`。 |
    | VQF322BF16_PRE | 将`float`按Tensor量化参数量化为`bfloat16_t`。 |
    | QF322F32_PRE | 将`float`按Scalar量化参数量化为`float`。 |
    | VQF322F32_PRE | 将`float`按Tensor量化参数量化为`float`。 |
<!-- end id3 -->