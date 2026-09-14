# SetQuantScalar

<!-- md-trans-meta sourceCommit=26d99808e1f7cb2f6dc612ec3b4ab34e4763f126 translatedAt=2026-09-09T06:58:11.818Z pushedAt=2026-09-12T09:55:18.133Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

This API applies the same coefficient to all values of the output matrix for quantization or dequantization. That is, the entire C matrix corresponds to a single quantization parameter, whose shape is \[1\].

Matmul dequantization scenario: During Matmul computation, the inputs of the left and right matrices are of the int8\_t or int4b\_t type, and the output is of the half type; or the inputs of the left and right matrices are of the int8\_t type, and the output is of the int8\_t type. In this scenario, when the data of the output C matrix is moved from CO1 to Global Memory, a dequantization operation is performed to dequantize the final result into the corresponding half or int8\_t type.

Matmul quantization scenario: During Matmul computation, the inputs of the left and right matrices are of the half or bfloat16\_t type, and the output is of the int8\_t type. In this scenario, when the data of the output C matrix is moved from CO1 to Global Memory, a quantization operation is performed to quantize the final result into the int8\_t type.

## Prototype

```
__aicore__ inline void SetQuantScalar(const uint64_t quantScalar)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| quantScalar | Input | Quantization or dequantization scalar. |

The formula for converting the float quantization parameters **scale** and **offset** into the uint64 input parameter is as follows:

1.  **quantScalar** is a 64-bit value, initially 0.
2.  **scale** is truncated to the upper 19 bits and stored at bit 32 of **quantScalar**, and bit 46 is set to 1.

    quantScalar = quantScalar ∣ \(scale & 0xFFFFE000\) ∣ \(1 << 46\)

3.  Perform subsequent computation based on the value of **offset**:
    -   If **offset** does not exist, no further computation is performed.
    -   If **offset** exists:
        1.  Convert the **offset** value to int, with a range of \[-256, 255\].

            offset = Max\(Min\(INT\(Round\(offset\)\), 255\), 256\)

        2.  Then retain 9 bits of **offset** and store them in bits 37 to 45 of **quantScalar**.

            quantScalar = \(quantScalar & 0x4000FFFFFFFF\) ∣ \(\(offset & 0x1FF\) << 37\)

## Return Value

None

## Constraints

It must be consistent with [SetDequantType](../Matmul-Tiling/SetDequantType.md).

This API must be called before [Iterate](Iterate.md) or [IterateAll](IterateAll.md).

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
float tmp = 0.1;  // The output GM is multiplied by 0.1.
// Convert the quantization or dequantization coefficient of the floating-point value to the uint64_t type.
uint64_t ans = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&tmp));
mm.SetQuantScalar(ans);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(biasGlobal);
}
mm.IterateAll(gm_c);
mm.End();
```
