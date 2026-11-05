# IsNan

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T03:15:51.563Z pushedAt=2026-10-29T02:15:02.803Z nl=2 -->

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
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Vector Core of Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/IsNan_interface/IsNan_res.md#id1 -->

## Description

Determines element-wise whether the input floating-point number is NaN, and outputs the result as a floating-point number or a Boolean value. When the output is of a floating-point type, for NaN input data, the result at the corresponding position is 1 of the floating-point type; otherwise, it is 0. When the output is of the bool type, for NaN input data, the result at the corresponding position is true; otherwise, it is false. The computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002349022528.png)

-   When the output is a floating-point type:

    ![](../../../../figures/zh-cn_formulaimage_0000002353021272.png)

-   When the output is of the bool type:

    ![](../../../../figures/zh-cn_formulaimage_0000002352887900.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter

    ```
    template <const IsNanConfig& config = DEFAULT_IS_NAN_CONFIG, typename T, typename U>
    __aicore__ inline void IsNan(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t count)
    ```

-   The API framework allocates temporary space

    ```
    template <const IsNanConfig& config = DEFAULT_IS_NAN_CONFIG, typename T, typename U>
    __aicore__ inline void IsNan(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint32_t count)
    ```

Because the internal implementation of this API involves precision conversion, additional temporary space is required to store intermediate variables during computation. Temporary space can be provided in two ways: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   When temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework does not allocate additional space. In this way, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.
-   When the API framework allocates temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

When temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor. When the API framework allocates temporary space, you need to reserve the temporary space. The size of the temporary space, BufferSize, is obtained as follows: use the API provided in [GetIsNanMaxMinTmpSize](GetIsNanMaxMinTmpSize.md) to obtain the size of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| IsNanConfig | Configuration of the IsNan algorithm. This parameter is optional and of the IsNanConfig type. Its definition is shown in the following code, where the parameters have the following meanings:<br><br>isReuseSource: Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |
| T | Data type of the destination operand. Supported data types: bool, half, and float. |
| U | Data type of the source operand. Supported data types: half and float. |

```
struct IsNanConfig {
    bool isReuseSource;
};
```

**Table 2**  API parameter description

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the destination operand is the same as that of the source operand or is bool. For the currently supported data type combinations, see [Table 3](#tab3). |
| src | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex computation inside IsNan. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetIsNanMaxMinTmpSize](GetIsNanMaxMinTmpSize.md). |
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

-   **Overlapping between the source operand and the destination operand is not supported.**
-   For the address offset alignment requirements of operands, see [General Description and Constraints](../../../general_description_and_constraints.md).

## Examples

-   Passed in through the sharedTmpBuffer parameter.

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize); // bufferSize is obtained from the tiling parameter on the host side.
    AscendC::LocalTensor<uint8_t> sharedTmpBuffer = tmpQue.AllocTensor<uint8_t>();
    // The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
    static constexpr AscendC::IsNanConfig isNanConfig = {false}; // Do not modify the source operand.
    // dst is a LocalTensor of the bool type, and src is a LocalTensor of the half type.
    AscendC::IsNan<isNanConfig, bool, half>(dst, src, sharedTmpBuffer, 512);
    ```

-   The API framework allocates temporary space.

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize); // bufferSize is obtained from the tiling parameter on the host side.
    // The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
    static constexpr AscendC::IsNanConfig isNanConfig = {false}; // Do not modify the source operand.
    // dst is a LocalTensor of the bool type, and src is a LocalTensor of the half type.
    AscendC::IsNan<isNanConfig, bool, half>(dst, src, 512);
    ```

The result example is as follows:

```
The input data type is half, and the output data type is bool.
Input data (src):[1.0 nan 3.0 4.0 nan 6.0 nan 8.0]
Output data (dst):[false true false false true false true false]
```
