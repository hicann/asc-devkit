# Ndtri

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T06:31:25.639Z pushedAt=2026-11-18T11:07:26.789Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->

## Description

Ndtri is currently an experimental API. Before using it, define `CANN_ASC_USE_EXPERIMENTAL` and explicitly include the header file:

```cpp
#define CANN_ASC_USE_EXPERIMENTAL
#include "adv_api/experimental/ndtri.h"
```

Computes the inverse of the cumulative distribution function of the standard normal distribution element-wise. For an input probability p, the output x satisfies:

$$\Phi(x)=p$$

The input values and corresponding outputs are as follows:

| Input p | Output x |
| --- | --- |
| 0 | Negative infinity |
| 1 | Positive infinity |
| (0, 1) | The value satisfying $\Phi(x)=p$ |
| Less than 0, greater than 1, NaN, or positive/negative infinity | NaN |

## Prototype

- Specify the number of elements involved in the computation

    ```cpp
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void Ndtri(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
        const uint32_t calCount)
    ```

- Compute the entire source Tensor

    ```cpp
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void Ndtri(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
    ```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. The supported data type is float. |
| isReuseSource | Reserved parameter for source operand reuse. The current implementation does not use this parameter. Keep the default value false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Read-only source operand whose element values represent probabilities.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. The data type must be the same as that of dstTensor. When the input and output addresses are separated, the API does not modify the data in this tensor. |
| calCount | Input | Number of elements involved in the computation. The value range is [0, min(srcTensor.GetSize(), dstTensor.GetSize())]. When the value is 0, the API returns directly without reading srcTensor or writing dstTensor. When this parameter is not passed, the number of elements to compute is srcTensor.GetSize(). |

## Return Value

None

## Constraints

- srcTensor and dstTensor support fully in-place computation, but partially overlapping addresses are not supported.
- The operand address alignment requirements follow the general address alignment constraints.

## Examples

```cpp
// dstLocal: Tensor that stores the computation results.
// srcLocal: Tensor that stores the input probabilities.
// Compute 1024 float elements.
AscendC::experimental::Ndtri<float, false>(dstLocal, srcLocal, 1024);
// Compute all elements in srcLocal.
AscendC::experimental::Ndtri<float, false>(dstLocal, srcLocal);
```
