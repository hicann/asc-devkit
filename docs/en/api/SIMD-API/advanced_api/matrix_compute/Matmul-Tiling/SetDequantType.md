# SetDequantType

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:23:39.034Z pushedAt=2026-09-12T09:55:18.197Z -->

## Description

This API sets the quantization or dequantization mode.

Matmul dequantization scenario: During Matmul computation, the input of the left and right matrices is of the int8\_t or int4b\_t type, and the output is of the half type; or the input of the left and right matrices is of the int8\_t type, and the output is of the int8\_t type. In this scenario, when the data of the output C matrix is moved from CO1 to Global Memory, a dequantization operation is performed to dequantize the final result into the corresponding half or int8\_t type.

Matmul quantization scenario: During Matmul computation, the input of the left and right matrices is of the half or bfloat16\_t type, and the output is of the int8\_t type. In this scenario, when the data of the output C matrix is moved from CO1 to Global Memory, a quantization operation is performed to quantize the final result into the int8\_t type.

There are two modes for quantization or dequantization: one is the uniform scalar quantization/dequantization mode, and the other is the vector quantization/dequantization mode.

-   Uniform scalar quantization or dequantization mode: A uniform scalar is used to quantize or dequantize all values of the output matrix.
-   Vector quantization or dequantization mode: A parameter vector is provided, and the scalar of the corresponding column in the vector is used to quantize or dequantize each column of the output matrix.

## Prototype

```
int32_t SetDequantType(DequantType dequantType)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dequantType | Input | Sets the mode for quantization or dequantization. The type is **DequantType**, which is defined in the following code. The values and their meanings are as follows:<br>SCALAR: uniform scalar quantization or dequantization mode.<br>TENSOR: vector quantization or dequantization mode. |

```
enum class DequantType {
    SCALAR = 0,
    TENSOR = 1,
};
```

## Return Value

-1 indicates failure; 0 indicates success.

## Constraints

The uniform scalar quantization/dequantization mode and the vector quantization/dequantization mode supported by this API correspond to the Kernel side interfaces [SetQuantScalar](../Matmul-Kernel/SetQuantScalar.md) and [SetQuantVector](../Matmul-Kernel/SetQuantVector.md), respectively. The quantization/dequantization mode set by this API must be consistent with the interface used on the Kernel side.

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
tiling.SetDequantType(DequantType::SCALAR);  // Set the uniform scalar quantization/dequantization mode.
// tiling.SetDequantType(DequantType::TENSOR);  // Set the vector quantization/dequantization mode.
tiling.SetBufferSpace(-1, -1, -1);
optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);
```
