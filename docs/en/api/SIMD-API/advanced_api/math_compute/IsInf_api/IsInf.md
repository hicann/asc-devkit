# IsInf

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T15:50:12.058Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Determines, element by element, whether the input floating-point number is ±INF, and outputs the result as a floating-point number or a Boolean value. When the output is of a floating-point type, the result at the corresponding position is 1 of the floating-point type for input data of ±INF, and 0 otherwise; when the output is of the bool type, the result at the corresponding position is true for input data of ±INF, and false otherwise. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002382902989.png)

-   When the output is of a floating-point type:

    ![](../../../../figures/zh-cn_formulaimage_0000002386808777.png)

-   When the output is of the bool type:

    ![](../../../../figures/zh-cn_formulaimage_0000002386809017.png)

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <const IsInfConfig& config = DEFAULT_IS_INF_CONFIG, typename T, typename U>
    __aicore__ inline void IsInf(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t count)
    ```

-   The API framework allocates temporary space.

    ```
    template <const IsInfConfig& config = DEFAULT_IS_INF_CONFIG, typename T, typename U>
    __aicore__ inline void IsInf(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint32_t count)
    ```

Because the internal implementation of this API involves precision conversion, additional temporary space is required to store intermediate variables during computation. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by you passing it via the sharedTmpBuffer input parameter**.







-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

When **sharedTmpBuffer** is passed, you need to allocate space for the tensor; when the API framework allocates temporary space, you need to reserve the temporary space. The temporary space size **BufferSize** is obtained as follows: use the API provided in [GetIsInfMaxMinTmpSize](GetIsInfMaxMinTmpSize.md) to obtain the size of the space that needs to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| IsInfConfig | Related configuration of the IsInf algorithm. This parameter is optional and of the IsInfConfig type. Its specific definition is shown in the following code, where the parameters mean:<br><br>isReuseSource: Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value **false**. |
| T | Data type of the destination operand. Supported data types are bool, half, and float. |
| U | Data type of the source operand. Supported data types are half and float. |

```
struct IsInfConfig {
    bool isReuseSource;
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the destination operand is the same as that of the source operand or is bool. For the currently supported data type combinations, see [Table 3](#table-3). |
| src | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside IsInf, provided by yourself.<br><br>For how to obtain the temporary space size BufferSize, see [GetIsInfMaxMinTmpSize](GetIsInfMaxMinTmpSize.md). |
| count | Input | Number of elements involved in the computation. |

<a id="table-3"></a>
**Table 3**  Supported data type combinations for input and output

| srcDtype | dstDtype |
| --- | --- |
| half | half |
| half | bool |
| float | float |
| float | bool |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**
-   For the address offset alignment requirements of operands, see [General Description and Constraints](../../../general_description_and_constraints.md).

## Examples

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize);  // Obtain bufferSize through the tiling parameter on the Host side.
    AscendC::LocalTensor<uint8_t> sharedTmpBuffer = tmpQue.AllocTensor<uint8_t>();
    // The input tensor length is 1024, the data type of the operator input is half, and the actual number of elements to compute is 512.
    static constexpr AscendC::IsInfConfig isInfConfig = { false }; // Do not modify the source operand.
    // dst is a LocalTensor of the bool type, and src is a LocalTensor of the half type.
    AscendC::IsInf<isInfConfig, bool, half>(dst, src, sharedTmpBuffer, 512);
    ```

-   The API framework allocates temporary space.

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize);  // bufferSize is obtained from the tiling parameter on the Host side.
    // The input tensor length is 1024, the operator input data type is half, and the actual number of elements to compute is 512.
    static constexpr AscendC::IsInfConfig isInfConfig = { false }; // Do not modify the source operand.
    // dst is a LocalTensor of bool type, and src is a LocalTensor of half type.
    AscendC::IsInf<isInfConfig, bool, half>(dst, src, 512);
    ```

The results are as follows:

```
The input data type is half, and the output data type is bool.
Input data (src): [1.0 inf 3.0 4.0 inf 6.0 -inf 8.0]
Output data (dst): [false true false false true false true false]
```
