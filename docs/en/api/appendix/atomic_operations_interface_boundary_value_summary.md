# Summary of Atomic Operation Boundary Values<a name="ZH-CN_TOPIC_0000002608310699"></a>

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-17T03:08:11.510Z pushedAt=2026-11-17T09:52:41.739Z nl=2 -->

## Overview<a name="section3674181114910"></a>

This section summarizes the two types of output results of atomic operation APIs under boundary value input. The following content applies to the following models:

<!-- npu="A3" id2 -->
- Atlas A3 products
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products
<!-- end id3 -->

Under boundary value input, atomic operation APIs produce two types of output results:

- INF/NAN mode (non-saturation mode)

    For the three scenarios where the computation result is inf, -inf, or nan, the result is output as is.

- Saturation mode

    This mode is set through the [SetSaturationFlag](../SIMD-API/basic_api/special_register_access/SetSaturationFlag_ISASI.md) API. In saturation mode, inf and -inf are saturated to MAX and MIN for output, and nan is saturated to 0 for output.

## Specific Calculation Results of Atomic Operation APIs Under Boundary Value Input<a name="section8282511541"></a>

Note: In the following table, MAX/MIN correspond to the maximum and minimum values under the data type of the corresponding operand.

**Table 1**  Calculation results of SetAtomicAdd under special value/boundary value input

<a name="table15675934145714"></a>

| data type of src/dst | element values of src0 | element values of src1 | element values of dst<br> (INF/NAN mode) | element values of dst<br> (saturation mode) |
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

Note: The element values of dst (INF/NAN mode) column result are consistent with the IEEE 754 theoretical result.

**Table 2**  Calculation results of SetAtomicMax under special value/boundary value input

<a name="table57228341577"></a>

| data type of src/dst | element values of src0 | element values of src1 | element values of dst<br> (INF/NAN mode) | element values of dst<br> (saturation mode) |
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

Note: The element values of dst (INF/NAN mode) column result are consistent with the IEEE 754 theoretical result.

**Table 3** Calculation results of SetAtomicMin for special value/boundary value inputs

<a name="table673793455714"></a>

| data type of src/dst | element values of src0 | element values of src1 | element values of dst (register non-saturation mode) | element values of dst (register saturation mode) |
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

Note: The element values of dst (INF/NAN mode) column result are consistent with the IEEE 754 theoretical result.