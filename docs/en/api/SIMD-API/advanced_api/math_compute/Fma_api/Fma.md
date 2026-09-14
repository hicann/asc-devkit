# Fma

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T15:10:43.234Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the result of multiplying two inputs element-wise and then adding a third input. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002353015644.png)

![](../../../../figures/zh-cn_formulaimage_0000002386692741.png)

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <const FmaConfig& config = DEFAULT_FMA_CONFIG, typename T>
    __aicore__ inline void Fma(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,const LocalTensor<T>& src2, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t count)
    ```

-   The API framework allocates temporary space.

    ```
    template <const FmaConfig& config = DEFAULT_FMA_CONFIG, typename T>
    __aicore__ inline void Fma(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const LocalTensor<T>& src2, const uint32_t count)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by you passing it via the sharedTmpBuffer input parameter**.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

When the temporary space is passed through **sharedTmpBuffer**, you need to allocate space for the tensor. When the API framework allocates it, you need to reserve the temporary space. The size of the temporary space, **BufferSize**, is obtained as follows: obtain the size of the space that needs to be reserved through the API provided in [GetFmaMaxMinTmpSize](GetFmaMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| FmaConfig | Fma algorithm-related configuration. This parameter is optional and is of the FmaConfig type. Its specific definition is shown in the following code, where the parameter meanings are as follows:<br><br>isReuseSource: Whether the source operands are allowed to be modified. This parameter is reserved; pass the default value **false**. |
| T | Data type of the operand. Supported data types are half and float. |

```
struct FmaConfig {
    bool isReuseSource;
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src0, src1, src2 | Input | Source operands.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside Fma, provided by yourself.<br><br>For how to obtain the temporary space size BufferSize, see [GetFmaMaxMinTmpSize](GetFmaMaxMinTmpSize.md). |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize);  // bufferSize is obtained from the tiling parameter on the host side.
    AscendC::LocalTensor<uint8_t> sharedTmpBuffer = tmpQue.AllocTensor<uint8_t>();
    // The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
    static constexpr AscendC::FmaConfig fmaConfig = { false }; // Do not modify the source operands.
    // dst, src0, src1, and src2 are LocalTensors of the half type.
    AscendC::Fma<fmaConfig, half>(dst, src0, src1, src2, sharedTmpBuffer, 512);
    ```

-   The API framework allocates temporary space.

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize);  // bufferSize is obtained from the tiling parameter on the host side.
    // The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
    static constexpr AscendC::FmaConfig fmaConfig = { false }; // Do not modify the source operands.
    // dst, src0, src1, and src2 are LocalTensors of the half type.
    AscendC::Fma<fmaConfig, half>(dst, src0, src1, src2, 512);
    ```

The results are as follows:

```
Input data (src0):[1 1 1 ...... 1 1 1]
Input data (src1):[2 2 2 ...... 2 2 2]
Input data (src2):[1 1 1 ...... 1 1 1]
Output data (dst):[3 3 3 ...... 3 3 3]
```
