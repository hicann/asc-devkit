# Rint

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T09:25:42.397Z -->

## Product Support

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the integer closest to the input data. If two integers are equally close, the even one is obtained. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002352876716.png)

Examples:

Rint\(3.9\) = 4

Rint\(3.3\) = 3

Rint\(3.5\) = 4

## Prototype

-   Pass temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <const RintConfig& config = DEFAULT_RINT_CONFIG, typename T>
    __aicore__ inline void Rint(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t count)
    ```

-   The API framework applies for temporary space.

    ```
    template <const RintConfig& config = DEFAULT_RINT_CONFIG, typename T>
    __aicore__ inline void Rint(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space can be provided in two ways: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   Pass temporary space through the **sharedTmpBuffer** input parameter, using this tensor as the temporary space for processing, and the API framework no longer applies for it. In this way, you can manage the **sharedTmpBuffer** memory space on your own and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, offering high flexibility and high memory utilization.
-   The API framework applies for temporary space, so you do not need to apply for it, but you need to reserve the size of the temporary space.

When temporary space is passed through **sharedTmpBuffer**, you need to allocate space for the tensor. When the API framework applies for temporary space, you need to reserve the temporary space. The temporary space size **BufferSize** is obtained as follows: use the API provided in [GetRintMaxMinTmpSize](GetRintMaxMinTmpSize.md) to obtain the size of the space range that needs to be reserved.

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| **RintConfig** | Rint algorithm-related configuration. This parameter is optional and of the **RintConfig** type. Its definition is shown in the following code, where the parameter meanings are as follows:<br><br>**isReuseSource**: Whether the source operand can be modified. This parameter is reserved. Pass the default value **false**. |
| **T** | Data type of the operand. Supported data types: **half** and **float**. |

```
struct RintConfig {
    bool isReuseSource;
};
```

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **dst** | Output | Destination operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| **src** | Input | Source operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be the same as that of the destination operand. |
| **sharedTmpBuffer** | Input | Temporary buffer.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside Rint. Provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetRintMaxMinTmpSize](GetRintMaxMinTmpSize.md). |
| **count** | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **The source operand and destination operand addresses must not overlap.**
-   **sharedTmpBuffer** must not overlap with the source operand and destination operand addresses.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

For the complete sample, see the [Rint operator sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/10_math/rint).

-   Pass through the **sharedTmpBuffer** parameter.

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize);  // bufferSize is obtained from the tiling parameter on the host side.
    AscendC::LocalTensor<uint8_t> sharedTmpBuffer = tmpQue.AllocTensor<uint8_t>();
    // The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
    static constexpr AscendC::RintConfig rintConfig = { false }; // Do not modify the source operand.
    // dst and src are half-type LocalTensor.
    AscendC::Rint<rintConfig, half>(dst, src, sharedTmpBuffer, 512);
    ```

-   The API framework applies for temporary space.

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize);  // bufferSize is obtained from the tiling parameter on the host side.
    // The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
    static constexpr AscendC::RintConfig rintConfig = { false }; // Do not modify the source operand.
    // dst and src are LocalTensor of the half type.
    AscendC::Rint<rintConfig, half>(dst, src, 512);
    ```

The result example is as follows:

```
Input data (src): [-1.5, -1.3, -0.9, -0.5, 0.5, 0.9, 1.1, 1.5, 2.2, 2.5, 2.8, 3.2]
Output data (dst): [-2, -1, -1, 0, 0, 1, 1, 2, 2, 2, 3, 3]
```
