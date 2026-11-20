# SetQuantVector

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-02T06:46:34.337Z pushedAt=2026-11-18T04:53:53.283Z nl=2 -->

## Applicable Products

<!-- npu="950" id2 -->
- Ascend 950PR&950DT products: Supported
<!-- end id2 -->
<!-- npu="A3" id3 -->
- Atlas A3 products: Supported
<!-- end id3 -->
<!-- npu="910b" id4 -->
- Atlas A2 products: Supported
<!-- end id4 -->
<!-- npu="310b" id5 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- npu="x90" id1 -->
- Kirin X90 processor products: Supported
<!-- end id1 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetQuantVector_res.md#id1 -->

## Description

This API quantizes or dequantizes the output cube using a vector. That is, for a parameter vector whose input shape is \[1, N\], where N is the N value in M/N/K during Matmul cube computation, each column of the output cube is quantized or dequantized using the coefficient of the corresponding column in the vector.

Matmul dequantization scenario: During Matmul computation, the input of the left and right cubes is of type int8\_t or int4b\_t, and the output is of type half; or the input of the left and right cubes is of type int8\_t, and the output is of type int8\_t. In this scenario, when the data of the output C cube is moved from the L0C Buffer (CO1) to Global Memory, a dequantization operation is performed to dequantize the final result into the corresponding half or int8\_t type.

Matmul quantization scenario: During Matmul computation, the input of the left and right cubes is of type half or bfloat16\_t, and the output is of type int8\_t. In this scenario, when the data of the output C cube is moved from the L0C Buffer (CO1) to Global Memory, a quantization operation is performed to quantize the final result into the int8\_t type.

## Prototype

-   The storage location of the quantization parameter is GM.

    ```
    __aicore__ inline void SetQuantVector(const GlobalTensor<uint64_t>& quantTensor)
    ```

-   The storage location of the quantization parameter is L1 Buffer.

    ```
    __aicore__ inline void SetQuantVector(const LocalTensor<uint64_t>& quantTensor)
    ```

    <!-- npu="310b" id8 -->
    -   Atlas 200I/500 A2 inference products do not support storing the quantization parameter in L1 Buffer.
    <!-- end id8 -->
    <!-- npu="310p" id9 -->
    -   The AI Core of Atlas inference products does not support storing the quantization parameter in L1 Buffer.
    <!-- end id9 -->
    <!-- npu="x90" id10 -->
    -   The Kirin X90 processor products do not support storing quantization parameters in the L1 Buffer.
    <!-- end id10 -->

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| quantTensor | Input | Parameter vector used in quantization or dequantization operations. The storage location of the parameter vector is GM or L1 Buffer. |

The formula for converting the float-type quantization computation parameters **scale** and **offset** into a **uint64**-type input parameter is as follows:

1.  **quantTensor** is in 64-bit format and is initially 0.
2.  **scale** is clipped to the upper 19 bits, stored at bit 32 of **quantTensor**, and bit 46 is set to 1.

    quantTensor = quantTensor ∣ \(scale & 0xFFFFE000\) ∣ \(1 ≪ 46\)

3.  Perform subsequent calculation based on the value of **offset**:
    -   If **offset** does not exist, no subsequent calculation is performed.
    -   If **offset** exists:
        1.  Process the **offset** value as int, with a range of \[-256, 255\].

            offset = Max\(Min\(INT\(Round\(offset\)\), 255\), −256\)

        2.  Then retain 9 bits of **offset** and store them in bits 37 to 45 of **quantTensor**.

            quantTensor = \(quantTensor & 0x4000FFFFFFFF\) ∣ \(\(offset & 0x1FF\) ≪ 37\)

## Return Value

None

## Constraints

The quantization parameter cannot be inf/nan or a denormalized number.

It must be consistent with [SetDequantType](../Matmul_Tiling/SetDequantType.md).

This API must be called before [Iterate](Iterate.md) or [IterateAll](IterateAll.md).

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);

// quantTensor: Tensor used in quantization or dequantization operations. Supported storage locations are GM and L1.
mm.SetQuantVector(quantTensor);

mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    matmulObj.SetBias(biasGlobal);
}
mm.IterateAll(gm_c);
mm.End();
```
