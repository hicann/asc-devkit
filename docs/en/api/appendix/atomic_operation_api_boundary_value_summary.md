# Atomic Operation Boundary Value Summary<a name="ZH-CN_TOPIC_0000002608310699"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-09-06T08:36:08.785Z pushedAt=2026-09-07T03:40:09.491Z -->

## Overview<a name="section3674181114910"></a>

This section summarizes the two types of output results of atomic operation APIs under boundary value inputs. The following content applies to the following models:

<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products
<!-- end id3 -->

Under boundary value inputs, atomic operation APIs produce two types of output results:

- INF/NAN mode (non-saturation mode)

    For the three scenarios where the calculation result is inf, -inf, or nan, the result is output as is.

- Saturation mode

    Set through the [SetSaturationFlag](../SIMD-API/basic_api/special_register_access/SetSaturationFlag_ISASI.md) API. In saturation mode, inf and -inf are saturated to MAX and MIN for output, and nan is saturated to 0 for output.

## Specific Calculation Results of Atomic Operation API for Boundary Value Inputs<a name="section8282511541"></a>

Note: In the following tables, MAX/MIN correspond to the maximum value and minimum value of the operand's data type.

**Table 1** Calculation results of SetAtomicAdd for special value/boundary value inputs

<a name="table15675934145714"></a>
| Data Type of src/dst | Element Value of src0 | Element Value of src1 | Element Value of dst<br>(INF/NAN Mode) | Element Value of dst<br>(Saturation Mode) |
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

Note: The results in the element value of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical results.

**Table 2** Calculation results of SetAtomicMax for special value/boundary value inputs

<a name="table57228341577"></a>
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

Note: The results in the element value of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical results.

**Table 3** Calculation results of SetAtomicMin with special value/boundary value inputs

<a name="table673793455714"></a>
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

Note: The results in the element value of dst (INF/NAN mode) column are consistent with the IEEE 754 theoretical results.

