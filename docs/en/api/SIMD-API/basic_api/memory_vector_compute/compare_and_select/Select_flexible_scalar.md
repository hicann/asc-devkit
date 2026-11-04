# Select (Flexible Scalar Position)<a name="ZH-CN_TOPIC_0000002171712357"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:59:21.382Z pushedAt=2026-10-28T03:14:39.445Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/compare_and_select/Select_flexible_scalar_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Given two source operands src0 and src1, selects elements according to the bit values of selMask (the mask used for selection) to obtain the destination operand dst. The selection rule is: when the bit of selMask is 1, the element is selected from src0; when the bit is 0, it is selected from src1.

For the tensor high-dimensional slicing computation API, the selected results can be filtered again based on the mask parameter. The significant bits are written to the final dst, while the insignificant bits retain the original dst values. For example, if src0 is \[1,2,3,4,5,6,7,8\], src1 is \[9,10,11,12,13,14,15,16\], selMask is \[0,0,0,0,1,1,1,1\], mask is \[1,1,1,1,0,0,0,0\], and the original dst value is \[-1,-2,-3,-4,-5,-6,-7,-8\], then the result dst\_temp selected based on the bits of selMask is \[9,10,11,12,5,6,7,8\]. After filtering based on mask, the final output of dst is \[9,10,11,12,-5,-6,-7,-8\].

This selection function supports three modes:

-   Mode 0: Selects elements from two tensors based on selMask. The number of significant data in selMask is limited, depending on the data type of the source operands. In each iteration, the selection operation is performed based on the significant bit data of selMask. The selMask used in each iteration is the same value, that is, the significant value of selMask.
-   Mode 1: Selects elements from one tensor and one scalar based on selMask. selMask has no significant data restriction. During multiple iterations, each iteration consecutively uses a different part of selMask.
-   Mode 2: Selects elements from two tensors based on selMask. selMask has no significant data restriction. During multiple iterations, each iteration consecutively uses a different part of selMask.

For mode 1, an API for flexible scalar position is provided.

## Prototype<a name="section620mcpsimp"></a>

-   Tensor First n Elements Computation

    ```cpp
    template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
    __aicore__ inline void Select(const T2& dst, const LocalTensor<T1>& selMask, const T3& src0, const T4& src1, SELMODE selMode, uint32_t count)
    ```

-   Tensor High-Dimensional Split Computation
    -   **mask** bit-by-bit mode

        ```cpp
        template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
        __aicore__ inline void Select(const T2& dst, const LocalTensor<T1>& selMask, const T3& src0, const T4& src1, SELMODE selMode, uint64_t mask[], uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

    -   **mask** continuous mode

        ```cpp
        template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
        __aicore__ inline void Select(const T2& dst, const LocalTensor<T1>& selMask, const T3& src0, const T4& src1, SELMODE selMode, uint64_t mask, uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Meaning |
| :--- | :--- |
| T0 | Data type of the source operands and the destination operand.<br>In particular, for the flexible scalar position API, this is a reserved parameter that is not yet enabled and is reserved for future function extension. When it needs to be specified, pass the default value BinaryDefaultType. |
| T1 | Data type of selMask. |
| isSetMask | Reserved parameter. Keep the default value. To set the mask outside the API, call the API that does not pass the mask parameter. |
| selMode | Same as the selMode parameter description in [Table 2 Parameter Description](#table8955841508). |
| config | Type BinaryConfig. It takes effect when the scalar is of the LocalTensor single-point element type and is used to specify the position of the single-point element operand. The default value DEFAULT_BINARY_CONFIG indicates that the right operand is a scalar.<br>struct BinaryConfig {<br>&nbsp;&nbsp;&nbsp;&nbsp;int8_t scalarTensorIndex = 1; // Specifies the position of the scalar when the scalar is a LocalTensor single-point element. 0 indicates the left operand, and 1 indicates the right operand.<br>};<br>constexpr BinaryConfig DEFAULT_BINARY_CONFIG = {1}; |
| T2 | LocalTensor type. The corresponding data type is automatically inferred based on the input parameter dst. You do not need to configure this parameter. Ensure that dst meets the data type constraints. |
| T3 | LocalTensor type or scalar type. The corresponding data type is automatically inferred based on the input parameter src0. You do not need to configure this parameter. Ensure that src0 meets the data type constraints. |
| T4 | LocalTensor type or scalar type. The corresponding data type is automatically inferred based on the input parameter src1. You do not need to configure this parameter. Ensure that src1 meets the data type constraints. |

**Table 2**  Parameters

<a name="table8955841508"></a>

| Parameter | Input/Output | Meaning |
| :--- | :--- | :--- |
| dst | Output | Destination operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned. |
| selMask | Input | Selection mask.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned.<br>For Ascend 950PR&950DT products, the supported data types are: uint8_t/uint16_t/uint32_t/uint64_t.<br>Each bit indicates the selection of one element. When a bit of selMask is 1, the element is selected from src0; when the bit is 0, the element is selected from src1.<br>When selMode is mode 0, the selection operation is performed based on the significant bit data of selMask in each iteration. The selMask used in each iteration is the same value, that is, the significant value of selMask. When selMode is mode 1/2, selMask is consumed continuously across multiple iterations.<br>- Mode 0: Selects elements from two tensors based on selMask. selMask has a bit count limit. Regardless of the number of iterations, each iteration performs selection based only on the truncated fixed number of selMask bits. When the data type of the source operand is 8-bit, the first 256 bits of selMask are significant; when the data type of the source operand is 16-bit, the first 128 bits of selMask are significant; when the data type of the source operand is 32-bit, the first 64 bits of selMask are significant; when the data type of the source operand is 64-bit, the first 32 bits of selMask are significant.<br>- Mode 1: Selects elements from one tensor and one scalar based on selMask. Multiple iterations are supported. The selection method is as follows: based on the set value of selMask, if the selMask bit value is 1, the value at the same position in src0 is selected; if the selMask bit value is 0, the scalar value is selected. selMask is stored contiguously. When the data type of the source operand is 8-bit, 256 bits of selMask data are obtained per comparison; when the data type of the source operand is 16-bit, 128 bits of selMask data are obtained per comparison; when the data type of the source operand is 32-bit, 64 bits of selMask data are obtained per comparison; when the data type of the source operand is 64-bit, 32 bits of selMask data are obtained per comparison.<br>- Mode 2: Selects elements from two tensors based on selMask. Multiple iterations are supported. The selection method is as follows: based on the set value of selMask, if the selMask bit value is 1, the value at the same position in src0 is selected; if the selMask bit value is 0, the value at the same position in src1 is selected. selMask is stored contiguously. When the data type of the source operand is 8-bit, 256 bits of selMask data are obtained per comparison; when the data type of the source operand is 16-bit, 128 bits of selMask data are obtained per comparison; when the data type of the source operand is 32-bit, 64 bits of selMask data are obtained per comparison; when the data type of the source operand is 64-bit, 32 bits of selMask data are obtained per comparison. |
| src0/src1 | Input | Source operands of the flexible scalar position API.<br>Supported types: LocalTensor or scalar. The data type must be consistent with that of the destination operand.<br>When the type is LocalTensor, it can be used as a vector operand or a scalar single-point element. Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned. |
| selMode | Input | Instruction mode, of the SELMODE type. Valid values are as follows:<br>enum class SELMODE : uint8_t {<br>&nbsp;&nbsp;&nbsp;&nbsp;VSEL_CMPMASK_SPR = 0,<br>&nbsp;&nbsp;&nbsp;&nbsp;VSEL_TENSOR_SCALAR_MODE,<br>&nbsp;&nbsp;&nbsp;&nbsp;VSEL_TENSOR_TENSOR_MODE,<br>};<br>- Mode 0: The value is VSEL_CMPMASK_SPR. Selects elements from two tensors based on selMask. The number of significant data in selMask is limited, depending on the data type of the source operand. In each iteration, the selection operation is performed based on the significant bit data of selMask. The selMask used in each iteration is the same value, that is, the significant value of selMask.<br>- Mode 1: The value is VSEL_TENSOR_SCALAR_MODE. Selects elements from one tensor and one scalar based on selMask. selMask has no significant data restriction. During multiple iterations, each iteration continuously uses a different part of selMask.<br>- Mode 2: The value is VSEL_TENSOR_TENSOR_MODE. Selects elements from two tensors based on selMask. selMask has no significant data restriction. During multiple iterations, each iteration continuously uses a different part of selMask. |
| mask/mask[] | Input | mask controls the elements that participate in the computation within each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 consecutive bytes of data for computation each time. To complete the processing of the input data, multiple iterations (repeat) are required to finish reading and computing all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Slicing API](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. Type: [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md). It contains parameters such as the address stride of the same data block between adjacent iterations of the operands and the address stride of different data blocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride parameters of data blocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |
| count | Input | Number of elements that participate in the computation. |

## Data Type

<!-- npu="950" id8 -->
For Ascend 950PR&950DT products, the data types supported by the source operand dst and the destination operand src are: int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/float/int32_t/uint32_t/complex32/int64_t/uint64_t/complex64. Among them, the int8\_t/uint8\_t/uint64\_t/int64\_t/complex32/complex64 data type supports only the API for data computation on the first n data of the tensor.
<!-- end id8 -->

## Return Value<a name="section198548421851"></a>

None

## Constraints<a name="section633mcpsimp"></a>

-   In the scenario where the flexible scalar position API is called and the source operand is a LocalTensor single-point element, address overlap between the source operand and the destination operand is not supported.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

-   At least one of the left operand and the right operand must be a vector; currently, both operands being scalars simultaneously is not supported.
-   When this API passes LocalTensor single-point data as a scalar, the idx parameter must be a compile-time constant; when a variable is passed, it must be declared as constexpr.
-   In mode 1 scenarios, when using the flexible scalar position API, fill in the template parameter config to prevent the API from matching other modes.

## Example<a name="section642mcpsimp"></a>

-   Example of computing the first n data of Select-tensor (mode 1)

    ```cpp
    // Flexible scalar position, with src1Local[0] as the scalar.
    static constexpr AscendC::BinaryConfig config = { 1 };
    AscendC::Select<BinaryDefaultType, uint8_t, config>(dstLocal, maskLocal, src0Local, src1Local[0], AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE, dataSize);
    
    // Flexible scalar position, with src0Local[0] as the scalar.
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Select<BinaryDefaultType, uint8_t, config>(dstLocal, maskLocal, src0Local[0], src1Local, AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE, dataSize);
    ```
