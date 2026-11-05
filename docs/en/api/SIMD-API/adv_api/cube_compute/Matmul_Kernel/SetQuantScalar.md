# SetQuantScalar

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:52:22.766Z pushedAt=2026-10-27T11:30:10.501Z nl=2 -->

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
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetQuantScalar_res.md#id1 -->

## Description

This API quantizes or dequantizes all values of the output cube using the same coefficient, that is, the entire C cube corresponds to one quantization parameter, and the shape of the quantization parameter is \[1\].

**Matmul** dequantization scenario: During **Matmul** computation, the inputs of the left and right cubes are of type int8\_t or int4b\_t, and the output is of type half; or the inputs of the left and right cubes are of type int8\_t, and the output is of type int8\_t. In this scenario, when the data of the output C cube is moved from the L0C Buffer (CO1) to Global Memory, a dequantization operation is performed to dequantize the final result to the corresponding half or int8\_t type.

Matmul quantization scenario: During Matmul computation, the input of the left and right cubes is of the half or bfloat16\_t type, and the output is of the int8\_t type. In this scenario, when the data of the output C cube is moved from the L0C Buffer (CO1) to Global Memory, a quantization operation is performed to quantize the final result to the int8\_t type.

## Prototype

```
__aicore__ inline void SetQuantScalar(const uint64_t quantScalar)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| quantScalar | Input | Quantization or dequantization coefficient. |

The formula for converting the float-type quantization parameters scale and offset into the uint64-type input parameter is as follows:

1.  quantScalar is in 64-bit format and is initially 0.
2.  scale is truncated to the high 19 bits and stored starting at bit 32 of quantScalar, and bit 46 is modified to 1.

    quantScalar = quantScalar ∣ \(scale & 0xFFFFE000\) ∣ \(1 << 46\)

3.  Perform subsequent calculations based on the value of offset:
    -   If offset does not exist, no subsequent calculation is performed.
    -   If offset exists:
        1.  Process the offset value as int, with a range of \[-256, 255\].

            offset = Max\(Min\(INT\(Round\(offset\)\), 255\), 256\)

        2.  Then retain 9 bits of offset and store them in bits 37 to 45 of **quantScalar**.

            quantScalar = \(quantScalar & 0x4000FFFFFFFF\) ∣ \(\(offset & 0x1FF\) << 37\)

## Return Value

None

## Constraints

The quantization parameter cannot be inf/nan or a denormalized number.

It must be consistent with [SetDequantType](../Matmul_Tiling/SetDequantType.md).

This API must be called before [Iterate](Iterate.md) or [IterateAll](IterateAll.md).

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
float tmp = 0.1; // Multiply by 0.1 when outputting to GM.
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
