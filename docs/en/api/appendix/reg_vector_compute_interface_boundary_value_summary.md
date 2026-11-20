# Reg Vector Computation API Boundary Value Summary<a name="ZH-CN_TOPIC_0000002565891916"></a>

<!-- md-trans-meta sourceCommit=c51e14d6a6af298868c2a79da34632849f9ac5a9 translatedAt=2026-11-17T03:43:39.492Z pushedAt=2026-11-17T09:52:41.783Z nl=2 -->

## Overview<a name="section3674181114910"></a>

This section summarizes the two types of output results of the Reg vector computation API under boundary value input. The following content applies to the following models:

- Ascend 950PR&950DT products:

Under boundary value input, the Reg vector computation API has two types of output results:

- INF/NAN mode (non-saturation mode)

    For the three scenarios where the computation result is inf, -inf, or nan, the result is output as is. When the output is an integer, a finite value that exceeds the representable range of the target type is truncated to the bit width of the target type (low-order bits are retained).

- Saturation mode

    Set through the [SetSaturationFlag](../SIMD-API/basic_api/special_register_access/SetSaturationFlag_ISASI.md) API. In saturation mode, inf and -inf are saturated to MAX and MIN for output, and nan is saturated to 0 for output. When the output is an integer, a finite value that exceeds the representable range of the target type is saturated to the maximum or minimum value of the target data type.

> [!NOTE] Note
>
>- Saturation mode is configured by data type. For the data types affected by each saturation mode (`SaturationMode::FLOAT`, `SaturationMode::INT`, `SaturationMode::CAST`), see [SetSaturationFlag](../SIMD-API/basic_api/special_register_access/SetSaturationFlag_ISASI.md#data-types). In the table, the "INF/NAN mode" column shows the results when the corresponding saturation mode is not enabled, and the "saturation mode" column shows the results when it is enabled.
>
>- **In most scenarios, the output results in INF/NAN mode are consistent with the IEEE 754 standard results. If there are inconsistencies, they are noted at the corresponding API.**

## Basic Arithmetic

Note: In the following table, MAX/MIN correspond to the maximum and minimum values under the data type of the corresponding operand.

**Table 1** Result description of Exp special value/boundary value input calculation

<a name="table1088820617487"></a>

| Data Type of src/dst | Element Values of src | Element Values of dst (INF/NAN Mode) | Element Values of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | -65504.0 (MIN) | 0.0 | 0.0 |
| half | 65504.0 (MAX) | inf | 65504.0 |
| half | 0.0 | 1.0 | 1.0 |
| half | -0.0 | 1.0 | 1.0 |
| half | nan | nan | 0.0 |
| half | inf | inf | 65504.0 |
| half | -inf | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | inf | inf |
| float | 0.0 | 1.0 | 1.0 |
| float | -0.0 | 1.0 | 1.0 |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | 0.0 | 0.0 |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 2** Result description of Ln special value/boundary value input calculation

<a name="table10972164142410"></a>

| Data Type of src/dst | Element Values of src | Element Values of dst (INF/NAN Mode) | Element Values of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | -65504.0 (MIN) | nan | 0.0 |
| half | 65504.0 (MAX) | 11.094 | 11.094 |
| half | 0.0 | -inf | -65504.0 |
| half | -0.0 | -inf | -65504.0 |
| half | nan | nan | 0.0 |
| half | inf | inf | 65504.0 |
| half | -inf | nan | 0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan |
| float | 3.4028235e+38 (MAX) | 88.723 | 88.723 |
| float | 0.0 | -inf | -inf |
| float | -0.0 | -inf | -inf |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | nan | nan |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 3** Result description of Log special value/boundary value input calculation

<a name="table58051753113819"></a>

| Data Type of src/dst | Element Values of src | Element Values of dst (INF/NAN Mode) | Element Values of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | -65504.0 (MIN) | nan | 0.0 |
| half | 65504.0 (MAX) | 11.094 | 11.094 |
| half | 0.0 | -inf | -65504.0 |
| half | -0.0 | -inf | -65504.0 |
| half | nan | nan | 0.0 |
| half | inf | inf | 65504.0 |
| half | -inf | nan | 0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan |
| float | 3.4028235e+38 (MAX) | 88.723 | 88.723 |
| float | 0.0 | -inf | -inf |
| float | -0.0 | -inf | -inf |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | nan | nan |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 4** Result description of Log2 special value/boundary value input calculation

<a name="table1234181619537"></a>

| Data Type of src/dst | Element Values of src | Element Values of dst (INF/NAN Mode) | Element Values of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | -65504.0 (MIN) | nan | 0.0 |
| half | 65504.0 (MAX) | 16 | 16 |
| half | 0.0 | -inf | -65504.0 |
| half | -0.0 | -inf | -65504.0 |
| half | nan | nan | 0.0 |
| half | inf | inf | 65504.0 |
| half | -inf | nan | 0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan |
| float | 3.4028235e+38 (MAX) | 128 | 128 |
| float | 0.0 | -inf | -inf |
| float | -0.0 | -inf | -inf |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | nan | nan |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 5** Result description of Log10 special value/boundary value input calculation

<a name="table546135512543"></a>

| Data Type of src/dst | Element Values of src | Element Values of dst (INF/NAN Mode) | Element Values of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | -65504.0 (MIN) | nan | 0.0 |
| half | 65504.0 (MAX) | 4.8164 | 4.8164 |
| half | 0.0 | -inf | -65504.0 |
| half | -0.0 | -inf | -65504.0 |
| half | nan | nan | 0.0 |
| half | inf | inf | 65504.0 |
| half | -inf | nan | 0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan |
| float | 3.4028235e+38 (MAX) | 38.5318 | 38.53 |
| float | 0.0 | -inf | -inf |
| float | -0.0 | -inf | -inf |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | nan | nan |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 6** Result description of Neg special value/boundary value input calculation

<a name="table81271921555"></a>

| Data Type of src/dst | Element Values of src | Element Values of dst (INF/NAN Mode) | Element Values of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | -65504.0 (MIN) | 65504.0 (MAX) | 65504.0 (MAX) |
| half | 65504.0 (MAX) | -65504.0 (MIN) | -65504.0 (MIN) |
| half | 0.0 | -0.0 | -0.0 |
| half | -0.0 | 0.0 | 0.0 |
| half | nan | nan | 0.0 |
| half | inf | -inf | -65504.0 (MIN) |
| half | -inf | inf | 65504.0 (MAX) |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX) |
| float | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN) |
| float | 0.0 | -0.0 | -0.0 |
| float | -0.0 | 0.0 | 0.0 |
| float | nan | nan | nan |
| float | inf | -inf | -inf |
| float | -inf | inf | inf |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 7** Description of calculation results for Abs special value/boundary value inputs

<a name="table1975185932512"></a>

| Data Type of src/dst | Element Values of src | Element Values of dst (INF/NAN Mode) | Element Values of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | -65504.0 (MIN) | 65504.0 | 65504.0 |
| half | 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | 0.0 | 0.0 | 0.0 |
| half | -0.0 | 0.0 | 0.0 |
| half | nan | nan | 0.0 |
| half | inf | inf | 65504.0 |
| half | -inf | inf | 65504.0 |
| float | -3.4028235e+38 (MIN) | 3.40282E+38 | 3.40282E+38 |
| float | 3.4028235e+38 (MAX) | 3.40282E+38 | 3.40282E+38 |
| float | 0.0 | 0.0 | 0.0 |
| float | -0.0 | 0.0 | 0.0 |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | inf | inf |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 8** Sqrt special value/boundary value input calculation result description

<a name="table1648943062718"></a>

| Data Type of src/dst | Element Values of src | Element Values of dst (INF/NAN Mode) | Element Values of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | -65504.0 (MIN) | nan | 0.0 |
| half | 65504.0 (MAX) | 256.0 | 256.0 |
| half | 0.0 | 0.0 | 0.0 |
| half | -0.0 | -0.0 | -0.0 |
| half | nan | nan | 0.0 |
| half | inf | inf | 65504.0 |
| half | -inf | nan | 0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan |
| float | 3.4028235e+38 (MAX) | 1.84467E+19 | 1.84467E+19 |
| float | 0.0 | 0.0 | 0.0 |
| float | -0.0 | -0.0 | -0.0 |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | nan | nan |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 9** Relu special value/boundary value input calculation result description

<a name="table135522523284"></a>

| Data Type of src/dst | Element Values of src | Element Values of dst (INF/NAN Mode) | Element Values of dst (Saturation Mode) |
| --- | --- | --- | --- |
| half | -65504.0 (MIN) | 0.0 | 0.0 |
| half | 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | 0.0 | 0.0 | 0.0 |
| half | -0.0 | 0.0 | 0.0 |
| half | nan | nan | 0.0 |
| half | inf | inf | 65504.0 |
| half | -inf | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | 3.40282E+38 | 3.40282E+38 |
| float | 0.0 | 0.0 | 0.0 |
| float | -0.0 | 0.0 | 0.0 |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | 0.0 | 0.0 |
| int32_t | -2147483648 (MIN) | 0 | 0 |
| int32_t | 2147483647 (MAX) | 2147483647 | 2147483647 |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 10** Prelu special value/boundary value input calculation result description

<a name="table16370150182115"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | 65504.0 (MAX) | MAX/MIN/inf/-inf/0.0/-0.0/nan | 65504.0 | 65504.0 |
| half | -65504.0 (MIN) | MAX/inf | -inf | -65504.0 |
| half | -65504.0 (MIN) | MIN/-inf | inf | 65504.0 |
| half | -65504.0 (MIN) | 0.0 | -0.0 | -0.0 |
| half | -65504.0 (MIN) | -0.0 | 0.0 | 0.0 |
| half | -65504.0 (MIN) | nan | nan | 0.0 |
| half | 0.0 | MAX/MIN/inf/-inf/0.0/-0.0/nan | 0.0 | 0.0 |
| half | -0.0 | MAX/0.0 | -0.0 | -0.0 |
| half | -0.0 | MIN/-0.0 | 0.0 | 0.0 |
| half | -0.0 | inf/-inf/nan | nan | 0.0 |
| half | nan | MAX/MIN/inf/-inf/0.0/-0.0/nan | nan | 0.0 |
| half | inf | MAX/MIN/inf/-inf/0.0/-0.0/nan | inf | 65504.0 |
| half | -inf | MAX/inf | -inf | -65504.0 |
| half | -inf | MIN/-inf | inf | 65504.0 |
| half | -inf | 0.0/-0.0/nan | nan | 0.0 |
| float | -3.4028235e+38 (MIN) | MAX/inf | -inf | -inf |
| float | -3.4028235e+38 (MIN) | MIN/-inf | inf | inf |
| float | -3.4028235e+38 (MIN) | 0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38 (MIN) | -0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | MAX/MIN/inf/-inf/0.0/-0.0/nan | 3.40282E+38 | 3.40282E+38 |
| float | 0.0 | MAX/MIN/inf/-inf/0.0/-0.0/nan | 0.0 | 0.0 |
| float | -0.0 | MAX/0.0 | -0.0 | -0.0 |
| float | -0.0 | MIN/-0.0 | 0.0 | 0.0 |
| float | -0.0 | inf/-inf/nan | nan | 0.0 |
| float | nan | MAX/MIN/inf/-inf/0.0/-0.0/nan | nan | nan |
| float | inf | MAX/MIN/inf/-inf/0.0/-0.0/nan | inf | inf |
| float | -inf | MAX/inf | -inf | -inf |
| float | -inf | MIN/-inf | inf | inf |
| float | -inf | 0.0/-0.0/nan | nan | nan |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 11** Add special value/boundary value input calculation result description

<a name="table071981613"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN)/-inf | -inf | -65504.0 |
| half | -65504.0 (MIN) | 65504.0 (MAX) | 0.0 | 0.0 |
| half | -65504.0 (MIN) | 0.0/-0.0 | -65504.0 | -65504.0 |
| half | -65504.0 (MIN) | nan | nan | 0.0 |
| half | -65504.0 (MIN) | inf | inf | 65504.0 |
| half | 65504.0 (MAX) | 65504.0 (MAX)/inf | inf | 65504.0 |
| half | 65504.0 (MAX) | 0.0/-0.0 | 65504.0 | 65504.0 |
| half | 65504.0 (MAX) | nan | nan | 0.0 |
| half | 65504.0 (MAX) | -inf | -inf | -65504.0 |
| half | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| half | 0.0 | nan | nan | 0.0 |
| half | 0.0 | inf | inf | 65504.0 |
| half | 0.0 | -inf | -inf | -65504.0 |
| half | -0.0 | -0.0 | -0.0 | -0.0 |
| half | -0.0 | nan | nan | 0.0 |
| half | -0.0 | inf | inf | 65504.0 |
| half | -0.0 | -inf | -inf | -65504.0 |
| half | nan | nan/inf/-inf | nan | 0.0 |
| half | inf | inf | inf | 65504.0 |
| half | inf | -inf | nan | 0.0 |
| half | -inf | -inf | -inf | -65504.0 |
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
| int16_t | -32768 (MIN) | -32768 (MIN) | 0 | -32768 |
| int16_t | -32768 (MIN) | 32767 (MAX) | -1 | -1 |
| int16_t | 32767 (MAX) | 32767 (MAX) | -2 | 32767 |
| int32_t | -2147483648 (MIN) | -2147483648 (MIN) | 0 | -2147483648 |
| int32_t | -2147483648 (MIN) | 2147483647 (MAX) | -1 | -1 |
| int32_t | 2147483647 (MAX) | 2147483647 (MAX) | -2 | 2147483647 |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 12** Adds special value/boundary value input calculation result description

<a name="table1415167829"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of scalar | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN)/-inf | -inf | -65504.0 |
| half | -65504.0 (MIN) | 65504.0 (MAX) | 0.0 | 0.0 |
| half | -65504.0 (MIN) | 0.0/-0.0 | -65504.0 | -65504.0 |
| half | -65504.0 (MIN) | nan | nan | 0.0 |
| half | -65504.0 (MIN) | inf | inf | 65504.0 |
| half | 65504.0 (MAX) | 65504.0 (MAX)/inf | inf | 65504.0 |
| half | 65504.0 (MAX) | 0.0/-0.0 | 65504.0 | 65504.0 |
| half | 65504.0 (MAX) | nan | nan | 0.0 |
| half | 65504.0 (MAX) | -inf | -inf | -65504.0 |
| half | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| half | 0.0 | nan | nan | 0.0 |
| half | 0.0 | inf | inf | 65504.0 |
| half | 0.0 | -inf | -inf | -65504.0 |
| half | -0.0 | -0.0 | -0.0 | -0.0 |
| half | -0.0 | nan | nan | 0.0 |
| half | -0.0 | inf | inf | 65504.0 |
| half | -0.0 | -inf | -inf | -65504.0 |
| half | nan | nan/inf/-inf | nan | 0.0 |
| half | inf | inf | inf | 65504.0 |
| half | inf | -inf | nan | 0.0 |
| half | -inf | -inf | -inf | -65504.0 |
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
| int16_t | -32768 (MIN) | -32768 (MIN) | 0 | -32768 |
| int16_t | -32768 (MIN) | 32767 (MAX) | -1 | -1 |
| int16_t | 32767 (MAX) | 32767 (MAX) | -2 | 32767 |
| int32_t | -2147483648 (MIN) | -2147483648 (MIN) | 0 | -2147483648 |
| int32_t | -2147483648 (MIN) | 2147483647 (MAX) | -1 | -1 |
| int32_t | 2147483647 (MAX) | 2147483647 (MAX) | -2 | 2147483647 |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 13** AddC special value/boundary value input calculation result description

<a name="table145171041144910"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| uint32_t | 4294967295 (MAX) | 4294967295 (MAX) | 4294967295 (MAX) | 4294967295 (MAX) |
| int32_t | -2147483648 (MIN) | -2147483648 (MIN) | 0 | 0 |
| int32_t | -2147483648 (MIN) | 2147483647 (MAX) | -1 | -1 |
| int32_t | 2147483647 (MAX) | 2147483647 (MAX) | -2 | -2 |


**Table 14** Sub special value/boundary value input calculation result description

<a name="table163421214360"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN) | 0.0 | 0.0 |
| half | -65504.0 (MIN) | 65504.0 (MAX) | -inf | -65504.0 |
| half | -65504.0 (MIN) | 0.0/-0.0 | -65504.0 | -65504.0 |
| half | -65504.0 (MIN) | nan | nan | 0.0 |
| half | -65504.0 (MIN) | inf | -inf | -65504.0 |
| half | -65504.0 (MIN) | -inf | inf | 65504.0 |
| half | 65504.0 (MAX) | -65504.0 (MIN) | inf | 65504.0 |
| half | 65504.0 (MAX) | 65504.0 (MAX) | 0.0 | 0.0 |
| half | 65504.0 (MAX) | 0.0/-0.0 | 65504.0 | 65504.0 |
| half | 65504.0 (MAX) | nan | nan | 0.0 |
| half | 65504.0 (MAX) | inf | -inf | -65504.0 |
| half | 65504.0 (MAX) | -inf | inf | 65504.0 |
| half | 0.0/-0.0 | -65504.0 (MIN) | 65504.0 | 65504.0 |
| half | 0.0/-0.0 | 65504.0 (MAX) | -65504.0 | -65504.0 |
| half | 0.0/-0.0 | nan | nan | 0.0 |
| half | 0.0/-0.0 | inf | -inf | -65504.0 |
| half | 0.0/-0.0 | -inf | inf | 65504.0 |
| half | 0.0 | -0.0/0.0 | 0.0 | 0.0 |
| half | -0.0 | -0.0/0.0 | -0.0 | -0.0 |
| half | nan | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/nan/inf/-inf | nan | 0.0 |
| half | inf | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/-inf | inf | 65504.0 |
| half | inf | nan/inf | nan | 0.0 |
| half | -inf | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/inf | -inf | -65504.0 |
| half | -inf | nan/-inf | nan | 0.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN) | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | -inf | -inf |
| float | -3.4028235e+38 (MIN) | 0.0/-0.0 | -3.40282E+38 | -3.40282E+38 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | -inf | -inf |
| float | -3.4028235e+38 (MIN) | -inf | inf | inf |
| float | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN) | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX) | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | 0.0/-0.0 | 3.40282E+38 | 3.40282E+38 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | inf | -inf | -inf |
| float | 3.4028235e+38 (MAX) | -inf | inf | inf |
| float | 0.0/-0.0 | -3.4028235e+38 (MIN) | 3.40282E+38 | 3.40282E+38 |
| float | 0.0/-0.0 | 3.4028235e+38 (MAX) | -3.40282E+38 | -3.40282E+38 |
| float | 0.0/-0.0 | nan | nan | nan |
| float | 0.0/-0.0 | inf | -inf | -inf |
| float | 0.0/-0.0 | -inf | inf | inf |
| float | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| float | -0.0 | 0.0/-0.0 | -0.0 | -0.0 |
| float | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/nan/inf/-inf | nan | nan |
| float | inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/-inf | inf | inf |
| float | inf | nan/inf | nan | nan |
| float | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/inf | -inf | -inf |
| float | -inf | nan/-inf | nan | nan |
| int16_t | -32768 (MIN) | -32768 (MIN) | 0 | 0 |
| int16_t | -32768 (MIN) | 32767 (MAX) | 1 | -32768 |
| int16_t | 32767 (MAX) | -32768 (MIN) | -1 | 32767 |
| int16_t | 32767 (MAX) | 32767 (MAX) | 0 | 0 |
| int32_t | -2147483648 (MIN) | -2147483648 (MIN) | 0 | 0 |
| int32_t | -2147483648 (MIN) | 2147483647 (MAX) | 1 | -2147483648 |
| int32_t | 2147483647 (MAX) | -2147483648 (MIN) | -1 | 2147483647 |
| int32_t | 2147483647 (MAX) | 2147483647 (MAX) | 0 | 0 |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 15** SubC special value/boundary value input calculation result description

<a name="table425565151818"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| uint32_t | 4294967295 (MAX) | 4294967295 (MAX) | 0 | 0 |
| int32_t | -2147483648 (MIN) | -2147483648 (MIN) | 0 | 0 |
| int32_t | -2147483648 (MIN) | 2147483647 (MAX) | 1 | 1 |
| int32_t | 2147483647 (MAX) | 2147483647 (MAX) | 0 | 0 |
| int32_t | 2147483647 (MAX) | -2147483648 (MIN) | -1 | -1 |


**Table 16** Mul special value/boundary value input calculation result description

<a name="table168220376319"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN) | inf | 65504.0 |
| half | -65504.0 (MIN) | 65504.0 (MAX) | -inf | -65504.0 |
| half | -65504.0 (MIN) | 0.0 | -0.0 | -0.0 |
| half | -65504.0 (MIN) | -0.0 | 0.0 | 0.0 |
| half | -65504.0 (MIN) | nan | nan | 0.0 |
| half | -65504.0 (MIN) | inf | -inf | -65504.0 |
| half | -65504.0 (MIN) | -inf | inf | 65504.0 |
| half | 65504.0 (MAX) | 65504.0 (MAX) | inf | 65504.0 |
| half | 65504.0 (MAX) | 0.0 | 0.0 | 0.0 |
| half | 65504.0 (MAX) | -0.0 | -0.0 | -0.0 |
| half | 65504.0 (MAX) | nan | nan | 0.0 |
| half | 65504.0 (MAX) | inf | inf | 65504.0 |
| half | 65504.0 (MAX) | -inf | -inf | -65504.0 |
| half | 0.0 | 0.0 | 0.0 | 0.0 |
| half | 0.0 | -0.0 | -0.0 | -0.0 |
| half | 0.0 | nan/inf/-inf | nan | 0.0 |
| half | -0.0 | -0.0 | 0.0 | 0.0 |
| half | -0.0 | nan/inf/-inf | nan | 0.0 |
| half | nan | nan/inf/-inf | nan | 0.0 |
| half | inf | inf | inf | 65504.0 |
| half | inf | -inf | -inf | -65504.0 |
| half | -inf | -inf | inf | 65504.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN) | inf | inf |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | -inf | -inf |
| float | -3.4028235e+38 (MIN) | 0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38 (MIN) | 0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | -inf | -inf |
| float | -3.4028235e+38 (MIN) | -inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX) | inf | inf |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
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
| int16_t | -32768 (MIN) | -32768 (MIN) | 0 | 32767 |
| int16_t | -32768 (MIN) | 32767 (MAX) | -32768 | -32768 |
| int16_t | 32767 (MAX) | 32767 (MAX) | 1 | 32767 |
| int32_t | -2147483648 (MIN) | -2147483648 (MIN) | 0 | 2147483647 |
| int32_t | -2147483648 (MIN) | 2147483647 (MAX) | -2147483648 | -2147483648 |
| int32_t | 2147483647 (MAX) | 2147483647 (MAX) | 1 | 2147483647 |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 17** Mull special value/boundary value input calculation result description

<a name="table889992553813"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst0 | Element Values of dst1 |
| --- | --- | --- | --- | --- |
| uint32_t | 4294967295 (MAX) | 4294967295 (MAX) | 1 | 4294967294 |
| int32_t | -2147483648 (MIN) | -2147483648 (MIN) | 0 | 1073741824 |
| int32_t | -2147483648 (MIN) | 2147483647 (MAX) | -2147483648 (MIN) | -1073741824 |
| int32_t | 2147483647 (MAX) | 2147483647 (MAX) | 1 | 1073741823 |


**Table 18** Div special value/boundary value input calculation result description

<a name="table1325925915217"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN) | 1 | 1 |
| half | -65504.0 (MIN) | 65504.0 (MAX) | -1 | -1 |
| half | -65504.0 (MIN) | 0.0 | -inf | -65504.0 |
| half | -65504.0 (MIN) | 0.0 | inf | 65504.0 |
| half | -65504.0 (MIN) | nan | nan | 0.0 |
| half | -65504.0 (MIN) | inf | 0.0 | 0.0 |
| half | -65504.0 (MIN) | -inf | 0.0 | 0.0 |
| half | 65504.0 (MAX) | -65504.0 (MIN) | -1 | -1 |
| half | 65504.0 (MAX) | 65504.0 (MAX) | 1 | 1 |
| half | 65504.0 (MAX) | 0.0 | inf | 65504.0 |
| half | 65504.0 (MAX) | 0.0 | -inf | -65504.0 |
| half | 65504.0 (MAX) | nan | nan | 0.0 |
| half | 65504.0 (MAX) | inf | 0.0 | 0.0 |
| half | 65504.0 (MAX) | -inf | 0.0 | 0.0 |
| half | 0.0 | -65504.0 (MIN)/-inf | 0.0 | 0.0 |
| half | 0.0 | 65504.0 (MAX)/inf | 0.0 | 0.0 |
| half | 0.0 | 0.0/-0.0/nan | nan | 0.0 |
| half | -0.0 | -65504.0 (MIN)/-inf | 0.0 | 0.0 |
| half | -0.0 | 65504.0 (MAX)/inf | 0.0 | 0.0 |
| half | -0.0 | 0.0/-0.0/nan | nan | 0.0 |
| half | nan | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/nan/inf/-inf | nan | 0.0 |
| half | inf | -65504.0 (MIN)/-0.0 | -inf | -65504.0 |
| half | inf | 65504.0 (MAX)/0.0 | inf | 65504.0 |
| half | inf | nan/inf/-inf | nan | 0.0 |
| half | -inf | -65504.0 (MIN)/-0.0 | inf | 65504.0 |
| half | -inf | 65504.0 (MAX)/0.0 | -inf | -65504.0 |
| half | -inf | nan/inf/-inf | nan | 0.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN) | 1 | 1 |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | -1 | -1 |
| float | -3.4028235e+38 (MIN) | 0.0 | -inf | -inf |
| float | -3.4028235e+38 (MIN) | 0.0 | inf | inf |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | -inf | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN) | -1 | -1 |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX) | 1 | 1 |
| float | 3.4028235e+38 (MAX) | 0.0 | inf | inf |
| float | 3.4028235e+38 (MAX) | 0.0 | -inf | -inf |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | inf | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | -inf | 0.0 | 0.0 |
| float | 0.0 | -3.4028235e+38 (MIN)/-inf | 0.0 | 0.0 |
| float | 0.0 | 3.4028235e+38 (MAX)/inf | 0.0 | 0.0 |
| float | 0.0 | 0.0/-0.0/nan | nan | nan |
| float | -0.0 | -3.4028235e+38 (MIN)/-inf | 0.0 | 0.0 |
| float | -0.0 | 3.4028235e+38 (MAX)/inf | 0.0 | 0.0 |
| float | -0.0 | 0.0/-0.0/nan | nan | nan |
| float | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/nan/inf/-inf | nan | nan |
| float | inf | -3.4028235e+38 (MIN)/-0.0 | -inf | -inf |
| float | inf | 3.4028235e+38 (MAX)/0.0 | inf | inf |
| float | inf | nan/inf/-inf | nan | nan |
| float | -inf | -3.4028235e+38 (MIN)/-0.0 | inf | inf |
| float | -inf | 3.4028235e+38 (MAX)/0.0 | -inf | -inf |
| float | -inf | nan/inf/-inf | nan | nan |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 19** Calculation result description for Max with special value/boundary value input

<a name="table7822641411"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN)/-inf | -65504.0 | -65504.0 |
| half | -65504.0 (MIN) | 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | -65504.0 (MIN) | 0.0 | 0.0 | 0.0 |
| half | -65504.0 (MIN) | 0.0 | 0.0 | 0.0 |
| half | -65504.0 (MIN) | nan | nan | 0.0 |
| half | -65504.0 (MIN) | inf | inf | 65504.0 |
| half | 65504.0 (MAX) | 65504.0 (MAX)/0.0/-0.0/-inf | 65504.0 | 65504.0 |
| half | 65504.0 (MAX) | nan | nan | 0.0 |
| half | 65504.0 (MAX) | inf | inf | 65504.0 |
| half | 0.0 | 0.0/-0.0/-inf | 0.0 | 0.0 |
| half | 0.0 | nan | nan | 0.0 |
| half | 0.0 | inf | inf | 65504.0 |
| half | -0.0 | -0.0/-inf | -0.0 | -0.0 |
| half | -0.0 | nan | nan | 0.0 |
| half | -0.0 | inf | inf | 65504.0 |
| half | nan | nan/inf/-inf | nan | 0.0 |
| half | inf | inf/-inf | inf | 65504.0 |
| half | -inf | -inf | -inf | -65504.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/-inf | -3.40282E+38 | -3.40282E+38 |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | 3.40282E+38 | 3.40282E+38 |
| float | -3.4028235e+38 (MIN) | 0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | 0.0 | 0.0 | 0.0 |
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
| int32_t | -2147483648 (MIN) | 2147483647 (MAX) | 2147483647 | 2147483647 |
| int32_t | 2147483647 (MAX) | 2147483647 (MAX) | 2147483647 | 2147483647 |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 20** Calculation result description for Min with special value/boundary value input

<a name="table242810341242"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/inf | -65504.0 | -65504.0 |
| half | -65504.0 (MIN) | nan | nan | 0.0 |
| half | -65504.0 (MIN) | -inf | -inf | -65504.0 |
| half | 65504.0 (MAX) | 65504.0 (MAX)/inf | 65504.0 | 65504.0 |
| half | 65504.0 (MAX) | 0.0 | 0.0 | 0.0 |
| half | 65504.0 (MAX) | 0.0 | 0.0 | 0.0 |
| half | 65504.0 (MAX) | nan | nan | 0.0 |
| half | 65504.0 (MAX) | -inf | -inf | -65504.0 |
| half | 0.0 | 0.0/inf | 0.0 | 0.0 |
| half | 0.0 | 0.0 | 0.0 | 0.0 |
| half | 0.0 | nan | nan | 0.0 |
| half | 0.0 | -inf | -inf | -65504.0 |
| half | -0.0 | -0.0/inf | -0.0 | -0.0 |
| half | -0.0 | nan | nan | 0.0 |
| half | -0.0 | -inf | -inf | -65504.0 |
| half | nan | nan/inf/-inf | nan | 0.0 |
| half | inf | inf | inf | 65504.0 |
| half | inf | -inf | -inf | -65504.0 |
| half | -inf | -inf | -inf | -65504.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/inf | -3.40282E+38 | -3.40282E+38 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | -inf | -inf | -inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX)/inf | 3.40282E+38 | 3.40282E+38 |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | -inf | -inf | -inf |
| float | 0.0 | 0.0/inf | 0.0 | 0.0 |
| float | 0.0 | 0.0 | 0.0 | 0.0 |
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


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 21** Calculation result description for Muls with special value/boundary value input

<a name="table8397142191119"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN) | inf | 65504.0 |
| half | -65504.0 (MIN) | 65504.0 (MAX) | -inf | -65504.0 |
| half | -65504.0 (MIN) | 0.0 | -0.0 | -0.0 |
| half | -65504.0 (MIN) | -0.0 | 0.0 | 0.0 |
| half | -65504.0 (MIN) | nan | nan | 0.0 |
| half | -65504.0 (MIN) | inf | -inf | -65504.0 |
| half | -65504.0 (MIN) | -inf | inf | 65504.0 |
| half | 65504.0 (MAX) | 65504.0 (MAX) | inf | 65504.0 |
| half | 65504.0 (MAX) | 0.0 | 0.0 | 0.0 |
| half | 65504.0 (MAX) | -0.0 | -0.0 | -0.0 |
| half | 65504.0 (MAX) | nan | nan | 0.0 |
| half | 65504.0 (MAX) | inf | inf | 65504.0 |
| half | 65504.0 (MAX) | -inf | -inf | -65504.0 |
| half | 0.0 | 0.0 | 0.0 | 0.0 |
| half | 0.0 | -0.0 | -0.0 | -0.0 |
| half | 0.0 | nan/inf/-inf | nan | 0.0 |
| half | -0.0 | -0.0 | 0.0 | 0.0 |
| half | -0.0 | nan/inf/-inf | nan | 0.0 |
| half | nan | nan/inf/-inf | nan | 0.0 |
| half | inf | inf | inf | 65504.0 |
| half | inf | -inf | -inf | -65504.0 |
| half | -inf | -inf | inf | 65504.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN) | inf | inf |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | -inf | -inf |
| float | -3.4028235e+38 (MIN) | 0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38 (MIN) | 0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | -inf | -inf |
| float | -3.4028235e+38 (MIN) | -inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX) | inf | inf |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | inf | inf | inf |
| float | 3.4028235e+38 (MAX) | -inf | -inf | -inf |
| float | 0.0 | 0.0 | 0.0 | 0.0 |
| float | 0.0 | 0.0 | 0.0 | 0.0 |
| float | 0.0 | nan/inf/-inf | nan | nan |
| float | -0.0 | 0.0 | -0.0 | -0.0 |
| float | -0.0 | nan/inf/-inf | nan | nan |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | inf | inf |
| float | inf | -inf | -inf | -inf |
| float | -inf | -inf | inf | inf |
| int16_t | -32768 (MIN) | -32768 (MIN) | 0 | 32767 |
| int16_t | -32768 (MIN) | 32767 (MAX) | -32768 | -32768 |
| int16_t | 32767 (MAX) | 32767 (MAX) | 1 | 32767 |
| int32_t | -2147483648 (MIN) | -2147483648 (MIN) | 0 | 2147483647 |
| int32_t | -2147483648 (MIN) | 2147483647 (MAX) | -2147483648 | -2147483648 |
| int32_t | 2147483647 (MAX) | 2147483647 (MAX) | 1 | 2147483647 |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 22** Calculation result description for Maxs with special value/boundary value input

<a name="table149066456147"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN)/-inf | -65504.0 | -65504.0 |
| half | -65504.0 (MIN) | 65504.0 (MAX) | 65504.0 | 65504.0 |
| half | -65504.0 (MIN) | 0.0 | 0.0 | 0.0 |
| half | -65504.0 (MIN) | 0.0 | 0.0 | 0.0 |
| half | -65504.0 (MIN) | nan | nan | 0.0 |
| half | -65504.0 (MIN) | inf | inf | 65504.0 |
| half | 65504.0 (MAX) | 65504.0 (MAX)/0.0/-0.0/-inf | 65504.0 | 65504.0 |
| half | 65504.0 (MAX) | nan | nan | 0.0 |
| half | 65504.0 (MAX) | inf | inf | 65504.0 |
| half | 0.0 | 0.0/-0.0/-inf | 0.0 | 0.0 |
| half | 0.0 | nan | nan | 0.0 |
| half | 0.0 | inf | inf | 65504.0 |
| half | -0.0 | -0.0/-inf | -0.0 | -0.0 |
| half | -0.0 | nan | nan | 0.0 |
| half | -0.0 | inf | inf | 65504.0 |
| half | nan | nan/inf/-inf | nan | 0.0 |
| half | inf | inf/-inf | inf | 65504.0 |
| half | -inf | -inf | -inf | -65504.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/-inf | -3.40282E+38 | -3.40282E+38 |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | 3.40282E+38 | 3.40282E+38 |
| float | -3.4028235e+38 (MIN) | 0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | 0.0 | 0.0 | 0.0 |
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
| int32_t | -2147483648 (MIN) | 2147483647 (MAX) | 2147483647 | 2147483647 |
| int32_t | 2147483647 (MAX) | 2147483647 (MAX) | 2147483647 | 2147483647 |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 23** Calculation result description for Mins with special value/boundary value input

<a name="table195381845171710"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/inf | -65504.0 | -65504.0 |
| half | -65504.0 (MIN) | nan | nan | 0.0 |
| half | -65504.0 (MIN) | -inf | -inf | -65504.0 |
| half | 65504.0 (MAX) | 65504.0 (MAX)/inf | 65504.0 | 65504.0 |
| half | 65504.0 (MAX) | 0.0 | 0.0 | 0.0 |
| half | 65504.0 (MAX) | 0.0 | 0.0 | 0.0 |
| half | 65504.0 (MAX) | nan | nan | 0.0 |
| half | 65504.0 (MAX) | -inf | -inf | -65504.0 |
| half | 0.0 | 0.0/inf | 0.0 | 0.0 |
| half | 0.0 | 0.0 | 0.0 | 0.0 |
| half | 0.0 | nan | nan | 0.0 |
| half | 0.0 | -inf | -inf | -65504.0 |
| half | -0.0 | -0.0/inf | -0.0 | -0.0 |
| half | -0.0 | nan | nan | 0.0 |
| half | -0.0 | -inf | -inf | -65504.0 |
| half | nan | nan/inf/-inf | nan | 0.0 |
| half | inf | inf | inf | 65504.0 |
| half | inf | -inf | -inf | -65504.0 |
| half | -inf | -inf | -inf | -65504.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/inf | -3.40282E+38 | -3.40282E+38 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | -inf | -inf | -inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX)/inf | 3.4e+38 | 3.4e+38 |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | -inf | -inf | -inf |
| float | 0.0 | 0.0/inf | 0.0 | 0.0 |
| float | 0.0 | 0.0 | 0.0 | 0.0 |
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


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 24** Calculation result description for special value/boundary value input of LeakyRelu

<a name="table163011385209"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN) | inf | 65504.0 |
| half | -65504.0 (MIN) | 65504.0 (MAX) | -inf | -65504.0 |
| half | -65504.0 (MIN) | 0.0 | 0.0 | 0.0 |
| half | -65504.0 (MIN) | 0.0 | 0.0 | 0.0 |
| half | -65504.0 (MIN) | nan | nan | 0.0 |
| half | -65504.0 (MIN) | inf | -inf | -65504.0 |
| half | -65504.0 (MIN) | -inf | inf | 65504.0 |
| half | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/nan/inf/-inf | 65504.0 | 65504.0 |
| half | 0.0 | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/nan/inf/-inf | 0.0 | 0.0 |
| half | -0.0 | -65504.0 (MIN)/-0.0 | 0.0 | 0.0 |
| half | -0.0 | 65504.0 (MAX)/0.0 | 0.0 | 0.0 |
| half | -0.0 | nan/inf/-inf | nan | 0.0 |
| half | nan | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/nan/inf/-inf | nan | 0.0 |
| half | inf | -65504.0 (MIN)/65504.0 (MAX)/0.0/-0.0/nan/inf/-inf | inf | 65504.0 |
| half | -inf | -65504.0 (MIN)/-inf | inf | 65504.0 |
| half | -inf | 65504.0 (MAX)/inf | -inf | -65504.0 |
| half | -inf | 0.0/-0.0/nan | nan | 0.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN) | inf | inf |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | -inf | -inf |
| float | -3.4028235e+38 (MIN) | 0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | 0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | -inf | -inf |
| float | -3.4028235e+38 (MIN) | -inf | inf | inf |
| float | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/nan/inf/-inf | 3.4e+38 | 3.4e+38 |
| float | 0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/nan/inf/-inf | 0.0 | 0.0 |
| float | -0.0 | -3.4028235e+38 (MIN)/-0.0 | 0.0 | 0.0 |
| float | -0.0 | 3.4028235e+38 (MAX)/0.0 | 0.0 | 0.0 |
| float | -0.0 | nan/inf/-inf | nan | nan |
| float | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/nan/inf/-inf | nan | nan |
| float | inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/0.0/-0.0/nan/inf/-inf | inf | inf |
| float | -inf | -3.4028235e+38 (MIN)/-inf | inf | inf |
| float | -inf | 3.4028235e+38 (MAX)/inf | -inf | -inf |
| float | -inf | 0.0/-0.0/nan | nan | nan |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

## Compound Calculation

Note: In the following table, MAX/MIN correspond to the maximum and minimum values under the data type of the corresponding operand.

**Table 25** Result description of Axpy special value/boundary value input calculation

<a name="table1819218568247"></a>

Note: In the following table, MAX/MIN correspond to the maximum and minimum values under the data type of the corresponding operand.

| Data Type of src/dst | value of src/value of scalarValue | dst value | dst value<br> (INF/NAN Mode) | dst value<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0.0 | +0 | +0.0 |
| half | +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | -0.0 | +0.0 | +0 |
| half | +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +inf | +inf | MAX |
| half | +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | -inf | -inf | MIN |
| half | +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | MAX | MAX | MAX |
| half | +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | MIN | MIN | MIN |
| half | +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | nan | nan | +0.0 |
| half | +0/MIN<br>-0/MAX<br>+0/-0<br>-0/+0 | +0.0 | +0 | +0.0 |
| half | +0/MIN<br>-0/MAX<br>+0/-0<br>-0/+0 | -0.0 | -0.0 | -0.0 |
| half | +0/MIN<br>-0/MAX<br>+0/-0<br>-0/+0 | +inf | +inf | MAX |
| half | +0/MIN<br>-0/MAX<br>+0/-0<br>-0/+0 | -inf | -inf | MIN |
| half | +0/MIN<br>-0/MAX<br>+0/-0<br>-0/+0 | MAX | MAX | MAX |
| half | +0/MIN<br>-0/MAX<br>+0/-0<br>-0/+0 | MIN | MIN | MIN |
| half | +0/MIN<br>-0/MAX<br>+0/-0<br>-0/+0 | nan | nan | +0.0 |
| half | MAX/+inf<br>MIN/-inf<br>+inf/+inf<br>-inf/-inf | +0.0 | +inf | MAX |
| half | MAX/+inf<br>MIN/-inf<br>+inf/+inf<br>-inf/-inf | -0.0 | +inf | MAX |
| half | MAX/+inf<br>MIN/-inf<br>+inf/+inf<br>-inf/-inf | +inf | +inf | MAX |
| half | MAX/+inf<br>MIN/-inf<br>+inf/+inf<br>-inf/-inf | -inf | nan | +0.0 |
| half | MAX/+inf<br>MIN/-inf<br>+inf/+inf<br>-inf/-inf | MAX | +inf | MAX |
| half | MAX/+inf<br>MIN/-inf<br>+inf/+inf<br>-inf/-inf | MIN | +inf | MAX |
| half | MAX/+inf<br>MIN/-inf<br>+inf/+inf<br>-inf/-inf | nan | nan | +0.0 |
| half | MAX/MAX<br>MIN/MIN | +0.0 | +inf | MAX |
| half | MAX/MAX<br>MIN/MIN | -0.0 | +inf | MAX |
| half | MAX/MAX<br>MIN/MIN | +inf | +inf | MAX |
| half | MAX/MAX<br>MIN/MIN | -inf | -inf | MIN |
| half | MAX/MAX<br>MIN/MIN | MAX | +inf | MAX |
| half | MAX/MAX<br>MIN/MIN | MIN | +inf | MAX |
| half | MAX/MAX<br>MIN/MIN | nan | nan | +0.0 |
| half | MAX/MIN<br>MIN/MAX | +0.0 | -inf | MIN |
| half | MAX/MIN<br>MIN/MAX | -0.0 | -inf | MIN |
| half | MAX/MIN<br>MIN/MAX | +inf | +inf | MAX |
| half | MAX/MIN<br>MIN/MAX | -inf | -inf | MIN |
| half | MAX/MIN<br>MIN/MAX | MAX | -inf | MIN |
| half | MAX/MIN<br>MIN/MAX | MIN | -inf | MIN |
| half | MAX/MIN<br>MIN/MAX | nan | nan | +0.0 |
| half | MAX/-inf<br>MIN/+inf<br>+inf/-inf | +0.0 | -inf | MIN |
| half | MAX/-inf<br>MIN/+inf<br>+inf/-inf | -0.0 | -inf | MIN |
| half | MAX/-inf<br>MIN/+inf<br>+inf/-inf | +inf | nan | +0.0 |
| half | MAX/-inf<br>MIN/+inf<br>+inf/-inf | -inf | -inf | MIN |
| half | MAX/-inf<br>MIN/+inf<br>+inf/-inf | MAX | -inf | MIN |
| half | MAX/-inf<br>MIN/+inf<br>+inf/-inf | MIN | -inf | MIN |
| half | MAX/-inf<br>MIN/+inf<br>+inf/-inf | nan | nan | +0.0 |
| half | nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>+0/nan<br>-0/+inf<br>-0/-inf<br>-0/nan | +0.0 | nan | +0.0 |
| half | nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>+0/nan<br>-0/+inf<br>-0/-inf<br>-0/nan | -0.0 | nan | +0.0 |
| half | nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>+0/nan<br>-0/+inf<br>-0/-inf<br>-0/nan | +inf | nan | +0.0 |
| half | nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>+0/nan<br>-0/+inf<br>-0/-inf<br>-0/nan | -inf | nan | +0.0 |
| half | nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>+0/nan<br>-0/+inf<br>-0/-inf<br>-0/nan | MAX | nan | +0.0 |
| half | nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>+0/nan<br>-0/+inf<br>-0/-inf<br>-0/nan | MIN | nan | +0.0 |
| half | nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>+0/nan<br>-0/+inf<br>-0/-inf<br>-0/nan | nan | nan | +0.0 |
| float | -0/MIN<br>+0/+0<br>+0/MAX<br>-0/-0 | +0.0 | +0 | +0.0 |
| float | -0/MIN<br>+0/+0<br>+0/MAX<br>-0/-0 | -0.0 | +0.0 | +0 |
| float | -0/MIN<br>+0/+0<br>+0/MAX<br>-0/-0 | +inf | +inf | +inf |
| float | -0/MIN<br>+0/+0<br>+0/MAX<br>-0/-0 | -inf | -inf | -inf |
| float | -0/MIN<br>+0/+0<br>+0/MAX<br>-0/-0 | MAX | MAX | MAX |
| float | -0/MIN<br>+0/+0<br>+0/MAX<br>-0/-0 | MIN | MIN | MIN |
| float | -0/MIN<br>+0/+0<br>+0/MAX<br>-0/-0 | nan | nan | nan |
| float | -0/MAX<br>+0/-0<br>+0/MIN | +0.0 | +0 | +0.0 |
| float | -0/MAX<br>+0/-0<br>+0/MIN | -0.0 | -0.0 | -0.0 |
| float | -0/MAX<br>+0/-0<br>+0/MIN | +inf | +inf | +inf |
| float | -0/MAX<br>+0/-0<br>+0/MIN | -inf | -inf | -inf |
| float | -0/MAX<br>+0/-0<br>+0/MIN | MAX | MAX | MAX |
| float | -0/MAX<br>+0/-0<br>+0/MIN | MIN | MIN | MIN |
| float | -0/MAX<br>+0/-0<br>+0/MIN | nan | nan | nan |
| float | +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>-inf/-inf | +0.0 | +inf | +inf |
| float | +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>-inf/-inf | -0.0 | +inf | +inf |
| float | +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>-inf/-inf | +inf | +inf | +inf |
| float | +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>-inf/-inf | -inf | nan | nan |
| float | +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>-inf/-inf | MAX | +inf | +inf |
| float | +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>-inf/-inf | MIN | +inf | +inf |
| float | +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>-inf/-inf | nan | nan | nan |
| float | +inf/-inf<br>+inf/MIN<br>-inf/MAX | +0.0 | -inf | -inf |
| float | +inf/-inf<br>+inf/MIN<br>-inf/MAX | -0.0 | -inf | -inf |
| float | +inf/-inf<br>+inf/MIN<br>-inf/MAX | +inf | nan | nan |
| float | +inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | -inf | -inf |
| float | +inf/-inf<br>+inf/MIN<br>-inf/MAX | MAX | -inf | -inf |
| float | +inf/-inf<br>+inf/MIN<br>-inf/MAX | MIN | -inf | -inf |
| float | +inf/-inf<br>+inf/MIN<br>-inf/MAX | nan | nan | nan |
| float | MAX/MAX<br>MIN/MIN | +0.0 | +inf | +inf |
| float | MAX/MAX<br>MIN/MIN | -0.0 | +inf | +inf |
| float | MAX/MAX<br>MIN/MIN | +inf | +inf | +inf |
| float | MAX/MAX<br>MIN/MIN | -inf | -inf | -inf |
| float | MAX/MAX<br>MIN/MIN | MAX | +inf | +inf |
| float | MAX/MAX<br>MIN/MIN | MIN | +inf | +inf |
| float | MAX/MAX<br>MIN/MIN | nan | nan | nan |
| float | MAX/MIN | +0.0 | -inf | -inf |
| float | MAX/MIN | -0.0 | -inf | -inf |
| float | MAX/MIN | +inf | +inf | +inf |
| float | MAX/MIN | -inf | -inf | -inf |
| float | MAX/MIN | MAX | -inf | -inf |
| float | MAX/MIN | MIN | -inf | -inf |
| float | MAX/MIN | nan | nan | nan |
| float | nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | +0.0 | nan | nan |
| float | nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | -0.0 | nan | nan |
| float | nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | +inf | nan | nan |
| float | nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | -inf | nan | nan |
| float | nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | MAX | nan | nan |
| float | nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | MIN | nan | nan |
| float | nan/arbitrary value<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | nan | nan |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 26** Result description of AbsSub special value/boundary value input calculation

<a name="table13182948124817"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half | -65504.0 (MIN) | -65504.0 (MIN) | 0.0 | 0.0 |
| half | -65504.0 (MIN) | 65504.0 (MAX) | inf | 65504.0 |
| half | -65504.0 (MIN) | 0.0/-0.0 | 65504.0 | 65504.0 |
| half | -65504.0 (MIN) | nan | nan | 0.0 |
| half | -65504.0 (MIN) | inf/-inf | inf | 65504.0 |
| half | 65504.0 (MAX) | 65504.0 (MAX) | 0.0 | 0.0 |
| half | 65504.0 (MAX) | 0.0/-0.0 | 65504.0 | 65504.0 |
| half | 65504.0 (MAX) | nan | nan | 0.0 |
| half | 65504.0 (MAX) | -inf/inf | inf | 65504.0 |
| half | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| half | 0.0 | nan | nan | 0.0 |
| half | 0.0 | inf/-inf | inf | 65504.0 |
| half | -0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| half | -0.0 | nan | nan | 0.0 |
| half | -0.0 | inf/-inf | inf | 65504.0 |
| half | nan | nan/inf/-inf | nan | 0.0 |
| half | inf | inf | nan | 0.0 |
| half | inf | -inf | inf | 65504.0 |
| half | -inf | -inf | nan | 0.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN) | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | inf | inf |
| float | -3.4028235e+38 (MIN) | 0.0/-0.0 | MAX | MAX |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf/-inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX) | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | 0.0/-0.0 | 3.40282E+38 | 3.40282E+38 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | -inf/inf | inf | inf |
| float | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | inf/-inf | inf | inf |
| float | -0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | inf | inf | inf |
| float | -0.0 | -inf | -inf | -inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | nan | nan |
| float | inf | -inf | inf | inf |
| float | -inf | -inf | nan | nan |
| int64_t | -9223372036854775808 (MIN) | -9223372036854775808 (MIN) | 0 | 0 |
| int64_t | -9223372036854775808 (MIN) | 9223372036854775808 (MAX) | 1 | 1 |
| int64_t | 9223372036854775808 (MAX) | 9223372036854775808 (MAX) | 0 | 0 |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 27** ExpSub special value/boundary value input calculation result description

<a name="table9246161118410"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| half/float | -65504.0 (MIN) | -65504.0 (MIN) | 1 | 1 |
| half/float | -65504.0 (MIN) | 65504.0 (MAX)/0.0/-0.0/inf | 0.0 | 0.0 |
| half/float | -65504.0 (MIN) | nan | nan | nan |
| half/float | -65504.0 (MIN) | -inf | -inf | -inf |
| half/float | 65504.0 (MAX) | 65504.0 (MAX) | 1 | 1 |
| half/float | 65504.0 (MAX) | 0.0/-0.0/-3.4028235e+38 (MIN)/-inf | inf | inf |
| half/float | 65504.0 (MAX) | nan | nan | nan |
| half/float | 65504.0 (MAX) | inf | 0.0 | 0.0 |
| half/float | 0.0/-0.0 | 3.4028235e+38 (MAX)/inf | 0.0 | 0.0 |
| half/float | 0.0/-0.0 | 0.0/-0.0 | 1 | 1 |
| half/float | 0.0/-0.0 | nan | nan | nan |
| half/float | 0.0/-0.0 | -3.4028235e+38 (MIN)/-inf | inf | inf |
| half/float | nan | nan/inf/-inf/0/-0/max/min | nan | nan |
| half/float | inf | -inf/0/-0/max/min | inf | inf |
| half/float | inf | nan/inf | nan | nan |
| half/float | -inf | inf/-inf/0/-0/max/min | 0.0 | 0.0 |
| half/float | -inf | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN) | 1 | 1 |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX)/0.0/-0.0/inf | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | -inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX) | 1 | 1 |
| float | 3.4028235e+38 (MAX) | 0.0/-0.0/-3.4028235e+38 (MIN)/-inf | inf | inf |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | inf | 0.0 | 0.0 |
| float | 0.0/-0.0 | 3.4028235e+38 (MAX)/inf | 0.0 | 0.0 |
| float | 0.0/-0.0 | 0.0/-0.0 | 1 | 1 |
| float | 0.0/-0.0 | nan | nan | nan |
| float | 0.0/-0.0 | -3.4028235e+38 (MIN)/-inf | inf | inf |
| float | nan | nan/inf/-inf/0/-0/max/min | nan | nan |
| float | inf | inf/nan | nan | nan |
| float | inf | -inf/0/-0/max/min | 0.0 | 0.0 |
| float | -inf | inf/-inf/0/-0/max/min | 0.0 | 0.0 |
| float | -inf | nan | nan | nan |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 28** MulDstAdd special value/boundary value input calculation result description

<a name="table0741155811341"></a>

Note: In the following table, MAX/MIN correspond to the maximum and minimum values under the data type of the corresponding operand.

| Data Type of src/dst | src0 value/src1 value | dst value | dst value<br> (INF/NAN Mode) | dst value<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| float | +0/+0<br>-0/+0 | +0/-0/MAX/MIN | +0.0 | +0 |
| float | +0/+0<br>-0/+0 | nan/+inf/-inf | nan | nan |
| float | +0/-0 | +0/MAX | +0.0 | +0 |
| float | +0/-0 | -0/MIN | -0.0 | -0.0 |
| float | +0/-0 | nan/+inf/-inf | nan | nan |
| float | -0/-0 | +0/MAX | -0.0 | -0.0 |
| float | -0/-0 | -0/MIN | +0.0 | +0 |
| float | -0/-0 | nan/+inf/-inf | nan | nan |
| float | -0/MAX<br>+0/MAX | +0/-0/MAX/MIN | MAX | MAX |
| float | -0/MAX<br>+0/MAX | nan/+inf/-inf | nan | nan |
| float | -0/MIN<br>+0/MIN | +0/-0/MAX/MIN | MIN | MIN |
| float | -0/MIN<br>+0/MIN | nan/+inf/-inf | nan | nan |
| float | +0/inf<br>-0/inf | +0/-0/MAX/MIN | inf | inf |
| float | +0/inf<br>-0/inf | nan/+inf/-inf | nan | nan |
| float | +0/-inf<br>-0/-inf | +0/-0/MAX/MIN | -inf | -inf |
| float | +0/-inf<br>-0/-inf | nan/+inf/-inf | nan | nan |
| float | MAX/+inf | +0/-0/MAX/MIN/+inf | inf | inf |
| float | MAX/+inf | -inf/nan | nan | nan |
| float | MIN/+inf | +0/-0/MAX/MIN/-inf | inf | inf |
| float | MIN/+inf | +inf/nan | nan | nan |
| float | MAX/-inf | +0/-0/MAX/MIN/-inf | -inf | -inf |
| float | MAX/-inf | +inf/nan | nan | nan |
| float | MIN/-inf | +0/-0/MAX/MIN/+inf | -inf | -inf |
| float | MIN/-inf | -inf/nan | nan | nan |
| float | MAX/+0 | MAX/+inf | inf | inf |
| float | MAX/+0 | +0/-0 | +0.0 | +0 |
| float | MAX/+0 | MIN/-inf | -inf | -inf |
| float | MAX/+0 | nan | nan | nan |
| float | MIN/+0 | MAX/+inf | -inf | -inf |
| float | MIN/+0 | +0/-0 | +0.0 | +0 |
| float | MIN/+0 | MIN/-inf | inf | inf |
| float | MIN/+0 | nan | nan | nan |
| float | MAX/-0 | MAX/+inf | inf | inf |
| float | MAX/-0 | MIN/-inf | -inf | -inf |
| float | MAX/-0 | +0.0 | +0 | +0.0 |
| float | MAX/-0 | -0.0 | -0.0 | -0.0 |
| float | MAX/-0 | nan | nan | nan |
| float | MIN/-0 | MAX/+inf | inf | inf |
| float | MIN/-0 | MIN/-inf | -inf | -inf |
| float | MIN/-0 | +0.0 | -0.0 | -0.0 |
| float | MIN/-0 | -0.0 | +0.0 | +0 |
| float | MIN/-0 | nan | nan | nan |
| float | MAX/MAX | MAX/+inf | inf | inf |
| float | MAX/MAX | MIN/-inf | -inf | -inf |
| float | MAX/MAX | +0/-0 | MAX | MAX |
| float | MAX/MAX | nan | nan | nan |
| float | MIN/MIN | MAX/+inf | -inf | -inf |
| float | MIN/MIN | MIN/-inf | inf | inf |
| float | MIN/MIN | +0/-0 | MAX | MAX |
| float | MIN/MIN | nan | nan | nan |
| float | MAX/MIN | MAX/+inf | inf | inf |
| float | MAX/MIN | MIN/-inf | -inf | -inf |
| float | MAX/MIN | +0/-0 | MIN | MIN |
| float | MAX/MIN | nan | nan | nan |
| float | MIN/MAX | MAX/+inf | -inf | -inf |
| float | MIN/MAX | MIN/-inf | inf | inf |
| float | MIN/MAX | +0/-0 | MAX | MAX |
| float | MIN/MAX | nan | nan | nan |
| float | inf/inf | MAX/+inf | inf | inf |
| float | inf/inf | MIN/-inf/0/-0/-inf/nan | nan | nan |
| float | -inf/inf | MIN/-inf | inf | inf |
| float | -inf/inf | MAX/+inf/0/-0/-inf/nan | nan | nan |
| float | inf/-inf | MIN/-inf | -inf | -inf |
| float | inf/-inf | MAX/+inf/0/-0/-inf/nan | nan | nan |
| float | -inf/-inf | MAX/+inf | -inf | -inf |
| float | -inf/-inf | MIN/-inf/0/-0/-inf/nan | nan | nan |
| float | inf/+0<br>inf/-0<br>inf/MAX<br>inf/MIN | MAX/+inf | inf | inf |
| float | inf/+0<br>inf/-0<br>inf/MAX<br>inf/MIN | MIN/-inf | -inf | -inf |
| float | inf/+0<br>inf/-0<br>inf/MAX<br>inf/MIN | nan/+0/-0 | nan | nan |
| float | -inf/+0<br>-inf/-0<br>-inf/MAX<br>-inf/MIN | MAX/+inf | -inf | -inf |
| float | -inf/+0<br>-inf/-0<br>-inf/MAX<br>-inf/MIN | MIN/-inf | inf | inf |
| float | -inf/+0<br>-inf/-0<br>-inf/MAX<br>-inf/MIN | nan/+0/-0 | nan | nan |
| float | nan/arbitrary value | +0/-0/MAX/MIN/nan/+inf/-inf | nan | nan |
| half | +0/+0<br>-0/+0 | +0/-0/MAX/MIN | +0.0 | +0 |
| half | +0/+0<br>-0/+0 | nan/+inf/-inf | nan | +0.0 |
| half | +0/-0 | +0/MAX | +0.0 | +0 |
| half | +0/-0 | -0/MIN | -0.0 | -0.0 |
| half | +0/-0 | nan/+inf/-inf | nan | +0.0 |
| half | -0/-0 | +0/MAX | -0.0 | -0.0 |
| half | -0/-0 | -0/MIN | +0.0 | +0 |
| half | -0/-0 | nan/+inf/-inf | nan | +0.0 |
| half | -0/MAX<br>+0/MAX | +0/-0/MAX/MIN | MAX | MAX |
| half | -0/MAX<br>+0/MAX | nan/+inf/-inf | nan | +0.0 |
| half | -0/MIN<br>+0/MIN | +0/-0/MAX/MIN | MIN | MIN |
| half | -0/MIN<br>+0/MIN | nan/+inf/-inf | nan | +0.0 |
| half | +0/inf<br>-0/inf | +0/-0/MAX/MIN | inf | MAX |
| half | +0/inf<br>-0/inf | nan/+inf/-inf | nan | +0.0 |
| half | +0/-inf<br>-0/-inf | +0/-0/MAX/MIN | -inf | MIN |
| half | +0/-inf<br>-0/-inf | nan/+inf/-inf | nan | +0.0 |
| half | MAX/+inf | +0/-0/MAX/MIN/+inf | inf | MAX |
| half | MAX/+inf | -inf/nan | nan | +0.0 |
| half | MIN/+inf | +0/-0/MAX/MIN/-inf | inf | MAX |
| half | MIN/+inf | +inf/nan | nan | +0.0 |
| half | MAX/-inf | +0/-0/MAX/MIN/-inf | -inf | MIN |
| half | MAX/-inf | +inf/nan | nan | +0.0 |
| half | MIN/-inf | +0/-0/MAX/MIN/+inf | -inf | MIN |
| half | MIN/-inf | -inf/nan | nan | +0.0 |
| half | MAX/+0 | MAX/+inf | inf | MAX |
| half | MAX/+0 | +0/-0 | +0.0 | +0 |
| half | MAX/+0 | MIN/-inf | -inf | MIN |
| half | MAX/+0 | nan | nan | +0.0 |
| half | MIN/+0 | MAX/+inf | -inf | MIN |
| half | MIN/+0 | +0/-0 | +0.0 | +0 |
| half | MIN/+0 | MIN/-inf | inf | MAX |
| half | MIN/+0 | nan | nan | +0.0 |
| half | MAX/-0 | MAX/+inf | inf | MAX |
| half | MAX/-0 | MIN/-inf | -inf | MIN |
| half | MAX/-0 | +0.0 | +0 | +0.0 |
| half | MAX/-0 | -0.0 | -0.0 | -0.0 |
| half | MAX/-0 | nan | nan | +0.0 |
| half | MIN/-0 | MAX/+inf | inf | MAX |
| half | MIN/-0 | MIN/-inf | -inf | MIN |
| half | MIN/-0 | +0.0 | -0.0 | -0.0 |
| half | MIN/-0 | -0.0 | +0.0 | +0 |
| half | MIN/-0 | nan | nan | +0.0 |
| half | MAX/MAX | MAX/+inf | inf | MAX |
| half | MAX/MAX | MIN/-inf | -inf | MIN |
| half | MAX/MAX | +0/-0 | MAX | MAX |
| half | MAX/MAX | nan | nan | +0.0 |
| half | MIN/MIN | MAX/+inf | -inf | MIN |
| half | MIN/MIN | MIN/-inf | inf | MAX |
| half | MIN/MIN | +0/-0 | MAX | MAX |
| half | MIN/MIN | nan | nan | +0.0 |
| half | MAX/MIN | MAX/+inf | inf | MAX |
| half | MAX/MIN | MIN/-inf | -inf | MIN |
| half | MAX/MIN | +0/-0 | MIN | MIN |
| half | MAX/MIN | nan | nan | +0.0 |
| half | MIN/MAX | MAX/+inf | -inf | MIN |
| half | MIN/MAX | MIN/-inf | inf | MAX |
| half | MIN/MAX | +0/-0 | MAX | MAX |
| half | MIN/MAX | nan | nan | +0.0 |
| half | inf/inf | MAX/+inf | inf | MAX |
| half | inf/inf | MIN/-inf/0/-0/-inf/nan | nan | +0.0 |
| half | -inf/inf | MIN/-inf | inf | MAX |
| half | -inf/inf | MAX/+inf/0/-0/-inf/nan | nan | +0.0 |
| half | inf/-inf | MIN/-inf | -inf | MIN |
| half | inf/-inf | MAX/+inf/0/-0/-inf/nan | nan | +0.0 |
| half | -inf/-inf | MAX/+inf | -inf | MIN |
| half | -inf/-inf | MIN/-inf/0/-0/-inf/nan | nan | +0.0 |
| half | inf/+0<br>inf/-0<br>inf/MAX<br>inf/MIN | MAX/+inf | inf | MAX |
| half | inf/+0<br>inf/-0<br>inf/MAX<br>inf/MIN | MIN/-inf | -inf | MIN |
| half | inf/+0<br>inf/-0<br>inf/MAX<br>inf/MIN | nan/+0/-0 | nan | +0.0 |
| half | -inf/+0<br>-inf/-0<br>-inf/MAX<br>-inf/MIN | MAX/+inf | -inf | MIN |
| half | -inf/+0<br>-inf/-0<br>-inf/MAX<br>-inf/MIN | MIN/-inf | inf | MAX |
| half | -inf/+0<br>-inf/-0<br>-inf/MAX<br>-inf/MIN | nan/+0/-0 | nan | +0.0 |
| half | nan/arbitrary value | +0/-0/MAX/MIN/nan/+inf/-inf | nan | +0.0 |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 29** Calculation result description for special value/boundary value input of MulAddDst

<a name="table31201353184118"></a>

Note: In the following table, MAX/MIN correspond to the maximum and minimum values under the data type of the corresponding operand.

| Data Type of src/dst | src0 value/src1 value | dst value | dst value<br> (INF/NAN Mode) | dst value<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| float | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | MAX | MAX | MAX |
| float | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | MIN | MIN | MIN |
| float | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | nan | nan | nan |
| float | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | -0/+0 | 0.0 | 0.0 |
| float | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | +inf | inf | inf |
| float | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | -inf | -inf | -inf |
| float | +0/inf    -0/-inf<br>+0/-inf   -0/inf<br>+0/nan   -0/nan | +0/MAX/-0/MIN/nan/+inf/-inf | nan | nan |
| float | MAX/MAX<br>MIN/MIN | +0/MAX/-0/MIN/inf | inf | inf |
| float | MAX/MAX<br>MIN/MIN | -inf | -inf | -inf |
| float | MAX/MAX<br>MIN/MIN | nan | nan | nan |
| float | MAX/MIN | +0/MAX/-0/MIN/-inf | -inf | -inf |
| float | MAX/MIN | inf | inf | inf |
| float | MAX/MIN | nan | nan | nan |
| float | MAX/+inf<br>MIN/-inf | +0/-0/MAX/MIN/+inf | inf | inf |
| float | MAX/+inf<br>MIN/-inf | -inf/nan | nan | nan |
| float | MAX/-inf<br>MIN/+inf | +0/-0/MAX/MIN/-inf | -inf | -inf |
| float | MAX/-inf<br>MIN/+inf | +inf/nan | nan | nan |
| float | inf/inf | +0/-0/MAX/MIN/+inf | inf | inf |
| float | inf/inf | nan/-inf | nan | nan |
| float | inf/-inf | +0/-0/MAX/MIN/-inf | -inf | -inf |
| float | inf/-inf | nan/+inf | nan | nan |
| float | -inf/-inf | +0/-0/MAX/MIN/+inf | -inf | -inf |
| float | -inf/-inf | nan/-inf | nan | nan |
| float | nan/arbitrary value | +0/-0/MAX/MIN/nan/+inf/-inf | nan | nan |
| half | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | MAX | MAX | MAX |
| half | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | MIN | MIN | MIN |
| half | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | nan | nan | 0.0 |
| half | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | -0/+0 | 0.0 | 0.0 |
| half | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | +inf | inf | MAX |
| half | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | -inf | -inf | MIN |
| half | +0/inf    -0/-inf<br>+0/-inf   -0/inf<br>+0/nan   -0/nan | +0/MAX/-0/MIN/nan/+inf/-inf | nan | 0.0 |
| half | MAX/MAX<br>MIN/MIN | +0/MAX/-0/MIN/inf | inf | MAX |
| half | MAX/MAX<br>MIN/MIN | -inf | -inf | MIN |
| half | MAX/MAX<br>MIN/MIN | nan | nan | 0.0 |
| half | MAX/MIN | +0/MAX/-0/MIN/-inf | -inf | MIN |
| half | MAX/MIN | inf | inf | MAX |
| half | MAX/MIN | nan | nan | 0.0 |
| half | MAX/+inf<br>MIN/-inf | +0/-0/MAX/MIN/+inf | inf | MAX |
| half | MAX/+inf<br>MIN/-inf | -inf/nan | nan | 0.0 |
| half | MAX/-inf<br>MIN/+inf | +0/-0/MAX/MIN/-inf | -inf | MIN |
| half | MAX/-inf<br>MIN/+inf | +inf/nan | nan | 0.0 |
| half | inf/inf | +0/-0/MAX/MIN/+inf | inf | MAX |
| half | inf/inf | nan/-inf | nan | 0.0 |
| half | inf/-inf | +0/-0/MAX/MIN/-inf | -inf | MIN |
| half | inf/-inf | nan/+inf | nan | 0.0 |
| half | -inf/-inf | +0/-0/MAX/MIN/+inf | -inf | MIN |
| half | -inf/-inf | nan/-inf | nan | 0.0 |
| half | nan/arbitrary value | +0/-0/MAX/MIN/nan/+inf/-inf | nan | 0.0 |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 30** Calculation result description for special value/boundary value input of MulsCast

<a name="table122760369423"></a>

| Data Type of src/dst | Element Values of src0 | Element Values of src1 | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| float/half | MIN | nan | nan | 0.0 |
| float/half | MIN | -0.0 | 0.0 | 0.0 |
| float/half | MIN | 0.0 | -0.0 | -0.0 |
| float/half | MIN | -inf/MIN | inf | MAX |
| float/half | MIN | inf/MAX | -inf | MIN |
| float/half | MAX | nan | nan | 0.0 |
| float/half | MAX | -0.0 | -0.0 | -0.0 |
| float/half | MAX | 0.0 | 0.0 | 0.0 |
| float/half | MAX | -inf/MIN | -inf | MIN |
| float/half | MAX | inf/MAX | inf | MAX |
| float/half | 0.0 | 0.0 | 0.0 | 0.0 |
| float/half | 0.0 | inf/-inf/nan | nan | 0.0 |
| float/half | 0.0 | -0.0 | -0.0 | -0.0 |
| float/half | -0.0 | 0.0 | -0.0 | -0.0 |
| float/half | -0.0 | inf/-inf/nan | nan | 0.0 |
| float/half | -0.0 | -0.0 | 0.0 | 0.0 |
| float/half | nan | nan/inf/-inf/0/-0/max/min | nan | 0.0 |
| float/half | inf | -inf/0/-0/max/min | inf | MAX |
| float/half | inf | nan/inf | nan | 0.0 |
| float/half | -inf | -inf/MIN | inf | MAX |
| float/half | -inf | inf/MAX | -inf | MIN |
| float/half | -inf | 0/-0/nan | nan | 0.0 |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

## Comparison and selection

**Table 31** Calculation results of Compare with special value/boundary value input

<a name="table1961220798"></a>

| src data type/src1 data type | cmpMode comparison mode | Data Type of src0 | src1 data type | dst output data (INF/NAN Mode) | dst output data (Saturation Mode) |
| --- | --- | --- | --- | --- | --- |
| half/half | CMPMODE::EQ | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/0 | 1/0/0/0/0/0/0 |
| half/half | CMPMODE::EQ | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/0/0 | 0/1/0/0/0/0/0 |
| half/half | CMPMODE::EQ | +0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| half/half | CMPMODE::EQ | -0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| half/half | CMPMODE::EQ | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::EQ | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::EQ | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GE | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GE | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::GE | +0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GE | -0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GE | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::GE | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| half/half | CMPMODE::GE | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GT | +0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | -0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::GT | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::GT | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LE | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LE | +0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | -0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LE | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::LE | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| half/half | CMPMODE::LT | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LT | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::LT | +0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LT | -0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LT | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LT | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LT | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| half/half | CMPMODE::NE | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/1/1/1 | 0/1/1/1/1/1/1 |
| half/half | CMPMODE::NE | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/1/1/1 | 1/0/1/1/1/1/1 |
| half/half | CMPMODE::NE | +0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/0/0/1/1/1 | 1/1/0/0/1/1/1 |
| half/half | CMPMODE::NE | -0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/0/0/1/1/1 | 1/1/0/0/1/1/1 |
| half/half | CMPMODE::NE | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/1/1/1 | 1/1/1/1/1/1/1 |
| half/half | CMPMODE::NE | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/1/0/1 | 1/1/1/1/1/0/1 |
| half/half | CMPMODE::NE | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/1/1/0 | 1/1/1/1/1/1/0 |
| float/float | CMPMODE::EQ | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/0 | 1/0/0/0/0/0/0 |
| float/float | CMPMODE::EQ | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/0/0 | 0/1/0/0/0/0/0 |
| float/float | CMPMODE::EQ | +0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| float/float | CMPMODE::EQ | -0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| float/float | CMPMODE::EQ | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::EQ | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::EQ | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GE | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GE | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::GE | +0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GE | -0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GE | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::GE | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| float/float | CMPMODE::GE | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GT | +0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | -0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::GT | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::GT | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LE | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LE | +0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | -0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LE | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::LE | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| float/float | CMPMODE::LT | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LT | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::LT | +0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LT | -0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LT | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LT | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LT | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::NE | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/1/1/1 | 0/1/1/1/1/1/1 |
| float/float | CMPMODE::NE | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/1/1/1 | 1/0/1/1/1/1/1 |
| float/float | CMPMODE::NE | +0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/0/0/1/1/1 | 1/1/0/0/1/1/1 |
| float/float | CMPMODE::NE | -0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/0/0/1/1/1 | 1/1/0/0/1/1/1 |
| float/float | CMPMODE::NE | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/1/1/1 | 1/1/1/1/1/1/1 |
| float/float | CMPMODE::NE | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/1/0/1 | 1/1/1/1/1/0/1 |
| float/float | CMPMODE::NE | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/1/1/0 | 1/1/1/1/1/1/0 |
| int32_t/int32_t | CMPMODE::EQ | -2147483648 (MIN) | -2147483648 (MIN)/2147483647 (MAX) | 1/0 | 1/0 |
| int32_t/int32_t | CMPMODE::EQ | 2147483647 (MAX) | -2147483648 (MIN)/2147483647 (MAX) | 0/1 | 0/1 |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 32** Calculation results for special value/boundary value inputs of Compares

<a name="table43881347182311"></a>

| src data type/src1 data type | cmpMode comparison mode | Data Type of src0 | src1 data type | dst output data (INF/NAN Mode) | dst output data (Saturation Mode) |
| --- | --- | --- | --- | --- | --- |
| half/half | CMPMODE::EQ | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/0 | 1/0/0/0/0/0/0 |
| half/half | CMPMODE::EQ | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/0/0 | 0/1/0/0/0/0/0 |
| half/half | CMPMODE::EQ | +0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| half/half | CMPMODE::EQ | -0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| half/half | CMPMODE::EQ | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::EQ | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::EQ | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GE | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GE | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::GE | +0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GE | -0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GE | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::GE | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| half/half | CMPMODE::GE | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GT | +0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | -0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::GT | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::GT | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LE | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LE | +0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | -0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LE | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::LE | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| half/half | CMPMODE::LT | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LT | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::LT | +0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LT | -0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LT | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LT | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LT | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| half/half | CMPMODE::NE | -65504.0 (MIN) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/1/1/1 | 0/1/1/1/1/1/1 |
| half/half | CMPMODE::NE | 65504.0 (MAX) | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/1/1/1 | 1/0/1/1/1/1/1 |
| half/half | CMPMODE::NE | +0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/0/0/1/1/1 | 1/1/0/0/1/1/1 |
| half/half | CMPMODE::NE | -0.0 | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/0/0/1/1/1 | 1/1/0/0/1/1/1 |
| half/half | CMPMODE::NE | nan | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/1/1/1 | 1/1/1/1/1/1/1 |
| half/half | CMPMODE::NE | +inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/1/0/1 | 1/1/1/1/1/0/1 |
| half/half | CMPMODE::NE | -inf | -65504.0 (MIN)/65504.0 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/1/1/0 | 1/1/1/1/1/1/0 |
| float/float | CMPMODE::EQ | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/0 | 1/0/0/0/0/0/0 |
| float/float | CMPMODE::EQ | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/0/0 | 0/1/0/0/0/0/0 |
| float/float | CMPMODE::EQ | +0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| float/float | CMPMODE::EQ | -0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| float/float | CMPMODE::EQ | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::EQ | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::EQ | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GE | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GE | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::GE | +0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GE | -0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GE | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::GE | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| float/float | CMPMODE::GE | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GT | +0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | -0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::GT | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::GT | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LE | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LE | +0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | -0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LE | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::LE | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| float/float | CMPMODE::LT | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LT | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::LT | +0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LT | -0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LT | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LT | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LT | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::NE | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 0/1/1/1/1/1/1 | 0/1/1/1/1/1/1 |
| float/float | CMPMODE::NE | 3.4028235e+38 (MAX) | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/0/1/1/1/1/1 | 1/0/1/1/1/1/1 |
| float/float | CMPMODE::NE | +0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/0/0/1/1/1 | 1/1/0/0/1/1/1 |
| float/float | CMPMODE::NE | -0.0 | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/0/0/1/1/1 | 1/1/0/0/1/1/1 |
| float/float | CMPMODE::NE | nan | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/1/1/1 | 1/1/1/1/1/1/1 |
| float/float | CMPMODE::NE | +inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/1/0/1 | 1/1/1/1/1/0/1 |
| float/float | CMPMODE::NE | -inf | -3.4028235e+38 (MIN)/3.4028235e+38 (MAX)/+0/-0/nan/+inf/-inf | 1/1/1/1/1/1/0 | 1/1/1/1/1/1/0 |
| int32_t/int32_t | CMPMODE::EQ | -2147483648 (MIN) | -2147483648 (MIN)/2147483647 (MAX) | 1/0 | 1/0 |
| int32_t/int32_t | CMPMODE::EQ | 2147483647 (MAX) | -2147483648 (MIN)/2147483647 (MAX) | 0/1 | 0/1 |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

## Type Conversion

**Table 33** Truncate special value/boundary value input calculation result description

<a name="table1730814818583"></a>

| Data Type of src/dst | Element Values of scalar | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- |
| half | inf | inf | inf |
| half | -inf | -inf | -inf |
| half | 0/-0 | 0.0 | 0.0 |
| half | nan | nan | nan |
| half | MAX | MAX | MAX |
| half | MIN | MIN | MIN |
| float | inf | inf | inf |
| float | -inf | -inf | -inf |
| float | 0/-0 | 0.0 | 0.0 |
| float | nan | nan | nan |
| float | MAX | MAX | MAX |
| float | MIN | MIN | MIN |
| bfloat16 | inf | inf | inf |
| bfloat16 | -inf | -inf | -inf |
| bfloat16 | 0/-0 | 0.0 | 0.0 |
| bfloat16 | nan | nan | nan |
| bfloat16 | MAX | MAX | MAX |
| bfloat16 | MIN | MIN | MIN |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 34** Cast special value/boundary value input calculation result

<a name="table1225292414418"></a>

| src data type/dst data type | src input data | dst output data (INF/NAN Mode) | dst output data (Saturation Mode) |
| --- | --- | --- | --- |
| int4b_t/half, int4b_t/int16_t, int4b_t/bfloat16_t | MIN | -8 | -8 |
| int4b_t/half, int4b_t/int16_t, int4b_t/bfloat16_t | MAX | 7 | 7 |
| int8_t/int16_t, int8_t/half,<br>int8_t/int32_t | MIN | -128 | -128 |
| int8_t/int16_t, int8_t/half,<br>int8_t/int32_t | MAX | 127 | 127 |
| uint8_t/uint16_t, uint8_t/half, uint8_t/uint32_t, int16_t/uint8_t, int32_t/uint8_t, uint16_t/uint8_t,<br>uint32_t/uint8_t | 0.0 | 0.0 | 0.0 |
| uint8_t/uint16_t, uint8_t/half, uint8_t/uint32_t, int16_t/uint8_t, int32_t/uint8_t, uint16_t/uint8_t,<br>uint32_t/uint8_t | MAX | 255 | 255 |
| int16_t/uint32_t | MIN | 0.0 | 0.0 |
| int16_t/uint32_t | MAX | 32767 | 32767 |
| int16_t/int4b_t | MIN | 0.0 | -8 |
| int16_t/int4b_t | MAX | -1 | 7 |
| int16_t/int32_t, int16_t/float | MIN | -32768 | -32768 |
| int16_t/int32_t, int16_t/float | MAX | 32767 | 32767 |
| int16_t/half | MIN | -32768 | -32768 |
| int16_t/half | MAX | The output is different under different rounding modes. In CAST_RINT/CAST_CEIL/CAST_ROUND modes, the output is 32768.00; in CAST_FLOOR/CAST_TRUNC modes, the output is 32752.00 | The output is different under different rounding modes. In CAST_RINT/CAST_CEIL/CAST_ROUND modes, the output is 32768.00; in CAST_FLOOR/CAST_TRUNC modes, the output is 32752.00 |
| uint16_t/uint32_t, uint32_t/uint16_t | 0.0 | 0.0 | 0.0 |
| uint16_t/uint32_t, uint32_t/uint16_t | MAX | 65535 | 65535 |
| int32_t/int16_t | MIN | 0.0 | -32768 |
| int32_t/int16_t | MAX | -1 | 32767 |
| int32_t/float | MIN | -2147483648 | -2147483648 |
| int32_t/float | MAX | The output is different under different rounding modes. In CAST_RINT/CAST_CEIL/CAST_ROUND modes, the output is 2147483648.00; in CAST_FLOOR/CAST_TRUNC modes, the output is 2147483520.00 | The output is different under different rounding modes. In CAST_RINT/CAST_CEIL/CAST_ROUND modes, the output is 2147483648.00; in CAST_FLOOR/CAST_TRUNC modes, the output is 2147483520.00 |
| int32_t/int64_t | MIN | -2147483648 | -2147483648 |
| int32_t/int64_t | MAX | 2147483647 | 2147483647 |
| uint32_t/int16_t | 0.0 | 0.0 | 0.0 |
| uint32_t/int16_t | MAX | -1 | 32767 |
| int64_t/int32_t | MIN | 0.0 | -2147483648 |
| int64_t/int32_t | MAX | -1 | 2147483647 |
| int64_t/float | MIN | -9223372036854775808.00 | -9223372036854775808.00 |
| int64_t/float | MAX | The output is different under different rounding modes. In CAST_RINT/CAST_CEIL/CAST_ROUND modes, the output is 9223372036854775808.00; in CAST_FLOOR/CAST_TRUNC modes, the output is 9223371487098961920.00 | The output is different under different rounding modes. In CAST_RINT/CAST_CEIL/CAST_ROUND modes, the output is 9223372036854775808.00; in CAST_FLOOR/CAST_TRUNC modes, the output is 9223371487098961920.00 |
| float/int16_t | 0.0 | 0.0 | 0.0 |
| float/int16_t | MIN | 0.0 | -32768 |
| float/int16_t | MAX | 0.0 | 32767 |
| float/int16_t | inf | 32767 | 32767 |
| float/int16_t | -inf | -32768 | -32768 |
| float/int16_t | nan | 0.0 | 0.0 |
| float/int32_t | 0.0 | 0.0 | 0.0 |
| float/int32_t | MIN | 0.0 | -2147483648 |
| float/int32_t | MAX | 0.0 | 2147483647 |
| float/int32_t | inf | 2147483647 | 2147483647 |
| float/int32_t | -inf | -2147483648 | -2147483648 |
| float/int32_t | nan | 0.0 | 0.0 |
| float/bfloat16_t | 0.0 | 0.0 | 0.0 |
| float/bfloat16_t | MIN | The output is different under different rounding modes. In CAST_RINT/CAST_FLOOR/CAST_ROUND modes, the output is -inf; in CAST_CEIL/CAST_TRUNC modes, the output is -3.3895314e+38 | -3.3895314e+38 |
| float/bfloat16_t | MAX | The output is different under different rounding modes. In CAST_RINT/CAST_CEIL/CAST_ROUND modes, the output is inf; in CAST_FLOOR/CAST_TRUNC modes, the output is 3.3895314e+38 | 3.3895314e+38 |
| float/bfloat16_t | inf | inf | 3.3895314e+38 |
| float/bfloat16_t | -inf | -inf | -3.3895314e+38 |
| float/bfloat16_t | nan | nan | 0.0 |
| float/hifloat8_t | 0.0 | 0.0 | 0.0 |
| float/hifloat8_t | MIN | -inf | -32768 |
| float/hifloat8_t | MAX | inf | 32768 |
| float/hifloat8_t | inf | inf | 32768 |
| float/hifloat8_t | -inf | -inf | -32768 |
| float/hifloat8_t | nan | nan | 0.0 |
| float/half | 0.0 | 0.0 | 0.0 |
| float/half | MIN | -inf | -65504.0 |
| float/half | MAX | inf | 65504.0 |
| float/half | inf | inf | 65504.0 |
| float/half | -inf | -inf | -65504.0 |
| float/half | nan | nan | 0.0 |
| float/fp8_e4m3fn_t | 0.0 | 0.0 | 0.0 |
| float/fp8_e4m3fn_t | MIN | nan | -448 |
| float/fp8_e4m3fn_t | MAX | nan | 448 |
| float/fp8_e4m3fn_t | inf | nan | 448 |
| float/fp8_e4m3fn_t | -inf | nan | -448 |
| float/fp8_e4m3fn_t | nan | nan | 0.0 |
| float/fp8_e5m2_t | 0.0 | 0.0 | 0.0 |
| float/fp8_e5m2_t | MIN | -inf | -57344 |
| float/fp8_e5m2_t | MAX | inf | 57344 |
| float/fp8_e5m2_t | inf | inf | 57344 |
| float/fp8_e5m2_t | -inf | -inf | -57344 |
| float/fp8_e5m2_t | nan | nan | 0.0 |
| fp8_e5m2_t/float | 0.0 | 0.0 | 0.0 |
| fp8_e5m2_t/float | MIN | -57344 | -57344 |
| fp8_e5m2_t/float | MAX | 57344 | 57344 |
| fp8_e5m2_t/float | inf | inf | inf |
| fp8_e5m2_t/float | -inf | -inf | -inf |
| fp8_e5m2_t/float | nan | nan | nan |
| fp8_e4m3fn_t/float | 0.0 | 0.0 | 0.0 |
| fp8_e4m3fn_t/float | MIN | -448 | -448 |
| fp8_e4m3fn_t/float | MAX | 448 | 448 |
| fp8_e4m3fn_t/float | inf | inf | inf |
| fp8_e4m3fn_t/float | -inf | -inf | -inf |
| fp8_e4m3fn_t/float | nan | nan | nan |
| hifloat8_t/float | 0.0 | 0.0 | 0.0 |
| hifloat8_t/float | MIN | -32768 | -32768 |
| hifloat8_t/float | MAX | 32768 | 32768 |
| hifloat8_t/float | inf | inf | inf |
| hifloat8_t/float | -inf | -inf | -inf |
| hifloat8_t/float | nan | nan | nan |
| hifloat8_t/half | 0.0 | 0.0 | 0.0 |
| hifloat8_t/half | MIN | -32768 | -32768 |
| hifloat8_t/half | MAX | 32768 | 32768 |
| hifloat8_t/half | inf | inf | inf |
| hifloat8_t/half | -inf | -inf | -inf |
| hifloat8_t/half | nan | nan | nan |
| half/hifloat8_t | 0.0 | 0.0 | 0.0 |
| half/hifloat8_t | MIN | -inf | -32768 |
| half/hifloat8_t | MAX | inf | 32768 |
| half/hifloat8_t | inf | inf | 32768 |
| half/hifloat8_t | -inf | -inf | -32768 |
| half/hifloat8_t | nan | nan | 0.0 |
| half/int32_t | 0.0 | 0.0 | 0.0 |
| half/int32_t | MIN | -65504.0 | -65504 |
| half/int32_t | MAX | 65504.0 | 65504 |
| half/int32_t | inf | 2147483647 | 2147483647 |
| half/int32_t | -inf | -2147483648 | -2147483648 |
| half/int32_t | nan | 0.0 | 0.0 |
| half/int16_t | 0.0 | 0.0 | 0.0 |
| half/int16_t | MIN | 32 | -32768 |
| half/int16_t | MAX | -32 | 32767 |
| half/int16_t | inf | 32767 | 32767 |
| half/int16_t | -inf | -32768 | -32768 |
| half/int16_t | nan | 0.0 | 0.0 |
| half/bfloat16_t | 0.0 | 0.0 | 0.0 |
| half/bfloat16_t | MIN | -65504.0 | -65504.0 |
| half/bfloat16_t | MAX | 65504.0 | 65504.0 |
| half/bfloat16_t | inf | inf | inf |
| half/bfloat16_t | -inf | -inf | -inf |
| half/bfloat16_t | nan | nan | nan |
| half/int8_t | 0.0 | 0.0 | 0.0 |
| half/int8_t | MIN | 32 | -128 |
| half/int8_t | MAX | -32 | 127 |
| half/int8_t | inf | 127 | 127 |
| half/int8_t | -inf | -128 | -128 |
| half/int8_t | nan | 0.0 | 0.0 |
| half/uint8_t | 0.0 | 0.0 | 0.0 |
| half/uint8_t | MIN | 32 | 0.0 |
| half/uint8_t | MAX | 224 | 255 |
| half/uint8_t | inf | 255 | 255 |
| half/uint8_t | -inf | 0.0 | 0.0 |
| half/uint8_t | nan | 0.0 | 0.0 |
| half/float | 0.0 | 0.0 | 0.0 |
| half/float | MIN | -65504.0 | -65504.0 |
| half/float | MAX | 65504.0 | 65504.0 |
| half/float | inf | inf | inf |
| half/float | -inf | -inf | -inf |
| half/float | nan | nan | nan |
| bfloat16_t/int32_t | 0.0 | 0.0 | 0.0 |
| bfloat16_t/int32_t | MIN | 0.0 | -2147483648 |
| bfloat16_t/int32_t | MAX | 0.0 | 2147483647 |
| bfloat16_t/int32_t | inf | 2147483647 | 2147483647 |
| bfloat16_t/int32_t | -inf | -2147483648 | -2147483648 |
| bfloat16_t/int32_t | nan | 0.0 | 0.0 |
| bfloat16_t/half | 0.0 | 0.0 | 0.0 |
| bfloat16_t/half | MIN | -inf | -65504.0 |
| bfloat16_t/half | MAX | inf | 65504.0 |
| bfloat16_t/half | inf | inf | 65504.0 |
| bfloat16_t/half | -inf | -inf | -65504.0 |
| bfloat16_t/half | nan | nan | 0.0 |
| bfloat16_t/float | 0.0 | 0.0 | 0.0 |
| bfloat16_t/float | MIN | -3.3895314e+38 | -3.3895314e+38 |
| bfloat16_t/float | MAX | 3.3895314e+38 | 3.3895314e+38 |
| bfloat16_t/float | inf | inf | inf |
| bfloat16_t/float | -inf | -inf | -inf |
| bfloat16_t/float | nan | nan | nan |
| bfloat16_t/fp4x2_e2m1_t | 0.0 | 0.0 | 0.0 |
| bfloat16_t/fp4x2_e2m1_t | MIN | -6 | -6 |
| bfloat16_t/fp4x2_e2m1_t | MAX | 6 | 6 |
| bfloat16_t/fp4x2_e2m1_t | inf | 6 | 6 |
| bfloat16_t/fp4x2_e2m1_t | -inf | -6 | -6 |
| bfloat16_t/fp4x2_e2m1_t | nan | 0.0 | 0.0 |
| bfloat16_t/fp4x2_e1m2_t | 0.0 | 0.0 | 0.0 |
| bfloat16_t/fp4x2_e1m2_t | MIN | -1.75 | -1.75 |
| bfloat16_t/fp4x2_e1m2_t | MAX | 1.75 | 1.75 |
| bfloat16_t/fp4x2_e1m2_t | inf | 1.75 | 1.75 |
| bfloat16_t/fp4x2_e1m2_t | -inf | -1.75 | -1.75 |
| bfloat16_t/fp4x2_e1m2_t | nan | 0.0 | 0.0 |
| bfloat16_t/fp8_e8m0_t | 0.0 | 0.0 | 0.0 |
| bfloat16_t/fp8_e8m0_t | MIN | 1.70141e+38 | 1.70141e+38 |
| bfloat16_t/fp8_e8m0_t | MAX | 1.70141e+38 | 1.70141e+38 |
| bfloat16_t/fp8_e8m0_t | inf | nan | nan |
| bfloat16_t/fp8_e8m0_t | -inf | nan | nan |
| bfloat16_t/fp8_e8m0_t | nan | nan | nan |
| fp4x2_e2m1_t/bfloat16_t | MIN | -6 | -6 |
| fp4x2_e2m1_t/bfloat16_t | MAX | 6 | 6 |
| fp4x2_e1m2_t/bfloat16_t | MIN | -1.75 | -1.75 |
| fp4x2_e1m2_t/bfloat16_t | MAX | 1.75 | 1.75 |
| fp8_e8m0_t/bfloat16_t | MIN | 0 | 0 |
| fp8_e8m0_t/bfloat16_t | MAX | 1.70141e+38 | 1.70141e+38 |
| fp8_e8m0_t/bfloat16_t | nan | nan | nan |


Note: The results in the element values of dst (INF/NAN mode) column that involve floating-point type conversion are consistent with the IEEE 754 theoretical result.

## Reduction Computation

**Table 35** Computation results of Reduce with special value/boundary value input

<a name="table491948105210"></a>

| ReduceType type | Data Type of src/dst | Element Values of src | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| max | half | -65504.0 (MIN) | -65504.0 | -65504.0 |
| max | half | 65504.0 (MAX) | 65504.0 | 65504.0 |
| max | half | 0.0 | 0.0 | 0.0 |
| max | half | -0.0 | -0.0 | -0.0 |
| max | half | nan | nan | 0.0 |
| max | half | inf | inf | 65504.0 |
| max | half | -inf | -inf | -65504.0 |
| max | float | -3.4028235e+38 (MIN) | -3.4028235e+38 | -3.4028235e+38 |
| max | float | 3.4028235e+38 (MAX) | 3.4028235e+38 | 3.4028235e+38 |
| max | float | 0.0 | 0.0 | 0.0 |
| max | float | -0.0 | -0.0 | -0.0 |
| max | float | nan | nan | nan |
| max | float | inf | inf | inf |
| max | float | -inf | -inf | -inf |
| min | half | -65504.0 (MIN) | -65504.0 | -65504.0 |
| min | half | 65504.0 (MAX) | 65504.0 | 65504.0 |
| min | half | 0.0 | 0.0 | 0.0 |
| min | half | -0.0 | -0.0 | -0.0 |
| min | half | nan | nan | 0.0 |
| min | half | inf | inf | 65504.0 |
| min | half | -inf | -inf | -65504.0 |
| min | float | -3.4028235e+38 (MIN) | -3.4028235e+38 | -3.4028235e+38 |
| min | float | 3.4028235e+38 (MAX) | 3.4028235e+38 | 3.4028235e+38 |
| min | float | 0.0 | 0.0 | 0.0 |
| min | float | -0.0 | -0.0 | -0.0 |
| min | float | nan | nan | nan |
| min | float | inf | inf | inf |
| min | float | -inf | -inf | -inf |
| sum | half | -65504.0 (MIN) | -inf | -65504.0 |
| sum | half | 65504.0 (MAX) | inf | 65504.0 |
| sum | half | 0.0 | 0.0 | 0.0 |
| sum | half | -0.0 | -0.0 | -0.0 |
| sum | half | nan | nan | 0.0 |
| sum | half | inf | inf | 65504.0 |
| sum | half | -inf | -inf | -65504.0 |
| sum | float | -3.4028235e+38 (MIN) | -inf | -inf |
| sum | float | 3.4028235e+38 (MAX) | inf | inf |
| sum | float | 0.0 | 0.0 | 0.0 |
| sum | float | -0.0 | -0.0 | -0.0 |
| sum | float | nan | nan | nan |
| sum | float | inf | inf | inf |
| sum | float | -inf | -inf | -inf |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 36** Computation results of ReduceDataBlock with special value/boundary value input

<a name="table1532212314305"></a>

| ReduceType type | Data Type of src/dst | Element Values of src | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| max | half | -65504.0 (MIN) | -65504.0 | -65504.0 |
| max | half | 65504.0 (MAX) | 65504.0 | 65504.0 |
| max | half | 0.0 | 0.0 | 0.0 |
| max | half | -0.0 | -0.0 | -0.0 |
| max | half | nan | nan | 0.0 |
| max | half | inf | inf | 65504.0 |
| max | half | -inf | -inf | -65504.0 |
| max | float | -3.4028235e+38 (MIN) | -3.4028235e+38 | -3.4028235e+38 |
| max | float | 3.4028235e+38 (MAX) | 3.4028235e+38 | 3.4028235e+38 |
| max | float | 0.0 | 0.0 | 0.0 |
| max | float | -0.0 | -0.0 | -0.0 |
| max | float | nan | nan | nan |
| max | float | inf | inf | inf |
| max | float | -inf | -inf | -inf |
| min | half | -65504.0 (MIN) | -65504.0 | -65504.0 |
| min | half | 65504.0 (MAX) | 65504.0 | 65504.0 |
| min | half | 0.0 | 0.0 | 0.0 |
| min | half | -0.0 | -0.0 | -0.0 |
| min | half | nan | nan | 0.0 |
| min | half | inf | inf | 65504.0 |
| min | half | -inf | -inf | -65504.0 |
| min | float | -3.4028235e+38 (MIN) | -3.4028235e+38 | -3.4028235e+38 |
| min | float | 3.4028235e+38 (MAX) | 3.4028235e+38 | 3.4028235e+38 |
| min | float | 0.0 | 0.0 | 0.0 |
| min | float | -0.0 | -0.0 | -0.0 |
| min | float | nan | nan | nan |
| min | float | inf | inf | inf |
| min | float | -inf | -inf | -inf |
| sum | half | -65504.0 (MIN) | -inf | -65504.0 |
| sum | half | 65504.0 (MAX) | inf | 65504.0 |
| sum | half | 0.0 | 0.0 | 0.0 |
| sum | half | -0.0 | -0.0 | -0.0 |
| sum | half | nan | nan | 0.0 |
| sum | half | inf | inf | 65504.0 |
| sum | half | -inf | -inf | -65504.0 |
| sum | float | -3.4028235e+38 (MIN) | -inf | -inf |
| sum | float | 3.4028235e+38 (MAX) | inf | inf |
| sum | float | 0.0 | 0.0 | 0.0 |
| sum | float | -0.0 | -0.0 | -0.0 |
| sum | float | nan | nan | nan |
| sum | float | inf | inf | inf |
| sum | float | -inf | -inf | -inf |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

**Table 37** Computation results of PairReduceElem with special value/boundary value input

<a name="table1866163012710"></a>

| ReduceType type | Data Type of src/dst | Element Values of src | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- | --- |
| Sum | half | -65504.0 (MIN) | -inf | -65504.0 |
| Sum | half | 65504.0 (MAX) | inf | 65504.0 |
| Sum | half | 0.0 | 0.0 | 0.0 |
| Sum | half | -0.0 | -0.0 | -0.0 |
| Sum | half | nan | nan | 0.0 |
| Sum | half | inf | inf | 65504.0 |
| Sum | half | -inf | -inf | -65504.0 |
| Sum | float | -3.4028235e+38 (MIN) | -inf | -inf |
| Sum | float | 3.4028235e+38 (MAX) | inf | inf |
| Sum | float | 0.0 | 0.0 | 0.0 |
| Sum | float | -0.0 | -0.0 | -0.0 |
| Sum | float | nan | nan | nan |
| Sum | float | inf | inf | inf |
| Sum | float | -inf | -inf | -inf |


Note: The results in the Element Values of dst (INF/NAN Mode) column are consistent with the IEEE 754 theoretical result.

## index operation

**Table 38** Result description of Arange special value/boundary value input calculation

<a name="table10167938144218"></a>

| Data Type of src/dst | Element Values of scalar | Element Values of dst<br> (INF/NAN Mode) | Element Values of dst<br> (Saturation Mode) |
| --- | --- | --- | --- |
| half | inf | inf | 65504.0 |
| half | -inf | -inf | -65504.0 |
| half | 0/-0 | 0~127 | 0~127 |
| half | nan | nan | 0.0 |
| half | MAX | 16 elements of 65504, and the rest are inf | 65504.0 |
| half | MIN | 16 elements of -65504, 32 elements of -65472, 32 elements of -65440, 32 elements of -65408, and 16 elements of -65376 | 16 elements of -65504, 32 elements of -65472, 32 elements of -65440, 32 elements of -65408, and 16 elements of -65376 |
| float | inf | inf | inf |
| float | -inf | -inf | -inf |
| float | 0/-0 | 0~63 | 0~63 |
| float | nan | nan | nan |
| float | MAX | MAX | MAX |
| float | MIN | MIN | MIN |
| int8_t | 127 (MAX) | 127 (MAX), -128~126 | 127 (MAX), -128~126 |
| int8_t | -128 (MIN) | -128 (MIN)~127 | -128 (MIN)~127 |
| int16_t | 32767 (MAX) | 32767 (MAX), -32768 (MIN)~-32642 | 32767 (MAX), -32768 (MIN)~-32642 |
| int16_t | -32768 (MIN) | -32768 (MIN)~-32641 | -32768 (MIN)~-32641 |
| int32_t | -2147483648 (MIN) | -2147483648 (MIN)~-2147483585 | -2147483648 (MIN)~-2147483585 |
| int32_t | 2147483647 (MAX) | 2147483647 (MAX), -2147483648 (MIN)~-2147483586 | 2147483647 (MAX), -2147483648 (MIN)~-2147483586 |
