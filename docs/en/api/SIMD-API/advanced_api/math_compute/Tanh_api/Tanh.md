# Tanh

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T10:40:10.793Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs element-wise logistic Tanh computation. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002218327685.png)

![](../../../../figures/zh-cn_formulaimage_0000002193334172.png)

## Prototype

-   Pass temporary space through the **sharedTmpBuffer** input parameter.
    -   All or part of the source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false, const TanhConfig& config = DEFAULT_TANH_CONFIG>
        __aicore__ inline void Tanh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false, const TanhConfig& config = DEFAULT_TANH_CONFIG>
        __aicore__ inline void Tanh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework applies for temporary space.

    -   All or part of the source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false, const TanhConfig& config = DEFAULT_TANH_CONFIG>
        __aicore__ inline void Tanh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false, const TanhConfig& config = DEFAULT_TANH_CONFIG>
        __aicore__ inline void Tanh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

    Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space supports two methods: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

    -   When temporary space is passed through the **sharedTmpBuffer** input parameter, this tensor is used as the temporary space for processing, and the API framework no longer applies for it. With this method, you can manage the **sharedTmpBuffer** memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, providing higher flexibility and higher memory utilization.
    -   When the API framework applies for temporary space, you do not need to apply for it, but you need to reserve the size of the temporary space.

    When **sharedTmpBuffer** is passed, you need to allocate space for the tensor; when the API framework applies for the space, you need to reserve temporary space. The size of the temporary space, **BufferSize**, is obtained as follows: obtain the size range of the space to be reserved through the API provided in [GetTanhMaxMinTmpSize](GetTanhMaxMinTmpSize.md).

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether the source operand can be modified. This parameter is reserved; pass the default value **false**. |
| config | This parameter is only supported on Ascend 950PR/Ascend 950DT.<br><br>Configuration related to the Tanh algorithm. This parameter is optional and of the **TanhConfig** type. It is defined in the following code, where the parameter meanings are as follows:<br>**algo**: Algorithm used in the internal implementation of Tanh. It is of the **TanhAlgo** type. Supported values are as follows: **INTRINSIC**: Default value. This algorithm directly computes the formula y = (e^(2x)-1)/(e^(2x)+1), offering better performance. **SUBSECTION_COMPENSATION**: This algorithm implements Tanh through error compensation, offering higher precision. |

```
enum class TanhAlgo {
  INTRINSIC = 0,
  SUBSECTION_COMPENSATION,
};
struct TanhConfig {
  TanhAlgo algo = TanhAlgo::INTRINSIC;
};
```

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition values: VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition values: VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition values: VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex internal computation of Tanh. Provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetTanhMaxMinTmpSize](GetTanhMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Overlapping between **sharedTmpBuffer** and the source operand or destination operand addresses is not supported.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
// dstLocal: Tensor that stores the Tanh computation result.
// srcLocal: Tensor that stores the Tanh computation input.
// sharedTmpBuffer: Tensor that stores the temporary buffer during Tanh computation.

// The API framework applies for temporary space, and all elements participate in the computation.
AscendC::Tanh(dstLocal, srcLocal);
// The API framework applies for temporary space, and some elements participate in the computation. The number of elements involved in the computation is 512.
AscendC::Tanh(dstLocal, srcLocal, 512);

// Pass temporary space through the sharedTmpBuffer input parameter, and all elements participate in the computation.
AscendC::Tanh(dstLocal, srcLocal, sharedTmpBuffer);
// Pass temporary space through the sharedTmpBuffer input parameter, and some elements participate in the computation. The number of elements involved in the computation is 512.
AscendC::Tanh(dstLocal, srcLocal, sharedTmpBuffer, 512);
static constexpr AscendC::TanhAlgo algo = AscendC::TanhAlgo::SUBSECTION_COMPENSATION;
static constexpr AscendC::TanhConfig config = { algo };
AscendC::Tanh<half, false, config>(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The result is as follows:

```
Input data (srcLocal):
[-2.56 -2.55 -2.54 ... 0. ... 2.53  2.54  2.55]
Output data(dstLocal):
[-0.98813187 -0.9878992 -0.98761402 ... 0. ... 0.98737127 0.98761402 0.9878992]
```
