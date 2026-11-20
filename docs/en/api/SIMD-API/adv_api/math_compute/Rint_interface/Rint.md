# Rint

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-09T11:17:09.524Z pushedAt=2026-11-18T08:54:29.615Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Rint_interface/Rint_res.md#id1 -->

## Description

Obtains the integer closest to the input data. If two integers are equally close, the even one is obtained. The computation formula is as follows:

![](../../../../figures/formulaimage_0000002352876716.png)

For example:

Rint\(3.9\) = 4

Rint\(3.3\) = 3

Rint\(3.5\) = 4

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter.

    ```
    template <const RintConfig& config = DEFAULT_RINT_CONFIG, typename T>
    __aicore__ inline void Rint(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t count)
    ```

-   The API framework allocates temporary space.

    ```
    template <const RintConfig& config = DEFAULT_RINT_CONFIG, typename T>
    __aicore__ inline void Rint(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space supports two methods: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   Pass temporary space through the sharedTmpBuffer input parameter, and use this tensor as the temporary space for processing; the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space yourself and reuse this portion of memory after the API call is complete. Memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.
-   The API framework allocates temporary space, so you do not need to allocate it, but you need to reserve the size of the temporary space.

When temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor. When the API framework allocates temporary space, you need to reserve the temporary space. The size of the temporary space, BufferSize, is obtained as follows: use the API provided in [GetRintMaxMinTmpSize](GetRintMaxMinTmpSize.md) to obtain the size of the space range that needs to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| RintConfig | Rint algorithm configuration. This parameter is optional and is of the RintConfig type. Its definition is shown in the following code, where the parameter meanings are as follows:<br><br>isReuseSource: Whether to allow modification of the source operand. This parameter is reserved. Pass the default value false. |
| T | Data type of the operands. Supported data types: half and float. |

```
struct RintConfig {
    bool isReuseSource;
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be the same as that of the destination operands. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex internal computation of Rint. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetRintMaxMinTmpSize](GetRintMaxMinTmpSize.md). |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Overlapping between sharedTmpBuffer and the source operand or destination operand addresses is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

For the complete example, see [Rint Operator Examples](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/10_math/rint).

-   Passed through the sharedTmpBuffer input parameter

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize); // bufferSize is obtained from the tiling parameter on the host side.
    AscendC::LocalTensor<uint8_t> sharedTmpBuffer = tmpQue.AllocTensor<uint8_t>();
    // The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
    static constexpr AscendC::RintConfig rintConfig = {false}; // Do not modify the source operand.
    // dst and src are LocalTensors of the half type.
    AscendC::Rint<rintConfig, half>(dst, src, sharedTmpBuffer, 512);
    ```

-   The API framework allocates temporary space.

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize); // bufferSize is obtained from the tiling parameter on the host side.
    // The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
    static constexpr AscendC::RintConfig rintConfig = {false}; // Do not modify the source operand.
    // dst and src are LocalTensor of the half type.
    AscendC::Rint<rintConfig, half>(dst, src, 512);
    ```

The result is as follows:

```
Input data (src): [-1.5, -1.3, -0.9, -0.5, 0.5, 0.9, 1.1, 1.5, 2.2, 2.5, 2.8, 3.2]
Output data (dst): [-2, -1, -1, 0, 0, 1, 1, 2, 2, 2, 3, 3]
```
