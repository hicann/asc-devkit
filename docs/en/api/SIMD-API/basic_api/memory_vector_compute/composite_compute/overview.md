# Overview<a name="ZH-CN_TOPIC_0000002573904727"></a>

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T08:30:04.897Z -->

Compound computation generally refers to the processing of compound functions or operations in mathematics or programming.

**Table 1** Compound instruction functions

| API | Mathematical Expression |
| :--- | :--- |
| [Axpy](Axpy.md) | $dst_j=scalar \times src_i + dst_i$ |
| [MulAddDst](MulAddDst.md) | $dst_j=src0_i \times src1_i + dst_i$ |
| [MulCast](MulCast.md) | $dst_i=Cast(src0_i \times src1_i)$ |
| [AddRelu](AddRelu.md) | $dst_i=Relu(src0_i + src1_i)$ |
| [AddReluCast](AddReluCast.md) | $dst_i=Cast(Relu(src0_i + src1_i))$ |
| [SubRelu](SubRelu.md) | $dst_i=Relu(src0_i - src1_i)$ |
| [SubReluCast](SubReluCast.md) | $dst_i=Cast(Relu(src0_i - src1_i))$ |
| [FusedMulAdd](FusedMulAdd.md) | $dst_j=src0_i \times dst_i + src1_i$ |
| [MulAddRelu](MulAddRelu.md) | $dst_i=Relu(src0_i \times dst_i + src1_i)$ |
| [CastDequant](CastDequant.md) | • Input data type is int16_t, with vector quantization mode disabled<br/>$dst_i=Cast(src_i \times scale + offset)$<br>• Input data type is int16_t, with vector quantization mode enabled<br>$dst_i=Cast(src_i \times scale_j + offset_j),j \in [0,15]$<br>• Input type is int32_t, quantization is performed on the int32_t input and precision conversion is applied (using RINT rounding)<br>$dst_i=Cast(src_i \times scale)$ |
| [AddDeqRelu](AddDeqRelu.md) | $dst_i=Relu(Deq(src0_i + src1_i))$<br>$Deq(x)=Cast(x \times scale)$ |

## Recommended Scenarios<a name="zh-cn_topic_0000002558185995_section23771839304"></a>

- Scenarios with limited UB resources.

  Compound instructions can reduce UB space usage while keeping the original data of the source operands unchanged.

- High-performance scenarios.

  When other factors such as bank conflicts are not considered, using compound instructions reduces the number of computation instructions and therefore improves performance.

- High-precision scenarios.

  When the source operands and the destination operand have the same data type, compound instructions do not improve precision;

  When the data type of the destination operand has higher precision than that of the source operands, compound instructions improve precision (for example, when the source operand data type is half and the destination operand data type is float), such as Axpy and MulAddDst.

## Key Feature Description

### Precision Conversion

The MulCast, AddReluCast, and SubReluCast instructions include type conversion operations. For the conversion rules, see the following table:

**Table 2** Precision conversion rules

| Data Type of the Source Operand | Data Type of the Destination Operand | Type Conversion Mode Introduction |
| :----------------- | :------------------- | :--------------- |
| float | half | The source operand is converted to the nearest value representable by half in CAST_NONE mode, and the result is stored to the destination operand in half format (overflow is saturating by default). |
| half | int8_t | The source operand is rounded in CAST_NONE mode, and the result is stored to the destination operand in int8_t format (overflow is saturating by default). |
| half | uint8_t | The source operand is rounded in CAST_NONE mode, and the result is stored to the destination operand in uint8_t format (overflow is saturating by default). |
| int16_t | int8_t | The source operand is converted to the nearest value representable by int8_t in CAST_NONE mode, and the result is stored to the destination operand in int8_t format (overflow is saturating by default). |

### Quantization Computation

The CastDequant and AddDeqRelu APIs involve the setting of quantization parameters and need to be used in conjunction with the [SetDeqScale](../type_conversion_aux_config/SetDeqScale.md) API.

### Reading Three Operands

For the MulAddDst, FusedMulAdd, and MulAddRelu APIs, if the three operands do not overlap, a three-read-one-write scenario occurs (in which the destination operand is subject to both a read operation and a write operation). Because the hardware supports at most two reads and one write, the theoretical parallelism of the instruction is halved from its original level. In this case, the original parallelism can be restored by overlapping operands to reduce read operations.
