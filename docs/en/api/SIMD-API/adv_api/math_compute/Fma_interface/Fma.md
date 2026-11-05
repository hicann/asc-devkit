# Fma

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T02:57:33.955Z pushedAt=2026-10-29T02:15:02.721Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Fma_interface/Fma_res.md#id1 -->

## Description

Computes the element-wise result of multiplying two inputs and then adding a third input. The computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002353015644.png)

![](../../../../figures/zh-cn_formulaimage_0000002386692741.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter.

    ```
    template <const FmaConfig& config = DEFAULT_FMA_CONFIG, typename T>
    __aicore__ inline void Fma(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,const LocalTensor<T>& src2, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t count)
    ```

-   The API framework allocates temporary space.

    ```
    template <const FmaConfig& config = DEFAULT_FMA_CONFIG, typename T>
    __aicore__ inline void Fma(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const LocalTensor<T>& src2, const uint32_t count)
    ```

Because the internal implementation of this API involves precision conversion, additional temporary space is required to store intermediate variables during computation. Temporary space can be provided in either of the following two ways: **passing it through the sharedTmpBuffer input parameter** or **having the API framework allocate it**.

-   Pass temporary space through the sharedTmpBuffer input parameter. In this case, the tensor is used as temporary space for processing, and the API framework does not allocate additional space. You can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, which offers higher flexibility and higher memory utilization.
-   The API framework allocates temporary space. In this case, you do not need to allocate it, but you must reserve the size of the temporary space.

When temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor. When the API framework allocates temporary space, you need to reserve the temporary space. The size of the temporary space, BufferSize, is obtained as follows: use the API provided in [GetFmaMaxMinTmpSize](GetFmaMaxMinTmpSize.md) to obtain the size of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| FmaConfig | Fma algorithm configuration. This parameter is optional and of the FmaConfig type. Its definition is shown in the following code, where the parameters have the following meanings:<br><br>isReuseSource: Whether the source operands are allowed to be modified. This parameter is reserved. Pass the default value false. |
| T | Data type of the operands. Supported data types: half and float. |

```
struct FmaConfig {
    bool isReuseSource;
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition values are VECIN/VECCALC/VECOUT. |
| src0, src1, src2 | Input | Source operands.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operands must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside Fma. Provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetFmaMaxMinTmpSize](GetFmaMaxMinTmpSize.md). |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operands and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operands and the destination operand is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

-   Passed in through the sharedTmpBuffer input parameter

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize); // bufferSize is obtained from the tiling parameter on the host side.
    AscendC::LocalTensor<uint8_t> sharedTmpBuffer = tmpQue.AllocTensor<uint8_t>();
    // The input tensor length is 1024, the operator input data type is half, and the actual number of elements to compute is 512.
    static constexpr AscendC::FmaConfig fmaConfig = {false}; // Do not modify the source operands.
    // dst, src0, src1, and src2 are LocalTensor of the half type.
    AscendC::Fma<fmaConfig, half>(dst, src0, src1, src2, sharedTmpBuffer, 512);
    ```

-   The API framework allocates temporary space

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize); // Obtain bufferSize from the tiling parameter on the host side.
    // The input tensor length is 1024, the operator input data type is half, and the actual number of elements to compute is 512.
    static constexpr AscendC::FmaConfig fmaConfig = {false}; // Do not modify the source operands.
    // dst, src0, src1, and src2 are LocalTensors of the half type.
    AscendC::Fma<fmaConfig, half>(dst, src0, src1, src2, 512);
    ```

The result example is as follows:

```
Input data (src0): [1 1 1 ...... 1 1 1]
Input data (src1): [2 2 2 ...... 2 2 2]
Input data (src2): [1 1 1 ...... 1 1 1]
Output data (dst): [3 3 3 ...... 3 3 3]
```
