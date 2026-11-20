# BesselI0

<!-- md-trans-meta sourceCommit=971bc6393be4cb3e0f8244913e605c5710bb2cc9 translatedAt=2026-11-10T06:31:01.911Z pushedAt=2026-11-18T11:07:26.773Z nl=2 -->

## Applicable Products

<!-- npu="950" id3 -->
- Ascend 950PR&950DT products: Supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 products: Not supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products: Not supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- npu="x90" id1 -->
- KirinX90 processor products: not supported
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin9030 processor products: not supported
<!-- end id2 -->

## Description

This API is experimental and may be adjusted or improved in later versions, with no guarantee of backward compatibility. Pay attention to updates in later versions when using it.

Computes the modified Bessel function of the first kind, order 0, I₀(x), element-wise. The formula is as follows:

```text
I0(x) = Σ_{k=0}^{+∞} (1 / (k!)^2) * (x/2)^{2k}
```

## Prototype

- Pass temporary space through the sharedTmpBuffer input parameter.
    - All or part of the source operand Tensor participates in the computation

        ```cpp
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void BesselI0(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    - The entire source operand Tensor participates in the computation

        ```cpp
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void BesselI0(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

- The API framework allocates temporary space.
    - All or part of the source operand Tensor participates in the computation

        ```cpp
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void BesselI0(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    - The entire source operand Tensor participates in the computation

        ```cpp
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void BesselI0(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

This API does not require temporary space for computation. In the overload that passes temporary space through the sharedTmpBuffer input parameter, sharedTmpBuffer can be any valid `LocalTensor<uint8_t>`. For the method of obtaining the temporary space size BufferSize, see [GetBesselI0MaxMinTmpSize](GetBesselI0MaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. The supported data type is float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>This API does not require temporary space for computation, so this parameter can be any valid LocalTensor. For the method of obtaining the temporary space size BufferSize, see [GetBesselI0MaxMinTmpSize](GetBesselI0MaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

- Before using this API, include the header file. The header file path is `adv_api/experimental/bessel_i0.h`.
- Add the compilation option `-DCANN_ASC_USE_EXPERIMENTAL` to enable the experimental feature before use.
- **Overlapping between the source operand and destination operand addresses is not supported.**
- For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```cpp
#include "adv_api/experimental/bessel_i0.h"
// dstLocal: Tensor for storing the computation result.
// srcLocal: Input tensor that participates in the computation.
// The input tensor length is 1024, the operator input data type is float, and the actual number of computations is 512.
AscendC::experimental::BesselI0(dstLocal, srcLocal, 512);
```

The result is as follows:

```text
Input data (srcLocal): [0.00            0.01            0.02             ...  5.10            5.11]
Output data (dstLocal): [1.00000000e+00  1.00002500e+00  1.00010000e+00   ...  2.97888554e+01  3.00568915e+01]
```
