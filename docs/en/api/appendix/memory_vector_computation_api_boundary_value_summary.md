# Memory Vector Computation API Boundary Value Summary<a name="ZH-CN_TOPIC_0000002533514388"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-09-06T08:43:16.393Z pushedAt=2026-09-11T00:44:30.149Z -->

## Overview<a name="section3674181114910"></a>

This section summarizes the two types of output results of the Memory vector computation API under boundary value input. The following content applies to the following models:

<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products
<!-- end id3 -->

Under boundary value input, the Memory vector computation API produces two types of output results:

- INF/NAN Mode (non-saturation mode)

    The three scenarios where the computation result is inf, -inf, or nan are output as is.

- Saturation mode

    Configured through the [SetSaturationFlag](../SIMD-API/basic_api/special_register_access/SetSaturationFlag_ISASI.md) API. In saturation mode, inf and -inf are saturated to MAX and MIN for output, and nan is saturated to 0 for output.

Note:

- The float data type input supports only the INF/NAN Mode (even if saturation mode is configured, the output result is the same as that of the INF/NAN mode); half, int16_t, and int32_t type inputs output the INF/NAN mode result by default.
- **In most scenarios, the output result of the INF/NAN mode is consistent with the IEEE754 standard result. If there is any inconsistency, it is noted at the corresponding API.**

## Summary of Scenarios Where the Memory Vector Computation API Outputs inf/nan

**Table 1**  Scenarios where floating-point computation outputs inf/nan

<a name="table1645210351498"></a>
| Scenario Description | Floating-Point Computation Output Result | Description |
| --- | --- | --- |
| **Input contains inf**<br>Exceptions:<br>&bull; exp(-inf)=0<br>&bull; inf * 0 = nan<br>&bull; inf + (-inf) = nan | inf | inf distinguishes +inf and -inf |
| **Computation result overflows**<br>Exceptions:<br>&bull; A non-nan value (excluding 0) divided by 0, such as Div/Sqrt/Reciprocal and other instructions<br>&bull; ln(0.0) = -inf; ln(-0.0) = -inf<br>&bull; rsqrt(0.0) = inf; rsqrt(-0.0) = -inf | inf | inf distinguishes +inf and -inf |
| **Input contains nan** | nan | nan does not distinguish positive and negative |
| **Computation result produces nan**<br>Examples:<br>&bull; (-inf) - (-inf) = nan, such as Add/Sub/Reduce and other instructions<br>&bull; (-inf) + inf = nan, such as Add/Sub/Reduce and other instructions<br>&bull; 0 * (+/-inf) = nan, such as Mul and other instructions<br>&bull; (+/-inf)/(+/-inf) = nan<br>&bull; 0/0 = nan<br>&bull; ln/Sqrt/Rsqrt instructions with negative input | nan | nan does not distinguish positive and negative |

For details, see the following APIs.

## Basic Arithmetic<a name="section12606472427"></a>

Note: In the following tables, MAX/MIN correspond to the maximum and minimum values of the corresponding operand's data type.

**Table 2** Computation result of Exp special value/boundary value input

<a name="table864650154918"></a>
| Data Type of src/dst | Element Value of src | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
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

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 3** Computation result of Ln special value/boundary value input

<a name="table10972164142410"></a>
| Data Type of src/dst | Element Value of src | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
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

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 4** Computation result of Abs with special value/boundary value input

<a name="table1975185932512"></a>
| Data Type of src/dst | Element Value of src | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
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

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE 754 theoretical result.

**Table 5** Computation result of Reciprocal with special value/boundary value input

<a name="table1046894562620"></a>
| Data Type of src/dst | Element Value of src | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
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

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE 754 theoretical result.

**Table 6** Computation result of Sqrt with special value/boundary value input

<a name="table1648943062718"></a>
| Data Type of src/dst | Element Value of src | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
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

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 7** Computation result of Rsqrt with special value/boundary value input

<a name="table12701922112818"></a>
| Data Type of src/dst | Element Value of src | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
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

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 8** Computation result of Relu with special value/boundary value input

<a name="table135522523284"></a>
| Data Type of src/dst | Element Value of src | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
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

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 9** Computation result of Add with special value/boundary value input

<a name="table071981613"></a>
| Data Type of src/dst | Element Value of src0 | Element Value of src1 | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
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

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 10** Computation result of Sub with special value/boundary value input

<a name="table12357151012179"></a>
| Data Type of src/dst | Element Value of src0 | Element Value of src1 | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
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

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 11** Computation result of Mul with special value/boundary value input

<a name="table89442511309"></a>
| Data Type of src/dst | Element Value of src0 | Element Value of src1 | Element Value of dst<br>(INF/NAN Mode) | Element Value of dst<br>(Saturation Mode) |
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

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 12** Computation result of Div special value/boundary value input

<a name="table118329223319"></a>
| Data Type of src/dst | Element Value of src0 | Element Value of src1 | Element Value of dst<br>(INF/NAN Mode) | Element Value of dst<br>(Saturation Mode) |
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

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 13** Computation result of Max special value/boundary value input

<a name="table172607531318"></a>
| Data Type of src/dst | Element Value of src0 | Element Value of src1 | Element Value of dst<br>(INF/NAN Mode) | Element Value of dst<br>(Saturation Mode) |
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

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 14**  Computation result description of Min special value/boundary value input

<a name="table179652226325"></a>
| Data Type of src/dst | Element Value of src0 | Element Value of src1 | Element Value of dst<br>(Register Non-Saturation Mode) | Element Value of dst<br>(Register Saturation Mode) |
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

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 15**  Computation result of Adds special value/boundary value input

<a name="table15619175083314"></a>
| Data Type of src/dst | Element Value of src | Value of scalarValue | Element Value of dst<br>(INF/NAN Mode) | Element Value of dst<br>(Saturation Mode) |
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

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 16**  Computation result of Muls special value/boundary value input

<a name="table1510462203414"></a>
| Data Type of src/dst | Element Value of src | Value of scalarValue | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
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

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 17**  Computation result of Maxs special value/boundary value input

<a name="table825085423412"></a>
| Data Type of src/dst | Element Value of src | Value of scalarValue | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
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

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 18**  Computation result of Mins special value/boundary value input

<a name="table714843013511"></a>
| Data Type of src/dst | Element Value of src | Value of scalarValue | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
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

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 19** Computation results of LeakyRelu special value/boundary value input

<a name="table2399171617392"></a>
| Data Type of src/dst | Element Value of src | Value of scalarValue | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
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

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

## Composite Computation<a name="section1978519408454"></a>

Note: In the following table, MAX/MIN correspond to the maximum and minimum values of the corresponding data type of the operand.

**Table 20** Computation result of Axpy with special value/boundary value input

<a name="table1819218568247"></a>
| Value of src/Value of scalarValue | Intermediate Stage (src*scalarValue) | Intermediate Stage (Saturation Mode, Supported Only When the Output Is of the half Data Type) | Value of dst | dst Output (INF/NAN Mode) | dst Output (Saturation Mode, Supported Only When the Output Is of the half Data Type) |
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
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | +0 | +inf | MAX |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | -0 | +inf | MAX |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | +inf | +inf | MAX |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | -inf | nan | MIN |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | MAX | +inf | MAX |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | MIN | +inf | +0 |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | nan | nan | +0 |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | +0 | -inf | MIN |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | -0 | -inf | MIN |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | +inf | nan | MAX |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | -inf | -inf | MIN |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | MAX | -inf | +0 |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | MIN | -inf | MIN |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | nan | nan | +0 |
| Such that src*scalarValue=MAX | MAX | MAX | +0 | MAX | MAX |
| Such that src*scalarValue=MAX | MAX | MAX | -0 | MAX | MAX |
| Such that src*scalarValue=MAX | MAX | MAX | +inf | +inf | MAX |
| Such that src*scalarValue=MAX | MAX | MAX | -inf | -inf | MIN |
| Such that src*scalarValue=MAX | MAX | MAX | MAX | MAX | MAX |
| Such that src*scalarValue=MAX | MAX | MAX | MIN | +0 | +0 |
| Such that src*scalarValue=MAX | MAX | MAX | nan | nan | +0 |
| Such that src*scalarValue=MIN | MIN | MIN | +0 | MIN | MIN |
| Such that src*scalarValue=MIN | MIN | MIN | -0 | MIN | MIN |
| Such that src*scalarValue=MIN | MIN | MIN | +inf | +inf | MAX |
| Such that src*scalarValue=MIN | MIN | MIN | -inf | -inf | MIN |
| Such that src*scalarValue=MIN | MIN | MIN | MAX | +0 | +0 |
| Such that src*scalarValue=MIN | MIN | MIN | MIN | -inf | MIN |
| Such that src*scalarValue=MIN | MIN | MIN | nan | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +0 | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -0 | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +inf | nan | MAX |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -inf | nan | MIN |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MAX | nan | MAX |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MIN | nan | MIN |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | nan | nan | +0 |

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 21** Computation result of MulAddDst with special value/boundary value input

<a name="table1042313583406"></a>
| Value of src0/Value of src1 | Intermediate Stage (src0*src1) | Intermediate Stage (Saturation Mode, Supported Only When the Output Is of the half Data Type) | Value of dst | dst Output (INF/NAN Mode) | dst Output (Saturation Mode, Supported Only When the Output Is of the half Data Type) |
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
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | +0 | +inf | MAX |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | -0 | +inf | MAX |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | +inf | +inf | MAX |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | -inf | nan | MIN |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | MAX | +inf | MAX |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | MIN | +inf | +0 |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN<br>MAX/MAX<br>MIN/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/+inf<br>+inf/MAX<br>-inf/MIN | +inf | MAX | nan | nan | +0 |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | +0 | -inf | MIN |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | -0 | -inf | MIN |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | +inf | nan | MAX |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | -inf | -inf | MIN |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | MAX | -inf | +0 |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | MIN | -inf | MIN |
| When the data type of the source operand is the same as that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX<br>MAX/MIN<br>When the data type of the source operand is different from that of the destination operand:<br>+inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | MIN | nan | nan | +0 |
| Such that src0*src1=MAX | MAX | MAX | +0 | MAX | MAX |
| Such that src0*src1=MAX | MAX | MAX | -0 | MAX | MAX |
| Such that src0*src1=MAX | MAX | MAX | +inf | +inf | MAX |
| Such that src0*src1=MAX | MAX | MAX | -inf | -inf | MIN |
| Such that src0*src1=MAX | MAX | MAX | MAX | MAX | MAX |
| Such that src0*src1=MAX | MAX | MAX | MIN | +0 | +0 |
| Such that src0*src1=MAX | MAX | MAX | nan | nan | +0 |
| Such that src0*src1=MIN | MIN | MIN | +0 | MIN | MIN |
| Such that src0*src1=MIN | MIN | MIN | -0 | MIN | MIN |
| Such that src0*src1=MIN | MIN | MIN | +inf | +inf | MAX |
| Such that src0*src1=MIN | MIN | MIN | -inf | -inf | MIN |
| Such that src0*src1=MIN | MIN | MIN | MAX | +0 | +0 |
| Such that src0*src1=MIN | MIN | MIN | MIN | -inf | MIN |
| Such that src0*src1=MIN | MIN | MIN | nan | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +0 | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -0 | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +inf | nan | MAX |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -inf | nan | MIN |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MAX | nan | MAX |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MIN | nan | MIN |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | nan | nan | +0 |

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 22** Computation results of FusedMulAdd with special value/boundary value input

<a name="table1158211814515"></a>
| Value of src0/Value of dst | Intermediate Stage (src0*dst) | Intermediate Stage (Saturation Mode, Supported Only When the Output Is of the half Data Type) | Value of src1 | dst Output (INF/NAN Mode) | dst Output (Saturation Mode, Supported Only When the Output Is of the half Data Type) |
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
| Such that src0*dst=MAX | MAX | MAX | +0 | MAX | MAX |
| Such that src0*dst=MAX | MAX | MAX | -0 | MAX | MAX |
| Such that src0*dst=MAX | MAX | MAX | +inf | +inf | MAX |
| Such that src0*dst=MAX | MAX | MAX | -inf | -inf | MIN |
| Such that src0*dst=MAX | MAX | MAX | MAX | MAX | MAX |
| Such that src0*dst=MAX | MAX | MAX | MIN | +0 | +0 |
| Such that src0*dst=MAX | MAX | MAX | nan | nan | +0 |
| Such that src0*dst=MIN | MIN | MIN | +0 | MIN | MIN |
| Such that src0*dst=MIN | MIN | MIN | -0 | MIN | MIN |
| Such that src0*dst=MIN | MIN | MIN | +inf | +inf | MAX |
| Such that src0*dst=MIN | MIN | MIN | -inf | -inf | MIN |
| Such that src0*dst=MIN | MIN | MIN | MAX | +0 | +0 |
| Such that src0*dst=MIN | MIN | MIN | MIN | -inf | MIN |
| Such that src0*dst=MIN | MIN | MIN | nan | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +0 | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -0 | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +inf | nan | MAX |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -inf | nan | MIN |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MAX | nan | MAX |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MIN | nan | MIN |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | nan | nan | +0 |

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 23** Computation results of MulAddRelu with special value/boundary value input

<a name="table126194714473"></a>
| Value of src0/Value of dst | Intermediate Stage 1 (src0*dst) | Intermediate Stage 1 (Saturation Mode, Supported Only When the Output Is of the half Data Type) | Value of src1 | Intermediate Stage 2 (INF/NAN Mode) | Intermediate Stage 2 (Saturation Mode, Supported Only When the Output Is of the half Data Type) | dst Output (INF/NAN Mode) | dst Output (Saturation Mode, Supported Only When the Output Is of the half Data Type) |
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
| Such that src0*dst=MAX | MAX | MAX | +0 | MAX | MAX | MAX | MAX |
| Such that src0*dst=MAX | MAX | MAX | -0 | MAX | MAX | MAX | MAX |
| Such that src0*dst=MAX | MAX | MAX | +inf | +inf | MAX | +inf | MAX |
| Such that src0*dst=MAX | MAX | MAX | -inf | -inf | MIN | +0 | +0 |
| Such that src0*dst=MAX | MAX | MAX | MAX | MAX | MAX | MAX | MAX |
| Such that src0*dst=MAX | MAX | MAX | MIN | +0 | +0 | +0 | +0 |
| Such that src0*dst=MAX | MAX | MAX | nan | nan | +0 | nan | +0 |
| Such that src0*dst=MIN | MIN | MIN | +0 | MIN | MIN | +0 | +0 |
| Such that src0*dst=MIN | MIN | MIN | -0 | MIN | MIN | +0 | +0 |
| Such that src0*dst=MIN | MIN | MIN | +inf | +inf | MAX | +inf | MAX |
| Such that src0*dst=MIN | MIN | MIN | -inf | -inf | MIN | +0 | +0 |
| Such that src0*dst=MIN | MIN | MIN | MAX | +0 | +0 | +0 | +0 |
| Such that src0*dst=MIN | MIN | MIN | MIN | -inf | MIN | +0 | +0 |
| Such that src0*dst=MIN | MIN | MIN | nan | nan | +0 | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +0 | nan | +0 | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -0 | nan | +0 | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | +inf | nan | MAX | nan | MAX |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | -inf | nan | MIN | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MAX | nan | MAX | nan | MAX |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | MIN | nan | MIN | nan | +0 |
| nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | nan | nan | +0 | nan | +0 |

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 24** Computation results of AddRelu with special value/boundary value input

<a name="table143801137164310"></a>
| Value of src0/Value of src1 | Intermediate Stage (src0+src1) | Intermediate Stage (Saturation Mode, Supported Only When the Output Is of the half Data Type) | dst Output (INF/NAN Mode) | dst Output (Saturation Mode, Supported Only When the Output Is of the half Data Type) |
| --- | --- | --- | --- | --- |
| +0/+0<br>+0/-0 | +0 | +0 | +0 | +0 |
| -0/-0 | -0 | -0 | +0 | +0 |
| +inf/+inf<br>+inf/MAX<br>+inf/MIN<br>+0/+inf<br>-0/+inf<br>MAX/MAX | +inf | MAX | +inf | MAX |
| -inf/-inf<br>-inf/MAX<br>-inf/MIN<br>+0/-inf<br>-0/+inf<br>MIN/MIN | -inf | MIN | +0 | +0 |
| Such that src0+src1=MAX | MAX | MAX | MAX | MAX |
| Such that src0+src1=MIN | MIN | MIN | +0 | +0 |
| nan/arbitrary value<br>+inf/-inf | nan | +0 | nan | +0 |

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 25**  Computation result of SubRelu with special value/boundary value input

<a name="table7783335111811"></a>
| Value of src0/Value of src1 | Intermediate Stage (src0-src1) | Intermediate Stage (Saturation Mode, Supported Only When the Output Is of the half Data Type) | dst Output (INF/NAN Mode) | dst Output (Saturation Mode, Supported Only When the Output Is of the half Data Type) |
| --- | --- | --- | --- | --- |
| +0/+0<br>+0/-0 | +0 | +0 | +0 | +0 |
| -0/+0 | -0 | -0 | +0 | +0 |
| +inf/-inf<br>+inf/MAX<br>+inf/MIN<br>+0/-inf<br>-0/-inf<br>MAX/MIN<br>MAX/-inf<br>MIN/-inf | +inf | MAX | +inf | MAX |
| -inf/-inf<br>-inf/MAX<br>-inf/MIN<br>+0/-inf<br>-0/+inf<br>MIN/MIN | -inf | MIN | +0 | +0 |
| Such that src0-src1=MAX | MAX | MAX | MAX | MAX |
| Such that src0-src1=MIN | MIN | MIN | +0 | +0 |
| nan/arbitrary value<br>+inf/+inf<br>-inf/-inf | nan | +0 | nan | +0 |

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 26**  Computation result of AddReluCast with special value/boundary value input

<a name="table494414452367"></a>
| Data Type of src0/Data Type of src1/Data Type of dst | Value of src0/Value of src1 | Intermediate Stage 1 (src0+src1) (INF/NAN Mode) | Intermediate Stage 2 (INF/NAN Mode) | dst Output (INF/NAN Mode) |
| --- | --- | --- | --- | --- |
| float/float/half | +0/+0<br>+0/-0 | +0 | +0 | +0 |
| float/float/half | -0/-0 | -0 | +0 | +0 |
| float/float/half | +inf/+inf<br>+inf/MAX<br>+inf/MIN<br>+0/+inf<br>-0/+inf<br>MAX/MAX | +inf | +inf | +inf |
| float/float/half | -inf/-inf<br>-inf/MAX<br>-inf/MIN<br>+0/-inf<br>-0/+inf<br>MIN/MIN | -inf | +0 | +0 |
| float/float/half | Such that src0+src1=MAX | MAX | MAX | MAX |
| float/float/half | Such that src0+src1=MIN | MIN | +0 | +0 |
| float/float/half | nan/arbitrary value<br>+inf/-inf | nan | nan | +0 |
| half/half/int8_t | +0/+0<br>+0/-0 | +0 | +0 | 0 |
| half/half/int8_t | -0/-0 | -0 | +0 | 0 |
| half/half/int8_t | +inf/+inf<br>+inf/MAX<br>+inf/MIN<br>+0/+inf<br>-0/+inf<br>MAX/MAX | +inf | +inf | MAX |
| half/half/int8_t | -inf/-inf<br>-inf/MAX<br>-inf/MIN<br>+0/-inf<br>-0/+inf<br>MIN/MIN | -inf | +0 | 0 |
| half/half/int8_t | Such that src0+src1=MAX | MAX | MAX | MAX |
| half/half/int8_t | Such that src0+src1=MIN | MIN | +0 | 0 |
| half/half/int8_t | nan/arbitrary value<br>+inf/-inf | nan | nan | 0 |
| int16_t/int16_t/int8_t | 0/0 | 0 | 0 | 0 |
| int16_t/int16_t/int8_t | MAX/MAX | MAX | MAX | MAX |
| int16_t/int16_t/int8_t | MIN/MIN | MIN | 0 | 0 |

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 27** Computation result of SubReluCast with special value/boundary value input

<a name="table1418126172917"></a>
| Data Type of src0/Data Type of src1/Data Type of dst | Value of src0/Value of src1 | Intermediate Stage 1 (src0-src1) (INF/NAN Mode) | Intermediate Stage 2 (INF/NAN Mode) | dst Output (INF/NAN Mode) |
| --- | --- | --- | --- | --- |
| float/float/half | +0/+0<br>+0/-0<br>-0/-0 | +0 | +0 | +0 |
| float/float/half | -0/+0 | -0 | +0 | +0 |
| float/float/half | +inf/-inf<br>+inf/MAX<br>+inf/MIN<br>+0/-inf<br>-0/-inf<br>MAX/MIN | +inf | +inf | +inf |
| float/float/half | -inf/+0<br>-inf/+0<br>-inf/MAX<br>-inf/MIN<br>+0/+inf<br>-0/+inf<br>MIN/MAX | -inf | +0 | +0 |
| float/float/half | Such that src0-src1=MAX | MAX | MAX | MAX |
| float/float/half | Such that src0-src1=MIN | MIN | +0 | +0 |
| float/float/half | nan/arbitrary value<br>+inf/+inf<br>-inf/-inf | nan | nan | +0 |
| half/half/int8_t | +0/+0<br>+0/-0<br>-0/-0 | +0 | +0 | 0 |
| half/half/int8_t | -0/-0 | -0 | +0 | 0 |
| half/half/int8_t | +inf/-inf<br>+inf/MAX<br>+inf/MIN<br>+inf/0<br>+inf/-0<br>+0/-inf<br>-0/-inf<br>MAX/MIN | +inf | +inf | MAX |
| half/half/int8_t | -inf/-inf<br>-inf/MAX<br>-inf/MIN<br>+0/-inf<br>-0/+inf<br>MIN/MIN | -inf | +0 | 0 |
| half/half/int8_t | Such that src0-src1=MAX | MAX | MAX | MAX |
| half/half/int8_t | Such that src0-src1=MIN | MIN | +0 | 0 |
| half/half/int8_t | nan/arbitrary value<br>+inf/+inf<br>-inf/-inf | nan | nan | 0 |
| int16_t/int16_t/int8_t | 0/0 | 0 | 0 | 0 |
| int16_t/int16_t/int8_t | MAX/MIN | MAX | MAX | MAX |
| int16_t/int16_t/int8_t | MIN/MAX | MIN | 0 | 0 |

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 28** Computation result of MulCast with special value/boundary value input

<a name="table1698216295159"></a>
| Value of src0/Value of src1 | Intermediate Stage (src0*src1) | Intermediate Stage (Saturation Mode, Supported Only When the Output Is of the half Data Type) | dst Output (INF/NAN mode) | dst Output (Saturation Mode, Supported Only When the Output Is of the half Data Type) |
| --- | --- | --- | --- | --- |
| +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0 | +0 | 0 | 0 |
| +0/-0<br>+0/MIN<br>-0/MAX | -0 | -0 | 0 | 0 |
| MAX/MAX<br>+inf/MAX<br>-inf/MIN | +inf | MAX | MAX | MAX |
| MAX/MIN<br>+inf/MIN<br>-inf/MAX | -inf | MIN | MIN | MIN |
| Such that src0*src1=MAX | MAX | MAX | MAX | MAX |
| Such that src0*src1=MIN | MIN | MIN | MIN | MIN |
| nan/arbitrary number<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | +0 | 0 | 0 |

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 29** Computation result of CastDequant with special value/boundary value input (MAX/MIN correspond to the maximum and minimum values of the data type of the corresponding operand, scale=1, offset=0)

<a name="table18277108174810"></a>
| Data Type of src/Data Type of dst | Value of src | dst Output (INF/NAN Mode) | dst Output (Saturation Mode, Supported Only When the Output Is of the half Data Type) |
| --- | --- | --- | --- |
| int32_t/half | 0 | 0 | 0 |
| int32_t/half | 65504 | MAX | MAX |
| int32_t/half | -65504 | MIN | MIN |
| int32_t/half | MAX | +inf | MAX |
| int32_t/half | MIN | -inf | MIN |
| int16_t/int8_t | 0 | 0 | / |
| int16_t/int8_t | MAX | MAX | / |
| int16_t/int8_t | MIN | MIN | / |

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE 754 theoretical result.

## Comparison and Selection<a name="section829519484517"></a>

Note:
- In the following table, MAX/MIN correspond to the maximum and minimum values of the data type of the corresponding operand.
- GetCmpMask is a data movement operation that sets the comparison mask register and does not involve special values or boundary values.

**Table 30** Computation result of Compare with special value/boundary value input

<a name="table1961220798"></a>
| Data Type of src/Data Type of src1 | cmpMode Comparison Mode | Data Type of src0 | Data Type of src1 | dst Output Data (INF/NAN Mode) | dst Output Data (Saturation Mode) |
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
| half/half | CMPMODE::GT | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | /0/1/1/0/0/1 |
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
    -   When src0=nan and src1=MIN/MAX/+0/-0/nan/+inf/-inf, the actual result is that dst output data (INF/NAN mode and saturation mode) is all 0/0/0/0/0/0/0, and the IEEE754 theoretical result is 1/1/1/1/1/1/1.
    -   When src0=MIN/MAX/+0/-0/nan/+inf/-inf and src1=nan, the actual dst output data is 0/0/0/0/0/0/0 in both INF/NAN mode and saturation mode, whereas the theoretical result per IEEE 754 should be 1/1/1/1/1/1/1.

-   In other modes, the dst output data (INF/NAN mode) and dst output data (saturation mode) results are consistent with the IEEE754 theoretical results.

**Table 31** Computation results of special value/boundary value input for Compares

<a name="table1126411512176"></a>
| Data Type of src/Data Type of src1 | cmpMode Comparison Mode | Data Type of src0 | scalar Data Type | dst Output Data (INF/NAN Mode) | dst Output Data (Saturation Mode) |
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
| half/half | CMPMODE::GT | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | /0/1/1/0/0/1 |
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
    -   When src0=nan and scalar=MIN/MAX/+0/-0/nan/+inf/-inf, the actual result is that the dst output data (in INF/NAN mode and saturation mode) is all 0/0/0/0/0/0/0, whereas the IEEE 754 theoretical result is 1/1/1/1/1/1/1.
    -   When src0=MIN/MAX/+0/-0/nan/+inf/-inf and scalar=nan, the actual result is that the dst output data (in INF/NAN mode and saturation mode) is all 0/0/0/0/0/0/0, whereas the IEEE 754 theoretical result is 1/1/1/1/1/1/1.

-   In other modes, the dst output data (INF/NAN mode) and dst output data (saturation mode) results are consistent with the IEEE754 theoretical results.

## Type Conversion<a name="section3558104315715"></a>

Note: In the following table, MAX/MIN correspond to the maximum and minimum values of the corresponding data type of the operand.

**Table 32** Computation results of Cast special value/boundary value input

<a name="table1225292414418"></a>
| Data Type of src/Data Type of dst | src Input Data | dst Output Data (INF/NAN Mode) | dst Output Data (Saturation Mode, Valid Only When the Output Data Type Is Half or bfloat16) |
| --- | --- | --- | --- |
| int4b_t/half | MIN | -8 | -8 |
| int4b_t/half | MAX | 7 | 7 |
| uint8_t/half | 0 | 0 | 0 |
| uint8_t/half | MAX | 255 | 255 |
| int8_t/half | MAX | 127 | 127 |
| int8_t/half | MIN | -128 | -128 |
| half/int4b_t | MAX | 7 | 7 |
| half/int4b_t | MIN | -8 | -8 |
| half/int4b_t | +0/-0 | 0 | 0 |
| half/int4b_t | nan | 0 | 0 |
| half/int4b_t | +inf | 7 | 7 |
| half/int4b_t | -inf | -8 | -8 |
| half/int8_t | MAX | 127 | 127 |
| half/int8_t | MIN | -128 | -128 |
| half/int8_t | +0/-0 | 0 | 0 |
| half/int8_t | nan | 0 | 0 |
| half/int8_t | +inf | 127 | 127 |
| half/int8_t | -inf | -128 | -128 |
| half/uint8_t | MAX | 255 | 255 |
| half/uint8_t | MIN | 0 | 0 |
| half/uint8_t | +0/-0 | 0 | 0 |
| half/uint8_t | nan | 0 | 0 |
| half/uint8_t | +inf | 255 | 255 |
| half/uint8_t | -inf | 0 | 0 |
| half/int16_t | MAX | 65504 | 65504 |
| half/int16_t | MIN | -65504 | -65504 |
| half/int16_t | +0/-0 | 0 | 0 |
| half/int16_t | nan | 0 | 0 |
| half/int16_t | +inf | 65504 | 65504 |
| half/int16_t | -inf | -65504 | -65504 |
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
| int16_t/half | MAX | 32768.00 | 32768.00 |
| int16_t/half | MIN | -32768.00 | -32768.00 |
| int16_t/half | 0 | +0 | +0 |
| int16_t/float | MAX | 32768.00 | 32768.00 |
| int16_t/float | MIN | -32768.00 | -32768.00 |
| int16_t/float | 0 | +0 | +0 |
| bfloat16_t/int32_t | 3.3895314e+38(MAX) | 2147483647 | 2147483647 |
| bfloat16_t/int32_t | -3.3895314e+38(MIN) | -2147483648 | -2147483648 |
| bfloat16_t/int32_t | 0 | 0 | 0 |
| bfloat16_t/int32_t | +inf | 2147483647 | 2147483647 |
| bfloat16_t/int32_t | -inf | -2147483648 | -2147483648 |
| bfloat16_t/int32_t | nan | 0 | 0 |
| bfloat16_t/float | 3.3895314e+38(MAX) | 3.3895314e+38 | 3.3895314e+38 |
| bfloat16_t/float | -3.3895314e+38(MIN) | -3.3895314e+38 | -3.3895314e+38 |
| bfloat16_t/float | 0 | 0 | 0 |
| bfloat16_t/float | +inf | +inf | +inf |
| bfloat16_t/float | -inf | -inf | -inf |
| bfloat16_t/float | nan | nan | nan |
| int32_t/int16_t | MAX | MAX | MAX |
| int32_t/int16_t | MIN | MIN | MIN |
| int32_t/int16_t | 0 | 0 | 0 |
| int32_t/half(refer to CastDequant) | / | / | / |
| int32_t/float | 2147483647(MAX) | 2147483520.00 | 2147483520.00 |
| int32_t/float | -2147483648(MIN) | -2147483648.00 | -2147483648.00 |
| int32_t/int64_t | 2147483647(MAX) | 2147483647 | 2147483647 |
| int32_t/int64_t | -2147483648(MIN) | -2147483648 | -2147483648 |
| float/int16_t | 3.4028235e+38(MAX) | 32767 | 32767 |
| float/int16_t | -3.4028235e+38(MIN) | -32768 | -32768 |
| float/int16_t | 0 | 0 | 0 |
| float/int16_t | +inf | 32767 | 32767 |
| float/int16_t | -inf | -32768 | -32768 |
| float/int16_t | nan | 0 | 0 |
| float/bfloat16_t | 3.4028235e+38(MAX) | +inf | MAX |
| float/bfloat16_t | -3.4028235e+38(MIN) | -inf | MIN |
| float/bfloat16_t | +0 | +0 | +0 |
| float/bfloat16_t | -0 | -0 | -0 |
| float/bfloat16_t | +inf | +inf | MAX |
| float/bfloat16_t | -inf | -inf | MIN |
| float/bfloat16_t | nan | nan | 0 |
| float/half | 3.4028235e+38(MAX) | +inf | MAX |
| float/half | -3.4028235e+38(MIN) | -inf | MIN |
| float/half | 0 | 0 | 0 |
| float/half | +inf | +inf | MAX |
| float/half | -inf | -inf | MIN |
| float/half | nan | nan | 0 |
| float/int32_t | 3.4028235e+38(MAX) | 2147483647 | 2147483647 |
| float/int32_t | -3.4028235e+38(MIN) | -2147483648 | -2147483648 |
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
| float/int64_t | 3.4028235e+38(MAX) | 9.22337E+18 | 9.22337E+18 |
| float/int64_t | -3.4028235e+38(MIN) | -9.22337E+18 | -9.22337E+18 |
| float/int64_t | 0 | 0 | 0 |
| float/int64_t | +inf | 9.22337E+18 | 9.22337E+18 |
| float/int64_t | -inf | -9.22337E+18 | -9.22337E+18 |
| float/int64_t | nan | 0 | 0 |
| int64_t/int32_t | MAX | 2147483647 | 2147483647 |
| int64_t/int32_t | MIN | -2147483648 | -2147483648 |
| int64_t/float | MAX | 9223372036854775808.00(MAX) | 9223372036854775808.00(MAX) |
| int64_t/float | MIN | -9223372036854775808.00(MIN) | -9223372036854775808.00(MIN) |
| int16_t/int8_t | MAX | 127 | 127 |
| int16_t/int8_t | MIN | -128 | -128 |
| int16_t/uint8_t | MAX | 255 | 255 |
| int16_t/uint8_t | MIN | 0 | 0 |

Note: In float->float conversion, -0 is converted to +0, which is inconsistent with IEEE754.

## Reduction Computation<a name="section156745111398"></a>

Note: In the following tables, MAX/MIN correspond to the maximum and minimum values of the data type of the corresponding operand.

**Table 33**  Computation results for special value/boundary value inputs of ReduceDataBlock<MAX\>

<a name="table2883162019243"></a>
| Data Type of src/dst | Element Value of src (1 DataBlock) | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | All are -65504.0 (MIN) | -65504.0 | -65504.0 |
| half | All are 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | All are 0.0 | 0.0 | 0.0 |
| half | All are -0.0 | -0.0 | -0.0 |
| half | All are nan | nan | 0.0 |
| half | All are inf | inf | 65504.0 |
| half | All are -inf | -inf | -65504.0 |
| half | inf, -inf, nan alternating | nan | 0.0 |
| half | inf, -inf alternating | inf | 65504.0 |
| half | inf, nan alternating | nan | 0.0 |
| half | -inf, nan alternating | nan | 0.0 |
| half | 15 elements of 1.0 plus 1 element of inf | inf | 65504.0 |
| half | 15 elements of 1.0 plus 1 element of -inf | 1.0 | 1.0 |
| half | 15 elements of 1.0 plus 1 element of nan | nan | 0.0 |
| float | All are -3.4028235e+38 (MIN) | -3.4028235e+38 | -3.4028235e+38 |
| float | All are 3.4028235e+38 (MAX) | 3.4028235e+38 | 3.4028235e+38 |
| float | All are 0.0 | 0.0 | 0.0 |
| float | All are -0.0 | -0.0 | -0.0 |
| float | All are nan | nan | nan |
| float | All are inf | inf | inf |
| float | All are -inf | -inf | -inf |
| float | inf, -inf, nan alternating | nan | nan |
| float | inf, -inf alternating | inf | inf |
| float | inf, nan alternating | nan | nan |
| float | -inf, nan alternating | nan | nan |
| float | 7 elements of 1.0 plus 1 element of inf | inf | inf |
| float | 7 elements of 1.0 plus 1 element of -inf | 1.0 | 1.0 |
| float | 7 elements of 1.0 plus 1 element of nan | nan | nan |

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 34**  ReduceDataBlock<MIN\> Computation result of special value/boundary value input

<a name="table6602183015217"></a>
| Data Type of src/dst | Element Value of src (1 DataBlock) | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | All are -65504.0 (MIN) | -65504.0 | -65504.0 |
| half | All are 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | All are 0.0 | 0.0 | 0.0 |
| half | All are -0.0 | -0.0 | -0.0 |
| half | All are nan | nan | 0.0 |
| half | All are inf | inf | 65504.0 |
| half | All are -inf | -inf | -65504.0 |
| half | inf, -inf, nan alternating | nan | 0.0 |
| half | inf, -inf alternating | -inf | -65504 |
| half | inf, nan alternating | nan | 0.0 |
| half | -inf, nan alternating | nan | 0.0 |
| half | 15 elements of 1.0 plus 1 element of inf | 1.0 | 1.0 |
| half | 15 elements of 1.0 plus 1 element of -inf | -inf | -65504.0 |
| half | 15 elements of 1.0 plus 1 element of nan | nan | 0.0 |
| float | All are -3.4028235e+38 (MIN) | -3.4028235e+38 | -3.4028235e+38 |
| float | All are 3.4028235e+38 (MAX) | 3.4028235e+38 | 3.4028235e+38 |
| float | All are 0.0 | 0.0 | 0.0 |
| float | All are -0.0 | -0.0 | -0.0 |
| float | All are nan | nan | nan |
| float | All are inf | inf | inf |
| float | All are -inf | -inf | -inf |
| float | inf, -inf, nan alternating | nan | nan |
| float | inf, -inf alternating | -inf | -inf |
| float | inf, nan alternating | nan | nan |
| float | -inf, nan alternating | nan | nan |
| float | 7 elements of 1.0 plus 1 element of inf | 1.0 | 1.0 |
| float | 7 elements of 1.0 plus 1 element of -inf | -inf | -inf |
| float | 7 elements of 1.0 plus 1 element of nan | nan | nan |

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 35**  ReduceDataBlock<SUM\> Computation result of special value/boundary value input

<a name="table47791758122218"></a>
| Data Type of src/dst | Element Value of src (1 DataBlock) | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | All -65504.0 (MIN) | -inf | -65504.0 |
| half | All 65504.0 (MAX) | inf | 65504.0 |
| half | All 0.0 | 0.0 | 0.0 |
| half | All -0.0 | -0.0 | -0.0 |
| half | All nan | nan | 0.0 |
| half | All inf | inf | 65504.0 |
| half | All -inf | -inf | -65504.0 |
| half | inf, -inf, nan alternating | nan | 0.0 |
| half | inf, -inf alternating | nan | 0.0 |
| half | inf, nan alternating | nan | 0.0 |
| half | -inf, nan alternating | nan | 0.0 |
| half | 15 elements of 1.0 plus 1 element of inf | inf | 65344.0 |
| half | 15 elements of 1.0 plus 1 element of -inf | -inf | -65344.0 |
| half | 15 elements of 1.0 plus 1 element of nan | nan | 14.0 |
| float | All -3.4028235e+38 (MIN) | -inf | -inf |
| float | All 3.4028235e+38 (MAX) | inf | inf |
| float | All 0.0 | 0.0 | 0.0 |
| float | All -0.0 | -0.0 | -0.0 |
| float | All nan | nan | nan |
| float | All inf | inf | inf |
| float | All -inf | -inf | -inf |
| float | inf, -inf, nan alternating | nan | nan |
| float | inf, -inf alternating | nan | nan |
| float | inf, nan alternating | nan | nan |
| float | -inf, nan alternating | nan | nan |
| float | 7 elements of 1.0 plus 1 element of inf | inf | inf |
| float | 7 elements of 1.0 plus 1 element of -inf | -inf | -inf |
| float | 7 elements of 1.0 plus 1 element of nan | nan | nan |

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 36**  ReduceRepeat<MAX\> Computation result of special value/boundary value input

<a name="table818992302819"></a>
| Data Type of src/dst | Element Value of src (8 DataBlocks) | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | All -65504.0 (MIN) | -65504.0 | -65504.0 |
| half | All 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | All 0.0 | 0.0 | 0.0 |
| half | All -0.0 | -0.0 | -0.0 |
| half | All nan | nan | 0.0 |
| half | All inf | inf | 65504.0 |
| half | All -inf | -inf | -65504.0 |
| half | inf, -inf, nan alternating | nan | 0.0 |
| half | inf, -inf alternating | inf | 65504.0 |
| half | inf, nan alternating | nan | 0.0 |
| half | -inf, nan alternating | nan | 0.0 |
| half | 127 elements of 1.0 plus 1 element of inf | inf | 65504.0 |
| half | 127 elements of 1.0 plus 1 element of -inf | 1 | 1.0 |
| half | 127 elements of 1.0 plus 1 element of nan | nan | 0.0 |
| float | All -3.4028235e+38 (MIN) | -3.4028235e+38 | -3.4028235e+38 |
| float | All 3.4028235e+38 (MAX) | 3.4028235e+38 | 3.4028235e+38 |
| float | All 0.0 | 0.0 | 0.0 |
| float | All -0.0 | -0.0 | -0.0 |
| float | All nan | nan | nan |
| float | All inf | inf | inf |
| float | All -inf | -inf | -inf |
| float | inf, -inf, nan alternating | nan | nan |
| float | inf, -inf alternating | inf | inf |
| float | inf, nan alternating | nan | nan |
| float | -inf, nan alternating | nan | nan |
| float | 63 elements of 1.0 plus 1 element of inf | inf | inf |
| float | 63 elements of 1.0 plus 1 element of -inf | 1 | 1 |
| float | 63 elements of 1.0 plus 1 element of nan | nan | nan |

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 37**  ReduceRepeat<MIN\> Computation result of special value/boundary value input

<a name="table11721236104119"></a>
| Data Type of src/dst | Element Value of src (8 DataBlocks) | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | All are -65504.0 (MIN) | -65504.0 | -65504.0 |
| half | All are 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | All are 0.0 | 0.0 | 0.0 |
| half | All are -0.0 | -0.0 | -0.0 |
| half | All are nan | nan | 0 |
| half | All are inf | inf | 65504 |
| half | All are -inf | -inf | -65504 |
| half | inf, -inf, nan alternating | nan | 0 |
| half | inf, -inf alternating | -inf | -65504 |
| half | inf, nan alternating | nan | 0 |
| half | -inf, nan alternating | nan | 0 |
| half | 127 elements of 1.0 plus 1 element of inf | 1 | 1 |
| half | 127 elements of 1.0 plus 1 element of -inf | -inf | -65504 |
| half | 127 elements of 1.0 plus 1 element of nan | nan | 0 |
| float | All are -3.4028235e+38 (MIN) | -3.4028235e+38 | -3.4028235e+38 |
| float | All are 3.4028235e+38 (MAX) | 3.4028235e+38 | 3.4028235e+38 |
| float | All are 0.0 | 0.0 | 0.0 |
| float | All are -0.0 | -0.0 | -0.0 |
| float | All are nan | nan | nan |
| float | All are inf | inf | inf |
| float | All are -inf | -inf | -inf |
| float | inf, -inf, nan alternating | nan | nan |
| float | inf, -inf alternating | -inf | -inf |
| float | inf, nan alternating | nan | nan |
| float | -inf, nan alternating | nan | nan |
| float | 63 elements of 1.0 plus 1 element of inf | 1.0 | 1.0 |
| float | 63 elements of 1.0 plus 1 element of -inf | -inf | -inf |
| float | 63 elements of 1.0 plus 1 element of nan | nan | nan |

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 38**  Computation results of special value/boundary value inputs for ReduceRepeat<SUM\>

<a name="table9281103820417"></a>
| Data Type of src/dst | Element Value of src (8 DataBlocks) | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | All are -65504.0 (MIN) | -inf | -65504.0 |
| half | All are 65504.0 (MAX) | inf | 65504.0 |
| half | All are 0.0 | 0.0 | 0.0 |
| half | All are -0.0 | -0.0 | -0.0 |
| half | All are nan | nan | 0.0 |
| half | All are inf | inf | 65504.0 |
| half | All are -inf | -inf | -65504.0 |
| half | inf, -inf, nan alternating | nan | 0.0 |
| half | inf, -inf alternating | nan | 0.0 |
| half | inf, nan alternating | nan | 0.0 |
| half | -inf, nan alternating | nan | 0.0 |
| half | 127 elements of 1.0 plus 1 element of inf | inf | 65504.0 |
| half | 127 elements of 1.0 plus 1 element of -inf | -inf | -65376.0 |
| half | 127 elements of 1.0 plus 1 element of nan | nan | 126.0 |
| float | All are -3.4028235e+38 (MIN) | -inf | -inf |
| float | All are 3.4028235e+38 (MAX) | inf | inf |
| float | All are 0.0 | 0.0 | 0.0 |
| float | All are -0.0 | -0.0 | -0.0 |
| float | All are nan | nan | nan |
| float | All are inf | inf | inf |
| float | All are -inf | -inf | -inf |
| float | inf, -inf, nan alternating | nan | nan |
| float | inf, -inf alternating | nan | nan |
| float | inf, nan alternating | nan | nan |
| float | -inf, nan alternating | nan | nan |
| float | 63 elements of 1.0 plus 1 element of inf | inf | inf |
| float | 63 elements of 1.0 plus 1 element of -inf | -inf | -inf |
| float | 63 elements of 1.0 plus 1 element of nan | nan | nan |

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 39**  Computation result of ReduceMax special value/boundary value input

<a name="table491948105210"></a>
| Data Type of src/dst | Element Value of src (8 DataBlocks) | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | All are -65504.0 (MIN) | -65504.0 | -65504.0 |
| half | All are 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | All are 0.0 | 0.0 | 0.0 |
| half | All are -0.0 | -0.0 | -0.0 |
| half | All are nan | nan | 0.0 |
| half | All are inf | inf | 65504.0 |
| half | All are -inf | -inf | -65504.0 |
| half | inf, -inf, nan plus 253 elements of 1.0 | nan | 1.0 |
| half | inf, -inf plus 254 elements of 1.0 | inf | 65504.0 |
| half | inf, nan plus 254 elements of 1.0 | nan | 1.0 |
| half | -inf, nan plus 254 elements of 1.0 | nan | 1.0 |
| half | 255 elements of 1.0 plus 1 element of inf | inf | 65504.0 |
| half | 255 elements of 1.0 plus 1 element of -inf | 1.0 | 1.0 |
| half | 255 elements of 1.0 plus 1 element of nan | nan | 1.0 |
| float | All are -3.4028235e+38 (MIN) | -3.4028235e+38 | -3.4028235e+38 |
| float | All are 3.4028235e+38 (MAX) | 3.4028235e+38 | 3.4028235e+38 |
| float | All are 0.0 | 0.0 | 0.0 |
| float | All are -0.0 | -0.0 | -0.0 |
| float | All are nan | nan | nan |
| float | All are inf | inf | inf |
| float | All are -inf | -inf | -inf |
| float | inf, -inf, nan alternating | nan | nan |
| float | inf, -inf alternating | inf | inf |
| float | inf, nan alternating | nan | nan |
| float | -inf, nan alternating | nan | nan |
| float | 63 elements of 1.0 plus 1 element of inf | inf | inf |
| float | 63 elements of 1.0 plus 1 element of -inf | 1.0 | 1.0 |
| float | 63 elements of 1.0 plus 1 element of nan | nan | nan |

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 40** Computation result of ReduceMin with special value/boundary value input

<a name="table144621256001"></a>
| Data Type of src/dst | Element Value of src (8 DataBlocks) | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | All are -65504.0 (MIN) | -65504.0 | -65504.0 |
| half | All are 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | All are 0.0 | 0.0 | 0.0 |
| half | All are -0.0 | -0.0 | -0.0 |
| half | All are nan | nan | 0.0 |
| half | All are inf | inf | 65504.0 |
| half | All are -inf | -inf | -65504.0 |
| half | inf, -inf, nan plus 253 elements of 1.0 | nan | 0.0 |
| half | inf, -inf plus 254 elements of 1.0 | -inf | -65504.0 |
| half | inf, nan plus 254 elements of 1.0 | nan | 0.0 |
| half | -inf, nan plus 254 elements of 1.0 | nan | 0.0 |
| half | 255 elements of 1.0 plus 1 element of inf | 1.0 | 1.0 |
| half | 255 elements of 1.0 plus 1 element of -inf | -inf | -65504.0 |
| half | 255 elements of 1.0 plus 1 element of nan | nan | 0.0 |
| float | All are -3.4028235e+38 (MIN) | -3.4028235e+38 | -3.4028235e+38 |
| float | All are 3.4028235e+38 (MAX) | 3.4028235e+38 | 3.4028235e+38 |
| float | All are 0.0 | 0.0 | 0.0 |
| float | All are -0.0 | -0.0 | -0.0 |
| float | All are nan | nan | nan |
| float | All are inf | inf | inf |
| float | All are -inf | -inf | -inf |
| float | inf, -inf, nan alternating | nan | nan |
| float | inf, -inf alternating | -inf | -inf |
| float | inf, nan alternating | nan | nan |
| float | -inf, nan alternating | nan | nan |
| float | 63 elements of 1.0 plus 1 element of inf | 1.0 | 1.0 |
| float | 63 elements of 1.0 plus 1 element of -inf | -inf | -inf |
| float | 63 elements of 1.0 plus 1 element of nan | nan | nan |

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

**Table 41** Computation result of ReduceSum with special value/boundary value input

<a name="table16600131416"></a>
| Data Type of src/dst | Element Value of src (8 DataBlocks) | Element Value of dst (INF/NAN Mode) | Element Value of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | All are -65504.0 (MIN) | -inf | -65504.0 |
| half | All are 65504.0 (MAX) | inf | 65504.0 |
| half | All are 0.0 | 0.0 | 0.0 |
| half | All are -0.0 | -0.0 | -0.0 |
| half | All are nan | nan | 0.0 |
| half | All are inf | inf | 65504.0 |
| half | All are -inf | -inf | -65504.0 |
| half | inf, -inf, nan plus 253 elements of 1.0 | nan | 252.0 |
| half | inf, -inf plus 254 elements of 1.0 | nan | 254.0 |
| half | inf, nan plus 254 elements of 1.0 | nan | 254.0 |
| half | -inf, nan plus 254 elements of 1.0 | nan | 254.0 |
| half | 255 elements of 1.0 plus 1 element of inf | inf | 65504.0 |
| half | 255 elements of 1.0 plus 1 element of -inf | -inf | -65248.0 |
| half | 255 elements of 1.0 plus 1 element of nan | nan | 254.0 |
| float | All are -3.4028235e+38 (MIN) | -inf | -inf |
| float | All are 3.4028235e+38 (MAX) | inf | inf |
| float | All are 0.0 | 0.0 | 0.0 |
| float | All are -0.0 | -0.0 | -0.0 |
| float | All are nan | nan | nan |
| float | All are inf | inf | inf |
| float | All are -inf | -inf | -inf |
| float | inf, -inf, nan alternating | nan | nan |
| float | inf, -inf alternating | nan | nan |
| float | inf, nan alternating | nan | nan |
| float | -inf, nan alternating | nan | nan |
| float | 63 elements of 1.0 plus 1 element of inf | inf | inf |
| float | 63 elements of 1.0 plus 1 element of -inf | -inf | -inf |
| float | 63 elements of 1.0 plus 1 element of nan | nan | nan |

Note: The result in the Element Value of dst (INF/NAN Mode) column is consistent with the IEEE754 theoretical result.

## Data Layout Conversion<a name="section33343814910"></a>

**Transpose** and **TransDataTo5HD** are data movement APIs, and special values/boundary values are not involved.

## Data Filling<a name="section10713185619915"></a>

**Brcb** and **Duplicate** are filling-type APIs, not involving special values/boundary values.

The computation result of special value/boundary value input of **CreateVecIndex** is the same as that of the **Adds** API.

## Sorting Combination<a name="section1342519199109"></a>

**Sort32**, **MrgSort**, and **GetMrgSortResult** do not involve special values/boundary values.

## Scatter and Gather<a name="section14495123581018"></a>

**Gather** and **Gatherb** are data movement APIs and do not involve special values or boundary values.

## Mask Operation<a name="section0460049181018"></a>

**SetMaskCount**, **SetMaskNorm**, **SetVectorMask**, and **ResetMask** are mask configuration APIs, and special values/boundary values are not involved.

