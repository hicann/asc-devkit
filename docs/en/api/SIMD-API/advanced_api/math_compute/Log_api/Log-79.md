# Log

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T16:13:39.150Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs element-wise logarithmic operations with e, 2, and 10 as the base. The calculation formulas are as follows:

![](../../../../figures/zh-cn_formulaimage_0000002182888036.png)

![](../../../../figures/zh-cn_formulaimage_0000002188813710.png)

![](../../../../figures/zh-cn_formulaimage_0000002218328397.png)

![](../../../../figures/zh-cn_formulaimage_0000002188654338.png)

![](../../../../figures/zh-cn_formulaimage_0000002182888172.png)

![](../../../../figures/zh-cn_formulaimage_0000002188814822.png)

## Prototype

-   With e as the base:

    -   The source operand tensor participates in computation either in whole or in part.

    ```
    template<typename T, bool isReuseSource = false>
    __aicore__ inline void Log(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, uint32_t calCount)
    ```

    -   The entire source operand Tensor participates in the computation.

    ```
    template<typename T, bool isReuseSource = false>
    __aicore__ inline void Log(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
    ```

-   With 2 as the base
    -   Pass the temporary space through the **sharedTmpBuffer** input parameter.
        -   The source operand tensor participates in computation either in whole or in part.

            ```
            template<typename T, bool isReuseSource = false>
            __aicore__ inline void Log2(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
            ```

        -   The entire source operand Tensor participates in the computation.

            ```
            template <typename T, bool isReuseSource = false>
            __aicore__ inline void Log2(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
            ```

    -   The API framework allocates temporary space.
        -   The source operand tensor participates in computation either in whole or in part.

            ```
            template<typename T, bool isReuseSource = false>
            __aicore__ inline void Log2(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, uint32_t calCount)
            ```

        -   The entire source operand Tensor participates in the computation.

            ```
            template <typename T, bool isReuseSource = false>
            __aicore__ inline void Log2(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
            ```

-   Use 10 as the base:
    -   The source operand tensor participates in computation either in whole or in part.

        ```
        template<typename T, bool isReuseSource = false>
        __aicore__ inline void Log10(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation.

        ```
        template<typename T, bool isReuseSource = false>
        __aicore__ inline void Log10(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: through the API framework allocation approach, or by you passing it via the `sharedTmpBuffer` input parameter.

-   Pass it through the **sharedTmpBuffer** input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the **sharedTmpBuffer** memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

When the temporary space is passed through **sharedTmpBuffer**, you need to allocate space for the tensor. When the API framework allocates it, you need to reserve the temporary space. The size of the temporary space, **BufferSize**, is obtained as follows: obtain the size of the space range that needs to be reserved through the API provided in [GetLogMaxMinTmpSize](GetLogMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value **false**. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetLogMaxMinTmpSize](GetLogMaxMinTmpSize.md). |
| calCount | Input | Number of elements participating in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

For the complete calling example, see the [log example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/10_math/log).

```
// dstLocal: Tensor that stores the computation result.
// srcLocal: Input tensor that participates in the computation.

// Use e as the base, with all elements fully participating in the computation.
AscendC::Log(dstLocal, srcLocal);
// Use 10 as the base.
// AscendC::Log10(dstLocal, srcLocal);
// Use 2 as the base.
// AscendC::Log2(dstLocal, srcLocal);
```

The Log API result example is as follows:

```
Input data (srcLocal): [144.22607 9634.764 ... 1835.1245 3145.5125]
Output data (dstLocal): [4.971382 9.173133 ... 7.514868 8.053732]
```

The Log2 API result example is as follows:

```
Input data (srcLocal): [6299.54 338.45963 ... 2.853525 5752.1323]
Output data (dstLocal): [12.621031 8.40284 ... 1.5127451 12.4898815]
```

The Log10 API result example is as follows:

```
Input data (srcLocal): [712.7535 78.36265 ... 3099.0571 9313.082]
Output data (dstLocal): [2.8529394 1.8941091 ... 3.4912295 3.9690933]
```
