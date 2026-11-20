# Log

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-09T11:13:41.539Z pushedAt=2026-11-18T08:54:29.521Z nl=2 -->

## Applicable Products

<!-- npu="950" id3 -->
- Ascend 950PR&950DT products: Supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 products: Supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products: Supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- npu="x90" id1 -->
- KirinX90 processor products: Supported
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin9030 processor products: Supported
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Log_interface/Log_res.md#id1 -->

## Description

Performs element-wise logarithmic operations with e, 2, and 10 as the base. The computation formula is as follows:

![](../../../../figures/formulaimage_0000002182888036.png)

![](../../../../figures/formulaimage_0000002188813710.png)

![](../../../../figures/formulaimage_0000002218328397.png)

![](../../../../figures/formulaimage_0000002188654338.png)

![](../../../../figures/formulaimage_0000002182888172.png)

![](../../../../figures/formulaimage_0000002188814822.png)

## Prototype

-   With e as base:

    -   All or part of the source operand Tensor participates in the computation

    ```
    template<typename T, bool isReuseSource = false>
    __aicore__ inline void Log(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, uint32_t calCount)
    ```

    -   The entire source operand Tensor participates in the computation

    ```
    template<typename T, bool isReuseSource = false>
    __aicore__ inline void Log(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
    ```

-   With 2 as base
    -   Pass temporary space through the sharedTmpBuffer input parameter.
        -   All or part of the source operand Tensor participates in the computation

            ```
            template<typename T, bool isReuseSource = false>
            __aicore__ inline void Log2(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
            ```

        -   The entire source operand Tensor participates in the computation

            ```
            template <typename T, bool isReuseSource = false>
            __aicore__ inline void Log2(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
            ```

    -   The API framework allocates temporary space.
        -   All or part of the source operand Tensor participates in the computation

            ```
            template<typename T, bool isReuseSource = false>
            __aicore__ inline void Log2(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, uint32_t calCount)
            ```

        -   The entire source operand Tensor participates in the computation

            ```
            template <typename T, bool isReuseSource = false>
            __aicore__ inline void Log2(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
            ```

-   With 10 as the base:
    -   All or part of the source operand Tensor participates in the computation

        ```
        template<typename T, bool isReuseSource = false>
        __aicore__ inline void Log10(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation

        ```
        template<typename T, bool isReuseSource = false>
        __aicore__ inline void Log10(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space supports two methods: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   Pass temporary space through the sharedTmpBuffer input parameter, and use this tensor as the temporary space for processing; the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space yourself and reuse this portion of memory after the API call is complete. Memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.
-   The API framework allocates temporary space, so you do not need to allocate it, but you need to reserve the size of the temporary space.

When passing through sharedTmpBuffer, you need to allocate space for the tensor; when the API framework allocates it, you need to reserve temporary space. The temporary space size BufferSize is obtained as follows: obtain the size of the space range to be reserved through the API provided in [GetLogMaxMinTmpSize](GetLogMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. Supported data types: half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetLogMaxMinTmpSize](GetLogMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

<!-- npu="950" id9 -->
-   For the Ascend 950PR&950DT products, the handling of Subnormal values in the internal computation of the API is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation process and participate in subsequent computation according to their actual values.
    -   When set to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal values generated or involved in the computation process are treated as 0, which may cause precision differences compared with preserving Subnormal values.
<!-- end id9 -->

## Examples

For the complete call example, see [Log Examples](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/10_math/log).

```
// dstLocal: Tensor for storing the computation result.
// srcLocal: Input tensor that participates in the computation.

// Use e as the base, with all elements participating in the computation.
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
