# SetQuantVector

<!-- md-trans-meta sourceCommit=26d99808e1f7cb2f6dc612ec3b4ab34e4763f126 translatedAt=2026-09-09T06:58:34.857Z pushedAt=2026-09-12T09:55:18.135Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

This API provides the function of quantizing or dequantizing the output matrix using a vector. That is, for the parameter vector whose input shape is \[1, N\], where N is the N value in M/N/K during Matmul matrix computation, each column of the output matrix is quantized or dequantized using the coefficient of the corresponding column in the vector.

Matmul dequantization scenario: During Matmul computation, the inputs of the left and right matrices are of the int8\_t or int4b\_t type and the output is of the half type; or the inputs of the left and right matrices are of the int8\_t type and the output is of the int8\_t type. In this scenario, when the data of the output C matrix is moved out of CO1 to Global Memory, a dequantization operation is performed to dequantize the final result into the corresponding half or int8\_t type.

Matmul quantization scenario: During Matmul computation, the inputs of the left and right matrices are of the half or bfloat16\_t type and the output is of the int8\_t type. In this scenario, when the data of the output C matrix is moved out of CO1 to Global Memory, a quantization operation is performed to quantize the final result into the int8\_t type.

## Prototype

-   The quantization parameter is stored in GM.

    ```
    __aicore__ inline void SetQuantVector(const GlobalTensor<uint64_t>& quantTensor)
    ```

-   The quantization parameter is stored in L1 Buffer.

    ```
    __aicore__ inline void SetQuantVector(const LocalTensor<uint64_t>& quantTensor)
    ```

    -   Atlas 200I/500 A2 inference products do not support storing the quantization parameter in L1 Buffer yet.
    -   The AI Core of Atlas inference products does not support storing the quantization parameter in L1 Buffer yet.

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| quantTensor | Input | Parameter vector used in the quantization or dequantization operation. The storage location of the parameter vector is GM or L1 Buffer. |

The formula for converting the float quantization parameters scale and offset into the uint64 input parameter is as follows:

1.  quantTensor is in 64-bit format and is initially 0.
2.  scale is truncated to the upper 19 bits, stored at bit 32 of quantTensor, and bit 46 is set to 1.

    quantTensor = quantTensor ∣ \(scale & 0xFFFFE000\) ∣ \(1 ≪ 46\)

3.  Subsequent computation is performed based on the value of offset:
    -   If offset does not exist, no subsequent computation is performed.
    -   If offset exists:
        1.  Process the offset value as int, with a range of \[-256, 255\].

            offset = Max\(Min\(INT\(Round\(offset\)\), 255\), −256\)

        2.  Then retain 9 bits of offset and store them in bits 37 to 45 of quantTensor.

            quantTensor = \(quantTensor & 0x4000FFFFFFFF\) ∣ \(\(offset & 0x1FF\) ≪ 37\)

## Return Value

None

## Constraints

This API must be consistent with [SetDequantType](../Matmul-Tiling/SetDequantType.md).

This API must be called before [Iterate](Iterate.md) or [IterateAll](IterateAll.md).

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);

// quantTensor: Tensor used for quantization or inverse quantization operations. Supported storage locations are GM and L1.
mm.SetQuantVector(quantTensor);

mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    matmulObj.SetBias(biasGlobal);
}
mm.IterateAll(gm_c);
mm.End();
```
