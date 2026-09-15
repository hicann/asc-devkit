# DataCopy (L0C-to-L1 Data Movement)<a id="ZH-CN_TOPIC_0000002538231198"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-09-01T06:49:55.235Z -->

## Product Support<a id="zh-cn_topic_0000002511188540_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: supported
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

## Function Description<a id="zh-cn_topic_0000002511188540_section106841136114319"></a>

The header file path is: `"basic_api/kernel_operator_data_copy_intf.h"`.

The results of cube computation are stored in the L0C Buffer. The DataCopy API is used to move the results to the L1 Buffer, and it supports operations such as on-the-fly format conversion during the movement.

<!-- npu="950,A3,910b" id8 -->
The following figure shows the valid combinations of on-the-fly quantization, on-the-fly ReLU, and on-the-fly channel merging, as well as the intermediate data types and data paths. In the following figure, F32-\>F16 and F32-\>BF16 are non-quantization modes and are only Cast, while the rest are on-the-fly scalar/tensor quantization modes.

<!-- npu="A3,910b" id9 -->
**Figure 1** L0C2L1 flowchart ([NPU architecture version 2201](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md))<a id="zh-cn_topic_0000002511188540_fig8956371257"></a>  

![](../../../../figures/L0C2L1_Function_Combination.png)
<!-- end id9 -->

<!-- npu="950" id11 -->
**Figure 2** L0C2L1 flowchart ([NPU architecture version 3510](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md))<a id="zh-cn_topic_0000002542828493_fig1828513492475"></a>  

![](../../../../figures/L0C2L1_Function_Combination_DataCopy_950.png)
<!-- end id11 -->
<!-- end id8 -->

## Function Prototype<a id="zh-cn_topic_0000002511188540_section82039854412"></a>

The DataCopy cube move-out API supports combinations of multiple in-line capabilities. Different registers need to be set, and different data movement capabilities are enabled together with the data movement instruction. The corresponding APIs are as follows:

- Data movement API, path L0C Buffer(CO1)->L1 Buffer(C1), which implements quantization and ReLU activation together with register settings.

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyCO12DstParams& intriParams)
    ```

- [SetFixPipeConfig](../cube_store_aux_config/SetFixPipeConfig.md): register setting API. Call this API to set in-line quantization, where each element of the tensor represents a quantization parameter. It needs to be set when tensor quantization is used.

- [SetFixpipePreQuantFlag](../cube_store_aux_config/SetFixpipePreQuantFlag.md): register setting API. Call this API to set the in-line quantization parameter, where this element represents the quantization parameter used by the entire output cube. It needs to be set when Scalar quantization is used.

- [SetFixpipeNz2ndFlag](../cube_store_aux_config/SetFixpipeNz2ndFlag.md): register setting API. Call this API to set the in-line NZ2ND format conversion configuration. It needs to be set when in-line NZ2ND is used.

<!-- npu="310b" id12 -->
For the Atlas 200I/500 A2 inference product, the following two APIs are also supported:

- [SetFixPipeClipRelu](../cube_store_aux_config/SetFixPipeClipRelu.md): register setting API. Call this API to set the maximum value of the ClipReLU operation.

- [SetFixPipeAddr](../cube_store_aux_config/SetFixPipeAddr.md): register setting API. Call this API to set the address of the LocalTensor during the Elementwise operation.
<!-- end id12 -->

## Parameter Description<a id="zh-cn_topic_0000002511188540_section16128134420472"></a>

**Table 1** DataCopy template parameters for data movement

| Parameter Name | Description |
| ---------- | ---------- |
| T | Data type of the destination operand. For supported data types, see [Data Type](#zh-cn_topic_0000002511188540_section4219135304818). |
| U | Data type of the source operand. For supported data types, see [Data Type](#zh-cn_topic_0000002511188540_section4219135304818). |

**Table 2** DataCopy API parameters for data movement

| Parameter Name | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of type LocalTensor, in NZ<!-- npu="950" id10 --> or ND (supported only on Ascend 950PR/Ascend 950DT)<!-- end id10 --> format. The address must be 32-byte aligned. |
| src | Input | Source operand, of type LocalTensor. The supported physical address is L0C Buffer (TPosition is CO1), which is the result computed by the Mmad API. The data format is NZ, and the address must be 64-byte aligned. |
| intriParams | Input | Movement parameters, of type [DataCopyCO12DstParams](#zh-cn_topic_0000002511188540_table35908519282).<br>For the specific definition, see \$\{INSTALL\_DIR\}/include/ascendc/basic\_api/API/kernel\_struct\_data_copy.h. Replace \$\{INSTALL\_DIR\} with the file storage path after the CANN software is installed. |

**Table 3** DataCopyCO12DstParams structure parameter definition<a id="zh-cn_topic_0000002511188540_table35908519282"></a>

| Parameter Name | Description |
| ---------- | ---------- |
| sid | This parameter does not need to be concerned by users. Set it to 0. |
| nSize | Size of the source NZ cube in the N direction. Value range: nSize∈[0, 4095]. nSize must be a multiple of 16.<br>**Note: nSize=0 means no movement is performed, and the API is treated as a NOP (no operation).** |
| mSize | Size of the source NZ cube in the M direction.<br>&nbsp;&nbsp;&bull;When the in-line NZ2ND function is not enabled ([NZ2NZ movement](../cube_store_key_features/NZ2NZ.md)): value range is mSize∈[0, 65535].<br><!-- npu="950" id25 -->&nbsp;&nbsp;&bull;When the in-line [NZ2ND](../cube_store_key_features/NZ2ND.md) function is enabled (supported only on Ascend 950PR/Ascend 950DT): value range is mSize∈[0, 8192].<br><!-- end id25 -->**Note: mSize=0 means no movement is performed, and the API is treated as a NOP (no operation).** |
| dstStride | &bull;When the NZ2ND function is not enabled ([NZ2NZ movement](../cube_store_key_features/NZ2NZ.md)): start address offset of adjacent Z layouts in the destination NZ cube. The value must not be 0, and the unit is datablock (32 bytes).<br><!-- npu="950" id26 -->&bull;When the in-line [NZ2ND](../cube_store_key_features/NZ2ND.md) function is enabled (supported only on Ascend 950PR/Ascend 950DT): number of elements in each row of the destination ND cube. The value must not be 0, and the unit is element.<!-- end id26 --> |
| srcStride | Start address offset of adjacent Z layouts in the source NZ cube. Value range: srcStride∈[0, 65535], and the unit is C0_Size (16*sizeof(T)), where T is the data type of src. Its value should be set to mSize rounded up to a multiple of 16. |
| unitFlag | unitFlag is a fine-grained parallelism between Mmad instructions and Fixpipe instructions. When this function is enabled, the computation result is moved out as soon as the hardware finishes computing each fractal. The values are described as follows:<br>&nbsp;&nbsp;&bull; 0 (2'b00): unitFlag is not enabled.<br>&nbsp;&nbsp;&bull; 2 (2'b10): unitFlag is enabled. After the hardware finishes executing the instruction, the unit flag bit is not reset.<br>&nbsp;&nbsp;&bull; 3 (2'b11): unitFlag is enabled. After the hardware finishes executing the instruction, the unit flag bit is reset.<br>When enabling this function, set the unitFlag value of both the Mmad instruction and the Fixpipe instruction to 2 or 3.<br>For the parameter setting scheme and feature details, see the [UnitFlag](../mmad_compute_key_features/mmad_computation_key_feature_description.md) section in the key feature description of Mmad computation. |
| clipReluPre | <!-- npu="950,A3,910b" id13 -->This parameter is supported only on Atlas 200I/500 A2 inference products.<br><!-- end id13 --><!-- npu="310b" id14 -->Used to configure whether to enable the ClipReLU operation. The parameter type is uint8_t, and the values are as follows: 0, ClipReLU is not enabled; 1, ClipReLU is enabled. In this case, call [SetFixPipeClipRelu](../cube_store_aux_config/SetFixPipeClipRelu.md) to set the maximum value of ClipReLU.<br>&nbsp;&nbsp;&bull;This operation is performed after in-line quantization and can be used only after quantPre is configured. The currently supported quantization modes are F322F16/DEQF16/VDEQF16/QF322B8_PRE/VQF322B8_PRE/REQ8/VREQ8.<br><!-- end id14 --> |
| eltWiseOp | <!-- npu="950,A3,910b" id15 -->This parameter is supported only on Atlas 200I/500 A2 inference products.<br><!-- end id15 --><!-- npu="310b" id16 -->Used to configure whether to enable the Elementwise operation and the operation mode. The Elementwise operation means that after in-line quantization, a LocalTensor can be added to or subtracted from element by element, with a size of mSize * nSize. For the specific LocalTensor address-related parameters, call [SetFixPipeAddr](../cube_store_aux_config/SetFixPipeAddr.md) to set them.<br>The eltWiseOp parameter type is uint8_t, and the values are as follows:<br>&nbsp;&nbsp;&bull; 0: Elementwise is not enabled;<br>&nbsp;&nbsp;&bull; 1: Elementwise Addition;<br>&nbsp;&nbsp;&bull; 2: Elementwise Subtraction.<br><!-- end id16 --> |
| quantPre | Used to control the quantization mode. The type is QuantMode_t, and the specific definition is as follows:<br>&nbsp;&nbsp;&bull;For half/bfloat16_t output, this parameter must be configured as QuantMode_t::F322F16/QuantMode_t::F322BF16.<br>&nbsp;&nbsp;&bull;When configured as scalar quantization, call the [SetFixpipePreQuantFlag](../cube_store_aux_config/SetFixpipePreQuantFlag.md) API to set the scalar quantization parameters.<br>&nbsp;&nbsp;&bull;When configured as tensor quantization, call [SetFixPipeConfig](../cube_store_aux_config/SetFixPipeConfig.md) to set the tensor quantization parameters, where the tensor quantization parameters need to be moved from L1 Buffer to Fixpipe Buffer through DataCopy.<br>Note: This parameter needs to be configured manually by users and will not be automatically derived to configure the corresponding quantization mode. The NoQuant mode is not supported on this path.<br>enum QuantMode_t<br>{<br>    NoQuant,      // Quantization is not enabled. This path does not support it.<br>    F322F16,      // Float32_2_Float16: float is cast to half, and the cast mode is CAST_RINT.<br>    F322BF16,     // Float32_2_BFloat16: float is cast to bfloat16_t, and the cast mode is CAST_RINT.<br>    DEQF16,       // DeQuant_Float16: int32_t is quantized to half, scalar quantization.<br>    VDEQF16,      // Vector_DeQuant_Float16: int32_t is quantized to half, tensor quantization.<br>    QF322B8_PRE,  // Quant_Float32_2_B8: float is quantized to int8_t/uint8_t, scalar quantization.<br>    VQF322B8_PRE, // Vector_Quant_Float32_2_B8: float is quantized to int8_t/uint8_t, tensor quantization.<br>    REQ8,         // ReQuant_int8: int32_t is quantized to int8_t/uint8_t, scalar quantization.<br>    VREQ8,        // Vector_ReQuant_int8: int32_t is quantized to int8_t/uint8_t, tensor quantization.<br>};<br>|
| reluPre | Used to configure the ReLU operation mode. The type is uint8_t, and the values are as follows:<br>&nbsp;&nbsp;&bull; 0: ReLU is not enabled<br>&nbsp;&nbsp;&bull; 1: Normal ReLU |
| channelSplit | <!-- npu="950" id27 -->**This parameter takes effect only on Ascend 950PR/Ascend 950DT**, and only when the output is in NZ format with float type. The type is bool, and it configures whether to enable the channel split function.<br>&nbsp;&nbsp;&bull; false: not enabled;<br>&nbsp;&nbsp;&bull; true: enabled.<br><!-- end id27 --><!-- npu="A3,910b,310b" id29 -->For the following product models, this parameter does not take effect on this path. Set it to false.<br><!-- npu="A3" id30 -->Atlas A3 training products/Atlas A3 inference products<br><!-- end id30 --><!-- npu="910b" id31 -->Atlas A2 training products/Atlas A2 inference products<br><!-- end id31 --><!-- npu="310b" id32 -->Atlas 200I/500 A2 inference products<!-- end id32 --><!-- end id29 --> |
| nz2ndEn | <!-- npu="950" id28 -->**This parameter takes effect only on Ascend 950PR/Ascend 950DT**. The type is bool, and it configures whether to enable the NZ2ND format conversion.<br>To enable the NZ2ND function, call [SetFixpipeNz2ndFlag](../cube_store_aux_config/SetFixpipeNz2ndFlag.md) to set the related configuration information for format conversion.<br>&nbsp;&nbsp;&bull; false: not enabled;<br>&nbsp;&nbsp;&bull; true: enabled.<br><!-- end id28 --><!-- npu="A3,910b,310b" id33 -->For the following product models, this parameter does not take effect on this path. Set it to false.<br><!-- npu="A3" id34 -->Atlas A3 training products/Atlas A3 inference products<br><!-- end id34 --><!-- npu="910b" id35 -->Atlas A2 training products/Atlas A2 inference products<br><!-- end id35 --><!-- npu="310b" id36 -->Atlas 200I/500 A2 inference products<!-- end id36 --><!-- end id33 --> |

## Data Type<a id="zh-cn_topic_0000002511188540_section4219135304818"></a>

**Data type combinations supported by the source cube and destination cube**

<!-- npu="950" id17 -->
For Ascend 950PR/Ascend 950DT, the DataCopy API supports the following data type combinations:

Source Cube (L0C Buffer) to Destination Cube (L1 Buffer):
float to int8_t, uint8_t, half, bfloat16_t, float.
int32_t to int8_t, uint8_t, half, int32_t.
<!-- end id17 -->

<!-- npu="A3" id18 -->
For Atlas A3 training products/Atlas A3 inference products, the DataCopy API supports the following data type combinations:

Source Cube (L0C Buffer) to Destination Cube (L1 Buffer):
float to int8_t, uint8_t, half, bfloat16_t.
int32_t to int8_t, uint8_t, half.
<!-- end id18 -->

<!-- npu="910b" id19 -->
For Atlas A2 training products/Atlas A2 inference products, the DataCopy API supports the following data type combinations:

Source Cube (L0C Buffer) to Destination Cube (L1 Buffer):
float to int8_t, uint8_t, half, bfloat16_t.
int32_t to int8_t, uint8_t, half.
<!-- end id19 -->

<!-- npu="310b" id20 -->
For Atlas 200I/500 A2 inference products, the DataCopy API supports the following data type combinations:

Source Cube (L0C Buffer) to Destination Cube (L1 Buffer):
float to int8_t, uint8_t, half, bfloat16_t.
int32_t to int8_t, uint8_t, half.
<!-- end id20 -->

## Return Value Description

None

## Constraints<a id="zh-cn_topic_0000002511188540_section2045914466492"></a>

- The description of the float32 data type for quantization input is as follows:
    - The standard IEEE 754 float32 format is: 1-bit sign, 8-bit exponent, 23-bit mantissa; the float32 format supported by the current AI processor is: 1-bit sign, 8-bit exponent, 10-bit mantissa.
    - If the user provides standard IEEE 754 float32 input, the API internally converts it to the processor-supported float32 format for computation. In this case, if the golden data is generated using standard IEEE 754 float32 data, a precision mismatch may be introduced. The golden data generation needs to be corrected by clearing the lower 13 bits of the 23-bit mantissa of the quantization parameter before participating in the quantization computation.
- The source cube NZ format address requires 64-byte alignment, and the destination cube NZ format address requires 32-byte alignment.
- When either mSize or nSize of the move-out is 0, this instruction will not be executed.
- The quantization and ReLU parameters cannot be Inf/NaN or denormalized numbers.
- The destination data must not overlap. If there is overlapping write to the destination address, the hardware will not report any warning or error, nor does it guarantee the write order of overlapping data.
- Enabling the unitFlag feature requires enabling Mmad at the same time.
<!-- npu="A3,910b" id21 -->
- For the following product models, the special value/boundary value constraints are described as follows:
    <!-- npu="A3" id22 -->
    Atlas A3 training products/Atlas A3 inference products
    <!-- end id22 -->
    <!-- npu="910b" id23 -->
    Atlas A2 training products/Atlas A2 inference products
    <!-- end id23 -->
    For floating-point Inf/NaN input and output, you can set the CTRL[48] bit of the CTRL register (control register) to control the saturation mode during quantized move-out of floating-point data:

    - Non-saturation mode: Set CTRL[48] to 1'b1, and Inf/NaN are output as-is.
    - Saturation mode: Set CTRL[48] to 1'b0, Inf output is saturated to ±MAX, and NaN output is saturated to 0.

        ```cpp
        // Set CTRL[48] to 0 to enable the floating-point saturation mode.
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    For integer types, only the saturation mode is available.
<!-- end id21 -->
<!-- npu="950" id24 -->
- Special value/boundary value constraint description for Ascend 950PR/Ascend 950DT:

    For floating-point Inf/NaN input and output, you can set the CTRL[48] bit of the CTRL register (control register) to control the saturation mode during quantized move-out of floating-point data;

    For the fp8_e4m3fn_t type, you can set the CTRL[48] bit of the CTRL register (control register) for finer-grained control;

    - Non-saturation mode: CTRL\[48\] is set to 1'b1, and Inf/NaN are output as-is. For the fp8\_e4m3fn\_t type, if the absolute value of the result is Inf or greater than the maximum normalized value of fp8\_e4m3fn\_t, the output result is NaN.
    - Saturation mode: CTRL\[48\] is set to 1'b0, the Inf output is saturated to ±MAX, and the NaN output is saturated to 0. For the fp8\_e4m3fn\_t type, when CTRL\[50\] is set to 1'b0, NaN is converted to zero; when CTRL\[50\] is set to 1'b1, NaN remains unchanged. If the absolute value of the result is Inf or greater than the maximum normalized value of fp8\_e4m3fn\_t, the result is the maximum normalized value.

        ```cpp
        // Set CTRL[48] to 0 to enable the floating-point saturation mode.
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    For integer types, only the saturation mode is available.
<!-- end id24 -->

## Calling Example<a id="zh-cn_topic_0000002511188540_section088124295117"></a>

None
