# Fixpipe (L0C Buffer to UB Data Movement)<a name="ZH-CN_TOPIC_0000002563847576"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:42:39.463Z pushedAt=2026-10-26T07:41:44.131Z nl=2 -->

## Applicable Products<a name="zh-cn_topic_0000002549846732_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported APIs that include the FixpipeParamsArch3510 parameter.
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Support APIs that include the FixpipeParamsM300 parameter.
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToUB_res.md#id1 -->

## Description<a name="zh-cn_topic_0000002549846732_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_fixpipe_intf.h"`.

The matrix computation results are stored in the L0C Buffer. The Fixpipe API is used to move the results to the Unified Buffer (UB), and it supports operations such as inline format conversion during the movement.

<!-- npu="950" id10 -->
Taking the Ascend 950PR&950DT products as an example, the following figure shows the valid combinations of inline quantization, inline ReLU, and inline channel merging, as well as the intermediate data types and data paths. In the figure, F32-\>F16 and F32-\>BF16 are non-quantization modes that only perform Cast, while the rest are inline scalar/tensor quantization modes.

**Figure 2** L0C2UB flowchart<a id="zh-cn_topic_0000002542828493_fig1828513492547"></a>  

![](../../../../figures/L0C2UB_Function_Combination_950.png)
<!-- end id10 -->

## Prototype<a name="zh-cn_topic_0000002549846732_section82039854412"></a>

The L0C Buffer-to-UB data movement provides the combined matrix move-out API **Fixpipe**, which includes register setting and data movement capabilities. The corresponding APIs are as follows:

<!-- npu="950" id11 -->
For Ascend 950PR&950DT products:

- L0C Buffer->UB path, without the tensor quantization function enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

- L0C Buffer->UB path, with tensor quantization enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint64_t>& cbufWorkspace, const FixpipeParamsArch3510<config.format>& intriParams)
    ```
<!-- end id11 -->

<!-- npu="310b" id12 -->
For Atlas 200I/500 A2 inference products:

- L0C Buffer->UB path, without the tensor quantization function enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsM300& intriParams)
    ```

- Path L0C Buffer->UB, with the tensor quantization function enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR, typename S = uint64_t, typename Std::enable_if<Std::is_same<PrimT<S>, uint64_t>::value, bool>::type = true>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>& cbufWorkspace, const FixpipeParamsM300& intriParams)
    ```
<!-- end id12 -->

**Note:**

- **When calling the L0C Buffer-to-UB Fixpipe API, the value of the template parameter "const FixpipeConfig& config" cannot be `CFG_ROW_MAJOR`, `CFG_COLUMN_MAJOR`, or `CFG_NZ`. Instead, a configuration with `isToUB = true` must be explicitly constructed, indicating that the destination physical address is UB.**

## Parameters<a name="zh-cn_topic_0000002549846732_section16128134420472"></a>

**Table 1** Fixpipe template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T/U | Data type of the destination operand/source operand. For supported data types, see [Data Types](#zh-cn_topic_0000002549846732_section4219135304818). |
| config | Fixpipe-related configuration parameters, of the type FixpipeConfig. You need to explicitly construct a configuration with `isToUB = true`, which indicates that the destination physical address is UB. The construction example is as follows:<br>When NZ2ND is enabled, the output data format is ND:<br>constexpr AscendC::FixpipeConfig CFG_ROW_MAJOR_UB = {AscendC::CO2Layout::ROW_MAJOR, true};<br>For NZ2NZ, the output data format is NZ:<br>constexpr AscendC::FixpipeConfig CFG_NZ_UB = {AscendC::CO2Layout::NZ, true};<br>When NZ2DN is enabled, the output data format is DN:<br>constexpr AscendC::FixpipeConfig CFG_COLUMN_MAJOR_UB = {AscendC::CO2Layout::COLUMN_MAJOR, true};<br>The FixpipeConfig structure is defined as follows:<br>struct FixpipeConfig {<br>    CO2Layout format;<br>    bool isToUB; // Used to specify whether the destination address is UB. <br>};<br>enum class CO2Layout : uint8_t {<br>    NZ = 0, // The output data format remains NZ.<br>    ROW_MAJOR, // When NZ2ND is enabled, the output data format is ND.<br><!-- npu="950" id15 -->    COLUMN_MAJOR, // Supported only by Ascend 950PR&950DT products. When NZ2DN is enabled, the output data format is DN.<br><!-- end id15 -->};<br>constexpr FixpipeConfig CFG_NZ = {CO2Layout::NZ};<br>constexpr FixpipeConfig CFG_ROW_MAJOR = {CO2Layout::ROW_MAJOR};<!-- npu="950" id16 --><br>constexpr FixpipeConfig CFG_COLUMN_MAJOR = {CO2Layout::COLUMN_MAJOR}; // Supported only by Ascend 950PR&950DT products.<!-- end id16 --> |
| S | Data type of the cbufWorkspace parameter, that is, the data type of the inline quantization parameter.<br>&nbsp;&nbsp;&bull;When the destination operand, source operand, and cbufWorkspace use basic data types, the template parameter S must be of the uint64_t type; otherwise, compilation fails.<br>&nbsp;&nbsp;&bull;When the destination operand, source operand, and cbufWorkspace use the TensorTrait type, the LiteType of the template parameter S must be of the uint64_t type; otherwise, compilation fails.<br>The template parameter after S is used only for the preceding data type check, and users do not need to be concerned with it. |

**Table 2** Fixpipe parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of the type LocalTensor. The address must meet 32-byte alignment. |
| src | Input | Source operand, of the type LocalTensor. The supported physical address is L0C Buffer (TPosition is CO1), which is the result computed by the Mmad API. The data format is NZ, and the address must meet 64B alignment. |
| intriParams | Input | Fixpipe movement parameter. For the specific definition, see "basic_api/kernel_struct_fixpipe.h".<br>For the parameter description, see the description of the Fixpipe movement parameter structures (FixpipeParamsArch3510 and FixpipeParamsM300). |
| cbufWorkspace | Input | Quantization parameter, of the type `LocalTensor<uint64_t>`. The supported physical address is L1 Buffer (TPosition is C1), and the address must meet 32-byte alignment.<br>&nbsp;&nbsp;&bull;Supported when quantPre is VDEQF16, VQF322B8_PRE, or VREQ8.<br><!-- npu="950" id13 -->&nbsp;&nbsp;&bull;For Ascend 950PR&950DT products, in addition to the preceding modes, VQF322FP8_PRE, VQF322HIF8_PRE, VQF322HIF8_PRE_HYBRID, VQS322BF16_PRE, VQF322F16_PRE, VQF322BF16_PRE, and VQF322F32_PRE are also supported.<br><!-- end id13 -->For the introduction to quantPre, see the quantPre section in the Fixpipe movement parameter structure. |

**Table 3** Description of the Fixpipe movement parameter (FixpipeParamsArch3510) structure

| Parameter | Data Type | Description |
| --- | --- | --- |
| nSize | Mandatory input | Size of the source NZ matrix in the N direction, with a value range of nSize∈[0, 4095].<br>&nbsp;&nbsp;&bull; If the [channelSplit](../cube_store_key_features/F32-Channel-Split.md) function is enabled, nSize must be a multiple of 8.<br>&nbsp;&nbsp;&bull; If the channelSplit function is not enabled, nSize must be a multiple of 16.<br>&nbsp;&nbsp;&bull; If NZ2ND is enabled, nSize*sizeof(T) must be a multiple of 32.<br>**Note: nSize=0 indicates that no movement is performed, and this API is treated as a NOP (no operation).** |
| mSize | Mandatory input | Size of the source NZ matrix in the M direction, with a value range of mSize∈[0, 65535]. If NZ2DN is enabled, mSize*sizeof(T) must be a multiple of 32.<br>**Note: mSize=0 indicates that no movement is performed, and this API is treated as a NOP (no operation).** |
| srcStride | Mandatory input | Start address offset of adjacent Z layouts in the source NZ matrix, with a value range of srcStride∈[0, 65535], in the unit of C0_Size (16*sizeof(T)), where T is the data type of src. Its value should be set to mSize rounded up to a multiple of 16. |
| dstStride | Mandatory input | &bull; When NZ2ND/NZ2DN is not enabled ([NZ2NZ movement](../cube_store_key_features/NZ2NZ.md)): start address offset of adjacent Z layouts in the destination NZ matrix, with a non-zero value, in the unit of element. (Compared with dstStride in the Fixpipe movement parameter (FixpipeParamsV220), the unit has changed. In FixpipeParamsV220, the unit of dstStride is datablock (32 bytes), while in FixpipeParamsArch3510, the unit of dstStride is element.)<br>&bull; When [NZ2ND](../cube_store_key_features/NZ2ND.md)/[NZ2DN](../cube_store_key_features/NZ2DN.md) is enabled: number of elements in each row of the destination ND/DN matrix, with a non-zero value, in the unit of element. |
| [quantPre](../cube_store_key_features/accompanying_quantization.md) | Optional input | Used to control the quantization mode, of the QuantMode_t type, with a default value of `QuantMode_t::NoQuant`. The specific definition is as follows:<br>Note: This parameter needs to be manually configured by the user and is not automatically derived to configure the corresponding quantization mode.<br>enum QuantMode_t<br>{<br>    NoQuant,               // Disable the quantization function.<br>    F322F16,               // Float32_2_Float16: cast float to half, with the cast mode of CAST_RINT.<br>    F322BF16,              // Float32_2_BFloat16: cast float to bfloat16_t, with the cast mode of CAST_RINT.<br>    DEQF16,                // DeQuant_Float16: quantize int32_t to half, scalar quantization.<br>    VDEQF16,               // Vector_DeQuant_Float16: quantize int32_t to half, tensor quantization.<br>    QF322B8_PRE,           // Quant_Float32_2_B8: quantize float to int8_t/uint8_t, scalar quantization.<br>    VQF322B8_PRE,          // Vector_Quant_Float32_2_B8: quantize float to int8_t/uint8_t, tensor quantization.<br>    REQ8,                  // ReQuant_int8: quantize int32_t to int8_t/uint8_t, scalar quantization.<br>    VREQ8,                 // Vector_ReQuant_int8: quantize int32_t to int8_t/uint8_t, tensor quantization.<br>    QF322FP8_PRE,          // Quant_Float32_2_FP8: quantize float to fp8_e4m3fn_t, scalar quantization.<br>    VQF322FP8_PRE,         // Vector_Quant_Float32_2_FP8: quantize float to fp8_e4m3fn_t, tensor quantization.<br>    QF322HIF8_PRE,         // Quant_Float32_2_HIF8: quantize float to hifloat8_t (Half to Away Round), scalar quantization.<br>    VQF322HIF8_PRE,        // Vector_Quant_Float32_2_HIF8: quantize float to hifloat8_t (Half to Away Round), tensor quantization.<br>    QF322HIF8_PRE_HYBRID,  // Quant_Float32_2_HIF8_Hybrid: quantize float to hifloat8_t (Hybrid Round), scalar quantization.<br>    VQF322HIF8_PRE_HYBRID, // Vector_Quant_Float32_2_HIF8_Hybrid: quantize float to hifloat8_t (Hybrid Round), tensor quantization.<br>    QS322BF16_PRE,         // Quant_Int32_2_BFloat16: quantize int32_t to bfloat16_t, scalar quantization.<br>    VQS322BF16_PRE,        // Vector_Quant_Int32_2_BFloat16: quantize int32_t to bfloat16_t, tensor quantization.<br>    QF322F16_PRE,          // Quant_Float32_2_Float16: quantize float to half, scalar quantization.<br>    VQF322F16_PRE,         // Vector_Quant_Float32_2_Float16: quantize float to half, tensor quantization.<br>    QF322BF16_PRE,         // Quant_Float32_2_BFloat16: quantize float to bfloat16_t, scalar quantization.<br>    VQF322BF16_PRE,        // Vector_Quant_Float32_2_BFloat16: quantize float to bfloat16_t, tensor quantization.<br>    QF322F32_PRE,          // Quant_Float32_2_Float32: quantize float to float, scalar quantization. The precision can reach dual-0.1% but cannot reach dual-0.01%. If a precision of dual-0.01% is required, use the [AscendDeQuant](../../../adv_api/quantization/AscendDequant.md) high-level API.<br>    VQF322F32_PRE,         // Vector_Quant_Float32_2_Float32: quantize float to float, tensor quantization. The precision can reach dual-0.1% but cannot reach dual-0.01%. If a precision of dual-0.01% is required, use the [AscendDeQuant](../../../adv_api/quantization/AscendDequant.md) high-level API.<br>}; |
| deqScalar | Optional input | Scalar quantization parameter, indicating a single scale value. This parameter needs to be set when the quantPre quantization mode is [inline quantization](../cube_store_key_features/accompanying_quantization.md). The supported data type is `uint64_t`. |
| [reluEn](../cube_store_key_features/accompanying_relu.md) | Optional input | Switch for enabling NormReLU:<br>&nbsp;&nbsp;&bull; `false`: disables the NormReLU function, and the default value is `false`;<br>&nbsp;&nbsp;&bull; `true`: enables the NormReLU function. |
| unitFlag | Optional input | unitFlag is a fine-grained parallelism between the Mmad instruction and the Fixpipe instruction. After this function is enabled, each time the hardware finishes computing a fractal, the computation result is moved out. The value description is as follows:<br>&nbsp;&nbsp;&bull; 0 (2'b00): disables unitFlag;<br>&nbsp;&nbsp;&bull; 2 (2'b10): enables unitFlag. After the hardware finishes executing the instruction, the unit flag bit is not reset;<br>&nbsp;&nbsp;&bull; 3 (2'b11): enables unitFlag. After the hardware finishes executing the instruction, the unit flag bit is reset.<br>When this function is enabled, the unitFlag values of the Mmad instruction and the Fixpipe instruction must be set to 2 or 3.<br>For the parameter setting scheme and feature details, see the [UnitFlag](../mmad_compute_key_features/UnitFlag.md) section in the description of key features in Mmad computation. |
| params | Optional input | Used to select and configure different inline format conversion (NZ2NZ/NZ2ND/NZ2DN). This parameter is a structure of the TransformParams type. The TransformParams structure is a type selector based on template parameters, used to automatically select the corresponding parameter type at compile time based on the template parameters used when defining the FixpipeParamsArch3510 movement parameter.<br>template \<CO2Layout format\><br>struct TransformParams {};<br>template \<\><br>struct TransformParams\<CO2Layout::NZ\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = uint8_t;<br>};<br>template \<\><br>struct TransformParams\<CO2Layout::ROW_MAJOR\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = Nz2NdParams;<br>};<br>template \<\><br>struct TransformParams\<CO2Layout::COLUMN_MAJOR\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = Nz2DnParams;<br>};<br>When the CO2Layout layout type is ROW_MAJOR, this instruction is defined as data movement from L0C Buffer to the destination location, with NZ2ND conversion attached. The structure parameters are as follows:<br>struct Nz2NdParams {<br>    uint16_t ndNum = 1;<br>    uint16_t srcNdStride = 0;<br>    uint32_t dstNdStride = 0; <br>};<br>&nbsp;&nbsp;&bull; ndNum: number of source NZ matrices, that is, the number of ND matrices to be transferred, with a value range of ndNum∈[0, 65535]. **Note: ndNum=0 indicates that no movement is performed, and this API is treated as a NOP (no operation).**<br>&nbsp;&nbsp;&bull; srcNdStride: interval between the start addresses of different NZ matrices, with a value range of srcNdStride∈[0, 65535], in the unit of C0_SIZE. When ndNum is set to 1, srcNdStride can be set to 0 and does not take effect.<br>&nbsp;&nbsp;&bull; dstNdStride: offset between the start addresses of adjacent destination ND matrices, with a value range of dstNdStride∈[1, 2^32 - 1], in the unit of element. When ndNum is set to 1, dstNdStride can be set to 0 and does not take effect.<br><br>When the CO2Layout layout type is COLUMN_MAJOR, this instruction is defined as data movement from L0C Buffer to the destination location, with NZ2DN conversion attached. The structure parameters are as follows:<br>struct Nz2DnParams {<br>    uint16_t dnNum = 1;<br>    uint16_t srcNzMatrixStride = 0;<br>    uint32_t dstDnMatrixStride = 0;<br>    uint16_t srcNzC0Stride = 0; <br>};<br>&nbsp;&nbsp;&bull; dnNum: number of DN matrices to be transferred, with a value range of dnNum∈[0, 65535]. **Note: dnNum=0 indicates that no movement is performed, and this API is treated as a NOP (no operation).**<br>&nbsp;&nbsp;&bull; srcNzMatrixStride: offset between different source NZ matrices (head to head), with a value range of srcNzMatrixStride∈[0, 65535], in the unit of C0_SIZE. When dnNum is set to 1, srcNzMatrixStride can be set to 0 and does not take effect.<br>&nbsp;&nbsp;&bull; dstDnMatrixStride: offset between the start addresses of adjacent destination ND matrices, with a value range of dstDnMatrixStride∈[0, 2^32 - 1], in the unit of element. When dnNum is set to 1, dstDnMatrixStride can be set to 0 and does not take effect.<br>&nbsp;&nbsp;&bull; srcNzC0Stride: address offset of adjacent rows in the source matrix NZ fractal (head to head), with a value range of srcNzC0Stride∈[0, 65535], in the unit of C0_SIZE. When NZ2DN is enabled, srcNzC0Stride cannot be 0.<br><br>When the CO2Layout layout type is NZ, it is the normal DMA movement mode, indicating normal data movement from L0C Buffer to the destination location. |
| dualDstCtrl | Optional input | Dual-destination mode control parameter. For Ascend 950PR&950DT products, there is one Cube Core and two Vector Cores in the same AI Core. When dual-destination mode control is enabled, the M×N matrix in L0C Buffer is split into two halves and written to the UBs of the two Vector Cores simultaneously, where the first half is written to SUB BLOCK0 and the second half is written to SUB BLOCK1.<br>&nbsp;&nbsp;&bull; 2'b00: single-destination mode, in which the entire matrix is written to the destination UB configured through the subBlockId parameter.<br>&nbsp;&nbsp;&bull; 2'b01: dual-destination mode, in which the matrix is split into two matrices of shape M / 2 \* N along the M dimension and written to two UBs respectively. M must be a multiple of 2.<br>&nbsp;&nbsp;&bull; 2'b10: dual-destination mode, in which the matrix is split into two matrices of shape M \* N / 2 along the N dimension and written to two UBs respectively. N must be a multiple of 32.<br>&nbsp;&nbsp;&bull; 2'b11: reserved value.<br>dualDstCtrl is supported only in the normal movement mode (NZ2NZ) or the NZ2ND movement scenario, and is not supported in inline function scenarios.<br>For the parameter setting scheme and feature details, see [L0C-to-UB Dual-Destination Mode](../cube_store_key_features/L0C_to_UB_dual_target_mode.md). |
| subBlockId | Optional input | Used to indicate the SUB BLOCK ID of the destination UB when single-destination mode is enabled. The value is 0 or 1. A value of 0 writes to SUB BLOCK0, and a value of 1 writes to SUB BLOCK1. The default value is 0. |
| [isChannelSplit](../cube_store_key_features/F32-Channel-Split.md) | Optional input | This parameter is valid only on the L0C Buffer-to-Global Memory path. |

**Table 4** Description of the Fixpipe movement parameter (FixpipeParamsM300) structure

| Parameter | Data Type | Description |
| ---------- | ---------- | ---------- |
| nSize | Input | Size of the source NZ matrix in the N direction, with a value range of nSize ∈ [1, 4095].<br>&nbsp;&nbsp;&bull;If the channelSplit function is enabled, nSize must be a multiple of 8.<br>&nbsp;&nbsp;&bull;If the channelSplit function is not enabled, nSize must be a multiple of 16. |
| mSize | Input | Size of the source NZ matrix in the M direction.<br>&nbsp;&nbsp;&bull;When NZ2ND is not enabled, the value range is mSize ∈ [1, 65535].<br>&nbsp;&nbsp;&bull;When NZ2ND is enabled, the value range is mSize ∈ [1, 8192]. |
| srcStride | Input | Start address offset of adjacent Z layouts in the source NZ matrix, with a value range of srcStride ∈ [0, 65535], in the unit of C0_SIZE (16\*sizeof(T), where T is the data type of src). |
| dstStride | Input | &bull;When NZ2ND is not enabled, the start address offset of adjacent Z layouts in the destination NZ matrix, with a non-zero value, in the unit of datablock (32 bytes).<br>&bull;When NZ2ND is enabled, the number of elements in each row of the destination ND matrix, with a non-zero value, in the unit of element. |
| quantPre | Input | QuantMode_t is an enumeration type used to control the quantization mode, with a default value of QuantMode_t::NoQuant, which means the quantization function is disabled. The values of QuantMode_t are as follows:<br>&nbsp;&nbsp;&bull; NoQuant, disables the quantization function.<br>&nbsp;&nbsp;&bull; F322F16, casts float to half, with the cast mode of CAST_RINT.<br>&nbsp;&nbsp;&bull; F322BF16, casts float to bfloat16_t, with the cast mode of CAST_RINT.<br>&nbsp;&nbsp;&bull; DEQF16, quantizes int32_t to half, scalar quantization.<br>&nbsp;&nbsp;&bull; VDEQF16, quantizes int32_t to half, tensor quantization.<br>&nbsp;&nbsp;&bull; QF322B8_PRE, quantizes float to uint8_t/int8_t, scalar quantization.<br>&nbsp;&nbsp;&bull; VQF322B8_PRE, quantizes float to uint8_t/int8_t, tensor quantization.<br>&nbsp;&nbsp;&bull; REQ8, quantizes int32_t to uint8_t/int8_t, scalar quantization.<br>&nbsp;&nbsp;&bull; VREQ8, quantizes int32_t to uint8_t/int8_t, tensor quantization. |
| deqScalar | Input | Scalar quantization parameter, indicating a single scale value. This parameter needs to be set when the quantPre quantization mode is scalar quantization. The supported data type is uint64_t. |
| ndNum | Input | Number of source NZ matrices, that is, the number of ND matrices to be transferred, with a value range of ndNum ∈ [1, 65535]. |
| srcNdStride | Input | Interval between the start addresses of different NZ matrices. When ndNum is greater than 1, the value range is srcNdStride ∈ [1, 512]; when ndNum is set to 1, srcNdStride can be set to 0 and does not take effect. The unit is data block (16 \* C0_SIZE). |
| dstNdStride | Input | Offset between the start addresses of adjacent destination ND matrices, with a value range of dstNdStride ∈ [1, 65535], in the unit of element. When ndNum is set to 1, dstNdStride can be set to 0 and does not take effect. |
| reluEn | Input | Switch for enabling ReLU:<br>&nbsp;&nbsp;&bull; false: disables the ReLU function;<br>&nbsp;&nbsp;&bull; true: enables the ReLU function. |
| unitFlag | Input | unitFlag is a fine-grained parallelism between the Mmad instruction and the Fixpipe instruction. After this function is enabled, each time the hardware finishes computing a fractal, the computation result is moved out. This function is not applicable to the scenario of accumulation in the L0C Buffer. The value description is as follows:<br>&nbsp;&nbsp;&bull; 0 (2'b00): disables unitFlag;<br>&nbsp;&nbsp;&bull; 2 (2'b10): enables unitFlag. After the hardware finishes executing the instruction, the register is not set;<br>&nbsp;&nbsp;&bull; 3 (2'b11): enables unitFlag. After the hardware finishes executing the instruction, unitFlag is disabled. |
| [isChannelSplit](../cube_store_key_features/F32-Channel-Split.md) | Input | This parameter is valid only on the L0C Buffer-to-Global Memory path. |

## Data Type<a id="zh-cn_topic_0000002549846732_section4219135304818"></a>

**Supported Data Type Combinations for the Source Matrix and Destination Matrix**

<!-- npu="950" id14 -->
For Ascend 950PR&950DT products, the supported data type combinations are as follows:

| source matrix (L0C Buffer) | destination matrix (UB) |
| ---------- | ---------- |
| float | int8_t, uint8_t, hifloat8_t, fp8_e4m3fn_t, half, bfloat16_t, float. |
| int32_t | int8_t, uint8_t, half, bfloat16_t, int32_t. |

<!-- end id14 -->

<!-- npu="310b" id17 -->
For Atlas 200I/500 A2 inference products, the supported data type combinations are as follows:

| source matrix (L0C Buffer) | destination matrix (UB) |
| ---------- | ---------- |
| float | int8_t, uint8_t, half, bfloat16_t, float. |
| int32_t | int8_t, uint8_t, half, int32_t. |

<!-- end id17 -->

## Return Value

None

## Constraints<a name="zh-cn_topic_0000002549846732_section2045914466492"></a>

- The description of the quantization input with the **float32** data type is as follows:
    - The standard IEEE 754 **float32** format is: 1-bit sign bit, 8-bit exponent bits, and 23-bit mantissa bits. The **float32** format supported by the current AI processor is: 1-bit sign bit, 8-bit exponent bits, and 10-bit mantissa bits.
    - If the user provides standard IEEE 754 **float32** input, the API internally processes it into the **float32** format supported by the processor for computation. In this case, if standard IEEE 754 **float32** data is used during golden data generation, a precision mismatch may be introduced. The golden data generation needs to be corrected by clearing the lower 13 bits of the 23-bit mantissa of the quantization parameter before it participates in quantization computation.
- The source matrix in **NZ** format requires 64-byte alignment for its address, and the destination matrix in **NZ** format requires 32-byte alignment for its address.
- When any of the moved-out **mSize**, **nSize**, or **ndNum** values is 0, the instruction is not executed. A warning is reported when **ndNum** is 0.
- The quantization and **ReLU** parameters cannot be inf/nan or denormalized numbers.
- The destination data must not overlap. If overlapping writes occur to the destination address, the hardware reports no warning or error, and the write order of the overlapping data is not guaranteed.
- Enabling the **unitFlag** feature must be used together with **Mmad**.
<!-- npu="950" id18 -->
- **dualDstCtrl** is supported only in the normal movement mode (**NZ2NZ**) or the **NZ2ND** movement scenario, and is not supported in inline function scenarios.
- When NZ2DN is enabled and **srcNzC0Stride** is not equal to 1, **unitFlag** cannot be enabled at the same time.
- This API applies only to the Mix operator scenario and supports only the configuration where the Cube core count to Vector core count ratio is 1:2. In this scenario, **subBlockId** supports only the values 0 or 1.
- Special value/boundary value constraints for Ascend 950PR&950DT products:

    For floating-point inf/nan input and output, the CTRL\[48\] bit of the CTRL register (control register) can be used to set the saturation mode when floating-point values are moved out in quantized form.

    For the fp8\_e4m3fn\_t type, the CTRL\[48\] bit of the CTRL register (control register) can be used for finer-grained settings.

    - Non-saturation mode: When CTRL\[48\] is set to 1'b1, inf/nan are output as-is. For the fp8\_e4m3fn\_t type, if the absolute value of the result is inf or greater than the maximum normalized value of fp8\_e4m3fn\_t, the output result is nan.
    - Saturation mode: When CTRL\[48\] is set to 1'b0, inf output is saturated to ±MAX, and nan output is saturated to 0. For the fp8\_e4m3fn\_t type, when CTRL\[50\] is set to 1'b0, nan is converted to zero; when CTRL\[50\] is set to 1'b1, nan remains unchanged. If the absolute value of the result is inf or greater than the maximum normalized value of fp8\_e4m3fn\_t, the result is the maximum normalized value.

        ```cpp
        // Set CTRL[48] to 0 to enable the floating-point saturation mode.
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    For integer types, only the saturation mode is available.
    For a summary of the boundary values of the L0C Buffer-to-UB movement API, see [Summary of Boundary Values of Matrix Compute Move-Out APIs](../../../../appendix/matrix_compute_move_out_interface_boundary_value_summary.md).
<!-- end id18 -->

<!-- npu="950" id20 -->
## Key Features

The following figures and descriptions apply only to the Ascend 950PR&950DT products:

### Without NZ2ND Inline Format Conversion

When NZ2ND is not enabled, an example of parameter setting (moving data and removing dummy data through the Fixpipe API) and its explanation are as follows:

When the number of data elements in the M direction is not a multiple of 16, dummy data is additionally read during the move-in, and this dummy data is discarded after being written to the destination. A matrix block is defined as a contiguous 16*16 data block. The number of data blocks is M/16 rounded up, and the length of a matrix block is M*16*sizeof(T), where T is the data type.

**Single movement mode:**

- **nSize** = 48, indicating that the matrix to be moved in the source NZ matrix (the blue area in the figure) has a size of 48 elements in the N direction.
- **mSize** = 24, indicating that the matrix to be moved in the source NZ matrix has a size of 24 elements in the M direction.
- **srcStride** = 64, indicating the start address offset between adjacent Z layouts of the matrix to be moved in the source NZ matrix, that is, the interval between the start address of the first blue Z layout and the start address of the second blue Z layout in the following figure is 64 \* C0\_Size.
- **dstStride** = 40 \* C0, indicating the start address offset between adjacent Z layouts in the destination NZ matrix, that is, the interval between the start address of the first blue Z layout and the start address of the second blue Z layout in the following figure is 40 \* 16 elements.

**Figure 1** Schematic diagram of the single-load mode setting without NZ2ND parameters<a name="fig128961542184620"></a>  

![](../../../../figures/no_nz2nd_single_mode.png "Schematic diagram of the single-load mode setting without NZ2ND parameters")

**Dual-destination control mode:**

Enabling the dual-destination mode in normal movement mode is shown in the following figure. It is divided into splitting by the M dimension and splitting by the N dimension. When splitting by the M dimension, M must be a multiple of 2; when splitting by the N dimension, N must be a multiple of 32:

Splitting in the N direction:

- **nSize** = 32, indicating that the size of the matrix to be moved in the source NZ matrix in the N direction is 32 elements.
- **mSize** = 48, indicating that the size of the matrix to be moved in the source NZ matrix in the M direction is 48 elements.
- **srcStride** = 64, indicating the start address offset between adjacent Z layouts of the matrix to be moved in the source NZ matrix, that is, the interval between the start address of the first block's Z-layout matrix and the start address of the second Z-layout matrix in the following figure is 64 \* C0\_Size.
- **dstStride** = 64 \* C0, indicating the start address offset between adjacent Z layouts in the destination NZ matrix, that is, the interval between the start address of the Z layout in UB0 and the start address of the Z layout in UB1 in the following figure is 64 \* 16 elements.

Splitting in the M direction:

- **nSize** = 32, indicating that the size of the matrix to be moved in the source NZ matrix in the N direction is 32 elements.
- **mSize** = 24, indicating that the size of the matrix to be moved in the source NZ matrix in the M direction is 24 elements.
- **srcStride** = 64, indicating the start address offset between adjacent Z layouts of the matrix to be moved in the source NZ matrix, that is, the interval between the start address of the first Z-layout matrix and the start address of the second Z-layout matrix in the following figure is 64 \* C0\_Size.
- **dstStride** = 40 \* C0, indicating the start address offset between adjacent Z layouts in the destination NZ matrix, that is, the interval between the start address of the first Z layout and the start address of the second Z layout in UB0 (or UB1) in the following figure is 40 \* 16 elements.

**Figure 2** Schematic diagram of the dual-target movement mode setting without the NZ2ND and NZ2DN parameters<a name="fig6561154491913"></a>  

![](../../../../figures/no_nz2nd_dual_target_mode.png "Schematic diagram of the dual-target movement mode setting without the NZ2ND and NZ2DN parameters")

### Enabling NZ2ND Inline Format Conversion

When NZ2ND is enabled, the parameter setting example and explanation are as follows:

- **ndNum** = 2, indicating that the number of source NZ matrices is 2. In the figure, the blue area is NZ matrix 1, and the purple area is NZ matrix 2.
- **nSize** = 32, indicating that the size of the source NZ matrix (the blue area in the figure) in the N direction is 32 elements.
- **mSize** = 48, indicating that the size of the source NZ matrix in the M direction is 48 elements.
- **srcStride** = 64, indicating the start address offset between adjacent Z layouts in the source NZ matrix, that is, the interval between the start address of the first blue Z layout and the start address of the second blue Z layout in the following figure is 64 \* C0\_Size.
- **dstStride** = 64, indicating that the number of elements in each row of the destination ND matrix is 64.
- **srcNdStride** = 16, indicating that the interval between the start addresses of different NZ matrices is 16 \*  16 \* C0\_Size.
- **dstNdStride** = 4096, indicating that the offset between the start addresses of adjacent destination ND matrices is 4096 elements.

**Figure 3** Schematic diagram of parameter setting with NZ2ND enabled<a name="fig79783143556"></a>  

![](../../../../figures/enable_nz2nd_config.png "Schematic diagram of NZ2ND parameter settings")

**Single movement mode:**

- **ndNum** = 2, indicating that the number of source NZ matrices is 2. In the figure, the blue area is NZ matrix 1, and the purple area is NZ matrix 2.
- **nSize** = 32, indicating that the size of the source NZ matrix (the blue area in the figure) in the N direction is 32 elements.
- **mSize** = 48, indicating that the size of the source NZ matrix in the M direction is 48 elements.
- **srcStride** = 64, indicating the start address offset between adjacent Z layouts in the source NZ matrix, that is, the interval between the start address of the first blue Z layout and the start address of the second blue Z layout in the following figure is 64 \* C0\_Size.
- **dstStride** = 64, indicating that the number of elements in each row of the destination ND matrix is 64.
- **srcNdStride** = 256, indicating that the interval between the start addresses of different NZ matrices is 256, in units of C0\_Size.
- **dstNdStride** = 4096, indicating that the offset between the start addresses of adjacent destination ND matrices is 4096 elements.

**Figure 4** Schematic diagram of single-load mode settings for NZ2ND parameters<a name="fig11991024112516"></a>  

![](../../../../figures/enable_nz2nd_single_load.png "Schematic diagram of the single-load mode setting with the NZ2ND parameter enabled")

**Dual-destination control mode:**

- **ndNum** = 2, indicating that the number of source NZ matrices is 2. In the figure, the red-boxed area is matrix 1, and the blue-boxed area is matrix 2.
- **nSize** = 32, indicating that the size of the source NZ matrix (the red-boxed or blue-boxed area in the figure) in the N direction is 32 elements.
- **mSize** = 48, indicating that the size of the source NZ matrix in the M direction is 48 elements.
- **srcStride** = 64, indicating the start address offset between adjacent Z layouts in the source NZ matrix, that is, the interval between the start address of the light-colored Z-layout matrix on the left and that of the dark-colored Z-layout matrix on the right in the red-boxed area of the following figure is 64 \* C0\_Size.
- **dstStride** = 64, indicating that the number of elements in each row of the destination ND matrix is 64.
- **ndNum** = 2, indicating the number of source NZ matrices.
- **srcNdStride** = 240, indicating that the interval between the start addresses of different NZ matrices is 240 \* C0\_Size.
- **dstNdStride**  = 4096, indicating that the offset between the start addresses of adjacent destination ND matrices is 4096 elements.

    **Figure 5** Schematic diagram of the NZ2ND parameter dual-load mode setting<a name="fig8810182815117"></a>  

    ![](../../../../figures/enable_nz2nd_dual_load.png "Schematic diagram of the NZ2ND parameter dual-load mode setting")

### Enabling NZ2DN Inline Format Conversion

When NZ2DN is enabled, the parameter setting examples and explanations are as follows:

**Single movement mode:**

Example 1: The parameter values are described as follows. See [Figure 6](#fig19772110476) for the schematic diagram:

- **dnNum** = 2, indicating that the number of source NZ matrices is 2. In the figure, the blue area is NZ matrix 1, and the purple area is NZ matrix 2.
- **nSize** = 32, indicating that the size of the source NZ matrix (the blue area in the figure) in the N direction is 32 elements.
- **mSize** = 48, indicating that the size of the source NZ matrix in the M direction is 48 elements.
- **srcStride** = 80, indicating the start address offset between adjacent Z layouts in the source NZ matrix, that is, the interval between the start addresses of two adjacent blue Z layouts in the following figure is 80 \* C0\_Size.
- **dstStride** = 80, indicating that the number of elements in each row of the destination DN matrix is 80.
- **ndNum** = 2, indicating the number of source NZ matrices.
- **srcNzMatrixStride** = 240, indicating the offset between different source NZ matrices, that is, the interval between the start address of the first blue Z layout and the start address of the second purple Z layout in the following figure is 240 \* C0\_Size.
- **srcNzC0Stride** = 1: Indicates the address offset between adjacent rows of the source matrix NZ fractal.
- **dstDnMatrixStride**: Indicates that the offset between the start addresses of adjacent DN matrices is 48 \* 80 = 3840 elements.

**Figure 6** Schematic diagram 1 of enabling NZ2DN single-load mode<a name="fig19772110476"></a>  

![](../../../../figures/enable_nz2dn_single_mode1.png "Schematic diagram 1 of enabling NZ2DN single-load mode")

Example 2: The parameter values are described as follows. See [Figure 7](#fig769114585716) for the schematic diagram:

- **dnNum** = 2, indicating that the number of source NZ matrices is 2. In the figure, the blue area is NZ matrix 1, and the red area is NZ matrix 2.
- **nSize** = 24, indicating that the size of the source NZ matrix (the blue area in the figure) in the N direction is 24 elements.
- **mSize** = 24, indicating that the size of the source NZ matrix in the M direction is 24 elements.
- **srcStride** = 80, indicating the start address offset between adjacent Z layouts in the source NZ matrix, that is, the interval between the start addresses of two adjacent blue Z layouts in the following figure is 80 \* C0\_Size.
- **dstStride** = 60, indicating that the number of elements in each row of the destination DN matrix is 60.
- **ndNum** = 2, indicating the number of source NZ matrices.
- **srcNzMatrixStride** = 240, indicating the offset between different source NZ matrices, that is, the interval between the start address of the first blue Z layout and the start address of the second purple Z layout in the following figure is 240 \* C0\_Size.
- **srcNzC0Stride** = 2: indicating the address offset between adjacent rows of the source matrix NZ fractal.
- **dstDnMatrixStride**: indicating that the offset between the start addresses of adjacent DN matrices is 48 \* 60 = 2880 elements.

**Figure 7** Diagram 2 of enabling NZ2DN single-load mode<a name="fig769114585716"></a>  

![](../../../../figures/enable_nz2dn_single_mode2.png "Diagram 2 of enabling NZ2DN single-load mode")
<!-- end id20 -->

## Example<a name="zh-cn_topic_0000002549846732_section088124295117"></a>

<!-- npu="950" id19 -->
For the complete sample, see [fixpipe_l0c2ub sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2ub).

- Example: path L0C Buffer-\>UB. The data type of input matrices A and B is half, and the output matrix C is float. The default configuration enables Nz2Nd format conversion. For the complete sample, see [sample link](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2ub).

    ```cpp
    // Move data from L0C Buffer to UB. The destination is UB, so set isToUB of FixpipeConfig to true.
    constexpr AscendC::FixpipeConfig CFG_ROW_MAJOR_UB = {AscendC::CO2Layout::ROW_MAJOR, true};

    AscendC::LocalTensor<outputType> cUB;
    cUB = AscendC::LocalTensor<outputType>(AscendC::TPosition::VECOUT, 0, cSingleSize);
    
    AscendC::FixpipeParamsArch3510<AscendC::CO2Layout::ROW_MAJOR> fixpipeParams;
    fixpipeParams.mSize = baseM;
    fixpipeParams.nSize = baseN;
    fixpipeParams.srcStride = CeilAlign(baseM, CUBE_BLOCK);
    fixpipeParams.dstStride = baseN;
    AscendC::Fixpipe<outputType, l0cType, CFG_ROW_MAJOR_UB>(cUB, c, fixpipeParams);
    ```
<!-- end id19 -->
