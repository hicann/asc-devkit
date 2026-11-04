# DataCopy (L0C to L1 Data Movement)<a id="ZH-CN_TOPIC_0000002538231198"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:22:22.844Z pushedAt=2026-10-26T07:41:44.113Z nl=2 -->

## Applicable Products<a id="zh-cn_topic_0000002511188540_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/DataCopy_L0CToL1_res.md#id1 -->

## Description<a id="zh-cn_topic_0000002511188540_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

The matrix computation result is stored in the L0C Buffer. The **DataCopy** API is used to move the result to the L1 Buffer, and it supports operations such as in-line format conversion during the movement.

<!-- npu="950,A3,910b" id8 -->
The following figure shows the valid combinations of on-the-fly quantization, in-line ReLU, and in-line channel merge, as well as the intermediate data types and data paths. In the figure, F32-\>F16 and F32-\>BF16 are non-quantization modes that perform only Cast, while the rest are in-line scalar/tensor quantization modes.

<!-- npu="A3,910b" id9 -->
**Figure 1** L0C2L1 flow diagram ([NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="zh-cn_topic_0000002511188540_fig8956371257"></a>  

![](../../../../figures/L0C2L1_Function_Combination.png)
<!-- end id9 -->

<!-- npu="950" id11 -->
**Figure 2** L0C2L1 flow diagram ([NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="zh-cn_topic_0000002542828493_fig1828513492475"></a>  

![](../../../../figures/L0C2L1_Function_Combination_DataCopy_950.png)
<!-- end id11 -->
<!-- end id8 -->

## Prototype<a id="zh-cn_topic_0000002511188540_section82039854412"></a>

The **DataCopy** matrix move-out API supports combinations of multiple in-line capabilities. Different registers need to be set, and different data movement capabilities are enabled in conjunction with data movement instructions. The corresponding APIs are as follows:

- Data movement API, path L0C Buffer(CO1)->L1 Buffer(C1), which implements quantization and ReLU activation in conjunction with register settings.

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyCO12DstParams& intriParams)
    ```

- [SetFixPipeConfig](../cube_store_aux_config/SetFixPipeConfig.md): Register setting API. Call this API to set on-the-fly quantization, where each element of the tensor represents a quantization parameter. This must be set when tensor quantization is used.

- [SetFixpipePreQuantFlag](../cube_store_aux_config/SetFixpipePreQuantFlag.md): Register setting API. Call this API to set the on-the-fly quantization parameter, where this element represents the quantization parameter used by the entire output matrix. This must be set when Scalar quantization is used.

- [SetFixpipeNz2ndFlag](../cube_store_aux_config/SetFixpipeNz2ndFlag.md): Register setting API. Call this API to set the in-line NZ2ND format conversion configuration. This must be set when in-line NZ2ND is used.

<!-- npu="310b" id12 -->
For the Atlas 200I/500 A2 inference products, the following two APIs are also supported:

- [SetFixPipeClipRelu](../cube_store_aux_config/SetFixPipeClipRelu.md): Register setting API. Call this API to set the maximum value of the ClipReLU operation.

- [SetFixPipeAddr](../cube_store_aux_config/SetFixPipeAddr.md): Register setting API. Call this API to set the address of the LocalTensor during the Elementwise operation.
<!-- end id12 -->

## Parameters<a id="zh-cn_topic_0000002511188540_section16128134420472"></a>

**Table 1** DataCopy template parameter description for data movement

| Parameter | Description |
| ---------- | ---------- |
| **T** | Data type of the destination operand. For supported data types, see [Data Type](#zh-cn_topic_0000002511188540_section4219135304818). |
| **U** | Data type of the source operand. For supported data types, see [Data Type](#zh-cn_topic_0000002511188540_section4219135304818). |

**Table 2** DataCopy API parameter description for data movement

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| **dst** | Output | Destination operand. The type is **LocalTensor**. The data format is NZ or ND (supported only on the Ascend 950PR&950DT products)<!-- npu="950" id10 --><!-- end id10 -->. The address must be 32-byte aligned. |
| **src** | Input | Source operand. The type is **LocalTensor**. The supported physical address is L0C Buffer (**TPosition** is **CO1**), which is the result computed by the **Mmad** API. The data format is NZ. The address must be 64-byte aligned. |
| **intriParams** | Input | Movement parameter. The type is [DataCopyCO12DstParams](#zh-cn_topic_0000002511188540_table35908519282).<br>For the specific definition, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_data_copy.h`. Replace `${INSTALL_DIR}` with the file storage path after the CANN software is installed. |

**Table 3** DataCopyCO12DstParams structure parameter definition<a id="zh-cn_topic_0000002511188540_table35908519282"></a>

| Parameter | Description |
| ---------- | ---------- |
| **sid** | This parameter does not require user attention. Set it to 0. |
| **nSize** | Size of the source NZ matrix in the N direction. Value range: nSize∈[0, 4095]. nSize must be a multiple of 16.<br>**Note: nSize=0 means no movement is performed, and the API is treated as a NOP (no operation).** |
| **mSize** | Size of the source NZ matrix in the M direction.<br>&nbsp;&nbsp;&bull;When the in-line NZ2ND function is not enabled ([NZ2NZ movement](../cube_store_key_features/NZ2NZ.md)): Value range: mSize∈[0, 65535].<br><!-- npu="950" id25 -->&nbsp;&nbsp;&bull;When the in-line [NZ2ND](../cube_store_key_features/NZ2ND.md) function is enabled (supported only on the Ascend 950PR&950DT products): Value range: mSize∈[0, 8192].<br><!-- end id25 -->**Note: mSize=0 means no movement is performed, and the API is treated as a NOP (no operation).** |
| **dstStride** | &bull;When the NZ2ND function is not enabled ([NZ2NZ movement](../cube_store_key_features/NZ2NZ.md)): Start address offset between adjacent Z layouts in the destination NZ matrix. The value must not be 0, in units of datablock (32 bytes).<br><!-- npu="950" id26 -->&bull;When the in-line [NZ2ND](../cube_store_key_features/NZ2ND.md) function is enabled (supported only on the Ascend 950PR&950DT products): Number of elements in each row of the destination ND matrix. The value must not be 0, in units of element.<!-- end id26 --> |
| **srcStride** | Start address offset between adjacent Z layouts in the source NZ matrix. Value range: srcStride∈[0, 65535], in units of C0_Size (16*sizeof(T)), where T is the data type of src. The value should be set to mSize rounded up to a multiple of 16. |
| **unitFlag** | **unitFlag** enables fine-grained parallelism between **Mmad** and **Fixpipe** instructions. When this function is enabled, the hardware moves out the computation result each time it finishes computing one fractal. The values are described as follows:<br>&nbsp;&nbsp;&bull; **0** (2'b00): Disable unitFlag.<br>&nbsp;&nbsp;&bull; **2** (2'b10): Enable unitFlag. After the hardware finishes executing the instruction, it does not reset the unit flag bit.<br>&nbsp;&nbsp;&bull; **3** (2'b11): Enable unitFlag. After the hardware finishes executing the instruction, it resets the unit flag bit.<br>When enabling this function, set the unitFlag value of both the **Mmad** and **Fixpipe** instructions to 2 or 3.<br>For parameter configuration schemes and feature details, see the [UnitFlag](../mmad_compute_key_features/UnitFlag.md) section in the key feature description of Mmad computation. |
| **clipReluPre** | <!-- npu="950,A3,910b" id13 -->This parameter is supported only on the Atlas 200I/500 A2 inference product.<br><!-- end id13 --><!-- npu="310b" id14 -->Configures whether to enable the ClipReLU operation. The parameter type is uint8_t. The values are as follows: **0**: Disable ClipReLU. **1**: Enable ClipReLU. In this case, call [SetFixPipeClipRelu](../cube_store_aux_config/SetFixPipeClipRelu.md) to set the maximum value of ClipReLU.<br>&nbsp;&nbsp;&bull;This operation is performed after on-the-fly quantization and can be used only after **quantPre** is configured. The currently supported quantization modes are F322F16/DEQF16/VDEQF16/QF322B8_PRE/VQF322B8_PRE/REQ8/VREQ8.<br><!-- end id14 --> |
| **eltWiseOp** | <!-- npu="950,A3,910b" id15 -->This parameter is supported only on the Atlas 200I/500 A2 inference product.<br><!-- end id15 --><!-- npu="310b" id16 -->Configures whether to enable the Elementwise operation and its operation mode. The Elementwise operation adds or subtracts a **LocalTensor** element by element after on-the-fly quantization. Its size is mSize * nSize. The parameters related to the **LocalTensor** address must be set by calling [SetFixPipeAddr](../cube_store_aux_config/SetFixPipeAddr.md).<br>The **eltWiseOp** parameter type is uint8_t. The values are as follows:<br>&nbsp;&nbsp;&bull; **0**: Disable Elementwise.<br>&nbsp;&nbsp;&bull; **1**: Elementwise Addition.<br>&nbsp;&nbsp;&bull; **2**: Elementwise Subtraction.<br><!-- end id16 --> |
| **quantPre** | Controls the quantization mode. The type is **QuantMode_t**, defined as follows:<br>&nbsp;&nbsp;&bull; For half/bfloat16_t output, configure this parameter to **QuantMode_t::F322F16**/**QuantMode_t::F322BF16**.<br>&nbsp;&nbsp;&bull; When scalar quantization is configured, call the [SetFixpipePreQuantFlag](../cube_store_aux_config/SetFixpipePreQuantFlag.md) API to set the scalar quantization parameters.<br>&nbsp;&nbsp;&bull; When tensor quantization is configured, call [SetFixPipeConfig](../cube_store_aux_config/SetFixPipeConfig.md) to set the tensor quantization parameters. The tensor quantization parameters must be moved from L1 Buffer to Fixpipe Buffer through **DataCopy**.<br>Note: This parameter must be configured manually by the user. The corresponding quantization mode is not derived automatically. This path does not support the NoQuant mode.<br>enum QuantMode_t<br>{<br>    NoQuant,      // Disable quantization. This path does not support it.<br>    F322F16,      // Float32_2_Float16: Cast float to half, with cast mode CAST_RINT.<br>    F322BF16,     // Float32_2_BFloat16: Cast float to bfloat16_t, with cast mode CAST_RINT.<br>    DEQF16,       // DeQuant_Float16: Quantize int32_t to half, scalar quantization.<br>    VDEQF16,      // Vector_DeQuant_Float16: Quantize int32_t to half, tensor quantization.<br>    QF322B8_PRE,  // Quant_Float32_2_B8: Quantize float to int8_t/uint8_t, scalar quantization.<br>    VQF322B8_PRE, // Vector_Quant_Float32_2_B8: Quantize float to int8_t/uint8_t, tensor quantization.<br>    REQ8,         // ReQuant_int8: Quantize int32_t to int8_t/uint8_t, scalar quantization.<br>    VREQ8,        // Vector_ReQuant_int8: Quantize int32_t to int8_t/uint8_t, tensor quantization.<br>}; |
| **reluPre** | Configures the ReLU operation mode. The type is uint8_t. The values are as follows:<br>&nbsp;&nbsp;&bull; **0**: Disable ReLU.<br>&nbsp;&nbsp;&bull; **1**: Normal ReLU. |
| **channelSplit** | <!-- npu="950" id27 -->**This parameter takes effect only on the Ascend 950PR&950DT products**, and only for float type output in NZ format. The type is bool. It configures whether to enable the channel split function.<br>&nbsp;&nbsp;&bull; **false**: Disable.<br>&nbsp;&nbsp;&bull; **true**: Enable.<br><!-- end id27 --><!-- npu="A3,910b,310b" id29 -->For the following product models, this parameter does not take effect on this path. Set it to false.<br><!-- npu="A3" id30 -->Atlas A3 products<br><!-- end id30 --><!-- npu="910b" id31 -->Atlas A2 products<br><!-- end id31 --><!-- npu="310b" id32 -->Atlas 200I/500 A2 inference product<!-- end id32 --><!-- end id29 --> |
| **nz2ndEn** | <!-- npu="950" id28 -->**This parameter takes effect only on the Ascend 950PR&950DT products**. The type is bool. It configures whether to enable NZ2ND format conversion.<br>To enable the NZ2ND function, call [SetFixpipeNz2ndFlag](../cube_store_aux_config/SetFixpipeNz2ndFlag.md) to set the related configuration information for format conversion.<br>&nbsp;&nbsp;&bull; **false**: Disable.<br>&nbsp;&nbsp;&bull; **true**: Enable.<br><!-- end id28 --><!-- npu="A3,910b,310b" id33 -->For the following product models, this parameter does not take effect on this path. Set it to false.<br><!-- npu="A3" id34 -->Atlas A3 products<br><!-- end id34 --><!-- npu="910b" id35 -->Atlas A2 products<br><!-- end id35 --><!-- npu="310b" id36 -->Atlas 200I/500 A2 inference product<!-- end id36 --><!-- end id33 --> |

## Data Type<a id="zh-cn_topic_0000002511188540_section4219135304818"></a>

**Supported data type combinations for the source matrix and destination matrix**

<!-- npu="950" id17 -->
For the Ascend 950PR&950DT products, the data type combinations supported by the DataCopy API are as follows:

| source matrix (L0C Buffer) | destination matrix (L1 Buffer) |
| ---------- | ---------- |
| float | int8_t, uint8_t, half, bfloat16_t, float. |
| int32_t | int8_t, uint8_t, half, int32_t. |

<!-- end id17 -->

<!-- npu="A3" id18 -->
For the Atlas A3 products, the data type combinations supported by the DataCopy API are as follows:

| source matrix (L0C Buffer) | destination matrix (L1 Buffer) |
| ---------- | ---------- |
| float | int8_t, uint8_t, half, bfloat16_t. |
| int32_t | int8_t, uint8_t, half. |

<!-- end id18 -->

<!-- npu="910b" id19 -->
For the Atlas A2 products, the data type combinations supported by the DataCopy API are as follows:

| source matrix (L0C Buffer) | destination matrix (L1 Buffer) |
| ---------- | ---------- |
| float | int8_t, uint8_t, half, bfloat16_t. |
| int32_t | int8_t, uint8_t, half. |

<!-- end id19 -->

<!-- npu="310b" id20 -->
For the Atlas 200I/500 A2 inference product, the data type combinations supported by the DataCopy API are as follows:

| source matrix (L0C Buffer) | destination matrix (L1 Buffer) |
| ---------- | ---------- |
| float | int8_t, uint8_t, half, bfloat16_t. |
| int32_t | int8_t, uint8_t, half. |

<!-- end id20 -->

## Return Value

None

## Constraints<a id="zh-cn_topic_0000002511188540_section2045914466492"></a>

- The following describes the case where the quantized input is of the **float32** data type:
    - The standard IEEE 754 **float32** format consists of a 1-bit sign bit, 8-bit exponent bits, and 23-bit mantissa bits. The **float32** format supported by the current AI processor consists of a 1-bit sign bit, 8-bit exponent bits, and 10-bit mantissa bits.
    - If the user provides standard IEEE 754 **float32** input, the API internally converts it to the processor-supported **float32** format for computation. In this case, if standard IEEE 754 **float32** data is used during golden data generation, a precision mismatch may be introduced. The golden data generation must be corrected by clearing the lower 13 bits of the 23-bit mantissa of the quantization parameter before it participates in quantization computation.
- The source matrix in **NZ** format requires 64-byte address alignment, and the destination matrix in **NZ** format requires 32-byte address alignment.
- When either **mSize** or **nSize** to be moved out is 0, the instruction is not executed.
- The quantization and **ReLU** parameters cannot be inf/nan or denormalized numbers.
- The destination data must not overlap. If overlapping writes occur to the destination address, the hardware does not report any warning or error, nor does it guarantee the write order of the overlapping data.
- Enabling the **unitFlag** feature must be accompanied by enabling **Mmad**.
<!-- npu="A3,910b" id21 -->
- For the following product models, the special value/boundary value constraints are described as follows:
    <!-- npu="A3" id22 -->
    Atlas A3 products
    <!-- end id22 -->
    <!-- npu="910b" id23 -->
    Atlas A2 products
    <!-- end id23 -->
    For floating-point inf/nan input and output, the saturation mode during quantized data movement can be set through the CTRL\[48\] bit of the CTRL register (control register):

    - Non-saturation mode: CTRL\[48\] is set to 1'b1, and inf/nan keep the original output.
    - Saturation mode: CTRL\[48\] is set to 1'b0, inf output is saturated to ±MAX, and nan output is saturated to 0.

        ```cpp
        // Set CTRL[48] to 0 to enable the floating-point saturation mode.
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    For integer types, only the saturation mode is available.
<!-- end id21 -->
<!-- npu="950" id24 -->
- Special value/boundary value constraint description for Ascend 950PR&950DT products:

    For floating-point inf/nan input and output, the saturation mode during quantized data movement can be set through the CTRL\[48\] bit of the CTRL register (control register).

    For the fp8\_e4m3fn\_t type, more refined settings can be made through the CTRL\[48\] bit of the CTRL register (control register).

    - Non-saturation mode: CTRL\[48\] is set to 1'b1, and inf/nan remain as the original output. For the fp8\_e4m3fn\_t type, if the absolute value of the result is inf or greater than the maximum normalized value of fp8\_e4m3fn\_t, the output result is nan.
    - Saturation mode: CTRL\[48\] is set to 1'b0, the Inf output is saturated to ±MAX, and the NaN output is saturated to 0. For the fp8\_e4m3fn\_t type, when CTRL\[50\] is set to 1'b0, NaN is converted to zero; when CTRL\[50\] is set to 1'b1, NaN remains unchanged. If the absolute value of the result is Inf or greater than the maximum normalized value of fp8\_e4m3fn\_t, the result is the maximum normalized value.

        ```cpp
        // Set CTRL[48] to 0 to enable the floating-point saturation mode.
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    For integer types, only the saturation mode is available.
<!-- end id24 -->

<!-- npu="950,A3,910b" id25 -->
## Example<a id="zh-cn_topic_0000002511188540_section088124295117"></a>

Example: Path L0C Buffer -> L1 Buffer. The data type of input matrices A and B is half, and the output matrix C is int8_t. NZ2ND format conversion is disabled, and scalar quantization is enabled.

    ```cpp
    constexpr uint16_t c0Size = 32;
    AscendC::DataCopyCO12DstParams dataCopyParams;

    dataCopyParams.dstStride = baseM * c0Size * sizeof(outputType) / AscendC::ONE_BLK_SIZE;
    dataCopyParams.mSize = baseM;
    dataCopyParams.srcStride = CeilAlign(baseM, CUBE_BLOCK);
    dataCopyParams.nSize = baseN;
    dataCopyParams.quantPre = QuantMode_t::QF322B8_PRE;

    uint64_t deqScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&quantScalar));
    constexpr bool sign = (AscendC::IsSameType<outputType, int8_t>::value) ? true : false;
    deqScalar = (deqScalar & ~(static_cast<uint64_t>(1) << 46)) | (static_cast<uint64_t>(sign) << 46);
    AscendC::SetFixpipePreQuantFlag(deqScalar);
    AscendC::DataCopy(c1Local, c, dataCopyParams);
    ```
<!-- end id25 -->
