# Log

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T03:20:07.537Z pushedAt=2026-10-29T02:15:02.826Z nl=2 -->

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
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Log_interface/Log_res.md#id1 -->

## Description

Performs element-wise logarithmic operations with e, 2, and 10 as the base. The computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002182888036.png)

![](../../../../figures/zh-cn_formulaimage_0000002188813710.png)

![](../../../../figures/zh-cn_formulaimage_0000002218328397.png)

![](../../../../figures/zh-cn_formulaimage_0000002188654338.png)

![](../../../../figures/zh-cn_formulaimage_0000002182888172.png)

![](../../../../figures/zh-cn_formulaimage_0000002188814822.png)

## Prototype

-   With e as base:

    -   The source operand Tensor fully/partially participates in the computation

    ```
    template<typename T, bool isReuseSource = false>
    __aicore__ inline void Log(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, uint32_t calCount)
    ```

    -   The source operand Tensor fully participates in the computation

    ```
    template<typename T, bool isReuseSource = false>
    __aicore__ inline void Log(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
    ```

-   With 2 as base
    -   Passes temporary space through the sharedTmpBuffer input parameter
        -   The source operand Tensor fully/partially participates in the computation.

            ```
            template<typename T, bool isReuseSource = false>
            __aicore__ inline void Log2(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
            ```

        -   The source operand Tensor fully participates in the computation.

            ```
            template <typename T, bool isReuseSource = false>
            __aicore__ inline void Log2(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
            ```

    -   The API framework allocates temporary space.
        -   The source operand Tensor fully/partially participates in the computation.

            ```
            template<typename T, bool isReuseSource = false>
            __aicore__ inline void Log2(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, uint32_t calCount)
            ```

        -   The source operand Tensor fully participates in the computation.

            ```
            template <typename T, bool isReuseSource = false>
            __aicore__ inline void Log2(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
            ```

-   With 10 as the base:
    -   The source operand Tensor fully/partially participates in the computation.

        ```
        template<typename T, bool isReuseSource = false>
        __aicore__ inline void Log10(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, uint32_t calCount)
        ```

    -   The source operand Tensor fully participates in the computation.

        ```
        template<typename T, bool isReuseSource = false>
        __aicore__ inline void Log10(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. Temporary space can be provided in two ways: passed in by the developer through the sharedTmpBuffer input parameter, or allocated by the API framework.

-   With input parameter passing through sharedTmpBuffer, this tensor is used as the temporary space for processing, and the API framework does not allocate it. With this method, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.
-   The API framework allocates temporary space, so you do not need to allocate it, but you need to reserve the size of the temporary space.

When passing through sharedTmpBuffer, you need to allocate space for the tensor; when the API framework allocates it, you need to reserve temporary space. The temporary space size BufferSize is obtained as follows: obtain the size of the space range to be reserved through the API provided in [GetLogMaxMinTmpSize](GetLogMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether to allow modification of the source operand. This parameter is reserved. Pass the default value false. |

**Table 2**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetLogMaxMinTmpSize](GetLogMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

<!-- npu="950" id9 -->
-   For the Ascend 950PR&950DT products, the handling of Subnormal in the internal computation of the API is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When configured as `false`, Subnormal is retained during the computation process and participates in subsequent computations according to its actual value.
    -   When set to `true`, FTZ (Flush-To-Zero) mode is enabled. Subnormal values generated or involved in the computation process are treated as 0, which may cause precision differences compared with retaining Subnormal values.
<!-- end id9 -->

## Examples

For the complete call example, see [Log Examples](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/10_math/log).

```
// dstLocal: Tensor that stores the computation result.
// srcLocal: Input tensor that participates in the computation.

// Use e as the base, with all elements in the computation.
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
Input data(srcLocal): [6299.54 338.45963 ... 2.853525 5752.1323]
Output data(dstLocal): [12.621031 8.40284 ... 1.5127451 12.4898815]
```

The Log10 API result example is as follows:

```
Input data(srcLocal): [712.7535 78.36265 ... 3099.0571 9313.082]
Output data(dstLocal): [2.8529394 1.8941091 ... 3.4912295 3.9690933]
```
