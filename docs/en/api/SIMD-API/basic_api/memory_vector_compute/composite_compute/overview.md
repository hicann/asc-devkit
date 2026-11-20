# Overview<a name="ZH-CN_TOPIC_0000002573904727"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-02T07:43:08.222Z pushedAt=2026-11-18T01:25:07.087Z nl=2 -->

Composite computation generally refers to the processing of composite functions or operations in mathematics or programming.

**Table 1** Composite instruction function description

| API | Mathematical Expression |
| :--- | :--- |
| [Axpy](Axpy.md) | $dst_j=scalar \times src_i + dst_i$ |
| [MulAddDst](MulAddDst.md) | $dst_j=src0_i \times src1_i + dst_i$ |
| [MulCast](MulCast.md) | $dst_i=Cast(src0_i \times src1_i)$ |
| [MulsCast(ISASI)](MulsCast_ISASI.md) | $dst_i=Cast(src_i \times scalar)$ |
| [AddRelu](AddRelu.md) | $dst_i=Relu(src0_i + src1_i)$ |
| [AddReluCast](AddReluCast.md) | $dst_i=Cast(Relu(src0_i + src1_i))$ |
| [SubRelu](SubRelu.md) | $dst_i=Relu(src0_i - src1_i)$ |
| [SubReluCast](SubReluCast.md) | $dst_i=Cast(Relu(src0_i - src1_i))$ |
| [AbsSub(ISASI)](AbsSub_ISASI.md) | $dst_i=\lvert src0_i - src1_i\rvert$ |
| [ExpSub(ISASI)](ExpSub_ISASI.md) | $dst_i=e^{src0_i - src1_i}$ |
| [FusedMulAdd](FusedMulAdd.md) | $dst_j=src0_i \times dst_i + src1_i$ |
| [MulAddRelu](MulAddRelu.md) | $dst_i=Relu(src0_i \times dst_i + src1_i)$ |
| [CastDequant](CastDequant.md) | • Input data type is int16_t, with vector quantization mode disabled<br/>$dst_i=Cast(src_i \times scale + offset)$<br>• Input data type is int16_t, with vector quantization mode enabled<br>$dst_i=Cast(src_i \times scale_j + offset_j),j \in [0,15]$<br>• Input type is int32_t. Quantize the int32_t input and perform precision conversion (using RINT rounding)<br>$dst_i=Cast(src_i \times scale)$ |
| [AddDeqRelu](AddDeqRelu.md) | $dst_i=Relu(Deq(src0_i + src1_i))$<br>$Deq(x)=Cast(x \times scale)$ |

## Recommended Use Scenarios<a name="zh-cn_topic_0000002558185995_section23771839304"></a>

- Scenarios with tight Unified Buffer (UB) resources.

  Using composite instructions reduces UB space usage without modifying the original data of the source operands.

- High-performance scenarios.

  When other factors such as bank conflicts are not considered, using composite instructions reduces the number of computation instructions and therefore improves performance.

- High-precision scenarios.

  When the source operand and destination operand have the same data type, composite instructions do not improve precision;

  When the destination operand has a higher-precision data type than the source operand, composite instructions improve precision (for example, when the source operand data type is half and the destination operand data type is float), such as Axpy and MulAddDst.

## Key Features

### Precision Conversion

The MulCast, AddReluCast, SubReluCast, and MulsCast instructions include type conversion operations. For conversion rules, refer to the following table:

**Table 2**  Precision Conversion Rules

| Data Type of the Source Operand | Data Type of the Destination Operand | Introduction to Type Conversion Modes |
| :----------------- | :------------------- | :--------------- |
| float | half | Rounds the source operand to the nearest value representable in half using the CAST_NONE mode, and stores the result in the destination operand in half format (overflow is saturated by default). |
| half | int8_t | Rounds the source operand to an integer using the CAST_NONE mode, and stores the result in the destination operand in int8_t format (overflow is saturated by default). |
| half | uint8_t | Rounds the source operand to an integer using the CAST_NONE mode, and stores the result in the destination operand in uint8_t format (overflow is saturated by default). |
| int16_t | int8_t | Rounds the source operand to the nearest value representable in int8_t using the CAST_NONE mode, and stores the result in the destination operand in int8_t format (overflow is saturated by default). |
| int64_t | float | Rounds the source operand to the nearest value representable in float using the CAST_NONE mode, and stores the result in the destination operand in float format (overflow is saturated by default). |
| int64_t | int32_t | Stores the source operand in the destination operand in int32_t format (overflow is saturated by default). |

### Quantization Computation

The CastDequant and AddDeqRelu APIs involve the setting of quantization parameters and must be used together with the [SetDeqScale](../type_conversion_aux_config/SetDeqScale.md) API.

### Three-Operand Read

For the MulAddDst, FusedMulAdd, and MulAddRelu APIs, if the three operands do not overlap, a three-reads-and-one-write scenario occurs (where the destination operand is both read and written). Because the hardware supports at most two reads and one write, the theoretical parallelism of the instruction is halved. In this case, you can overlap operands to reduce read operations and restore the original parallelism.
