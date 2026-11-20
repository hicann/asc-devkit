# IsInf

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-09T11:12:22.684Z pushedAt=2026-11-18T08:54:29.478Z nl=2 -->

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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Vector Core of Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/IsInf_interface/IsInf_res.md#id1 -->

## Description

Determines element-wise whether the input floating-point number is ±INF, and outputs the result as a floating-point number or a Boolean value. When the output is a floating-point type, for input data of ±INF, the result at the corresponding position is 1 of the floating-point type; otherwise, it is 0. When the output is a bool type, for input data of ±INF, the result at the corresponding position is true; otherwise, it is false. The computation formula is as follows:

![](../../../../figures/formulaimage_0000002382902989.png)

-   When the output is a floating-point type:

    ![](../../../../figures/formulaimage_0000002386808777.png)

-   When the output is of the bool type:

    ![](../../../../figures/formulaimage_0000002386809017.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter.

    ```
    template <const IsInfConfig& config = DEFAULT_IS_INF_CONFIG, typename T, typename U>
    __aicore__ inline void IsInf(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t count)
    ```

-   The API framework allocates temporary space.

    ```
    template <const IsInfConfig& config = DEFAULT_IS_INF_CONFIG, typename T, typename U>
    __aicore__ inline void IsInf(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint32_t count)
    ```

Because the internal implementation of this API involves precision conversion, additional temporary space is required to store intermediate variables during computation. Temporary space can be provided in two ways: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   Pass temporary space through the sharedTmpBuffer input parameter, and use this tensor as the temporary space for processing; the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space yourself and reuse this portion of memory after the API call is complete. Memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.
-   The API framework allocates temporary space, so you do not need to allocate it, but you need to reserve the size of the temporary space.

When the temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor. When the API framework allocates the temporary space, you need to reserve the temporary space. The size of the temporary space, BufferSize, is obtained as follows: use the API provided in [GetIsInfMaxMinTmpSize](GetIsInfMaxMinTmpSize.md) to obtain the size of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| IsInfConfig | Configuration related to the IsInf algorithm. This parameter is optional and of the IsInfConfig type. Its definition is shown in the following code, where the parameters mean:<br><br>isReuseSource: Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |
| T | Data type of the destination operand. Supported data types: bool, half, and float. |
| U | Data type of the source operand. Supported data types: half and float. |

```
struct IsInfConfig {
    bool isReuseSource;
};
```

**Table 2**  API parameter description

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the destination operand is the same as that of the source operand or is bool. For the currently supported data type combinations, see [Table 3](#tab3). |
| src | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex computation inside IsInf. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetIsInfMaxMinTmpSize](GetIsInfMaxMinTmpSize.md). |
| count | Input | Number of elements involved in the computation. |

**Table 3**  Supported data type combinations for input and output<a name="tab3"></a>

| srcDtype | dstDtype |
| --- | --- |
| half | half |
| half | bool |
| float | float |
| float | bool |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   For the operand address offset alignment requirements, see [General Description and Constraints](../../../general_description_and_constraints.md).

## Examples

-   Passed through the sharedTmpBuffer input parameter

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize); // Obtain bufferSize from the tiling parameter on the host side.
    AscendC::LocalTensor<uint8_t> sharedTmpBuffer = tmpQue.AllocTensor<uint8_t>();
    // The input tensor length is 1024, the operator input data type is half, and the actual number of computed elements is 512.
    static constexpr AscendC::IsInfConfig isInfConfig = {false}; // Do not modify the source operand.
    // dst is a LocalTensor of type bool, and src is a LocalTensor of type half.
    AscendC::IsInf<isInfConfig, bool, half>(dst, src, sharedTmpBuffer, 512);
    ```

-   The API framework allocates temporary space.

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize); // Obtain bufferSize from the tiling parameter on the host side.
    // The input tensor length is 1024, the operator input data type is half, and the actual number of computed elements is 512.
    static constexpr AscendC::IsInfConfig isInfConfig = {false}; // Do not modify the source operand.
    // dst is a LocalTensor of the bool type, and src is a LocalTensor of the half type.
    AscendC::IsInf<isInfConfig, bool, half>(dst, src, 512);
    ```

The result is as follows:

```
The input data type is half, and the output data type is bool.
Input data (src): [1.0 inf 3.0 4.0 inf 6.0 -inf 8.0]
Output data (dst): [false true false false true false true false]
```
