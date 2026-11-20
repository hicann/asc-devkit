# Memory Vector Computation API Boundary Value Summary<a name="ZH-CN_TOPIC_0000002533514388"></a>

<!-- md-trans-meta sourceCommit=80163306fef56d84a11a9d782c71c5d3edf60fba translatedAt=2026-11-17T03:21:46.195Z pushedAt=2026-11-17T09:52:41.757Z nl=2 -->

## Overview<a name="section3674181114910"></a>

This section summarizes the two types of output results of Memory vector computation APIs for boundary value inputs. The following content applies to these models:

<!-- npu="A3" id2 -->
- Atlas A3 products
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products
<!-- end id3 -->

Under boundary value input, Memory vector computation APIs produce two types of output results:

- INF/NAN mode (non-saturation mode)

    For the three scenarios where the computation result is inf, -inf, or nan, the result is output as is. When the output is an integer, a finite value that exceeds the representable range of the target type is truncated to the bit width of the target type (low-order bits are retained).

- Saturation mode

    Set through the [SetSaturationFlag](../SIMD-API/basic_api/special_register_access/SetSaturationFlag_ISASI.md) API. In saturation mode, inf and -inf are saturated to MAX and MIN for output, and nan is saturated to 0 for output. When the output is an integer, a finite value that exceeds the representable range of the target type is saturated to the maximum or minimum value of the target data type.

Note:

- Saturation mode is configured by data type. For the data types affected by each saturation mode (`SaturationMode::FLOAT`, `SaturationMode::INT`, `SaturationMode::CAST`), see [SetSaturationFlag](../SIMD-API/basic_api/special_register_access/SetSaturationFlag_ISASI.md#data-types). In the tables, the "INF/NAN mode" column shows the result when the corresponding saturation mode is not enabled, and the "saturation mode" column shows the result when it is enabled.
- **In most scenarios, the output result in INF/NAN mode is consistent with the IEEE754 standard result. If there is any inconsistency, it is noted at the corresponding API.**

## Scenarios Where Memory Vector Computation APIs Output inf/nan

**Table 1** Scenarios where floating-point computation outputs inf/nan

<a name="table1645210351498"></a>

| Scenario Description | Floating-Point Computation Output | Description |
| --- | --- | --- |
| **Input contains inf**<br>Exceptions:<br>&bull; exp(-inf)=0<br>&bull; inf * 0 = nan<br>&bull; inf + (-inf) = nan | inf | inf distinguishes +inf from -inf |
| **Computation result overflows**<br>Exceptions:<br>&bull; A non-nan value (excluding 0) divided by 0, such as Div/Sqrt/Reciprocal instructions<br>&bull; ln(0.0) = -inf; ln(-0.0) = -inf<br>&bull; rsqrt(0.0) = inf; rsqrt(-0.0) = -inf | inf | inf distinguishes +inf from -inf |
| **Input contains nan** | nan | nan does not distinguish positive from negative |
| **Computation result produces nan**<br>Examples:<br>&bull; (-inf) - (-inf) = nan, such as Add/Sub/Reduce instructions<br>&bull; (-inf) + inf = nan, such as Add/Sub/Reduce instructions<br>&bull; 0 * (+/-inf) = nan, such as Mul instructions<br>&bull; (+/-inf)/(+/-inf) = nan<br>&bull; 0/0 = nan<br>&bull; ln/Sqrt/Rsqrt instructions with negative input | nan | nan does not distinguish positive from negative |

For details, see the following API categories.

## Basic Arithmetic<a name="section12606472427"></a>

Note: In the following table, MAX/MIN correspond to the maximum and minimum values under the data type of the corresponding operand.

**Table 2** Computation results of Exp with special values/boundary values as input

<a name="table864650154918"></a>

| Data Type of src/dst | Element Values of src | Element Values of dst (INF/NAN mode) | Element Values of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | -65504.0 (MIN) | 0.0 | 0.0 |
| half | 65504.0 (MAX) | inf | **65504.0** |
| half | 0.0 | 1.0 | 1.0 |
| half | -0.0 | 1.0 | 1.0 |
| half | nan | nan | **0.0** |
| half | inf | inf | **65504.0** |
| half | -inf | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | inf | inf |
| float | 0.0 | 1.0 | 1.0 |
| float | -0.0 | 1.0 | 1.0 |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | 0.0 | 0.0 |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 3** Computation results of Ln with special values/boundary values as input

<a name="table10972164142410"></a>

| Data Type of src/dst | Element Values of src | Element Values of dst (INF/NAN Mode) | Element Values of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | -65504.0 (MIN) | nan | **0.0** |
| half | 65504.0 (MAX) | 11.09 | 11.09 |
| half | 0.0 | -inf | **-65504.0** |
| half | -0.0 | -inf | **-65504.0** |
| half | nan | nan | **0.0** |
| half | inf | inf | **65504.0** |
| half | -inf | nan | **0.0** |
| float | -3.4028235e+38 (MIN) | nan | nan |
| float | 3.4028235e+38 (MAX) | 88.72 | 88.72284 |
| float | 0.0 | -inf | -inf |
| float | -0.0 | -inf | -inf |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | nan | nan |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 4** Computation results of Abs with special value/boundary value inputs

<a name="table1975185932512"></a>

| Data Type of src/dst | Element Values of src | Element Values of dst (INF/NAN Mode) | Element Values of dst (saturation Mode) |
| --- | --- | --- | --- |
| half | -65504.0 (MIN) | 65504.0 | 65504.0 |
| half | 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | 0.0 | 0.0 | 0.0 |
| half | -0.0 | 0.0 | 0.0 |
| half | nan | nan | **0.0** |
| half | inf | inf | **65504.0** |
| half | -inf | inf | **65504.0** |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 | 3.4028235e+38 |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 | 3.4028235e+38 |
| float | 0.0 | 0.0 | 0.0 |
| float | -0.0 | 0.0 | 0.0 |
| float | nan | nan | 0.0 |
| float | inf | inf | inf |
| float | -inf | inf | inf |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 5** Computation results of Reciprocal with special value/boundary value inputs

<a name="table1046894562620"></a>

| Data Type of src/dst | Element Values of src | Element Values of dst (INF/NAN Mode) | Element Values of dst (saturation Mode) |
| --- | --- | --- | --- |
| half | -65504.0 (MIN) | -0.000015 | -0.000015 |
| half | 65504.0 (MAX) | 0.000015 | 0.000015 |
| half | 0.0 | inf | **65504.0** |
| half | -0.0 | -inf | **-65504.0** |
| half | nan | nan | **0.0** |
| half | inf | 0.0 | 0.0 |
| half | -inf | -0.0 | -0.0 |
| float | -3.4028235e+38 (MIN) | -2.938736e-39 | -2.938736e-39 |
| float | 3.4028235e+38 (MAX) | 2.938736e-39 | 2.938736e-39 |
| float | 0.0 | inf | inf |
| float | -0.0 | -inf | -inf |
| float | nan | nan | nan |
| float | inf | 0.0 | 0.0 |
| float | -inf | -0.0 | -0.0 |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 6** Computation results of Sqrt with special value/boundary value inputs

<a name="table1648943062718"></a>

| Data Type of src/dst | Element Values of src | Element Values of dst (INF/NAN Mode) | Element Values of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | -65504.0 (MIN) | nan | **0.0** |
| half | 65504.0 (MAX) | 256.0 | 256.0 |
| half | 0.0 | 0.0 | 0.0 |
| half | -0.0 | -0.0 | -0.0 |
| half | nan | nan | **0.0** |
| half | inf | inf | **65504.0** |
| half | -inf | nan | **0.0** |
| float | -3.4028235e+38 (MIN) | nan | nan |
| float | 3.4028235e+38 (MAX) | 1.8446744e+19 | 1.8446744e+19 |
| float | 0.0 | 0.0 | 0.0 |
| float | -0.0 | -0.0 | -0.0 |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | nan | nan |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 7** Computation results of Rsqrt with special value/boundary value input

<a name="table12701922112818"></a>

| Data Type of src/dst | Element Values of src | Element Values of dst (INF/NAN Mode) | Element Values of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | -65504.0 (MIN) | nan | **0.0** |
| half | 65504.0 (MAX) | 0.003906 | 0.003906 |
| half | 0.0 | inf | **65504.0** |
| half | -0.0 | -inf | **-65504.0** |
| half | nan | nan | **0.0** |
| half | inf | 0.0 | 0.0 |
| half | -inf | nan | **0.0** |
| float | -3.4028235e+38 (MIN) | nan | nan |
| float | 3.4028235e+38 (MAX) | 5.421011e-20 | 5.421011e-20 |
| float | 0.0 | inf | inf |
| float | -0.0 | -inf | -inf |
| float | nan | nan | nan |
| float | inf | 0.0 | 0.0 |
| float | -inf | nan | nan |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 8** Computation results of Relu with special value/boundary value input

<a name="table135522523284"></a>

| Data Type of src/dst | Element Values of src | Element Values of dst (INF/NAN Mode) | Element Values of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | -65504.0 (MIN) | 0.0 | 0.0 |
| half | 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | 0.0 | 0.0 | 0.0 |
| half | -0.0 | 0.0 | 0.0 |
| half | nan | nan | **0.0** |
| half | inf | inf | **65504.0** |
| half | -inf | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 | 3.4028235e+38 |
| float | 0.0 | 0.0 | 0.0 |
| float | -0.0 | 0.0 | 0.0 |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | 0.0 | 0.0 |
| int32_t | -2147483648 (MIN) | 0 | 0 |
| int32_t | 2147483647 (MAX) | 2147483647 | 2147483647 |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 9** Computation results for Add special value/boundary value inputs

<a name="table071981613"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN)/-inf | -inf | **-65504.0** |
| half | -65504.0 (MIN) | 65504.0 (MAX) | 0.0 | 0.0 |
| half | -65504.0 (MIN) | 0.0/-0.0 | -65504.0 | -65504.0 |
| half | -65504.0 (MIN) | nan | nan | **0.0** |
| half | -65504.0 (MIN) | inf | inf | **65504.0** |
| half | 65504.0 (MAX) | 65504.0 (MAX)/inf | inf | **65504.0** |
| half | 65504.0 (MAX) | 0.0/-0.0 | 65504.0 | 65504.0 |
| half | 65504.0 (MAX) | nan | nan | **0.0** |
| half | 65504.0 (MAX) | -inf | -inf | **-65504.0** |
| half | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| half | 0.0 | nan | nan | **0.0** |
| half | 0.0 | inf | inf | **65504.0** |
| half | 0.0 | -inf | -inf | **-65504.0** |
| half | -0.0 | -0.0 | -0.0 | -0.0 |
| half | -0.0 | nan | nan | **0.0** |
| half | -0.0 | inf | inf | **65504.0** |
| half | -0.0 | -inf | -inf | **-65504.0** |
| half | nan | nan/inf/-inf | nan | **0.0** |
| half | inf | inf | inf | **65504.0** |
| half | inf | -inf | nan | **0.0** |
| half | -inf | -inf | -inf | **-65504.0** |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/-inf | -inf | -inf |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | 0.0/-0.0 | -3.4028235e+38 | -3.4028235e+38 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX)/inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 0.0/-0.0 | 3.4028235e+38 | 3.4028235e+38 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | -inf | -inf | -inf |
| float | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | inf | inf | inf |
| float | 0.0 | -inf | -inf | -inf |
| float | -0.0 | -0.0 | -0.0 | -0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | inf | inf | inf |
| float | -0.0 | -inf | -inf | -inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | inf | inf |
| float | inf | -inf | nan | nan |
| float | -inf | -inf | -inf | -inf |
| int16_t | -32768 (MIN) | -32768 (MIN) | 0 | **-32768** |
| int16_t | -32768 (MIN) | 32767 (MAX) | -1 | -1 |
| int16_t | 32767 (MAX) | 32767 (MAX) | -2 | **32767** |
| int32_t | -2147483648 (MIN) | -2147483648 (MIN) | 0 | **-2147483648** |
| int32_t | -2147483648 (MIN) | 2147483647 (MAX) | -1 | -1 |
| int32_t | 2147483647 (MAX) | 2147483647 (MAX) | -2 | **2147483647** |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 10** Computation results for Sub special value/boundary value inputs

<a name="table12357151012179"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN) | 0.0 | 0.0 |
| half | -65504.0 (MIN) | 65504.0 (MAX) | -inf | **-65504.0** |
| half | -65504.0 (MIN) | 0.0/-0.0 | -65504.0 | -65504.0 |
| half | -65504.0 (MIN) | nan | nan | **0.0** |
| half | -65504.0 (MIN) | inf | -inf | **-65504.0** |
| half | -65504.0 (MIN) | -inf | inf | **65504.0** |
| half | 65504.0 (MAX) | -65504.0 (MIN) | inf | **65504.0** |
| half | 65504.0 (MAX) | 65504.0 (MAX) | 0.0 | 0.0 |
| half | 65504.0 (MAX) | 0.0/-0.0 | 65504.0 | 65504.0 |
| half | 65504.0 (MAX) | nan | nan | **0.0** |
| half | 65504.0 (MAX) | inf | -inf | **-65504.0** |
| half | 65504.0 (MAX) | -inf | inf | **65504.0** |
| half | 0.0/-0.0 | -65504.0 (MIN) | 65504.0 | 65504.0 |
| half | 0.0/-0.0 | 65504.0 (MAX) | -65504.0 | -65504.0 |
| half | 0.0/-0.0 | nan | nan | **0.0** |
| half | 0.0/-0.0 | inf | -inf | **-65504.0** |
| half | 0.0/-0.0 | -inf | inf | **65504.0** |
| half | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| half | -0.0 | 0.0 | -0.0 | -0.0 |
| half | -0.0 | -0.0 | 0.0 | 0.0 |
| half | nan | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/nan/inf/-inf | nan | **0.0** |
| half | inf | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/-inf | inf | **65504.0** |
| half | inf | nan/+inf | nan | **0.0** |
| half | -inf | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/inf | -inf | **-65504.0** |
| half | -inf | nan/-inf | nan | **0.0** |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN) | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | -inf | -inf |
| float | -3.4028235e+38 (MIN) | 0.0/-0.0 | -3.4028235e+38 | -3.4028235e+38 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | -inf | -inf |
| float | -3.4028235e+38 (MIN) | -inf | inf | inf |
| float | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN) | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX) | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | 0.0/-0.0 | 3.4028235e+38 | 3.4028235e+38 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | inf | -inf | -inf |
| float | 3.4028235e+38 (MAX) | -inf | inf | inf |
| float | 0.0/-0.0 | -3.4028235e+38 (MIN) | 3.4028235e+38 | 3.4028235e+38 |
| float | 0.0/-0.0 | 3.4028235e+38 (MAX) | -3.4028235e+38 | -3.4028235e+38 |
| float | 0.0/-0.0 | nan | nan | nan |
| float | 0.0/-0.0 | inf | -inf | -inf |
| float | 0.0/-0.0 | -inf | inf | inf |
| float | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| float | -0.0 | 0.0 | -0.0 | -0.0 |
| float | -0.0 | -0.0 | 0.0 | 0.0 |
| float | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/nan/inf/-inf | nan | nan |
| float | inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/-inf | inf | inf |
| float | inf | nan/inf | nan | nan |
| float | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/inf | -inf | -inf |
| float | -inf | nan/-inf | nan | nan |
| int16_t | -32768 (MIN) | -32768 (MIN) | 0 | 0 |
| int16_t | -32768 (MIN) | 32767 (MAX) | 1 | **-32768** |
| int16_t | 32767 (MAX) | -32768 (MIN) | -1 | **32767** |
| int16_t | 32767 (MAX) | 32767 (MAX) | 0 | 0 |
| int32_t | -2147483648 (MIN) | -2147483648 (MIN) | 0 | 0 |
| int32_t | -2147483648 (MIN) | 2147483647 (MAX) | 1 | **-2147483648** |
| int32_t | 2147483647 (MAX) | -2147483648 (MIN) | -1 | **2147483647** |
| int32_t | 2147483647 (MAX) | 2147483647 (MAX) | 0 | 0 |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 11** Computation results for Mul special value/boundary value inputs

<a name="table89442511309"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN) | inf | **65504.0** |
| half | -65504.0 (MIN) | 65504.0 (MAX) | -inf | **-65504.0** |
| half | -65504.0 (MIN) | 0.0 | -0.0 | -0.0 |
| half | -65504.0 (MIN) | -0.0 | 0.0 | 0.0 |
| half | -65504.0 (MIN) | nan | nan | **0.0** |
| half | -65504.0 (MIN) | inf | -inf | **-65504.0** |
| half | -65504.0 (MIN) | -inf | inf | **65504.0** |
| half | 65504.0 (MAX) | 65504.0 (MAX) | inf | **65504.0** |
| half | 65504.0 (MAX) | 0.0 | 0.0 | 0.0 |
| half | 65504.0 (MAX) | -0.0 | -0.0 | -0.0 |
| half | 65504.0 (MAX) | nan | nan | **0.0** |
| half | 65504.0 (MAX) | inf | inf | **65504.0** |
| half | 65504.0 (MAX) | -inf | -inf | **-65504.0** |
| half | 0.0 | 0.0 | 0.0 | 0.0 |
| half | 0.0 | -0.0 | -0.0 | -0.0 |
| half | 0.0 | nan/inf/-inf | nan | **0.0** |
| half | -0.0 | -0.0 | 0.0 | 0.0 |
| half | -0.0 | nan/inf/-inf | nan | **0.0** |
| half | nan | nan/inf/-inf | nan | **0.0** |
| half | inf | inf | inf | **65504.0** |
| half | inf | -inf | -inf | **-65504.0** |
| half | -inf | -inf | inf | **65504.0** |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN) | inf | inf |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | -inf | -inf |
| float | -3.4028235e+38 (MIN) | 0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38 (MIN) | -0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | -inf | -inf |
| float | -3.4028235e+38 (MIN) | -inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX) | inf | inf |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | -0.0 | -0.0 | -0.0 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | inf | inf | inf |
| float | 3.4028235e+38 (MAX) | -inf | -inf | -inf |
| float | 0.0 | 0.0 | 0.0 | 0.0 |
| float | 0.0 | -0.0 | -0.0 | -0.0 |
| float | 0.0 | nan/inf/-inf | nan | nan |
| float | -0.0 | -0.0 | 0.0 | 0.0 |
| float | -0.0 | nan/inf/-inf | nan | nan |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | inf | inf |
| float | inf | -inf | -inf | -inf |
| float | -inf | -inf | inf | inf |
| int16_t | -32768 (MIN) | -32768 (MIN) | 0 | **32767** |
| int16_t | -32768 (MIN) | 32767 (MAX) | -32768 | -32768 |
| int16_t | 32767 (MAX) | 32767 (MAX) | 1 | **32767** |
| int32_t | -2147483648 (MIN) | -2147483648 (MIN) | 0 | **2147483647** |
| int32_t | -2147483648 (MIN) | 2147483647 (MAX) | -2147483648 | -2147483648 |
| int32_t | 2147483647 (MAX) | 2147483647 (MAX) | 1 | **2147483647** |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 12**  Computation results of Div for special value/boundary value inputs

<a name="table118329223319"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN) | 1.0 | 1.0 |
| half | -65504.0 (MIN) | 65504.0 (MAX) | -1.0 | -1.0 |
| half | -65504.0 (MIN) | 0.0 | -inf | **-65504.0** |
| half | -65504.0 (MIN) | -0.0 | inf | **65504.0** |
| half | -65504.0 (MIN) | nan | nan | **0.0** |
| half | -65504.0 (MIN) | inf | -0.0 | -0.0 |
| half | -65504.0 (MIN) | -inf | 0.0 | 0.0 |
| half | 65504.0 (MAX) | -65504.0 (MIN) | -1.0 | -1.0 |
| half | 65504.0 (MAX) | 65504.0 (MAX) | 1.0 | 1.0 |
| half | 65504.0 (MAX) | 0.0 | inf | **65504.0** |
| half | 65504.0 (MAX) | -0.0 | -inf | **-65504.0** |
| half | 65504.0 (MAX) | nan | nan | **0.0** |
| half | 65504.0 (MAX) | inf | 0.0 | 0.0 |
| half | 65504.0 (MAX) | -inf | -0.0 | -0.0 |
| half | 0.0 | -65504.0 (MIN)/-inf | -0.0 | -0.0 |
| half | 0.0 | 65504.0 (MAX)/inf | 0.0 | 0.0 |
| half | 0.0 | 0.0/-0.0/nan | nan | **0.0** |
| half | -0.0 | -65504.0 (MIN)/-inf | 0.0 | 0.0 |
| half | -0.0 | 65504.0 (MAX)/inf | -0.0 | -0.0 |
| half | -0.0 | 0.0/-0.0/nan | nan | **0.0** |
| half | nan | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/nan/inf/-inf | nan | **0.0** |
| half | inf | -65504.0 (MIN)/-0.0 | -inf | **-65504.0** |
| half | inf | 65504.0 (MAX)/0.0 | inf | **65504.0** |
| half | inf | nan/inf/-inf | nan | **0.0** |
| half | -inf | -65504.0 (MIN)/-0.0 | inf | **65504.0** |
| half | -inf | 65504.0 (MAX)/0.0 | -inf | **-65504.0** |
| half | -inf | nan/inf/-inf | nan | **0.0** |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN) | 1.0 | 1.0 |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | -1.0 | -1.0 |
| float | -3.4028235e+38 (MIN) | 0.0 | -inf | -inf |
| float | -3.4028235e+38 (MIN) | -0.0 | inf | inf |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | -0.0 | -0.0 |
| float | -3.4028235e+38 (MIN) | -inf | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN) | -1.0 | -1.0 |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX) | 1.0 | 1.0 |
| float | 3.4028235e+38 (MAX) | 0.0 | inf | inf |
| float | 3.4028235e+38 (MAX) | -0.0 | -inf | -inf |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | inf | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | -inf | -0.0 | -0.0 |
| float | 0.0 | -3.4028235e+38 (MIN)/-inf | -0.0 | -0.0 |
| float | 0.0 | 3.4028235e+38 (MAX)/inf | 0.0 | 0.0 |
| float | 0.0 | 0.0/-0.0/nan | nan | nan |
| float | -0.0 | -3.4028235e+38 (MIN)/-inf | 0.0 | 0.0 |
| float | -0.0 | 3.4028235e+38 (MAX)/inf | -0.0 | -0.0 |
| float | -0.0 | 0.0/-0.0/nan | nan | nan |
| float | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/nan/inf/-inf | nan | nan |
| float | inf | -3.4028235e+38 (MIN)/-0.0 | -inf | -inf |
| float | inf | 3.4028235e+38 (MAX)/0.0 | inf | inf |
| float | inf | nan/inf/-inf | nan | nan |
| float | -inf | -3.4028235e+38 (MIN)/-0.0 | inf | inf |
| float | -inf | 3.4028235e+38 (MAX)/0.0 | -inf | -inf |
| float | -inf | nan/inf/-inf | nan | nan |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 13**  Computation results of Max for special value/boundary value inputs

<a name="table172607531318"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN)/-inf | -65504.0 | -65504.0 |
| half | -65504.0 (MIN) | 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | -65504.0 (MIN) | 0.0 | 0.0 | 0.0 |
| half | -65504.0 (MIN) | -0.0 | -0.0 | -0.0 |
| half | -65504.0 (MIN) | nan | nan | **0.0** |
| half | -65504.0 (MIN) | inf | inf | **65504.0** |
| half | 65504.0 (MAX) | 65504.0 (MAX)/0.0/-0.0/-inf | 65504.0 | 65504.0 |
| half | 65504.0 (MAX) | nan | nan | **0.0** |
| half | 65504.0 (MAX) | inf | inf | **65504.0** |
| half | 0.0 | 0.0/-0.0/-inf | 0.0 | 0.0 |
| half | 0.0 | nan | nan | **0.0** |
| half | 0.0 | inf | inf | **65504.0** |
| half | -0.0 | -0.0/-inf | -0.0 | -0.0 |
| half | -0.0 | nan | nan | **0.0** |
| half | -0.0 | inf | inf | **65504.0** |
| half | nan | nan/inf/-inf | nan | **0.0** |
| half | inf | inf/-inf | inf | **65504.0** |
| half | -inf | -inf | -inf | **-65504.0** |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/-inf | -3.4028235e+38 | -3.4028235e+38 |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | 3.4028235e+38 | 3.4028235e+38 |
| float | -3.4028235e+38 (MIN) | 0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | -0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX)/0.0/-0.0/-inf | 3.4028235e+38 | 3.4028235e+38 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | inf | inf | inf |
| float | 0.0 | 0.0/-0.0/-inf | 0.0 | 0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | inf | inf | inf |
| float | -0.0 | -0.0/-inf | -0.0 | -0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | inf | inf | inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf/-inf | inf | inf |
| float | -inf | -inf | -inf | -inf |
| int16_t | -32768 (MIN) | -32768 (MIN) | -32768 | -32768 |
| int16_t | -32768 (MIN) | 32767 (MAX) | 32767 | 32767 |
| int16_t | 32767 (MAX) | 32767 (MAX) | 32767 | 32767 |
| int32_t | -2147483648 (MIN) | -2147483648 (MIN) | -2147483648 | -2147483648 |
| int32_t | -2147483648 (MIN) | 2147483647 (MAX) | 2147483647 | 2147483647 |
| int32_t | 2147483647 (MAX) | 2147483647 (MAX) | 2147483647 | 2147483647 |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 14** Computation results of Min with special value/boundary value inputs

<a name="table179652226325"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst (Register Non-saturation Mode) | Element Values of dst (Register Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/inf | -65504.0 | -65504.0 |
| half | -65504.0 (MIN) | nan | nan | **0.0** |
| half | -65504.0 (MIN) | -inf | -inf | **-65504.0** |
| half | 65504.0 (MAX) | 65504.0 (MAX)/inf | 65504.0 | 65504.0 |
| half | 65504.0 (MAX) | 0.0 | 0.0 | 0.0 |
| half | 65504.0 (MAX) | -0.0 | -0.0 | -0.0 |
| half | 65504.0 (MAX) | nan | nan | **0.0** |
| half | 65504.0 (MAX) | -inf | -inf | **-65504.0** |
| half | 0.0 | 0.0/inf | 0.0 | 0.0 |
| half | 0.0 | -0.0 | -0.0 | -0.0 |
| half | 0.0 | nan | nan | **0.0** |
| half | 0.0 | -inf | -inf | **-65504.0** |
| half | -0.0 | -0.0/inf | -0.0 | -0.0 |
| half | -0.0 | nan | nan | 0.0 |
| half | -0.0 | -inf | -inf | **-65504.0** |
| half | nan | nan/inf/-inf | nan | **0.0** |
| half | inf | inf | inf | **65504.0** |
| half | inf | -inf | -inf | **-65504.0** |
| half | -inf | -inf | -inf | **-65504.0** |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/inf | -3.4028235e+38 | -3.4028235e+38 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | -inf | -inf | -inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX)/inf | 3.4028235e+38 | 3.4028235e+38 |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | -0.0 | -0.0 | -0.0 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | -inf | -inf | -inf |
| float | 0.0 | 0.0/inf | 0.0 | 0.0 |
| float | 0.0 | -0.0 | -0.0 | -0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | -inf | -inf | -inf |
| float | -0.0 | -0.0/inf | -0.0 | -0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | -inf | -inf | -inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | inf | inf |
| float | inf | -inf | -inf | -inf |
| float | -inf | -inf | -inf | -inf |
| int16_t | -32768 (MIN) | -32768 (MIN)/32767 (MAX) | -32768 | -32768 |
| int16_t | 32767 (MAX) | 32767 (MAX) | 32767 | 32767 |
| int32_t | -2147483648 (MIN) | -2147483648 (MIN)/2147483647 (MAX) | -2147483648 | -2147483648 |
| int32_t | 2147483647 (MAX) | 2147483647 (MAX) | 2147483647 | 2147483647 |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 15** Computation results of Adds with special value/boundary value inputs

<a name="table15619175083314"></a>

| Data Type of src/dst | Element Values of src | value of scalarValue | Element Values of dst<br> (INF/NAN mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN)/-inf | -inf | **-65504.0** |
| half | -65504.0 (MIN) | 65504.0 (MAX) | 0.0 | 0.0 |
| half | -65504.0 (MIN) | 0.0/-0.0 | -65504.0 | -65504.0 |
| half | -65504.0 (MIN) | nan | nan | **0.0** |
| half | -65504.0 (MIN) | inf | inf | **65504.0** |
| half | 65504.0 (MAX) | 65504.0 (MAX)/inf | inf | **65504.0** |
| half | 65504.0 (MAX) | 0.0/-0.0 | 65504.0 | 65504.0 |
| half | 65504.0 (MAX) | nan | nan | **0.0** |
| half | 65504.0 (MAX) | -inf | -inf | **-65504.0** |
| half | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| half | 0.0 | nan | nan | **0.0** |
| half | 0.0 | inf | inf | **65504.0** |
| half | 0.0 | -inf | -inf | **-65504.0** |
| half | -0.0 | -0.0 | -0.0 | -0.0 |
| half | -0.0 | nan | nan | **0.0** |
| half | -0.0 | inf | inf | **65504.0** |
| half | -0.0 | -inf | -inf | **-65504.0** |
| half | nan | nan/inf/-inf | nan | **0.0** |
| half | inf | inf | inf | **65504.0** |
| half | inf | -inf | nan | **0.0** |
| half | -inf | -inf | -inf | **-65504.0** |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/-inf | -inf | -inf |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | 0.0/-0.0 | -3.40282E+38 | -3.40282E+38 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX)/inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 0.0/-0.0 | 3.40282E+38 | 3.40282E+38 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | -inf | -inf | -inf |
| float | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | inf | inf | inf |
| float | 0.0 | -inf | -inf | -inf |
| float | -0.0 | -0.0 | -0.0 | -0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | inf | inf | inf |
| float | -0.0 | -inf | -inf | -inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | inf | inf |
| float | inf | -inf | nan | nan |
| float | -inf | -inf | -inf | -inf |
| int16_t | -32768 (MIN) | -32768 (MIN) | 0 | **-32768** |
| int16_t | -32768 (MIN) | 32767 (MAX) | -1 | -1 |
| int16_t | 32767 (MAX) | 32767 (MAX) | -2 | **32767** |
| int32_t | -2147483648 (MIN) | -2147483648 (MIN) | 0 | **-2147483648** |
| int32_t | -2147483648 (MIN) | 2147483648 (MAX) | -1 | -1 |
| int32_t | 2147483648 (MAX) | 2147483648 (MAX) | -2 | **2147483648** |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 16** Computation results of Muls with special value/boundary value inputs

<a name="table1510462203414"></a>

| Data Type of src/dst | Element Values of src | value of scalarValue | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN) | inf | **65504.0** |
| half | -65504.0 (MIN) | 65504.0 (MAX) | -inf | **-65504.0** |
| half | -65504.0 (MIN) | 0.0 | -0.0 | -0.0 |
| half | -65504.0 (MIN) | -0.0 | 0.0 | 0.0 |
| half | -65504.0 (MIN) | nan | nan | **0.0** |
| half | -65504.0 (MIN) | inf | -inf | **-65504.0** |
| half | -65504.0 (MIN) | -inf | inf | **65504.0** |
| half | 65504.0 (MAX) | 65504.0 (MAX) | inf | **65504.0** |
| half | 65504.0 (MAX) | 0.0 | 0.0 | 0.0 |
| half | 65504.0 (MAX) | -0.0 | -0.0 | -0.0 |
| half | 65504.0 (MAX) | nan | nan | **0.0** |
| half | 65504.0 (MAX) | inf | inf | **65504.0** |
| half | 65504.0 (MAX) | -inf | -inf | **-65504.0** |
| half | 0.0 | 0.0 | 0.0 | 0.0 |
| half | 0.0 | -0.0 | -0.0 | -0.0 |
| half | 0.0 | nan/inf/-inf | nan | **0.0** |
| half | -0.0 | -0.0 | 0.0 | 0.0 |
| half | -0.0 | nan/inf/-inf | nan | **0.0** |
| half | nan | nan/inf/-inf | nan | **0.0** |
| half | inf | inf | inf | **65504.0** |
| half | inf | -inf | -inf | **-65504.0** |
| half | -inf | -inf | inf | **65504.0** |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN) | inf | inf |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | -inf | -inf |
| float | -3.4028235e+38 (MIN) | 0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38 (MIN) | -0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | -inf | -inf |
| float | -3.4028235e+38 (MIN) | -inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX) | inf | inf |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | -0.0 | -0.0 | -0.0 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | inf | inf | inf |
| float | 3.4028235e+38 (MAX) | -inf | -inf | -inf |
| float | 0.0 | 0.0 | 0.0 | 0.0 |
| float | 0.0 | -0.0 | -0.0 | -0.0 |
| float | 0.0 | nan/inf/-inf | nan | nan |
| float | -0.0 | -0.0 | 0.0 | 0.0 |
| float | -0.0 | nan/inf/-inf | nan | nan |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | inf | inf |
| float | inf | -inf | -inf | -inf |
| float | -inf | -inf | inf | inf |
| int16_t | -32768 (MIN) | -32768 (MIN) | 0 | **32767** |
| int16_t | -32768 (MIN) | 32767 (MAX) | -32768 | -32768 |
| int16_t | 32767 (MAX) | 32767 (MAX) | 1 | **32767** |
| int32_t | -2147483648 (MIN) | -2147483648 (MIN) | 0 | **2147483648** |
| int32_t | -2147483648 (MIN) | 2147483648 (MAX) | -2147483648 | -2147483648 |
| int32_t | 2147483648 (MAX) | 2147483648 (MAX) | 1 | **2147483648** |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 17** Computation results of Maxs with special value/boundary value inputs

<a name="table825085423412"></a>

| Data Type of src/dst | Element Values of src | value of scalarValue | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN)/-inf | -65504.0 | -65504.0 |
| half | -65504.0 (MIN) | 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | -65504.0 (MIN) | 0.0 | 0.0 | 0.0 |
| half | -65504.0 (MIN) | -0.0 | -0.0 | -0.0 |
| half | -65504.0 (MIN) | nan | nan | **0.0** |
| half | -65504.0 (MIN) | inf | inf | **65504.0** |
| half | 65504.0 (MAX) | 65504.0 (MAX)/0.0/-0.0/-inf | 65504.0 | 65504.0 |
| half | 65504.0 (MAX) | nan | nan | **0.0** |
| half | 65504.0 (MAX) | inf | inf | **65504.0** |
| half | 0.0 | 0.0/-0.0/-inf | 0.0 | 0.0 |
| half | 0.0 | nan | nan | **0.0** |
| half | 0.0 | inf | inf | **65504.0** |
| half | -0.0 | -0.0/-inf | -0.0 | -0.0 |
| half | -0.0 | nan | nan | **0.0** |
| half | -0.0 | inf | inf | **65504.0** |
| half | nan | nan/inf/-inf | nan | **0.0** |
| half | inf | inf/-inf | inf | **65504.0** |
| half | -inf | -inf | -inf | **-65504.0** |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/-inf | -3.40282E+38 | -3.40282E+38 |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | 3.40282E+38 | 3.40282E+38 |
| float | -3.4028235e+38 (MIN) | 0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | -0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX)/0.0/-0.0/-inf | 3.40282E+38 | 3.40282E+38 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | inf | inf | inf |
| float | 0.0 | 0.0/-0.0/-inf | 0.0 | 0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | inf | inf | inf |
| float | -0.0 | -0.0/-inf | -0.0 | -0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | inf | inf | inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf/-inf | inf | inf |
| float | -inf | -inf | -inf | -inf |
| int16_t | -32768 (MIN) | -32768 (MIN) | -32768 | -32768 |
| int16_t | -32768 (MIN) | 32767 (MAX) | 32767 | 32767 |
| int16_t | 32767 (MAX) | 32767 (MAX) | 32767 | 32767 |
| int32_t | -2147483648 (MIN) | -2147483648 (MIN) | -2147483648 | -2147483648 |
| int32_t | -2147483648 (MIN) | 2147483648 (MAX) | 2147483648 | 2147483648 |
| int32_t | 2147483648 (MAX) | 2147483648 (MAX) | 2147483648 | 2147483648 |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 18** Computation results of Mins with special value/boundary value inputs

<a name="table714843013511"></a>

| Data Type of src/dst | Element Values of src | Value of scalarValue | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/inf | -65504.0 | -65504.0 |
| half | -65504.0 (MIN) | nan | nan | **0.0** |
| half | -65504.0 (MIN) | -inf | -inf | **-65504.0** |
| half | 65504.0 (MAX) | 65504.0 (MAX)/inf | 65504.0 | 65504.0 |
| half | 65504.0 (MAX) | 0.0 | 0.0 | 0.0 |
| half | 65504.0 (MAX) | -0.0 | -0.0 | -0.0 |
| half | 65504.0 (MAX) | nan | nan | **0.0** |
| half | 65504.0 (MAX) | -inf | -inf | **-65504.0** |
| half | 0.0 | 0.0/inf | 0.0 | 0.0 |
| half | 0.0 | -0.0 | -0.0 | -0.0 |
| half | 0.0 | nan | nan | **0.0** |
| half | 0.0 | -inf | -inf | **-65504.0** |
| half | -0.0 | -0.0/inf | -0.0 | -0.0 |
| half | -0.0 | nan | nan | 0.0 |
| half | -0.0 | -inf | -inf | **-65504.0** |
| half | nan | nan/inf/-inf | nan | **0.0** |
| half | inf | inf | inf | **65504.0** |
| half | inf | -inf | -inf | **-65504.0** |
| half | -inf | -inf | -inf | **-65504.0** |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/inf | -3.40282E+38 | -3.40282E+38 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | -inf | -inf | -inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX)/inf | 3.40E+38 | 3.40E+38 |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | -0.0 | -0.0 | -0.0 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | -inf | -inf | -inf |
| float | 0.0 | 0.0/inf | 0.0 | 0.0 |
| float | 0.0 | -0.0 | -0.0 | -0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | -inf | -inf | -inf |
| float | -0.0 | -0.0/inf | -0.0 | -0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | -inf | -inf | -inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | inf | inf |
| float | inf | -inf | -inf | -inf |
| float | -inf | -inf | -inf | -inf |
| int16_t | -32768 (MIN) | -32768 (MIN)/32767 (MAX) | -32768 | -32768 |
| int16_t | 32767 (MAX) | 32767 (MAX) | 32767 | 32767 |
| int32_t | -2147483648 (MIN) | -2147483648 (MIN)/2147483648 (MAX) | -2147483648 | -2147483648 |
| int32_t | 2147483648 (MAX) | 2147483648 (MAX) | 2147483648 | 2147483648 |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 19** Computation results of LeakyRelu with special values/boundary values as input

<a name="table2399171617392"></a>

| Data Type of src/dst | Element Values of src | value of scalarValue | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN) | inf | **65504.0** |
| half | -65504.0 (MIN) | 65504.0 (MAX) | -inf | **-65504.0** |
| half | -65504.0 (MIN) | 0.0 | -0.0 | -0.0 |
| half | -65504.0 (MIN) | -0.0 | 0.0 | 0.0 |
| half | -65504.0 (MIN) | nan | nan | **0.0** |
| half | -65504.0 (MIN) | inf | -inf | **65504.0** |
| half | -65504.0 (MIN) | -inf | inf | **-65504.0** |
| half | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/nan/inf/-inf | 65504.0 | 65504.0 |
| half | 0.0 | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/nan/inf/-inf | 0.0 | 0.0 |
| half | -0.0 | -65504.0 (MIN)/-0.0 | 0.0 | 0.0 |
| half | -0.0 | 65504.0 (MAX)/0.0 | -0.0 | -0.0 |
| half | -0.0 | nan/inf/-inf | nan | 0.0 |
| half | nan | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/nan/inf/-inf | nan | **0.0** |
| half | inf | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/nan/inf/-inf | inf | **65504.0** |
| half | -inf | -65504.0 (MIN)/-inf | inf | **65504.0** |
| half | -inf | 65504.0 (MAX)/inf | -inf | **-65504.0** |
| half | -inf | 0.0/-0.0/nan | nan | **0.0** |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN) | inf | inf |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | -inf | -inf |
| float | -3.4028235e+38 (MIN) | 0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38 (MIN) | -0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | -inf | -inf |
| float | -3.4028235e+38 (MIN) | -inf | inf | inf |
| float | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/nan/inf/-inf | 3.40E+38 | 3.40E+38 |
| float | 0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/nan/inf/-inf | 0.0 | 0.0 |
| float | -0.0 | -3.4028235e+38 (MIN)/-0.0 | 0.0 | 0.0 |
| float | -0.0 | 3.4028235e+38 (MAX)/0.0 | -0.0 | -0.0 |
| float | -0.0 | nan/inf/-inf | nan | nan |
| float | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/nan/inf/-inf | nan | nan |
| float | inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/nan/inf/-inf | inf | inf |
| float | -inf | -3.4028235e+38 (MIN)/-inf | inf | inf |
| float | -inf | 3.4028235e+38 (MAX)/inf | -inf | -inf |
| float | -inf | 0.0/-0.0/nan | nan | nan |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

## Compound Computation<a name="section1978519408454"></a>

Note: In the following table, MAX/MIN correspond to the maximum and minimum values under the data type of the corresponding operand.

**Table 20** Computation results of Axpy with special value/boundary value inputs

<a name="table1819218568247"></a>

| Value of src/scalarValue | Intermediate Stage (src*scalarValue) | Intermediate Stage (Saturation Mode, Supported Only When the Output is of the half Data Type) | dst Value | dst Output (INF/NAN Mode) | dst Output (Saturation Mode, Supported Only When the Output is of the half Data Type) |
| --- | --- | --- | --- | --- | --- |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | +0 | +0 | +0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | -0 | +0 | -0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | +inf | +inf | MAX |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | -inf | -inf | MIN |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | MAX | MAX | MAX |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | MIN | MIN | MIN |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | nan | nan | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | +0 | +0 | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | -0 | -0 | -0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | +inf | +inf | +inf |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | -inf | -inf | -inf |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | MAX | MAX | MAX |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | MIN | MIN | MIN |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | nan | nan | +0 |
| when the source operand and destination operand have the same data type:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>when the source operand and destination operand have different data types:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | +0 | +inf | MAX |
| when the source operand and destination operand have the same data type:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>when the source operand and destination operand have different data types:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | -0 | +inf | MAX |
| when the source operand and destination operand have the same data type:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>when the source operand and destination operand have different data types:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | +inf | +inf | MAX |
| when the source operand and destination operand have the same data type:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>when the source operand and destination operand have different data types:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | -inf | nan | MIN |
| when the source operand and destination operand have the same data type:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>when the source operand and destination operand have different data types:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | MAX | +inf | MAX |
| when the source operand and destination operand have the same data type:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>when the source operand and destination operand have different data types:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | MIN | +inf | +0 |
| when the source operand and destination operand have the same data type:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>when the source operand and destination operand have different data types:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | nan | nan | +0 |
| when the source operand and destination operand have the same data type:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>when the source operand and destination operand have different data types:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | +0 | -inf | MIN |
| when the source operand and destination operand have the same data type:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>when the source operand and destination operand have different data types:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | -0 | -inf | MIN |
| when the source operand and destination operand have the same data type:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>when the source operand and destination operand have different data types:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | +inf | nan | MAX |
| when the source operand and destination operand have the same data type:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>when the source operand and destination operand have different data types:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | -inf | -inf | MIN |
| when the source operand and destination operand have the same data type:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>when the source operand and destination operand have different data types:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | MAX | -inf | +0 |
| when the source operand and destination operand have the same data type:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>when the source operand and destination operand have different data types:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | MIN | -inf | MIN |
| when the source operand and destination operand have the same data type:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>when the source operand and destination operand have different data types:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | nan | nan | +0 |
| such that src*scalarValue=MAX | MAX | MAX | +0 | MAX | MAX |
| such that src*scalarValue=MAX | MAX | MAX | -0 | MAX | MAX |
| such that src*scalarValue=MAX | MAX | MAX | +inf | +inf | MAX |
| such that src*scalarValue=MAX | MAX | MAX | -inf | -inf | MIN |
| such that src*scalarValue=MAX | MAX | MAX | MAX | MAX | MAX |
| such that src*scalarValue=MAX | MAX | MAX | MIN | +0 | +0 |
| such that src*scalarValue=MAX | MAX | MAX | nan | nan | +0 |
| such that src*scalarValue=MIN | MIN | MIN | +0 | MIN | MIN |
| such that src*scalarValue=MIN | MIN | MIN | -0 | MIN | MIN |
| such that src*scalarValue=MIN | MIN | MIN | +inf | +inf | MAX |
| such that src*scalarValue=MIN | MIN | MIN | -inf | -inf | MIN |
| such that src*scalarValue=MIN | MIN | MIN | MAX | +0 | +0 |
| such that src*scalarValue=MIN | MIN | MIN | MIN | -inf | MIN |
| such that src*scalarValue=MIN | MIN | MIN | nan | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +0 | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -0 | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +inf | nan | MAX |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -inf | nan | MIN |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MAX | nan | MAX |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MIN | nan | MIN |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | nan | nan | +0 |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 21** Computation results of MulAddDst with special value/boundary value inputs

<a name="table1042313583406"></a>

| src0/src1 value | Intermediate Stage (src0*src1) | Intermediate Stage (Saturation Mode, Supported Only When the Output is of the half Data Type) | dst Value | dst Output (INF/NAN Mode) | dst Output (Saturation Mode, Supported Only When the Output Is of the half Data Type) |
| --- | --- | --- | --- | --- | --- |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | +0 | +0 | +0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | -0 | +0 | -0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | +inf | +inf | MAX |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | -inf | -inf | MIN |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | MAX | MAX | MAX |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | MIN | MIN | MIN |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | nan | nan | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | +0 | +0 | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | -0 | -0 | -0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | +inf | +inf | +inf |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | -inf | -inf | -inf |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | MAX | MAX | MAX |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | MIN | MIN | MIN |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | nan | nan | +0 |
| when the source operand and destination operand have the same data type:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>when the source operand and destination operand have different data types:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | +0 | +inf | MAX |
| when the source operand and destination operand have the same data type:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>when the source operand and destination operand have different data types:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | -0 | +inf | MAX |
| when the source operand and destination operand have the same data type:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>when the source operand and destination operand have different data types:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | +inf | +inf | MAX |
| when the source operand and destination operand have the same data type:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>when the source operand and destination operand have different data types:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | -inf | nan | MIN |
| when the source operand and destination operand have the same data type:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>when the source operand and destination operand have different data types:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | MAX | +inf | MAX |
| when the source operand and destination operand have the same data type:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>when the source operand and destination operand have different data types:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | MIN | +inf | +0 |
| when the source operand and destination operand have the same data type:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>when the source operand and destination operand have different data types:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | nan | nan | +0 |
| when the source operand and destination operand have the same data type:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>when the source operand and destination operand have different data types:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | +0 | -inf | MIN |
| when the source operand and destination operand have the same data type:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>when the source operand and destination operand have different data types:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | -0 | -inf | MIN |
| when the source operand and destination operand have the same data type:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>when the source operand and destination operand have different data types:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | +inf | nan | MAX |
| when the source operand and destination operand have the same data type:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>when the source operand and destination operand have different data types:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | -inf | -inf | MIN |
| when the source operand and destination operand have the same data type:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>when the source operand and destination operand have different data types:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | MAX | -inf | +0 |
| when the source operand and destination operand have the same data type:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>when the source operand and destination operand have different data types:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | MIN | -inf | MIN |
| when the source operand and destination operand have the same data type:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>when the source operand and destination operand have different data types:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | nan | nan | +0 |
| such that src0*src1=MAX | MAX | MAX | +0 | MAX | MAX |
| such that src0*src1=MAX | MAX | MAX | -0 | MAX | MAX |
| such that src0*src1=MAX | MAX | MAX | +inf | +inf | MAX |
| such that src0*src1=MAX | MAX | MAX | -inf | -inf | MIN |
| such that src0*src1=MAX | MAX | MAX | MAX | MAX | MAX |
| such that src0*src1=MAX | MAX | MAX | MIN | +0 | +0 |
| such that src0*src1=MAX | MAX | MAX | nan | nan | +0 |
| such that src0*src1=MIN | MIN | MIN | +0 | MIN | MIN |
| such that src0*src1=MIN | MIN | MIN | -0 | MIN | MIN |
| such that src0*src1=MIN | MIN | MIN | +inf | +inf | MAX |
| such that src0*src1=MIN | MIN | MIN | -inf | -inf | MIN |
| such that src0*src1=MIN | MIN | MIN | MAX | +0 | +0 |
| such that src0*src1=MIN | MIN | MIN | MIN | -inf | MIN |
| such that src0*src1=MIN | MIN | MIN | nan | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +0 | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -0 | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +inf | nan | MAX |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -inf | nan | MIN |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MAX | nan | MAX |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MIN | nan | MIN |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | nan | nan | +0 |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 22** Computation results of FusedMulAdd for special value/boundary value inputs

<a name="table1158211814515"></a>

| Value of src0/dst | Intermediate Stage (src0*dst) | Intermediate Stage (Saturation Mode, Supported Only When the Output Is of the half Data Type) | Value of src1 | dst Output (INF/NAN Mode) | dst Output (Saturation Mode, Supported Only When the Output Is of the half Data Type) |
| --- | --- | --- | --- | --- | --- |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | +0 | +0 | +0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | -0 | +0 | -0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | +inf | +inf | MAX |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | -inf | -inf | MIN |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | MAX | MAX | MAX |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | MIN | MIN | MIN |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | nan | nan | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | +0 | +0 | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | -0 | -0 | -0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | +inf | +inf | +inf |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | -inf | -inf | -inf |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | MAX | MAX | MAX |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | MIN | MIN | MIN |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | nan | nan | +0 |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | +0 | +inf | MAX |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | -0 | +inf | MAX |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | +inf | +inf | MAX |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | -inf | nan | MIN |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | MAX | +inf | MAX |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | MIN | +inf | +0 |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | nan | nan | +0 |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | +0 | -inf | MIN |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | -0 | -inf | MIN |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | +inf | nan | MAX |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | -inf | -inf | MIN |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | MAX | -inf | +0 |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | MIN | -inf | MIN |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | nan | nan | +0 |
| such that src0*dst=MAX | MAX | MAX | +0 | MAX | MAX |
| such that src0*dst=MAX | MAX | MAX | -0 | MAX | MAX |
| such that src0*dst=MAX | MAX | MAX | +inf | +inf | MAX |
| such that src0*dst=MAX | MAX | MAX | -inf | -inf | MIN |
| such that src0*dst=MAX | MAX | MAX | MAX | MAX | MAX |
| such that src0*dst=MAX | MAX | MAX | MIN | +0 | +0 |
| such that src0*dst=MAX | MAX | MAX | nan | nan | +0 |
| such that src0*dst=MIN | MIN | MIN | +0 | MIN | MIN |
| such that src0*dst=MIN | MIN | MIN | -0 | MIN | MIN |
| such that src0*dst=MIN | MIN | MIN | +inf | +inf | MAX |
| such that src0*dst=MIN | MIN | MIN | -inf | -inf | MIN |
| such that src0*dst=MIN | MIN | MIN | MAX | +0 | +0 |
| such that src0*dst=MIN | MIN | MIN | MIN | -inf | MIN |
| such that src0*dst=MIN | MIN | MIN | nan | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +0 | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -0 | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +inf | nan | MAX |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -inf | nan | MIN |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MAX | nan | MAX |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MIN | nan | MIN |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | nan | nan | +0 |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 23** Computation results of MulAddRelu for special value/boundary value inputs

<a name="table126194714473"></a>

| value of src0 / value of dst | intermediate stage 1 (src0*dst) | intermediate stage 1 (saturation mode, supported only when the output data type is half) | value of src1 | intermediate stage 2 (INF/NAN Mode) | intermediate stage 2 (saturation mode, supported only when the output data type is half) | dst output (INF/NAN Mode) | dst Output (Saturation Mode, Supported Only When the Output Is of the half Data Type) |
| --- | --- | --- | --- | --- | --- | --- | --- |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | +0 | +0 | +0 | +0 | +0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | -0 | +0 | -0 | +0 | +0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | +inf | +inf | MAX | +inf | MAX |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | -inf | -inf | MIN | +0 | +0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | MAX | MAX | MAX | MAX | MAX |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | MIN | MIN | MIN | +0 | +0 |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | nan | nan | +0 | nan | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | +0 | +0 | +0 | +0 | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | -0 | -0 | -0 | +0 | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | +inf | +inf | +inf | +inf | MAX |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | -inf | -inf | -inf | +0 | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | MAX | MAX | MAX | MAX | MAX |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | MIN | MIN | MIN | +0 | +0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | nan | nan | +0 | nan | +0 |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | +0 | +inf | MAX | +inf | MAX |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | -0 | +inf | MAX | +inf | MAX |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | +inf | +inf | MAX | +inf | MAX |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | -inf | nan | MIN | nan | +0 |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | MAX | +inf | MAX | +inf | MAX |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | MIN | +inf | +0 | +inf | +0 |
| +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN | +inf | MAX | nan | nan | +0 | nan | +0 |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | +0 | -inf | MIN | +0 | +0 |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | -0 | -inf | MIN | +0 | +0 |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | +inf | nan | MAX | nan | MAX |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | -inf | -inf | MIN | +0 | +0 |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | MAX | -inf | +0 | +0 | +0 |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | MIN | -inf | MIN | +0 | +0 |
| +inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN | -inf | MIN | nan | nan | +0 | nan | +0 |
| such that src0*dst=MAX | MAX | MAX | +0 | MAX | MAX | MAX | MAX |
| such that src0*dst=MAX | MAX | MAX | -0 | MAX | MAX | MAX | MAX |
| such that src0*dst=MAX | MAX | MAX | +inf | +inf | MAX | +inf | MAX |
| such that src0*dst=MAX | MAX | MAX | -inf | -inf | MIN | +0 | +0 |
| such that src0*dst=MAX | MAX | MAX | MAX | MAX | MAX | MAX | MAX |
| such that src0*dst=MAX | MAX | MAX | MIN | +0 | +0 | +0 | +0 |
| such that src0*dst=MAX | MAX | MAX | nan | nan | +0 | nan | +0 |
| such that src0*dst=MIN | MIN | MIN | +0 | MIN | MIN | +0 | +0 |
| such that src0*dst=MIN | MIN | MIN | -0 | MIN | MIN | +0 | +0 |
| such that src0*dst=MIN | MIN | MIN | +inf | +inf | MAX | +inf | MAX |
| such that src0*dst=MIN | MIN | MIN | -inf | -inf | MIN | +0 | +0 |
| such that src0*dst=MIN | MIN | MIN | MAX | +0 | +0 | +0 | +0 |
| such that src0*dst=MIN | MIN | MIN | MIN | -inf | MIN | +0 | +0 |
| such that src0*dst=MIN | MIN | MIN | nan | nan | +0 | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +0 | nan | +0 | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -0 | nan | +0 | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +inf | nan | MAX | nan | MAX |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -inf | nan | MIN | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MAX | nan | MAX | nan | MAX |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MIN | nan | MIN | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | nan | nan | +0 | nan | +0 |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 24** Computation results of AddRelu for special value/boundary value inputs

<a name="table143801137164310"></a>

| src0 value/src1 value | intermediate stage (src0+src1) | intermediate stage (saturation mode, supported only when the output is of the half data type) | dst output (INF/NAN Mode) | dst Output (Saturation Mode, Supported Only When the Output Is of the half Data Type) |
| --- | --- | --- | --- | --- |
| +0/+0<br>+0/-0 | +0 | +0 | +0 | +0 |
| -0/-0 | -0 | -0 | +0 | +0 |
| +inf/+inf<br>+inf/MAX<br>+inf/MIN<br>+0/+inf<br>-0/+inf<br>MAX/MAX | +inf | MAX | +inf | MAX |
| -inf/-inf<br>-inf/MAX<br>-inf/MIN<br>+0/-inf<br>-0/+inf<br>MIN/MIN | -inf | MIN | +0 | +0 |
| such that src0+src1=MAX | MAX | MAX | MAX | MAX |
| such that src0+src1=MIN | MIN | MIN | +0 | +0 |
| nan/arbitrary value<br>+inf/-inf | nan | +0 | nan | +0 |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 25** Computation results of SubRelu for special value/boundary value inputs

<a name="table7783335111811"></a>

| src0 value/src1 value | intermediate stage (src0-src1) | intermediate stage (saturation mode, supported only when the output is of the half data type) | dst output (INF/NAN mode) | dst Output (Saturation Mode, Supported Only When the Output Is of the half Data Type) |
| --- | --- | --- | --- | --- |
| +0/+0<br>+0/-0 | +0 | +0 | +0 | +0 |
| -0/+0 | -0 | -0 | +0 | +0 |
| +inf/-inf<br>+inf/MAX<br>+inf/MIN<br>+0/-inf<br>-0/-inf<br>MAX/MIN<br>MAX/-inf<br>MIN/-inf | +inf | MAX | +inf | MAX |
| -inf/-inf<br>-inf/MAX<br>-inf/MIN<br>+0/-inf<br>-0/+inf<br>MIN/MIN | -inf | MIN | +0 | +0 |
| such that src0-src1=MAX | MAX | MAX | MAX | MAX |
| such that src0-src1=MIN | MIN | MIN | +0 | +0 |
| nan/arbitrary value<br>+inf/+inf<br>-inf/-inf | nan | +0 | nan | +0 |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 26** Computation results of AddReluCast for special value/boundary value inputs

<a name="table494414452367"></a>

| Data Type of src0 / data type of src1 / data type of dst | src0 value/src1 value | intermediate stage 1 (src0+src1) (INF/NAN mode) | intermediate stage 2 (INF/NAN mode) | dst output (INF/NAN mode) |
| --- | --- | --- | --- | --- |
| float/float/half | +0/+0<br>+0/-0 | +0 | +0 | +0 |
| float/float/half | -0/-0 | -0 | +0 | +0 |
| float/float/half | +inf/+inf<br>+inf/MAX<br>+inf/MIN<br>+0/+inf<br>-0/+inf<br>MAX/MAX | +inf | +inf | +inf |
| float/float/half | -inf/-inf<br>-inf/MAX<br>-inf/MIN<br>+0/-inf<br>-0/+inf<br>MIN/MIN | -inf | +0 | +0 |
| float/float/half | such that src0+src1=MAX | MAX | MAX | MAX |
| float/float/half | such that src0+src1=MIN | MIN | +0 | +0 |
| float/float/half | nan/arbitrary value<br>+inf/-inf | nan | nan | +0 |
| half/half/int8_t | +0/+0<br>+0/-0 | +0 | +0 | 0 |
| half/half/int8_t | -0/-0 | -0 | +0 | 0 |
| half/half/int8_t | +inf/+inf<br>+inf/MAX<br>+inf/MIN<br>+0/+inf<br>-0/+inf<br>MAX/MAX | +inf | +inf | MAX |
| half/half/int8_t | -inf/-inf<br>-inf/MAX<br>-inf/MIN<br>+0/-inf<br>-0/+inf<br>MIN/MIN | -inf | +0 | 0 |
| half/half/int8_t | such that src0+src1=MAX | MAX | MAX | MAX |
| half/half/int8_t | such that src0+src1=MIN | MIN | +0 | 0 |
| half/half/int8_t | nan/arbitrary value<br>+inf/-inf | nan | nan | 0 |
| int16_t/int16_t/int8_t | 0/0 | 0 | 0 | 0 |
| int16_t/int16_t/int8_t | MAX/MAX | -2 | 0 | 0 |
| int16_t/int16_t/int8_t | MIN/MIN | 0 | 0 | 0 |

Note:
- The value of dst elements (INF/NAN mode) column result is consistent with the IEEE 754 theoretical result.
- When the input is int16_t, src0+src1 is computed with int16_t bit-width wraparound (two's complement). The intermediate stage 1 result of MAX/MAX addition is -2, and the intermediate stage 1 result of MIN/MIN addition is 0. ReLU and saturation conversion to int8_t are then performed.

**Table 27** Computation results of SubReluCast with special values/boundary values as input

<a name="table1418126172917"></a>

| Data Type of src0 / data type of src1 / data type of dst | src0 value/src1 value | intermediate stage 1 (src0-src1) (INF/NAN mode) | intermediate stage 2 (INF/NAN mode) | dst output (INF/NAN mode) |
| --- | --- | --- | --- | --- |
| float/float/half | +0/+0<br>+0/-0<br>-0/-0 | +0 | +0 | +0 |
| float/float/half | -0/+0 | -0 | +0 | +0 |
| float/float/half | +inf/-inf<br>+inf/MAX<br>+inf/MIN<br>+0/-inf<br>-0/-inf<br>MAX/MIN | +inf | +inf | +inf |
| float/float/half | -inf/+0<br>-inf/MAX<br>-inf/MIN<br>+0/+inf<br>-0/+inf<br>MIN/MAX | -inf | +0 | +0 |
| float/float/half | such that src0-src1=MAX | MAX | MAX | MAX |
| float/float/half | such that src0-src1=MIN | MIN | +0 | +0 |
| float/float/half | nan/arbitrary value<br>+inf/+inf<br>-inf/-inf | nan | nan | +0 |
| half/half/int8_t | +0/+0<br>+0/-0<br>-0/-0 | +0 | +0 | 0 |
| half/half/int8_t | -0/-0 | -0 | +0 | 0 |
| half/half/int8_t | +inf/-inf<br>+inf/MAX<br>+inf/MIN<br>+inf/0<br>+inf/-0<br>+0/-inf<br>-0/-inf<br>MAX/MIN | +inf | +inf | MAX |
| half/half/int8_t | -inf/-inf<br>-inf/MAX<br>-inf/MIN<br>+0/-inf<br>-0/+inf<br>MIN/MIN | -inf | +0 | 0 |
| half/half/int8_t | such that src0-src1=MAX | MAX | MAX | MAX |
| half/half/int8_t | such that src0-src1=MIN | MIN | +0 | 0 |
| half/half/int8_t | nan/arbitrary value<br>+inf/+inf<br>-inf/-inf | nan | nan | 0 |
| int16_t/int16_t/int8_t | 0/0 | 0 | 0 | 0 |
| int16_t/int16_t/int8_t | MAX/MIN | -1 | 0 | 0 |
| int16_t/int16_t/int8_t | MIN/MAX | 1 | 1 | 1 |

Note:
- The value of dst elements (INF/NAN mode) column result is consistent with the IEEE 754 theoretical result.
- When the input is int16_t, src0-src1 is computed with int16_t bit-width wraparound (two's complement). The intermediate stage 1 result of MAX/MIN subtraction is -1, and the intermediate stage 1 result of MIN/MAX subtraction is 1. ReLU and saturation conversion to int8_t are then performed.

**Table 28** Computation results of MulCast with special values/boundary values as input

<a name="table1698216295159"></a>

| src0 value/src1 value | intermediate stage (src0*src1) | intermediate stage (saturation mode, supported only when the output is of the half data type) | dst output (INF/NAN mode) | dst Output (Saturation Mode, Supported Only When the Output Is of the half Data Type) |
| --- | --- | --- | --- | --- |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | 0 | 0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | 0 | 0 |
| MAX/MAX<br>+inf/MAX<br>-inf/MIN | +inf | MAX | MAX | MAX |
| MAX/MIN<br>+inf/MIN<br>-inf/MAX | -inf | MIN | MIN | MIN |
| such that src0*src1=MAX | MAX | MAX | MAX | MAX |
| such that src0*src1=MIN | MIN | MIN | MIN | MIN |
| nan/arbitrary number<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | 0 | 0 |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 29** Computation results of CastDequant with special values/boundary values as input (MAX/MIN correspond to the maximum value and minimum value under the data type of the corresponding operand, scale=1, offset=0)

<a name="table18277108174810"></a>

| src/dst Data Type | Value of src | dst Output (INF/NAN Mode) | dst Output (Saturation Mode, Supported Only When the Output Is of the half Data Type) |
| --- | --- | --- | --- |
| int32_t/half | 0 | 0 | 0 |
| int32_t/half | 65504 | MAX | MAX |
| int32_t/half | -65504 | MIN | MIN |
| int32_t/half | MAX | +inf | MAX |
| int32_t/half | MIN | -inf | MIN |
| int16_t/int8_t | 0 | 0 | / |
| int16_t/int8_t | MAX | MAX | / |
| int16_t/int8_t | MIN | MIN | / |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

## Comparison and Selection<a name="section829519484517"></a>

Note:
- In the following table, MAX/MIN correspond to the maximum value and minimum value under the data type of the corresponding operand.
- GetCmpMask is a data movement operation that sets the comparison mask register and does not involve special values/boundary values.

**Table 30** Computation results of Compare with special values/boundary values as input

<a name="table1961220798"></a>

| src Data Type/src1 Data Type | cmpMode Comparison Mode | src0 Data Type | src1 Data Type | dst Output Data (INF/NAN mode) | dst Output Data (Saturation Mode) |
| --- | --- | --- | --- | --- | --- |
| half/half | CMPMODE::EQ | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/0 | 1/0/0/0/0/0/0 |
| half/half | CMPMODE::EQ | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/0/0 | 0/1/0/0/0/0/0 |
| half/half | CMPMODE::EQ | +0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| half/half | CMPMODE::EQ | -0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| half/half | CMPMODE::EQ | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::EQ | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::EQ | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GE | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GE | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::GE | +0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GE | -0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GE | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::GE | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| half/half | CMPMODE::GE | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GT | +0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | -0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::GT | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::GT | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LE | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LE | +0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | -0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LE | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::LE | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| half/half | CMPMODE::LT | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LT | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::LT | +0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LT | -0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LT | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LT | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LT | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| half/half | CMPMODE::NE | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/1 | 0/1/1/1/0/1/1 |
| half/half | CMPMODE::NE | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/1/1 | 1/0/1/1/0/1/1 |
| half/half | CMPMODE::NE | +0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/0/0/0/1/1 | 1/1/0/0/0/1/1 |
| half/half | CMPMODE::NE | -0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/0/0/0/1/1 | 1/1/0/0/0/1/1 |
| half/half | CMPMODE::NE | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::NE | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::NE | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::EQ | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/0 | 1/0/0/0/0/0/0 |
| float/float | CMPMODE::EQ | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/0/0 | 0/1/0/0/0/0/0 |
| float/float | CMPMODE::EQ | +0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| float/float | CMPMODE::EQ | -0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| float/float | CMPMODE::EQ | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::EQ | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::EQ | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GE | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GE | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::GE | +0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GE | -0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GE | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::GE | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| float/float | CMPMODE::GE | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GT | +0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | -0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::GT | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::GT | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LE | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LE | +0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | -0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LE | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::LE | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| float/float | CMPMODE::LT | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LT | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::LT | +0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LT | -0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LT | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LT | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LT | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::NE | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/1 | 0/1/1/1/0/1/1 |
| float/float | CMPMODE::NE | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/1/1 | 1/0/1/1/0/1/1 |
| float/float | CMPMODE::NE | +0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/0/0/0/1/1 | 1/1/0/0/0/1/1 |
| float/float | CMPMODE::NE | -0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/0/0/0/1/1 | 1/1/0/0/0/1/1 |
| float/float | CMPMODE::NE | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::NE | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::NE | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| int32_t/int32_t | CMPMODE::EQ | -2147483648 (MIN) | -2147483648 (MIN)/2147483647 (MAX) | 1/0 | 1/0 |
| int32_t/int32_t | CMPMODE::EQ | 2147483647 (MAX) | -2147483648 (MIN)/2147483647 (MAX) | 0/1 | 0/1 |

Note:

-   In CMPMODE::NE mode, when the data type is half or float, the computation result of the Compare API differs from the IEEE754 theoretical result:
    -   When src0=nan and src1=MIN/MAX/+0/-0/nan/+inf/-inf, the actual result: the output data of dst (INF/NAN mode and saturation mode) is 0/0/0/0/0/0/0, while the IEEE754 theoretical result is 1/1/1/1/1/1/1.
    -   When src0=MIN/MAX/+0/-0/nan/+inf/-inf and src1=nan, the actual result is: output data of dst (INF/NAN mode and saturation mode) is 0/0/0/0/0/0/0, and the IEEE754 theoretical result is 1/1/1/1/1/1/1.

-   In the remaining modes, the output data of dst (INF/NAN mode) and output data of dst (saturation mode) are consistent with the IEEE754 theoretical result.

**Table 31** Computation results of Compares for special value/boundary value inputs

<a name="table1126411512176"></a>

| src/src1 Data Type | cmpMode Comparison Mode | Data Type of src0 | Data Type of scalar | dst Output Data (INF/NAN Mode) | dst Output Data (Saturation Mode) |
| --- | --- | --- | --- | --- | --- |
| half/half | CMPMODE::EQ | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/0 | 1/0/0/0/0/0/0 |
| half/half | CMPMODE::EQ | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/0/0 | 0/1/0/0/0/0/0 |
| half/half | CMPMODE::EQ | +0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| half/half | CMPMODE::EQ | -0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| half/half | CMPMODE::EQ | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::EQ | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::EQ | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GE | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GE | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::GE | +0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GE | -0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GE | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::GE | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| half/half | CMPMODE::GE | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GT | +0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | -0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::GT | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::GT | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LE | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LE | +0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | -0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LE | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::LE | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| half/half | CMPMODE::LT | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LT | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::LT | +0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LT | -0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LT | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LT | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LT | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| half/half | CMPMODE::NE | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/1 | 0/1/1/1/1/1/1 |
| half/half | CMPMODE::NE | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/1/1 | 1/0/1/1/1/1/1 |
| half/half | CMPMODE::NE | +0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/0/0/0/1/1 | 1/1/0/0/1/1/1 |
| half/half | CMPMODE::NE | -0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/0/0/0/1/1 | 1/1/0/0/1/1/1 |
| half/half | CMPMODE::NE | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::NE | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::NE | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::EQ | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/0 | 1/0/0/0/0/0/0 |
| float/float | CMPMODE::EQ | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/0/0 | 0/1/0/0/0/0/0 |
| float/float | CMPMODE::EQ | +0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| float/float | CMPMODE::EQ | -0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| float/float | CMPMODE::EQ | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::EQ | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::EQ | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GE | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GE | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::GE | +0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GE | -0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GE | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::GE | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| float/float | CMPMODE::GE | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GT | +0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | -0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::GT | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::GT | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LE | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LE | +0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | -0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LE | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::LE | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| float/float | CMPMODE::LT | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LT | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::LT | +0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LT | -0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LT | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LT | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LT | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::NE | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/1 | 0/1/1/1/0/1/1 |
| float/float | CMPMODE::NE | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/1/1 | 1/0/1/1/0/1/1 |
| float/float | CMPMODE::NE | +0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/0/0/0/1/1 | 1/1/0/0/0/1/1 |
| float/float | CMPMODE::NE | -0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/0/0/0/1/1 | 1/1/0/0/0/1/1 |
| float/float | CMPMODE::NE | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::NE | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::NE | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| int32_t/int32_t | CMPMODE::EQ | -2147483648 (MIN) | -2147483648 (MIN)/2147483647 (MAX) | 1/0 | 1/0 |
| int32_t/int32_t | CMPMODE::EQ | 2147483647 (MAX) | -2147483648 (MIN)/2147483647 (MAX) | 0/1 | 0/1 |

Note:

-   In CMPMODE::NE mode, when the data type is half or float, the computation result of the Compares API differs from the IEEE754 theoretical result:
    -   When src0=nan and scalar=MIN/MAX/+0/-0/nan/+inf/-inf, the actual result is: output data of dst (INF/NAN mode and saturation mode) is 0/0/0/0/0/0/0, and the IEEE754 theoretical result is 1/1/1/1/1/1/1.
    -   When src0=MIN/MAX/+0/-0/nan/+inf/-inf and scalar=nan, the actual result is: output data of dst (INF/NAN mode and saturation mode) is 0/0/0/0/0/0/0, and the IEEE754 theoretical result is 1/1/1/1/1/1/1.

-   In the remaining modes, the output data of dst (INF/NAN mode) and output data of dst (saturation mode) are consistent with the IEEE754 theoretical result.

## Type Conversion<a name="section3558104315715"></a>

Note: In the following table, MAX/MIN correspond to the maximum and minimum values under the data type of the corresponding operand.

**Table 32** Computation results of Cast special value/boundary value input

<a name="table1225292414418"></a>

| src Data Type/dst Data Type | src Input Data | dst Output Data (INF/NAN Mode) | dst Output Data (Saturation Mode) |
| --- | --- | --- | --- |
| int4b_t/half | MIN | -8 | -8 |
| int4b_t/half | MAX | 7 | 7 |
| uint8_t/half | 0 | 0 | 0 |
| uint8_t/half | MAX | 255 | 255 |
| int8_t/half | MAX | 127 | 127 |
| int8_t/half | MIN | -128 | -128 |
| half/int4b_t | MAX | 0 | 7 |
| half/int4b_t | MIN | 0 | -8 |
| half/int4b_t | +0/-0 | 0 | 0 |
| half/int4b_t | nan | 0 | 0 |
| half/int4b_t | +inf | 7 | 7 |
| half/int4b_t | -inf | -8 | -8 |
| half/int8_t | MAX | -32 | 127 |
| half/int8_t | MIN | 32 | -128 |
| half/int8_t | +0/-0 | 0 | 0 |
| half/int8_t | nan | 0 | 0 |
| half/int8_t | +inf | 127 | 127 |
| half/int8_t | -inf | -128 | -128 |
| half/uint8_t | MAX | 224 | 255 |
| half/uint8_t | MIN | 32 | 0 |
| half/uint8_t | +0/-0 | 0 | 0 |
| half/uint8_t | nan | 0 | 0 |
| half/uint8_t | +inf | 255 | 255 |
| half/uint8_t | -inf | 0 | 0 |
| half/int16_t | MAX | -32 | 32767 |
| half/int16_t | MIN | 32 | -32768 |
| half/int16_t | +0/-0 | 0 | 0 |
| half/int16_t | nan | 0 | 0 |
| half/int16_t | +inf | 32767 | 32767 |
| half/int16_t | -inf | -32768 | -32768 |
| half/int32_t | MAX | 65504 | 65504 |
| half/int32_t | MIN | -65504 | -65504 |
| half/int32_t | +0/-0 | 0 | 0 |
| half/int32_t | nan | 0 | 0 |
| half/int32_t | +inf | 2147483647 | 2147483647 |
| half/int32_t | -inf | -2147483648 | -2147483648 |
| half/float | MAX | 65504 | 65504 |
| half/float | MIN | -65504 | -65504 |
| half/float | +0/-0 | 0 | 0 |
| half/float | nan | nan | nan |
| half/float | +inf | +inf | +inf |
| half/float | -inf | -inf | -inf |
| int16_t/half | MAX | Output differs under different rounding modes: in CAST_RINT/CAST_CEIL/CAST_ROUND/CAST_NONE mode, the output is 32768.00; in CAST_FLOOR/CAST_TRUNC mode, the output is 32752.00 | Output differs under different rounding modes: in CAST_RINT/CAST_CEIL/CAST_ROUND/CAST_NONE mode, the output is 32768.00; in CAST_FLOOR/CAST_TRUNC mode, the output is 32752.00 |
| int16_t/half | MIN | -32768.00 | -32768.00 |
| int16_t/half | 0 | +0 | +0 |
| int16_t/float | MAX | 32767.00 | 32767.00 |
| int16_t/float | MIN | -32768.00 | -32768.00 |
| int16_t/float | 0 | +0 | +0 |
| bfloat16_t/int32_t | 3.3895314e+38 (MAX) | 0 | 2147483647 |
| bfloat16_t/int32_t | -3.3895314e+38 (MIN) | 0 | -2147483648 |
| bfloat16_t/int32_t | 0 | 0 | 0 |
| bfloat16_t/int32_t | +inf | 2147483647 | 2147483647 |
| bfloat16_t/int32_t | -inf | -2147483648 | -2147483648 |
| bfloat16_t/int32_t | nan | 0 | 0 |
| bfloat16_t/float | 3.3895314e+38 (MAX) | 3.3895314e+38 | 3.3895314e+38 |
| bfloat16_t/float | -3.3895314e+38 (MIN) | -3.3895314e+38 | -3.3895314e+38 |
| bfloat16_t/float | 0 | 0 | 0 |
| bfloat16_t/float | +inf | +inf | +inf |
| bfloat16_t/float | -inf | -inf | -inf |
| bfloat16_t/float | nan | nan | nan |
| int32_t/int16_t | MAX | -1 | 32767 |
| int32_t/int16_t | MIN | 0 | -32768 |
| int32_t/int16_t | 0 | 0 | 0 |
| int32_t/half (refer to CastDequant) | / | / | / |
| int32_t/float | 2147483647 (MAX) | Output differs under different rounding modes: in CAST_RINT/CAST_CEIL/CAST_ROUND/CAST_NONE mode, the output is 2147483648.00; in CAST_FLOOR/CAST_TRUNC mode, the output is 2147483520.00 | Output differs under different rounding modes: in CAST_RINT/CAST_CEIL/CAST_ROUND/CAST_NONE mode, the output is 2147483648.00; in CAST_FLOOR/CAST_TRUNC mode, the output is 2147483520.00 |
| int32_t/float | -2147483648 (MIN) | -2147483648.00 | -2147483648.00 |
| int32_t/int64_t | 2147483647 (MAX) | 2147483647 | 2147483647 |
| int32_t/int64_t | -2147483648 (MIN) | -2147483648 | -2147483648 |
| float/int16_t | 3.4028235e+38 (MAX) | 0 | 32767 |
| float/int16_t | -3.4028235e+38 (MIN) | 0 | -32768 |
| float/int16_t | 0 | 0 | 0 |
| float/int16_t | +inf | 32767 | 32767 |
| float/int16_t | -inf | -32768 | -32768 |
| float/int16_t | nan | 0 | 0 |
| float/bfloat16_t | 3.4028235e+38 (MAX) | Output differs under different rounding modes: in CAST_RINT/CAST_CEIL/CAST_ROUND mode, the output is +inf; in CAST_FLOOR/CAST_TRUNC mode, the output is 3.3895314e+38 | MAX (3.3895314e+38) |
| float/bfloat16_t | -3.4028235e+38 (MIN) | The output is different under different rounding modes. In CAST_RINT/CAST_FLOOR/CAST_ROUND modes, the output is -inf; in CAST_CEIL/CAST_TRUNC modes, the output is -3.3895314e+38 | MIN (-3.3895314e+38) |
| float/bfloat16_t | +0 | +0 | +0 |
| float/bfloat16_t | -0 | -0 | -0 |
| float/bfloat16_t | +inf | +inf | MAX |
| float/bfloat16_t | -inf | -inf | MIN |
| float/bfloat16_t | nan | nan | 0 |
| float/half | 3.4028235e+38 (MAX) | +inf | MAX |
| float/half | -3.4028235e+38 (MIN) | -inf | MIN |
| float/half | 0 | 0 | 0 |
| float/half | +inf | +inf | MAX |
| float/half | -inf | -inf | MIN |
| float/half | nan | nan | 0 |
| float/int32_t | 3.4028235e+38 (MAX) | 0 | 2147483647 |
| float/int32_t | -3.4028235e+38 (MIN) | 0 | -2147483648 |
| float/int32_t | 0 | 0 | 0 |
| float/int32_t | +inf | 2147483647 | 2147483647 |
| float/int32_t | -inf | -2147483648 | -2147483648 |
| float/int32_t | nan | 0 | 0 |
| float/float | MAX | MAX | MAX |
| float/float | MIN | MIN | MIN |
| float/float | +0 | +0 | +0 |
| float/float | -0 | +0 | +0 |
| float/float | +inf | +inf | +inf |
| float/float | -inf | -inf | -inf |
| float/float | nan | nan | nan |
| float/int64_t | 3.4028235e+38 (MAX) | 0 | 9223372036854775807 |
| float/int64_t | -3.4028235e+38 (MIN) | 0 | -9223372036854775808 |
| float/int64_t | 0 | 0 | 0 |
| float/int64_t | +inf | 9223372036854775807 | 9223372036854775807 |
| float/int64_t | -inf | -9223372036854775808 | -9223372036854775808 |
| float/int64_t | nan | 0 | 0 |
| int64_t/int32_t | MAX | -1 | 2147483647 |
| int64_t/int32_t | MIN | 0 | -2147483648 |
| int64_t/float | MAX | The output is different under different rounding modes. In CAST_RINT/CAST_CEIL/CAST_ROUND modes, the output is 9223372036854775808.00; in CAST_FLOOR/CAST_TRUNC modes, the output is 9223371487098961920.00 | The output is different under different rounding modes. In CAST_RINT/CAST_CEIL/CAST_ROUND modes, the output is 9223372036854775808.00; in CAST_FLOOR/CAST_TRUNC modes, the output is 9223371487098961920.00 |
| int64_t/float | MIN | -9223372036854775808.00 (MIN) | -9223372036854775808.00 (MIN) |

Note: For float->float conversion, -0 is converted to +0, which is inconsistent with IEEE754.

## Reduction Computation<a name="section156745111398"></a>

Note: In the following table, MAX/MIN correspond to the maximum and minimum values under the data type of the corresponding operand.

**Table 33**  ReduceDataBlock<MAX\> Computation results for special value/boundary value inputs

<a name="table2883162019243"></a>

| Data Type of src/dst | Element Values of src (1 DataBlock) | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- |
| half | all -65504.0 (MIN) | -65504.0 | -65504.0 |
| half | all 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | all 0.0 | 0.0 | 0.0 |
| half | all -0.0 | -0.0 | -0.0 |
| half | all nan | nan | 0.0 |
| half | all inf | inf | 65504.0 |
| half | all -inf | -inf | -65504.0 |
| half | inf, -inf, nan alternating | nan | 0.0 |
| half | inf, -inf alternating | inf | 65504.0 |
| half | inf, nan alternating | nan | 0.0 |
| half | -inf, nan alternating | nan | 0.0 |
| half | 15 values of 1.0 plus 1 inf | inf | 65504.0 |
| half | 15 values of 1.0 plus 1 -inf | 1.0 | 1.0 |
| half | 15 values of 1.0 plus 1 nan | nan | 0.0 |
| float | all -3.4028235e+38 (MIN) | -3.4028235e+38 | -3.4028235e+38 |
| float | all 3.4028235e+38 (MAX) | 3.4028235e+38 | 3.4028235e+38 |
| float | all 0.0 | 0.0 | 0.0 |
| float | all -0.0 | -0.0 | -0.0 |
| float | all nan | nan | nan |
| float | all inf | inf | inf |
| float | all -inf | -inf | -inf |
| float | inf, -inf, nan alternating | nan | nan |
| float | inf, -inf alternating | inf | inf |
| float | inf, nan alternating | nan | nan |
| float | -inf, nan alternating | nan | nan |
| float | 7 values of 1.0 plus 1 inf | inf | inf |
| float | 7 values of 1.0 plus 1 -inf | 1.0 | 1.0 |
| float | 7 values of 1.0 plus 1 nan | nan | nan |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 34**  ReduceDataBlock<MIN\> Computation results for special value/boundary value inputs

<a name="table6602183015217"></a>

| Data Type of src/dst | Element Values of src (1 DataBlock) | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- |
| half | all -65504.0 (MIN) | -65504.0 | -65504.0 |
| half | all 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | all 0.0 | 0.0 | 0.0 |
| half | all -0.0 | -0.0 | -0.0 |
| half | all nan | nan | 0.0 |
| half | all inf | inf | 65504.0 |
| half | all -inf | -inf | -65504.0 |
| half | inf, -inf, nan alternating | nan | 0.0 |
| half | inf, -inf alternating | -inf | -65504 |
| half | inf, nan alternating | nan | 0.0 |
| half | -inf, nan alternating | nan | 0.0 |
| half | 15 values of 1.0 plus 1 inf | 1.0 | 1.0 |
| half | 15 values of 1.0 plus 1 -inf | -inf | -65504.0 |
| half | 15 values of 1.0 plus 1 nan | nan | 0.0 |
| float | all -3.4028235e+38 (MIN) | -3.4028235e+38 | -3.4028235e+38 |
| float | all 3.4028235e+38 (MAX) | 3.4028235e+38 | 3.4028235e+38 |
| float | all 0.0 | 0.0 | 0.0 |
| float | all -0.0 | -0.0 | -0.0 |
| float | all nan | nan | nan |
| float | all inf | inf | inf |
| float | all -inf | -inf | -inf |
| float | inf, -inf, nan alternating | nan | nan |
| float | inf, -inf alternating | -inf | -inf |
| float | inf, nan alternating | nan | nan |
| float | -inf, nan alternating | nan | nan |
| float | 7 values of 1.0 plus 1 inf | 1.0 | 1.0 |
| float | 7 values of 1.0 plus 1 -inf | -inf | -inf |
| float | 7 values of 1.0 plus 1 nan | nan | nan |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 35**  ReduceDataBlock<SUM\> Computation results for special value/boundary value inputs

<a name="table47791758122218"></a>

| Data Type of src/dst | Element Values of src (1 DataBlock) | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- |
| half | all -65504.0 (MIN) | -inf | -65504.0 |
| half | all 65504.0 (MAX) | inf | 65504.0 |
| half | all 0.0 | 0.0 | 0.0 |
| half | all -0.0 | -0.0 | -0.0 |
| half | all nan | nan | 0.0 |
| half | all inf | inf | 65504.0 |
| half | all -inf | -inf | -65504.0 |
| half | inf, -inf, nan alternating | nan | 0.0 |
| half | inf, -inf alternating | nan | 0.0 |
| half | inf, nan alternating | nan | 0.0 |
| half | -inf, nan alternating | nan | 0.0 |
| half | 15 values of 1.0 plus 1 inf | inf | 65344.0 |
| half | 15 values of 1.0 plus 1 -inf | -inf | -65344.0 |
| half | 15 values of 1.0 plus 1 nan | nan | 14.0 |
| float | all -3.4028235e+38 (MIN) | -inf | -inf |
| float | all 3.4028235e+38 (MAX) | inf | inf |
| float | all 0.0 | 0.0 | 0.0 |
| float | all -0.0 | -0.0 | -0.0 |
| float | all nan | nan | nan |
| float | all inf | inf | inf |
| float | all -inf | -inf | -inf |
| float | inf, -inf, nan alternating | nan | nan |
| float | inf, -inf alternating | nan | nan |
| float | inf, nan alternating | nan | nan |
| float | -inf, nan alternating | nan | nan |
| float | 7 values of 1.0 plus 1 inf | inf | inf |
| float | 7 values of 1.0 plus 1 -inf | -inf | -inf |
| float | 7 values of 1.0 plus 1 nan | nan | nan |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 36**  ReduceRepeat<MAX\> Computation results for special value/boundary value inputs

<a name="table818992302819"></a>

| Data Type of src/dst | Element Values of src (8 DataBlocks) | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- |
| half | all -65504.0 (MIN) | -65504.0 | -65504.0 |
| half | all 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | all 0.0 | 0.0 | 0.0 |
| half | all -0.0 | -0.0 | -0.0 |
| half | all nan | nan | 0.0 |
| half | all inf | inf | 65504.0 |
| half | all -inf | -inf | -65504.0 |
| half | inf, -inf, nan alternating | nan | 0.0 |
| half | inf, -inf alternating | inf | 65504.0 |
| half | inf, nan alternating | nan | 0.0 |
| half | -inf, nan alternating | nan | 0.0 |
| half | 127 values of 1.0 plus one inf | inf | 65504.0 |
| half | 127 values of 1.0 plus one -inf | 1 | 1.0 |
| half | 127 values of 1.0 plus one nan | nan | 0.0 |
| float | all -3.4028235e+38 (MIN) | -3.4028235e+38 | -3.4028235e+38 |
| float | all 3.4028235e+38 (MAX) | 3.4028235e+38 | 3.4028235e+38 |
| float | all 0.0 | 0.0 | 0.0 |
| float | all -0.0 | -0.0 | -0.0 |
| float | all nan | nan | nan |
| float | all inf | inf | inf |
| float | all -inf | -inf | -inf |
| float | inf, -inf, nan alternating | nan | nan |
| float | inf, -inf alternating | inf | inf |
| float | inf, nan alternating | nan | nan |
| float | -inf, nan alternating | nan | nan |
| float | 63 values of 1.0 plus one inf | inf | inf |
| float | 63 values of 1.0 plus one -inf | 1 | 1 |
| float | 63 values of 1.0 plus one nan | nan | nan |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 37**  ReduceRepeat<MIN\> Computation results for special value/boundary value inputs

<a name="table11721236104119"></a>

| Data Type of src/dst | Element Values of src (8 DataBlocks) | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- |
| half | all -65504.0 (MIN) | -65504.0 | -65504.0 |
| half | all 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | all 0.0 | 0.0 | 0.0 |
| half | all -0.0 | -0.0 | -0.0 |
| half | all nan | nan | 0 |
| half | all inf | inf | 65504 |
| half | all -inf | -inf | -65504 |
| half | inf, -inf, nan alternating | nan | 0 |
| half | inf, -inf alternating | -inf | -65504 |
| half | inf, nan alternating | nan | 0 |
| half | -inf, nan alternating | nan | 0 |
| half | 127 values of 1.0 plus one inf | 1 | 1 |
| half | 127 values of 1.0 plus one -inf | -inf | -65504 |
| half | 127 values of 1.0 plus one nan | nan | 0 |
| float | all -3.4028235e+38 (MIN) | -3.4028235e+38 | -3.4028235e+38 |
| float | all 3.4028235e+38 (MAX) | 3.4028235e+38 | 3.4028235e+38 |
| float | all 0.0 | 0.0 | 0.0 |
| float | all -0.0 | -0.0 | -0.0 |
| float | all nan | nan | nan |
| float | all inf | inf | inf |
| float | all -inf | -inf | -inf |
| float | inf, -inf, nan alternating | nan | nan |
| float | inf, -inf alternating | -inf | -inf |
| float | inf, nan alternating | nan | nan |
| float | -inf, nan alternating | nan | nan |
| float | 63 values of 1.0 plus one inf | 1.0 | 1.0 |
| float | 63 values of 1.0 plus one -inf | -inf | -inf |
| float | 63 values of 1.0 plus one nan | nan | nan |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 38**  ReduceRepeat<SUM\> Computation results for special value/boundary value inputs

<a name="table9281103820417"></a>

| Data Type of src/dst | Element Values of src (8 DataBlocks) | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- |
| half | all -65504.0 (MIN) | -inf | -65504.0 |
| half | all 65504.0 (MAX) | inf | 65504.0 |
| half | all 0.0 | 0.0 | 0.0 |
| half | all -0.0 | -0.0 | -0.0 |
| half | all nan | nan | 0.0 |
| half | all inf | inf | 65504.0 |
| half | all -inf | -inf | -65504.0 |
| half | inf, -inf, nan alternating | nan | 0.0 |
| half | inf, -inf alternating | nan | 0.0 |
| half | inf, nan alternating | nan | 0.0 |
| half | -inf, nan alternating | nan | 0.0 |
| half | 127 values of 1.0 plus one inf | inf | 65504.0 |
| half | 127 values of 1.0 plus one -inf | -inf | -65376.0 |
| half | 127 values of 1.0 plus one nan | nan | 126.0 |
| float | all -3.4028235e+38 (MIN) | -inf | -inf |
| float | all 3.4028235e+38 (MAX) | inf | inf |
| float | all 0.0 | 0.0 | 0.0 |
| float | all -0.0 | -0.0 | -0.0 |
| float | all nan | nan | nan |
| float | all inf | inf | inf |
| float | all -inf | -inf | -inf |
| float | inf, -inf, nan alternating | nan | nan |
| float | inf, -inf alternating | nan | nan |
| float | inf, nan alternating | nan | nan |
| float | -inf, nan alternating | nan | nan |
| float | 63 values of 1.0 plus one inf | inf | inf |
| float | 63 values of 1.0 plus one -inf | -inf | -inf |
| float | 63 values of 1.0 plus one nan | nan | nan |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 39**  Computation results for ReduceMax special value/boundary value inputs

<a name="table491948105210"></a>

| Data Type of src/dst | Element Values of src (8 DataBlocks) | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- |
| half | all -65504.0 (MIN) | -65504.0 | -65504.0 |
| half | all 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | all 0.0 | 0.0 | 0.0 |
| half | all -0.0 | -0.0 | -0.0 |
| half | all nan | nan | 0.0 |
| half | all inf | inf | 65504.0 |
| half | all -inf | -inf | -65504.0 |
| half | inf, -inf, and nan plus 125 values of 1.0 | nan | 0.0 |
| half | inf and -inf plus 126 values of 1.0 | inf | 65504.0 |
| half | inf and nan plus 126 values of 1.0 | nan | 0.0 |
| half | -inf and nan plus 126 values of 1.0 | nan | 0.0 |
| half | 127 values of 1.0 plus one inf | inf | 65504.0 |
| half | 127 values of 1.0 plus one -inf | 1.0 | 1.0 |
| half | 127 values of 1.0 plus one nan | nan | 0.0 |
| float | all -3.4028235e+38 (MIN) | -3.4028235e+38 | -3.4028235e+38 |
| float | all 3.4028235e+38 (MAX) | 3.4028235e+38 | 3.4028235e+38 |
| float | all 0.0 | 0.0 | 0.0 |
| float | all -0.0 | -0.0 | -0.0 |
| float | all nan | nan | nan |
| float | all inf | inf | inf |
| float | all -inf | -inf | -inf |
| float | inf, -inf, nan alternating | nan | nan |
| float | inf, -inf alternating | inf | inf |
| float | inf, nan alternating | nan | nan |
| float | -inf, nan alternating | nan | nan |
| float | 63 values of 1.0 plus one inf | inf | inf |
| float | 63 values of 1.0 plus one -inf | 1.0 | 1.0 |
| float | 63 values of 1.0 plus one nan | nan | nan |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 40** Computation results of ReduceMin with special values/boundary values as input

<a name="table144621256001"></a>

| Data Type of src/dst | Element Values of src (8 DataBlocks) | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- |
| half | all -65504.0 (MIN) | -65504.0 | -65504.0 |
| half | all 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | all 0.0 | 0.0 | 0.0 |
| half | all -0.0 | -0.0 | -0.0 |
| half | all nan | nan | 0.0 |
| half | all inf | inf | 65504.0 |
| half | all -inf | -inf | -65504.0 |
| half | inf, -inf, and nan plus 125 values of 1.0 | nan | 0.0 |
| half | inf and -inf plus 126 values of 1.0 | -inf | -65504.0 |
| half | inf and nan plus 126 values of 1.0 | nan | 0.0 |
| half | -inf and nan plus 126 values of 1.0 | nan | 0.0 |
| half | 127 values of 1.0 plus one inf | 1.0 | 1.0 |
| half | 127 values of 1.0 plus one -inf | -inf | -65504.0 |
| half | 127 values of 1.0 plus one nan | nan | 0.0 |
| float | all -3.4028235e+38 (MIN) | -3.4028235e+38 | -3.4028235e+38 |
| float | all 3.4028235e+38 (MAX) | 3.4028235e+38 | 3.4028235e+38 |
| float | all 0.0 | 0.0 | 0.0 |
| float | all -0.0 | -0.0 | -0.0 |
| float | all nan | nan | nan |
| float | all inf | inf | inf |
| float | all -inf | -inf | -inf |
| float | inf, -inf, nan alternating | nan | nan |
| float | inf, -inf alternating | -inf | -inf |
| float | inf, nan alternating | nan | nan |
| float | -inf, nan alternating | nan | nan |
| float | 63 values of 1.0 plus one inf | 1.0 | 1.0 |
| float | 63 values of 1.0 plus one -inf | -inf | -inf |
| float | 63 values of 1.0 plus one nan | nan | nan |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

**Table 41** Computation results of ReduceSum with special values/boundary values as input

<a name="table16600131416"></a>

| Data Type of src/dst | Element Values of src (8 DataBlocks) | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- |
| half | all -65504.0 (MIN) | -inf | -65504.0 |
| half | all 65504.0 (MAX) | inf | 65504.0 |
| half | all 0.0 | 0.0 | 0.0 |
| half | all -0.0 | -0.0 | -0.0 |
| half | all nan | nan | 0.0 |
| half | all inf | inf | 65504.0 |
| half | all -inf | -inf | -65504.0 |
| half | inf, -inf, and nan plus 125 values of 1.0 | nan | 124.0 |
| half | inf and -inf plus 126 values of 1.0 | nan | 126.0 |
| half | inf and nan plus 126 values of 1.0 | nan | 126.0 |
| half | -inf and nan plus 126 values of 1.0 | nan | 126.0 |
| half | 127 values of 1.0 plus one inf | inf | 65504.0 |
| half | 127 values of 1.0 plus one -inf | -inf | -65376.0 |
| half | 127 values of 1.0 plus one nan | nan | 126.0 |
| float | all -3.4028235e+38 (MIN) | -inf | -inf |
| float | all 3.4028235e+38 (MAX) | inf | inf |
| float | all 0.0 | 0.0 | 0.0 |
| float | all -0.0 | -0.0 | -0.0 |
| float | all nan | nan | nan |
| float | all inf | inf | inf |
| float | all -inf | -inf | -inf |
| float | inf, -inf, nan alternating | nan | nan |
| float | inf, -inf alternating | nan | nan |
| float | inf, nan alternating | nan | nan |
| float | -inf, nan alternating | nan | nan |
| float | 63 values of 1.0 plus one inf | inf | inf |
| float | 63 values of 1.0 plus one -inf | -inf | -inf |
| float | 63 values of 1.0 plus one nan | nan | nan |

Note: The results in the Element Values of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical result.

## Data Layout Conversion<a name="section33343814910"></a>

Transpose and TransDataTo5HD are data movement APIs and do not involve special values or boundary values.

## Data Filling<a name="section10713185619915"></a>

Brcb and Duplicate are filling-type APIs and do not involve special values or boundary values.

The computation result of CreateVecIndex with special values or boundary values as input is consistent with that of the Adds API.

## Sort and Merge<a name="section1342519199109"></a>

Sort32, MrgSort, and GetMrgSortResult do not involve special values/boundary values.

## Scatter and Gather<a name="section14495123581018"></a>

Gather and Gatherb are data movement APIs and do not involve special values or boundary values.

## Mask Operations<a name="section0460049181018"></a>

SetMaskCount, SetMaskNorm, SetVectorMask, and ResetMask are mask configuration APIs and do not involve special values or boundary values.
