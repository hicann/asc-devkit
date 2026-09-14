# SinCos

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T10:09:53.136Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training series products/Atlas A3 inference series products: not supported
- Atlas A2 training series products/Atlas A2 inference series products: not supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference series products AI Core: not supported
- Atlas inference series products Vector Core: not supported
- Atlas training series products: not supported

## Description

Performs sine and cosine calculations element-wise to obtain the sine and cosine results, respectively.

![](../../../../figures/zh-cn_formulaimage_0000002386793361.png)

![](../../../../figures/zh-cn_formulaimage_0000002352873228.png)

## Prototype

-   Pass temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <const SinCosConfig& config = DEFAULT_SINCOS_CONFIG, typename T>
    __aicore__ inline void SinCos(const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t count)
    ```

-   Apply for temporary space through the API framework.

    ```
    template <const SinCosConfig& config = DEFAULT_SINCOS_CONFIG, typename T>
    __aicore__ inline void SinCos(const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src, const uint32_t count)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. Temporary space can be provided in two ways: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   When temporary space is passed through the **sharedTmpBuffer** input parameter, this tensor is used as the temporary space for processing, and the API framework no longer applies for it. With this method, you can manage the **sharedTmpBuffer** memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, which provides higher flexibility and higher memory utilization.
-   When the API framework applies for temporary space, you do not need to apply for it, but you need to reserve the size of the temporary space.

When temporary space is passed through **sharedTmpBuffer**, you need to apply for space for the tensor. When using the API framework allocation method, you need to reserve the temporary space. The size of the temporary space, **BufferSize**, is obtained as follows: use the API provided in [GetSinCosMaxMinTmpSize](GetSinCosMaxMinTmpSize.md) to obtain the size of the space range that needs to be reserved.

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| SinCosConfig | Related configuration of the SinCos algorithm. This parameter is optional and of the SinCosConfig type. Its definition is shown in the following code, where the parameters have the following meanings:<br><br>isReuseSource: Whether the source operand can be modified. This parameter is reserved. Pass the default value **false**. |
| T | Data type of the operand. Supported data types: **half** and **float**. |

```
struct SinCosConfig {
    bool isReuseSource;
};
```

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst0, dst1 | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand is the same as that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetSinCosMaxMinTmpSize](GetSinCosMaxMinTmpSize.md). |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between **sharedTmpBuffer** and the source operand or the destination operand is not supported.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

-   Passed through the **sharedTmpBuffer** input parameter

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize); // bufferSize is obtained from the tiling parameter on the host side.
    LocalTensor<uint8_t> sharedTmpBuffer = tmpQue.AllocTensor<uint8_t>();
    // The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
    static constexpr AscendC::SinCosConfig sincosConfig = { false };  // Do not modify the source operand.
    // dst0, dst1, and src are LocalTensor of the half type.
    AscendC::SinCos<sincosConfig, half>(dst0, dst1, src, sharedTmpBuffer, 512);
    ```

-   Temporary space applied for by the API framework

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize); // bufferSize is obtained from the tiling parameter on the host side.
    // The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
    static constexpr AscendC::SinCosConfig sincosConfig = { false };  // Do not modify the source operand.
    // dst0, dst1, and src are LocalTensors of the half type.
    AscendC::SinCos<sincosConfig, half>(dst0, dst1, src, 512);
    ```

The result example is as follows:

```
Input data (src0):
[ -360, -270, -180, -90, 0, 90, 180, 270, 360 ]
Output data (dst0):
[ 0, 1, 0, -1, 0, 1, 0, -1, 0]
Output data (dst1):
[ 1, 0, -1, 0, 1, 0, -1, 0, 1]
```
