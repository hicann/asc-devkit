# Fixpipe (L0C Buffer to UB Data Movement)<a name="ZH-CN_TOPIC_0000002563847576"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-27T15:52:07.222Z -->

## Applicable Products<a name="zh-cn_topic_0000002549846732_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported for APIs that include the **FixpipeParamsArch3510** parameter.
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: Supported for APIs that include the **FixpipeParamsM300** parameter.
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
## Description<a name="zh-cn_topic_0000002549846732_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_fixpipe_intf.h"`.

The cube computation results are stored in the L0C Buffer. The Fixpipe API is used to move the results to the Unified Buffer (UB), and it supports operations such as inline format conversion during the movement.

<!-- npu="950" id10 -->
Taking Ascend 950PR/Ascend 950DT as an example, the following figure shows the valid combinations of inline quantization, inline ReLU, and inline channel merge, as well as the intermediate data types and data paths. In the figure, F32->F16 and F32->BF16 are non-quantization modes that perform only Cast, while the rest are inline scalar/tensor quantization modes.

**Figure 2** L0C2UB flow diagram<a id="zh-cn_topic_0000002542828493_fig1828513492547"></a>  

![](../../../../figures/L0C2UB_Function_Combination_950.png)
<!-- end id10 -->

## Prototype<a name="zh-cn_topic_0000002549846732_section82039854412"></a>

The L0C Buffer-to-UB data movement provides the combined cube move-out API **Fixpipe**, which integrates register setting and data movement capabilities. The corresponding APIs are as follows:

<!-- npu="950" id11 -->
For Ascend 950PR/Ascend 950DT:

- Path L0C Buffer->UB, without enabling the tensor quantization function:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

- Path L0C Buffer->UB, with the tensor quantization function enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint64_t>& cbufWorkspace, const FixpipeParamsArch3510<config.format>& intriParams)
    ```
<!-- end id11 -->

<!-- npu="310b" id12 -->
For the Atlas 200I/500 A2 inference product:

- Path L0C Buffer->UB, without enabling the tensor quantization function:

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

- **When calling the L0C Buffer-to-UB Fixpipe API, the value of the template parameter `const FixpipeConfig& config` cannot be `CFG_ROW_MAJOR`, `CFG_COLUMN_MAJOR`, or `CFG_NZ`. Instead, you must explicitly construct a configuration with `isToUB = true`, indicating that the destination physical address is UB.**

## Parameters<a name="zh-cn_topic_0000002549846732_section16128134420472"></a>

**Table 1** Fixpipe template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T/U | Data type of the destination operand/source operand. For supported data types, see [Data Types](#zh-cn_topic_0000002549846732_section4219135304818). |
| config | Fixpipe-related configuration parameter, of the type **FixpipeConfig**. You need to explicitly construct a configuration with `isToUB = true`, indicating that the destination physical address is UB. The construction examples are as follows:<br>Enable NZ2ND, with the output data format being ND:<br>constexpr AscendC::FixpipeConfig CFG_ROW_MAJOR_UB = {AscendC::CO2Layout::ROW_MAJOR, true};<br>NZ2NZ, with the output data format being NZ:<br>constexpr AscendC::FixpipeConfig CFG_NZ_UB = {AscendC::CO2Layout::NZ, true};<br>Enable NZ2DN, with the output data format being DN:<br>constexpr AscendC::FixpipeConfig CFG_COLUMN_MAJOR_UB = {AscendC::CO2Layout::COLUMN_MAJOR, true};<br>The **FixpipeConfig** structure is defined as follows:<br>struct FixpipeConfig {<br>    CO2Layout format;<br>    bool isToUB; // Used to specify whether the destination address is in UB. <br>};<br>enum class CO2Layout : uint8_t {<br>    NZ = 0, // The output data format remains NZ.<br>    ROW_MAJOR, // Enables NZ2ND, with the output data format being ND.<br><!-- npu="950" id15 -->    COLUMN_MAJOR, // Supported only on Ascend 950PR/Ascend 950DT. Enables NZ2DN, with the output data format being DN.<br><!-- end id15 -->};<br>constexpr FixpipeConfig CFG_NZ = {CO2Layout::NZ};<br>constexpr FixpipeConfig CFG_ROW_MAJOR = {CO2Layout::ROW_MAJOR};<!-- npu="950" id16 --><br>constexpr FixpipeConfig CFG_COLUMN_MAJOR = {CO2Layout::COLUMN_MAJOR}; // Supported only on Ascend 950PR/Ascend 950DT.<!-- end id16 --><br>|
| S | Data type of the **cbufWorkspace** parameter, that is, the data type of the inline quantization parameter.<br>&nbsp;&nbsp;&bull;When the destination operand, source operand, and **cbufWorkspace** use basic data types, the template parameter **S** must be of the `uint64_t` type; otherwise, compilation fails.<br>&nbsp;&nbsp;&bull;When the destination operand, source operand, and **cbufWorkspace** use the **TensorTrait** type, the LiteType of the template parameter **S** must be of the `uint64_t` type; otherwise, compilation fails.<br>The template parameter following **S** is used only for the preceding data type check, and you do not need to pay attention to it. |

**Table 2** Fixpipe parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of the type **LocalTensor**. The address must be 32-byte aligned. |
| src | Input | Source operand, of the type **LocalTensor**. The supported physical address is L0C Buffer (**TPosition** is **CO1**), which is the result computed by the **Mmad** API. The data format is NZ, and the address must be 64B aligned. |
| intriParams | Input | Fixpipe movement parameter. For the specific definition, see "basic_api/API/kernel_struct_fixpipe.h".<br>For the parameter description, see the description of the Fixpipe movement parameter structures (**FixpipeParamsArch3510** and **FixpipeParamsM300**). |
| cbufWorkspace | Input | Quantization parameter, of the type `LocalTensor<uint64_t>`. The supported physical address is L1 Buffer (**TPosition** is **C1**), and the address must be 32-byte aligned.<br>&nbsp;&nbsp;&bull;Supported when **quantPre** is **VDEQF16**, **VQF322B8_PRE**, or **VREQ8**.<br><!-- npu="950" id13 -->&nbsp;&nbsp;&bull;For Ascend 950PR/Ascend 950DT, in addition to the preceding modes, **VQF322FP8_PRE**, **VQF322HIF8_PRE**, **VQF322HIF8_PRE_HYBRID**, **VQS322BF16_PRE**, **VQF322F16_PRE**, **VQF322BF16_PRE**, and **VQF322F32_PRE** are also supported.<br><!-- end id13 -->For the introduction to **quantPre**, see the **quantPre** section in the Fixpipe movement parameter structure. |

**Table 3** Description of the Fixpipe movement parameter structure (FixpipeParamsArch3510)

| Parameter | Data Type | Description |
| --- | --- | --- |
| nSize | Mandatory input | Size of the source NZ cube in the N direction, with a value range of nSize∈[0, 4095].<br>&nbsp;&nbsp;&bull; If the [channelSplit](../cube_store_key_features/F32-Channel-Split.md) function is enabled, nSize must be a multiple of 8.<br>&nbsp;&nbsp;&bull; If the channelSplit function is not enabled, nSize must be a multiple of 16.<br>&nbsp;&nbsp;&bull; If NZ2ND is enabled, nSize*sizeof(T) must be a multiple of 32.<br>**Note: nSize=0 indicates that no movement is performed, and the API is treated as a NOP (no operation).** |
| mSize | Mandatory input | Size of the source NZ cube in the M direction, with a value range of mSize∈[0, 65535]. If NZ2DN is enabled, mSize*sizeof(T) must be a multiple of 32.<br>**Note: mSize=0 indicates that no movement is performed, and the API is treated as a NOP (no operation).** |
| srcStride | Mandatory input | Offset of the start address of adjacent Z layouts in the source NZ cube, with a value range of srcStride∈[0, 65535], in the unit of C0_Size (16*sizeof(T)), where T is the data type of **src**. Its value should be set to mSize rounded up to a multiple of 16. |
| dstStride | Mandatory input | &bull; When NZ2ND/NZ2DN is not enabled ([NZ2NZ movement](../cube_store_key_features/NZ2NZ.md)): offset of the start address of adjacent Z layouts in the destination NZ cube. The value must not be 0, and the unit is element. (Compared with **dstStride** in the Fixpipe movement parameter (FixpipeParamsV220), the unit has changed: in FixpipeParamsV220, the unit of **dstStride** is datablock (32 bytes), while in FixpipeParamsArch3510, the unit of **dstStride** is element.)<br>&bull; When [NZ2ND](../cube_store_key_features/NZ2ND.md)/[NZ2DN](../cube_store_key_features/NZ2DN.md) is enabled: number of elements in each row of the destination ND/DN cube. The value must not be 0, and the unit is element. |
| [quantPre](../cube_store_key_features/on_the_fly_quantization.md) | Optional input |Used to control the quantization mode, of the **QuantMode_t** type, with a default value of `QuantMode_t::NoQuant`. The specific definition is as follows:<br>Note: This parameter must be manually configured by the user and is not automatically derived to configure the corresponding quantization mode.<br>enum QuantMode_t<br>{<br>    NoQuant,               // Disables the quantization function.<br>    F322F16,               // Float32_2_Float16: casts float to half, with the cast mode being CAST_RINT.<br>    F322BF16,              // Float32_2_BFloat16: casts float to bfloat16_t, with the cast mode being CAST_RINT.<br>    DEQF16,                // DeQuant_Float16: quantizes int32_t to half, scalar quantization.<br>    VDEQF16,               // Vector_DeQuant_Float16: quantizes int32_t to half, tensor quantization.<br>    QF322B8_PRE,           // Quant_Float32_2_B8: quantizes float to int8_t/uint8_t, scalar quantization.<br>    VQF322B8_PRE,          // Vector_Quant_Float32_2_B8: quantizes float to int8_t/uint8_t, tensor quantization.<br>    REQ8,                  // ReQuant_int8: quantizes int32_t to int8_t/uint8_t, scalar quantization.<br>    VREQ8,                 // Vector_ReQuant_int8: quantizes int32_t to int8_t/uint8_t, tensor quantization.<br>    QF322FP8_PRE,          // Quant_Float32_2_FP8: quantizes float to fp8_e4m3fn_t, scalar quantization.<br>    VQF322FP8_PRE,         // Vector_Quant_Float32_2_FP8: quantizes float to fp8_e4m3fn_t, tensor quantization.<br>    QF322HIF8_PRE,         // Quant_Float32_2_HIF8: quantizes float to hifloat8_t (Half to Away Round), scalar quantization.<br>    VQF322HIF8_PRE,        // Vector_Quant_Float32_2_HIF8: quantizes float to hifloat8_t (Half to Away Round), tensor quantization.<br>    QF322HIF8_PRE_HYBRID,  // Quant_Float32_2_HIF8_Hybrid: quantizes float to hifloat8_t (Hybrid Round), scalar quantization.<br>    VQF322HIF8_PRE_HYBRID, // Vector_Quant_Float32_2_HIF8_Hybrid: quantizes float to hifloat8_t (Hybrid Round), tensor quantization.<br>    QS322BF16_PRE,         // Quant_Int32_2_BFloat16: quantizes int32_t to bfloat16_t, scalar quantization.<br>    VQS322BF16_PRE,        // Vector_Quant_Int32_2_BFloat16: quantizes int32_t to bfloat16_t, tensor quantization.<br>    QF322F16_PRE,          // Quant_Float32_2_Float16: quantizes float to half, scalar quantization.<br>    VQF322F16_PRE,         // Vector_Quant_Float32_2_Float16: quantizes float to half, tensor quantization.<br>    QF322BF16_PRE,         // Quant_Float32_2_BFloat16: quantizes float to bfloat16_t, scalar quantization.<br>    VQF322BF16_PRE,        // Vector_Quant_Float32_2_BFloat16: quantizes float to bfloat16_t, tensor quantization.<br>    QF322F32_PRE,          // Quant_Float32_2_Float32: quantizes float to float, scalar quantization. The precision can reach two thousandths but cannot reach two ten-thousandths. If two ten-thousandths precision is required, use the [AscendDeQuant](../../../advanced_api/quantization_operations/AscendDequant.md) high-level API.<br>    VQF322F32_PRE,         // Vector_Quant_Float32_2_Float32: quantizes float to float, tensor quantization. The precision can reach two thousandths but cannot reach two ten-thousandths. If two ten-thousandths precision is required, use the [AscendDeQuant](../../../advanced_api/quantization_operations/AscendDequant.md) high-level API.<br>};<br>|
| deqScalar | Optional input | Scalar quantization parameter, indicating a single scale value. This parameter must be set when the **quantPre** quantization mode is [inline quantization](../cube_store_key_features/on_the_fly_quantization.md). The supported data type is `uint64_t`. |
| [reluEn](../cube_store_key_features/on_the_fly_relu.md) | Optional input | Switch for enabling NormReLU:<br>&nbsp;&nbsp;&bull; `false`: NormReLU is not enabled. The default value is `false`.<br>&nbsp;&nbsp;&bull; `true`: NormReLU is enabled. |
| unitFlag | Optional input | **unitFlag** is a fine-grained parallelism between the **Mmad** instruction and the **Fixpipe** instruction. After this function is enabled, the computation result is moved out as soon as the hardware finishes computing each fractal. The values are described as follows:<br>&nbsp;&nbsp;&bull; 0 (2'b00): **unitFlag** is not enabled.<br>&nbsp;&nbsp;&bull; 2 (2'b10): **unitFlag** is enabled. After the hardware finishes executing the instruction, the unit flag bit is not reset.<br>&nbsp;&nbsp;&bull; 3 (2'b11): **unitFlag** is enabled. After the hardware finishes executing the instruction, the unit flag bit is reset.<br>When this function is enabled, the **unitFlag** values of the **Mmad** instruction and the **Fixpipe** instruction must be set to 2 or 3.<br>For the parameter setting scheme and feature details, see the [UnitFlag](../mmad_compute_key_features/UnitFlag.md) section in the key features of Mmad computation. |
| params | Optional input | Used to select and configure different inline format conversions (NZ2NZ/NZ2ND/NZ2DN). This parameter is a structure of the **TransformParams** type. The **TransformParams** structure is a type selector based on template parameters, used to automatically select the corresponding parameter type at compile time according to the template parameter used when defining the **FixpipeParamsArch3510** movement parameter.<br>template \<CO2Layout format\><br>struct TransformParams {};<br>template \<\><br>struct TransformParams\<CO2Layout::NZ\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = uint8_t;<br>};<br>template \<\><br>struct TransformParams\<CO2Layout::ROW_MAJOR\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = Nz2NdParams;<br>};<br>template \<\><br>struct TransformParams\<CO2Layout::COLUMN_MAJOR\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = Nz2DnParams;<br>};<br>When the **CO2Layout** layout type is **ROW_MAJOR**, this instruction is defined as data movement from L0C Buffer to the destination location, with NZ2ND conversion attached. The structure parameters are as follows:<br>struct Nz2NdParams {<br>    uint16_t ndNum = 1;<br>    uint16_t srcNdStride = 0;<br>    uint32_t dstNdStride = 0; <br>};<br>&nbsp;&nbsp;&bull; ndNum: number of source NZ matrices, that is, the number of ND matrices to be transferred, with a value range of ndNum∈[0, 65535]. **Note: ndNum=0 indicates that no movement is performed, and the API is treated as a NOP (no operation).**<br>&nbsp;&nbsp;&bull; srcNdStride: interval between the start addresses of different NZ matrices, with a value range of srcNdStride∈[0, 65535], in the unit of C0_SIZE. When ndNum is set to 1, srcNdStride can be set to 0, which does not take effect.<br>&nbsp;&nbsp;&bull; dstNdStride: offset between the start addresses of adjacent destination ND matrices, with a value range of dstNdStride∈[1, 2^32 -1], in the unit of element. When ndNum is set to 1, dstNdStride can be set to 0, which does not take effect.<br><br>When the **CO2Layout** layout type is **COLUMN_MAJOR**, this instruction is defined as data movement from L0C Buffer to the destination location, with NZ2DN conversion attached. The structure parameters are as follows:<br>struct Nz2DnParams {<br>    uint16_t dnNum = 1;<br>    uint16_t srcNzCubeStride = 0;<br>    uint32_t dstDnCubeStride = 0;<br>    uint16_t srcNzC0Stride = 0; <br>};<br>&nbsp;&nbsp;&bull; dnNum: number of DN matrices to be transferred, with a value range of dnNum∈[0, 65535]. **Note: dnNum=0 indicates that no movement is performed, and the API is treated as a NOP (no operation).**<br>&nbsp;&nbsp;&bull; srcNzCubeStride: offset (head to head) between different source NZ matrices, with a value range of srcNzCubeStride∈[0, 65535], in the unit of C0_SIZE. When dnNum is set to 1, srcNzCubeStride can be set to 0, which does not take effect.<br>&nbsp;&nbsp;&bull; dstDnCubeStride: offset between the start addresses of adjacent destination ND matrices, with a value range of dstDnCubeStride∈[0, 2^32 -1], in the unit of element. When dnNum is set to 1, dstDnCubeStride can be set to 0, which does not take effect.<br>&nbsp;&nbsp;&bull; srcNzC0Stride: address offset (head to head) between adjacent rows in the source cube NZ fractal, with a value range of srcNzC0Stride∈[0, 65535], in the unit of C0_SIZE. When NZ2DN is enabled, srcNzC0Stride cannot be 0.<br><br>When the **CO2Layout** layout type is **NZ**, it is the normal movement DMA mode, indicating normal data movement from L0C Buffer to the destination location. |
| dualDstCtrl | Optional input | Dual-target mode control parameter. For Ascend 950PR/Ascend 950DT, there is one Cube Core and two Vector Cores in the same AI Core. When dual-target mode control is enabled, the M×N cube in L0C Buffer is split into two halves and written to the UB of the two Vector Cores simultaneously, where the first half is written to SUB BLOCK0 and the second half is written to SUB BLOCK1.<br>&nbsp;&nbsp;&bull; 2'b00: single-target mode, in which the entire cube is written to the destination UB configured through the **subBlockId** parameter.<br>&nbsp;&nbsp;&bull; 2'b01: dual-target mode, in which the cube is split along the M dimension into two matrices of shape M / 2 \* N and written to two UBs respectively. M must be a multiple of 2.<br>&nbsp;&nbsp;&bull; 2'b10: dual-target mode, in which the cube is split along the N dimension into two matrices of shape M \* N / 2 and written to two UBs respectively. N must be a multiple of 32.<br>&nbsp;&nbsp;&bull; 2'b11: reserved value.<br>**dualDstCtrl** is supported only in the normal movement mode (NZ2NZ) or the NZ2ND movement scenario, and is not supported in the inline function scenario.<br>For the parameter setting scheme and feature details, see [L0C-to-UB dual-target mode](../cube_store_key_features/l0c_buffer_to_ub_dual_target_mode.md). |
| subBlockId | Optional input | Used to indicate the SUB BLOCK ID of the destination UB when single-target mode is enabled. The value is 0 or 1. A value of 0 writes to SUB BLOCK0, and a value of 1 writes to SUB BLOCK1. The default value is 0. |
| [isChannelSplit](../cube_store_key_features/F32-Channel-Split.md) | Optional input | This parameter is valid only on the L0C Buffer-to-Global Memory path. |

**Table 4** Description of the Fixpipe movement parameter structure (FixpipeParamsM300)

| Parameter | Data Type | Description |
| ---------- | ---------- | ---------- |
| nSize | Input | Size of the source NZ cube in the N direction, with a value range of nSize ∈[1, 4095].<br>&nbsp;&nbsp;&bull; If the channelSplit function is enabled, nSize must be a multiple of 8.<br>&nbsp;&nbsp;&bull; If the channelSplit function is not enabled, nSize must be a multiple of 16. |
| mSize | Input | Size of the source NZ cube in the M direction.<br>&nbsp;&nbsp;&bull; When NZ2ND is not enabled, the value range is mSize∈[1, 65535].<br>&nbsp;&nbsp;&bull; When NZ2ND is enabled, the value range is mSize∈[1, 8192]. |
| srcStride | Input | Offset of the start address of adjacent Z layouts in the source NZ cube, with a value range of srcStride∈[0, 65535], in the unit of C0_SIZE (16\*sizeof(T), where T is the data type of **src**). |
| dstStride | Input | &bull;When NZ2ND is not enabled, offset of the start address of adjacent Z layouts in the destination NZ cube. The value must not be 0, and the unit is datablock (32 bytes).<br>&bull;When NZ2ND is enabled, number of elements in each row of the destination ND cube. The value must not be 0, and the unit is element. |
| quantPre | Input | **QuantMode_t** is an enumeration type used to control the quantization mode, with a default value of **QuantMode_t::NoQuant**, that is, the quantization function is not enabled. The values of **QuantMode_t** are as follows:<br>&nbsp;&nbsp;&bull; **NoQuant**: the quantization function is not enabled.<br>&nbsp;&nbsp;&bull; **F322F16**: casts float to half, with the cast mode being CAST_RINT.<br>&nbsp;&nbsp;&bull; **F322BF16**: casts float to bfloat16_t, with the cast mode being CAST_RINT.<br>&nbsp;&nbsp;&bull; **DEQF16**: quantizes int32_t to half, scalar quantization.<br>&nbsp;&nbsp;&bull; **VDEQF16**: quantizes int32_t to half, tensor quantization.<br>&nbsp;&nbsp;&bull; **QF322B8_PRE**: quantizes float to uint8_t/int8_t, scalar quantization.<br>&nbsp;&nbsp;&bull; **VQF322B8_PRE**: quantizes float to uint8_t/int8_t, tensor quantization.<br>&nbsp;&nbsp;&bull; **REQ8**: quantizes int32_t to uint8_t/int8_t, scalar quantization.<br>&nbsp;&nbsp;&bull; **VREQ8**: quantizes int32_t to uint8_t/int8_t, tensor quantization. |
| deqScalar | Input | Scalar quantization parameter, indicating a single scale value. This parameter must be set when the **quantPre** quantization mode is scalar quantization. The supported data type is uint64_t. |
| ndNum | Input | Number of source NZ matrices, that is, the number of ND matrices to be transferred, with a value range of ndNum∈[1, 65535]. |
| srcNdStride | Input | Interval between the start addresses of different NZ matrices. When ndNum is greater than 1, the value range is srcNdStride∈[1, 512]; when ndNum is set to 1, srcNdStride can be set to 0, which does not take effect. The unit is data block (16 \* C0_SIZE). |
| dstNdStride | Input | Offset between the start addresses of adjacent destination ND matrices, with a value range of dstNdStride∈[1, 65535], in the unit of element. When ndNum is set to 1, dstNdStride can be set to 0, which does not take effect. |
| reluEn | Input | Switch for enabling ReLU:<br>&nbsp;&nbsp;&bull; false: ReLU is not enabled.<br>&nbsp;&nbsp;&bull; true: ReLU is enabled. |
| unitFlag | Input | **unitFlag** is a fine-grained parallelism between the **Mmad** instruction and the **Fixpipe** instruction. After this function is enabled, the computation result is moved out as soon as the hardware finishes computing each fractal. This function is not applicable to the scenario of accumulation in L0C Buffer. The values are described as follows:<br>&nbsp;&nbsp;&bull; 0 (2'b00): **unitFlag** is not enabled.<br>&nbsp;&nbsp;&bull; 2 (2'b10): **unitFlag** is enabled. After the hardware finishes executing the instruction, the register is not set.<br>&nbsp;&nbsp;&bull; 3 (2'b11): **unitFlag** is enabled. After the hardware finishes executing the instruction, **unitFlag** is disabled. |
| [isChannelSplit](../cube_store_key_features/F32-Channel-Split.md) | Input | This parameter is valid only on the L0C Buffer-to-Global Memory path. |

## Data Types<a id="zh-cn_topic_0000002549846732_section4219135304818"></a>

**Supported data type combinations for the source cube and destination cube**

<!-- npu="950" id14 -->
For Ascend 950PR/Ascend 950DT, the supported data type combinations are as follows:

| source cube (L0C Buffer) | destination cube (UB) |
| ---------- | ---------- |
| float | int8_t, uint8_t, hifloat8_t, fp8_e4m3fn_t, half, bfloat16_t, float. |
| int32_t | int8_t, uint8_t, half, bfloat16_t, int32_t. |
<!-- end id14 -->

<!-- npu="310b" id17 -->
For the Atlas 200I/500 A2 inference product, the supported data type combinations are as follows:

| source cube (L0C Buffer) | destination cube (UB) |
| ---------- | ---------- |
| float | int8_t, uint8_t, half, bfloat16_t, float. |
| int32_t | int8_t, uint8_t, half, int32_t. |
<!-- end id17 -->

## Return Value

None

## Constraints<a name="zh-cn_topic_0000002549846732_section2045914466492"></a>

- The following describes the case where the quantization input is of the **float32** data type:
    - The standard IEEE 754 **float32** format consists of a 1-bit sign bit, 8-bit exponent bits, and 23-bit mantissa bits. The **float32** format supported by the current AI processor consists of a 1-bit sign bit, 8-bit exponent bits, and 10-bit mantissa bits.
    - If the user provides standard IEEE 754 **float32** input, the API internally converts it to the **float32** format supported by the processor for computation. In this case, if standard IEEE 754 **float32** data is used during golden data generation, a precision mismatch may be introduced. The golden data generation must be corrected by clearing the lower 13 bits of the 23-bit mantissa bits of the quantization parameter before it participates in quantization computation.
- The source cube (L0C Buffer) NZ format address must be 64-byte aligned, and the destination cube NZ format address must be 32-byte aligned.
- When any of the moved-out **mSize**, **nSize**, or **ndNum** values is 0, the instruction is not executed. A warning is reported when **ndNum** is 0.
- The quantization and ReLU parameters cannot be Inf/NaN or denormal numbers.
- The destination data must not overlap. If overlapping writes occur to the destination address, the hardware reports no warning or error, and the write order of the overlapping data is not guaranteed.
- Enabling the **unitFlag** feature requires enabling **Mmad** at the same time.
<!-- npu="950" id18 -->
- **dualDstCtrl** is supported only in the normal movement mode (NZ2NZ) or the NZ2ND movement scenario, and is not supported in inline function scenarios.
- When NZ2DN is enabled and **srcNzC0Stride** is not equal to 1, **unitFlag** cannot be enabled at the same time.
- Special value/boundary value constraints for Ascend 950PR/Ascend 950DT:

    For floating-point Inf/NaN inputs and outputs, the **CTRL[48]** bit of the CTRL register (control register) can be used to control the saturation mode during quantization move-out of floating-point values;

    For the **fp8_e4m3fn_t** type, the **CTRL[48]** bit of the CTRL register (control register) can be used for finer-grained settings;

    - Non-saturation mode: **CTRL[48]** is set to 1'b1, and Inf/NaN are output as-is. For the **fp8_e4m3fn_t** type, if the absolute value of the result is Inf or greater than the maximum normalized value of **fp8_e4m3fn_t**, the output result is NaN.
    - Saturation mode: **CTRL[48]** is set to 1'b0, Inf output is saturated to ±MAX, and NaN output is saturated to 0. For the **fp8_e4m3fn_t** type, when **CTRL[50]** is set to 1'b0, NaN is converted to zero; when **CTRL[50]** is set to 1'b1, NaN remains unchanged. If the absolute value of the result is Inf or greater than the maximum normalized value of **fp8_e4m3fn_t**, the result is the maximum normalized value.

        ```cpp
        // Set CTRL[48] to 0 to enable the floating-point saturation mode.
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    For integer types, only the saturation mode is available.
<!-- end id18 -->

<!-- npu="950" id20 -->
## Key Features

The following figures and descriptions apply only to Ascend 950PR/Ascend 950DT:

### Without Enabling NZ2ND Inline Format Conversion

When NZ2ND is not enabled, the parameter setting example (moving data through the Fixpipe API and removing dummy data) and its explanation are as follows:

When the number of data elements in the M direction is not a multiple of 16, dummy data is additionally read during the move-in and discarded after being written to the destination. A cube block is defined as a contiguous 16\*16 data block. The number of data blocks is M/16 rounded up, and the length of a cube block is M\*16\*sizeof\(T\), where T is the data type.

**Single movement mode:**

- **nSize** = 48, indicating that the cube to be moved in the source NZ cube (the blue area in the figure) has a size of 48 elements in the N direction.
- **mSize** = 24, indicating that the cube to be moved in the source NZ cube has a size of 24 elements in the M direction.
- **srcStride** = 64, indicating the start address offset between adjacent Z layouts of the cube to be moved in the source NZ cube, that is, the interval between the start address of the first blue Z layout and that of the second blue Z layout in the following figure is 64 \* C0\_Size.
- **dstStride** = 40 \* C0, indicating the start address offset between adjacent Z layouts in the destination NZ cube, that is, the interval between the start address of the first blue Z layout and that of the second blue Z layout in the following figure is 40 \* 16 elements.

**Figure 1** Schematic diagram of single movement mode parameter setting without NZ2ND<a name="fig128961542184620"></a>  

![](../../../../figures/single_move_no_nz2nd_config.png "Schematic diagram of single movement mode parameter setting without NZ2ND")

**Dual-target control mode:**

Enabling dual-target mode in normal movement mode is shown in the following figure, which is divided into splitting by the M dimension and splitting by the N dimension. When splitting by the M dimension, M must be a multiple of 2; when splitting by the N dimension, N must be a multiple of 32:

N-direction splitting:

- **nSize** = 32, indicating that the size of the cube to be moved in the source NZ cube in the N direction is 32 elements.
- **mSize** = 48, indicating that the size of the cube to be moved in the source NZ cube in the M direction is 48 elements.
- **srcStride** = 64, indicating the start address offset between adjacent Z layouts of the cube to be moved in the source NZ cube, that is, the interval between the start address of the first block Z-layout cube and the start address of the second Z-layout cube in the following figure is 64 \* C0\_Size.
- **dstStride** = 64 \* C0, indicating the start address offset between adjacent Z layouts in the destination NZ cube, that is, the interval between the start address of the Z layout in UB0 and the start address of the Z layout in UB1 in the following figure is 64 \* 16 elements.

M-direction splitting:

- **nSize** = 32, indicating that the size of the cube to be moved in the source NZ cube in the N direction is 32 elements.
- **mSize** = 24, indicating that the size of the cube to be moved in the source NZ cube in the M direction is 24 elements.
- **srcStride** = 64, indicating the start address offset between adjacent Z layouts of the cube to be moved in the source NZ cube, that is, the interval between the start address of the first Z-layout cube and the start address of the second Z-layout cube in the following figure is 64 \* C0\_Size.
- **dstStride** = 40 \* C0, indicating the start address offset between adjacent Z layouts in the destination NZ cube, that is, the interval between the start address of the first Z layout and the start address of the second Z layout in UB0 (or UB1) in the following figure is 40 \* 16 elements.

**Figure 2** Schematic diagram of the dual-target movement mode setting without the NZ2ND and NZ2DN parameters<a name="fig6561154491913"></a>  

![](../../../../figures/dual_target_move_no_nz2nd_nz2dn.png "Schematic diagram of the dual-target movement mode setting without the NZ2ND and NZ2DN parameters")

### Enabling NZ2ND Inline Format Conversion

When NZ2ND is enabled, the parameter setting example and explanation are as follows:

- **ndNum** = 2, indicating that the number of source NZ matrices is 2. In the figure, the blue area is NZ cube 1, and the purple area is NZ cube 2.
- **nSize** = 32, indicating that the size of the source NZ cube (the blue area in the figure) in the N direction is 32 elements.
- **mSize** = 48, indicating that the size of the source NZ cube in the M direction is 48 elements.
- **srcStride** = 64, indicating the start address offset between adjacent Z layouts in the source NZ cube, that is, the interval between the start address of the first blue Z layout and the start address of the second blue Z layout in the following figure is 64 \* C0\_Size.
- **dstStride** = 64, indicating that the number of elements in each row of the destination ND cube is 64.
- **srcNdStride** = 16, indicating that the interval between the start addresses of different NZ matrices is 16 \*  16 \* C0\_Size.
- **dstNdStride** = 4096, indicating that the offset between the start addresses of adjacent destination ND matrices is 4096 elements.

**Figure 3** Schematic diagram of parameter setting with NZ2ND enabled<a name="fig79783143556"></a>  

![](../../../../figures/nz2nd_config_enabled.png "NZ2ND Parameter Setting Diagram")

**Single movement mode:**

- **ndNum** = 2, indicating that the number of source NZ matrices is 2. In the figure, the blue area is NZ cube 1, and the purple area is NZ cube 2.
- **nSize** = 32, indicating that the size of the source NZ cube (the blue area in the figure) in the N direction is 32 elements.
- **mSize** = 48, indicating that the size of the source NZ cube in the M direction is 48 elements.
- **srcStride** = 64, indicating the start address offset between adjacent Z layouts in the source NZ cube, that is, the interval between the start address of the first blue Z layout and the start address of the second blue Z layout in the following figure is 64 \* C0\_Size.
- **dstStride** = 64, indicating that the number of elements in each row of the destination ND cube is 64.
- **srcNdStride** = 256, indicating that the interval between the start addresses of different NZ matrices is 256, in units of C0\_Size.
- **dstNdStride** = 4096, indicating that the offset between the start addresses of adjacent destination ND matrices is 4096 elements.

**Figure 4** Single movement-in mode setting diagram with NZ2ND parameters enabled<a name="fig11991024112516"></a>  

![](../../../../figures/single_load_nz2nd_enabled.png "Single move-in mode setting with NZ2ND parameters enabled")

**Dual-target control mode:**

- **ndNum** = 2, indicating that the number of source NZ matrices is 2. In the figure, the red-boxed area is cube 1, and the blue-boxed area is cube 2.
- **nSize** = 32, indicating that the size of the source NZ cube (the red-boxed area or the blue-boxed area in the figure) in the N direction is 32 elements.
- **mSize** = 48, indicating that the size of the source NZ cube in the M direction is 48 elements.
- **srcStride** = 64, indicating the start address offset between adjacent Z layouts in the source NZ cube, that is, the interval between the start address of the light-colored Z-layout cube on the left and the start address of the dark-colored Z-layout cube on the right in the red-boxed area of the figure below is 64 \* C0\_Size.
- **dstStride** = 64, indicating that the number of elements in each row of the destination ND cube is 64.
- **ndNum** = 2, indicating the number of source NZ matrices.
- **srcNdStride** = 240, indicating that the interval between the start addresses of different NZ matrices is 240 \* C0\_Size.
- **dstNdStride** = 4096, indicating that the offset between the start addresses of adjacent destination ND matrices is 4096 elements.

    **Figure 5** Setting diagram of the NZ2ND dual move-in mode<a name="fig8810182815117"></a>  

    ![](../../../../figures/dual_load_nz2nd_enabled.png "Setting diagram of the NZ2ND dual move-in mode")

### Enabling NZ2DN Inline Format Conversion

When NZ2DN is enabled, the parameter setting examples and explanations are as follows:

**Single movement mode:**

Example 1: The parameter values are described as follows. See [Figure 6](#fig19772110476) for the schematic diagram:

- **dnNum** = 2, indicating that the number of source NZ matrices is 2. In the figure, the blue area is NZ cube 1, and the purple area is NZ cube 2.
- **nSize** = 32, indicating that the size of the source NZ cube (the blue area in the figure) in the N direction is 32 elements.
- **mSize** = 48, indicating that the size of the source NZ cube in the M direction is 48 elements.
- **srcStride** = 80, indicating the start address offset between adjacent Z layouts in the source NZ cube, that is, the interval between the start addresses of two adjacent blue Z layouts in the following figure is 80 \* C0\_Size.
- **dstStride** = 80, indicating that the number of elements in each row of the destination DN cube is 80.
- **ndNum** = 2, indicating the number of source NZ matrices.
- **srcNzCubeStride** = 240, indicating the offset between different source NZ matrices, that is, the interval between the start address of the first blue Z layout and the start address of the second purple Z layout in the following figure is 240 \* C0\_Size.
- **srcNzC0Stride** = 1: Indicates the address offset between adjacent rows of the source cube NZ fractal.
- **dstDnCubeStride**: Indicates that the offset between the start addresses of adjacent DN matrices is 48 \* 80 = 3840 elements.

**Figure 6** Schematic diagram 1 of enabling NZ2DN single movement mode<a name="fig19772110476"></a>  

![](../../../../figures/single_move_nz2dn_enabled_1.png "Schematic diagram 1 of enabling NZ2DN single movement mode")

Example 2: The parameter values are described as follows. See [Figure 7](#fig769114585716) for the schematic diagram.

- **dnNum** = 2, indicating that the number of source NZ matrices is 2. In the figure, the blue area is NZ cube 1, and the red area is NZ cube 2.
- **nSize** = 24, indicating that the size of the source NZ cube (the blue area in the figure) in the N direction is 24 elements.
- **mSize** = 24, indicating that the size of the source NZ cube in the M direction is 24 elements.
- **srcStride** = 80, indicating the start address offset between adjacent Z layouts in the source NZ cube, that is, the interval between the start addresses of two adjacent blue Z layouts in the following figure is 80 \* C0\_Size.
- **dstStride** = 60, indicating that the number of elements in each row of the destination DN cube is 60.
- **ndNum** = 2, indicating the number of source NZ matrices.
- **srcNzCubeStride** = 240, indicating the offset between different source NZ matrices, that is, the interval between the start address of the first blue Z layout and the start address of the second purple Z layout in the following figure is 240 \* C0\_Size.
- **srcNzC0Stride** = 2: Indicates the address offset between adjacent rows of the NZ fractal of the source cube.
- **dstDnCubeStride**: Indicates that the offset between the start addresses of adjacent DN matrices is 48 \* 60 = 2880 elements.

**Figure 7** Schematic diagram 2 of enabling NZ2DN single-movement mode<a name="fig769114585716"></a>  

![](../../../../figures/single_move_nz2dn_enabled_2.png "Schematic diagram 2 of enabling NZ2DN single-movement mode")
<!-- end id20 -->

## Example<a name="zh-cn_topic_0000002549846732_section088124295117"></a>

<!-- npu="950" id19 -->
For the complete sample, see the [fixpipe\_l0c2ub sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2ub).

- Example: Path L0C Buffer-\>UB. The input matrices A and B use the **half** data type, and the output cube C uses **float**. The default configuration enables Nz2Nd format conversion. For the complete sample, see the [sample link](https://gitcode.com/cann/asc-devkit/blob/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2ub).

    ```cpp
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
