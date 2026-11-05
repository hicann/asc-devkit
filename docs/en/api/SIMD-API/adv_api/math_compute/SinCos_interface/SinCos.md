# SinCos

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T03:53:16.012Z pushedAt=2026-10-29T02:15:02.975Z nl=2 -->

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
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/SinCos_interface/SinCos_res.md#id1 -->

## Description

Performs sine and cosine computation element-wise to obtain the sine and cosine results respectively.

![](../../../../figures/zh-cn_formulaimage_0000002386793361.png)

![](../../../../figures/zh-cn_formulaimage_0000002352873228.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter

    ```
    template <const SinCosConfig& config = DEFAULT_SINCOS_CONFIG, typename T>
    __aicore__ inline void SinCos(const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t count)
    ```

-   The API framework requests temporary space

    ```
    template <const SinCosConfig& config = DEFAULT_SINCOS_CONFIG, typename T>
    __aicore__ inline void SinCos(const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src, const uint32_t count)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during computation. Temporary space can be provided in two ways: **passing it through the sharedTmpBuffer input parameter** and **requesting it through the API framework**.

-   When temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer requests it. In this way, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call completes. The memory is not repeatedly requested and released, providing higher flexibility and higher memory utilization.
-   The API framework requests temporary space, so you do not need to request it, but you must reserve the size of the temporary space.

When temporary space is passed through sharedTmpBuffer, you need to request space for the tensor. When the API framework requests temporary space, you need to reserve the temporary space. The size of the temporary space, BufferSize, is obtained as follows: obtain the size of the space to be reserved through the API provided in [GetSinCosMaxMinTmpSize](GetSinCosMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| SinCosConfig | Related configuration of the SinCos algorithm. This parameter is optional and of the SinCosConfig type. Its definition is shown in the following code, where the parameters have the following meanings:<br><br>isReuseSource: Whether the source operand can be modified. This parameter is reserved. Pass the default value false. |
| T | Data type of the operand. Supported data types: half and float. |

```
struct SinCosConfig {
    bool isReuseSource;
};
```

**Table 2**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst0, dst1 | Output | Destination operands.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| src | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be the same as that of the destination operands. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetSinCosMaxMinTmpSize](GetSinCosMaxMinTmpSize.md). |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operands is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operands is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

-   Passed through the sharedTmpBuffer input parameter

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize); // Obtain bufferSize from the tiling parameter on the Host side.
    LocalTensor<uint8_t> sharedTmpBuffer = tmpQue.AllocTensor<uint8_t>();
    // The input tensor length is 1024, the operator input data type is half, and the actual number of computed elements is 512.
    static constexpr AscendC::SinCosConfig sincosConfig = {false}; // Do not modify the source operand.
    // dst0, dst1, and src are LocalTensors of the half type.
    AscendC::SinCos<sincosConfig, half>(dst0, dst1, src, sharedTmpBuffer, 512);
    ```

-   The API framework requests temporary space

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize); // bufferSize is obtained from the tiling parameter on the host side.
    // The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
    static constexpr AscendC::SinCosConfig sincosConfig = {false}; // Do not modify the source operand.
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
