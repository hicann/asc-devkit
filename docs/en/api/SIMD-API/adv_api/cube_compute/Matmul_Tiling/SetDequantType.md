# SetDequantType

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-02T07:13:04.649Z pushedAt=2026-11-18T04:55:25.308Z nl=2 -->

## Description

This API is used to set the quantization or dequantization mode.

Matmul dequantization scenario: During Matmul computation, the input of the left and right cubes is of type int8\_t or int4b\_t, and the output is of type half; or the input of the left and right cubes is of type int8\_t, and the output is of type int8\_t. In this scenario, when the data of the output C cube is moved from the L0C Buffer (CO1) to Global Memory, a dequantization operation is performed to dequantize the final result into the corresponding half or int8\_t type.

Matmul quantization scenario: During Matmul computation, the input of the left and right cubes is of type half or bfloat16\_t, and the output is of type int8\_t. In this scenario, when the data of the output C cube is moved from the L0C Buffer (CO1) to Global Memory, a quantization operation is performed to quantize the final result into the int8\_t type.

There are two modes for quantization or dequantization: one is the quantization/dequantization mode with the same coefficient, and the other is the vector quantization/dequantization mode.

-   Quantization or dequantization mode with the same coefficient: The same coefficient is used to quantize or dequantize all values of the output cube.
-   Vector quantization or dequantization mode: A parameter vector is provided, and each column of the output cube is quantized or dequantized using the coefficient of the corresponding column in the vector.

## Prototype

```
int32_t SetDequantType(DequantType dequantType)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dequantType | Input | Sets the mode for quantization or dequantization. The type is **DequantType**, whose definition is shown in the following code. The values and meanings of the parameters are as follows:<br>**SCALAR**: Indicates the quantization or dequantization mode with the same coefficient.<br>**TENSOR**: Indicates the vector quantization or dequantization mode. |

```
enum class DequantType {
    SCALAR = 0,
    TENSOR = 1,
};
```

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

## Constraints

The quantization/dequantization mode with the same coefficient and the vector quantization/dequantization mode supported by this API correspond to the kernel-side APIs [SetQuantScalar](../Matmul_Kernel/SetQuantScalar.md) and [SetQuantVector](../Matmul_Kernel/SetQuantVector.md), respectively. The quantization/dequantization mode set by this API must be consistent with the API used on the kernel side.

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
tiling.SetShape(M, N, K);
tiling.SetOrgShape(M, N, K);
tiling.EnableBias(true);
tiling.SetDequantType(DequantType::SCALAR); // Set the quantization/dequantization mode with the same coefficient.
// tiling.SetDequantType(DequantType::TENSOR);  // Set the vector quantization/dequantization mode.
tiling.SetBufferSpace(-1, -1, -1);
optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);
```
