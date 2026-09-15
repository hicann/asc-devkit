# DataCopy (L0C to GM Data Movement)<a id="ZH-CN_TOPIC_0000002569070977"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-27T15:38:11.845Z -->

## Applicable Products<a id="zh-cn_topic_0000002542828493_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
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

## Description<a id="zh-cn_topic_0000002542828493_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

The result of cube computation is stored in the L0C Buffer. The **DataCopy** API is used to move the result to Global Memory (GM), and it supports operations such as inline format conversion during the movement.

<!-- npu="950,A3,910b" id8 -->
The following figure shows the valid combinations, intermediate data types, and data paths of inline quantization, inline ReLU, inline format conversion, inline channel splitting, and inline channel merging. In the figure, F32->F16 and F32->BF16 are non-quantization modes that perform only Cast, while the rest are inline scalar/tensor quantization modes.

**Figure 1** L0C2GM flowchart<a id="zh-cn_topic_0000002542828493_fig542810249417"></a>  

![](../../../../figures/L0C2GM_Function_Combination.png)
<!-- end id8 -->

## Prototype<a id="zh-cn_topic_0000002542828493_section82039854412"></a>

The DataCopy cube move-out API supports combinations of multiple inline capabilities. Different registers need to be set, and different data movement capabilities are enabled together with the data movement instruction. The corresponding APIs are as follows:

- Data movement API, with the path L0C Buffer (CO1)->GM, which implements quantization and ReLU activation by setting registers, and NZ-to-ND format conversion. The prototype is:

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyCO12DstParams& intriParams)
    ```

- [SetFixPipeConfig](../cube_store_aux_config/SetFixPipeConfig.md): Register setting API. By calling this API, Vector inline quantization is set, where each element of the tensor represents a quantization parameter. It needs to be set when tensor quantization is enabled.

- [SetFixpipePreQuantFlag](../cube_store_aux_config/SetFixpipePreQuantFlag.md): Register setting API. By calling this API, Scalar inline quantization parameters are set. This element represents the quantization parameter used by the entire output cube. It needs to be set when Scalar quantization is enabled.

- [SetFixpipeNz2ndFlag](../cube_store_aux_config/SetFixpipeNz2ndFlag.md): Register setting API. By calling this API, the inline NZ2ND format conversion configuration is set. It needs to be set when inline NZ2ND is used.

<!-- npu="310b" id12 -->
For the Atlas 200I/500 A2 inference product, the following two APIs are also supported:

- [SetFixPipeClipRelu](../cube_store_aux_config/SetFixPipeClipRelu.md): Register setting API. By calling this API, the maximum value of the ClipReLU operation is set.

- [SetFixPipeAddr](../cube_store_aux_config/SetFixPipeAddr.md): Register setting API. By calling this API, the address of the LocalTensor during the Elementwise operation is set.
<!-- end id12 -->

## Parameters<a id="zh-cn_topic_0000002542828493_section16128134420472"></a>

**Table 1** DataCopy template parameters for data movement

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the destination operand. For supported data types, refer to [Data Types](#zh-cn_topic_0000002542828493_section4219135304818). |
| U | Data type of the source operand. For supported data types, refer to [Data Types](#zh-cn_topic_0000002542828493_section4219135304818). |

**Table 2** DataCopy API parameters for data movement

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, whose type is **GlobalTensor**. The data format is NZ or ND. The ND address must satisfy 1-byte alignment, and the NZ address must satisfy 32-byte alignment. |
| src | Input | Source operand, whose type is **LocalTensor**. The supported physical address is the L0C Buffer (**TPosition** is **CO1**), which is the result calculated by the **Mmad** API. The data format is NZ, and the address must satisfy 64-byte alignment. |
| intriParams | Input | Movement parameters, whose type is [DataCopyCO12DstParams](#zh-cn_topic_0000002542828493_table35908519282). <br>For the specific definition, refer to \$\{INSTALL\_DIR\}/include/ascendc/basic\_api/API/kernel\_struct\_data_copy.h. Replace \$\{INSTALL\_DIR\} with the file storage path after the CANN software is installed. |

**Table 3** DataCopyCO12DstParams structure parameter definition<a id="zh-cn_topic_0000002542828493_table35908519282"></a>

| Parameter | Description |
| ---------- | ---------- |
| sid | This parameter does not need to be concerned by users. Set it to 0. |
| nSize | Size of the source NZ cube in the N direction. The value range is nSize∈[0, 4095], and nSize must be a multiple of 16.<br>&nbsp;&nbsp;&bull; For NZ output of the destination cube: when the output type is float and the [channelSplit](../cube_store_key_features/F32-Channel-Split.md) function is enabled, nSize must be a multiple of 8.<br>**Note: nSize=0 means that no movement is performed, and this API is treated as a NOP (no operation).** |
| mSize | Size of the source NZ cube in the M direction.<br>&nbsp;&nbsp;&bull; When the inline NZ2ND function is disabled ([NZ2NZ movement](../cube_store_key_features/NZ2NZ.md)): the value range is mSize∈[0, 65535].<br>&nbsp;&nbsp;&bull; When the inline [NZ2ND](../cube_store_key_features/NZ2ND.md) function is enabled: the value range is mSize∈[0, 8192].<br>**Note: mSize=0 means that no movement is performed, and this API is treated as a NOP (no operation).** |
| dstStride | &bull; When the NZ2ND function is disabled ([NZ2NZ movement](../cube_store_key_features/NZ2NZ.md)): start address offset of adjacent Z layouts in the destination NZ cube. The value must not be 0, and the unit is datablock (32 bytes).<br>&bull; When the inline [NZ2ND](../cube_store_key_features/NZ2ND.md) function is enabled: number of elements in each row of the destination ND cube. The value must not be 0, and the unit is element. |
| srcStride | Start address offset of adjacent Z layouts in the source NZ cube. The value range is srcStride∈[0, 65535], and the unit is C0_Size (16*sizeof(T)), where T is the data type of src. Its value should be set to mSize rounded up to a multiple of 16. |
| unitFlag | unitFlag is a fine-grained parallelism between the **Mmad** instruction and the **Fixpipe** instruction. When this function is enabled, the calculation result is moved out each time the hardware finishes calculating one fractal. The values are as follows:<br>&nbsp;&nbsp;&bull; 0 (2'b00): disable unitFlag.<br>&nbsp;&nbsp;&bull; 2 (2'b10): enable unitFlag. After the hardware finishes executing the instruction, the unit flag bit is not reset.<br>&nbsp;&nbsp;&bull; 3 (2'b11): enable unitFlag. After the hardware finishes executing the instruction, the unit flag bit is reset.<br>When this function is enabled, the unitFlag value of the **Mmad** instruction and the **Fixpipe** instruction must be set to 2 or 3.<br>For the parameter setting scheme and feature details, refer to [UnitFlag](../mmad_compute_key_features/UnitFlag.md) in the key feature description of Mmad calculation. |
| clipReluPre | <!-- npu="950,A3,910b" id13 -->This parameter is supported only on the Atlas 200I/500 A2 inference product.<br><!-- end id13 --><!-- npu="310b" id14 -->Used to configure whether to enable the ClipReLU operation. The parameter type is uint8_t, and the values are as follows: 0, disable ClipReLU; 1, enable ClipReLU. In this case, call [SetFixPipeClipRelu](../cube_store_aux_config/SetFixPipeClipRelu.md) to set the maximum value of ClipReLU.<br>&nbsp;&nbsp;&bull; This operation is performed after inline quantization and can be used only after quantPre is configured. The currently supported quantization modes are F322F16/DEQF16/VDEQF16/QF322B8_PRE/VQF322B8_PRE/REQ8/VREQ8.<br><!-- end id14 --> |
| eltWiseOp | <!-- npu="950,A3,910b" id15 -->This parameter is supported only on the Atlas 200I/500 A2 inference product.<br><!-- end id15 --><!-- npu="310b" id16 -->Used to configure whether to enable the Elementwise operation and the operation mode. The Elementwise operation means that after inline quantization, a **LocalTensor** can be added to or subtracted from element by element, with a size of mSize * nSize. For the specific **LocalTensor** address-related parameters, call [SetFixPipeAddr](../cube_store_aux_config/SetFixPipeAddr.md) to set them.<br>The eltWiseOp parameter type is uint8_t, and the values are as follows:<br>&nbsp;&nbsp;&bull; 0: disable Elementwise;<br>&nbsp;&nbsp;&bull; 1: Elementwise Addition;<br>&nbsp;&nbsp;&bull; 2: Elementwise Subtraction.<br><!-- end id16 --> |
| quantPre | Used to control the quantization mode. The type is QuantMode_t, and the specific definition is as follows:<br>&nbsp;&nbsp;&bull; For float/int32_t output, this parameter must be configured as QuantMode_t::NoQuant.<br>&nbsp;&nbsp;&bull; For half/bfloat16_t output, this parameter must be configured as QuantMode_t::F322F16/QuantMode_t::F322BF16.<br>&nbsp;&nbsp;&bull; When configured as scalar quantization, call the [SetFixpipePreQuantFlag](../cube_store_aux_config/SetFixpipePreQuantFlag.md) API to set the scalar quantization parameters.<br>&nbsp;&nbsp;&bull; When configured as tensor quantization, call [SetFixPipeConfig](../cube_store_aux_config/SetFixPipeConfig.md) to set the tensor quantization parameters, where the tensor quantization parameters need to be moved from the L1 Buffer to the Fixpipe Buffer through DataCopy.<br>Note: This parameter needs to be configured manually by users and is not automatically derived to configure the corresponding quantization mode.<br>enum QuantMode_t<br>{<br>    NoQuant,      // Disable the quantization function.<br>    F322F16,      // Float32_2_Float16: cast float to half, with the cast mode being CAST_RINT.<br>    F322BF16,     // Float32_2_BFloat16: cast float to bfloat16_t, with the cast mode being CAST_RINT.<br>    DEQF16,       // DeQuant_Float16: quantize int32_t to half, scalar quantization.<br>    VDEQF16,      // Vector_DeQuant_Float16: quantize int32_t to half, tensor quantization.<br>    QF322B8_PRE,  // Quant_Float32_2_B8: quantize float to int8_t/uint8_t, scalar quantization.<br>    VQF322B8_PRE, // Vector_Quant_Float32_2_B8: quantize float to int8_t/uint8_t, tensor quantization.<br>    REQ8,         // ReQuant_int8: quantize int32_t to int8_t/uint8_t, scalar quantization.<br>    VREQ8,        // Vector_ReQuant_int8: quantize int32_t to int8_t/uint8_t, tensor quantization.<br>};<br>|
| reluPre | Used to configure the mode of the ReLU operation. The type is uint8_t, and the values are as follows:<br>&nbsp;&nbsp;&bull; 0: disable ReLU;<br>&nbsp;&nbsp;&bull; 1: Normal ReLU. |
| channelSplit | The type is bool. Configures whether to enable the channel split function, which takes effect only for float output in NZ format.<br>&nbsp;&nbsp;&bull; false: disable;<br>&nbsp;&nbsp;&bull; true: enable. |
| nz2ndEn | The type is bool. Configures whether to enable the NZ2ND format conversion.<br>To enable the NZ2ND function, call [SetFixpipeNz2ndFlag](../cube_store_aux_config/SetFixpipeNz2ndFlag.md) to set the related configuration information of the format conversion.<br>&nbsp;&nbsp;&bull; false: disable;<br>&nbsp;&nbsp;&bull; true: enable. |

## Data Type<a id="zh-cn_topic_0000002542828493_section4219135304818"></a>

**Supported data type combinations for the source cube and destination cube**

| Source Cube (L0C Buffer) | Destination Cube (GM) |
| ---------- | ---------- |
| float | int8_t, uint8_t, half, bfloat16_t, float. |
| int32_t | int8_t, uint8_t, half, int32_t. |

## Return Value

None

## Constraints<a id="zh-cn_topic_0000002542828493_section2045914466492"></a>

- The description of the float32 data type for quantization input is as follows:
    - The standard IEEE 754 float32 format is: 1 sign bit, 8 exponent bits, and 23 mantissa bits. The float32 format supported by the current AI processor is: 1 sign bit, 8 exponent bits, and 10 mantissa bits.
    - If the user provides standard IEEE 754 float32 input, the API internally processes it into the float32 format supported by the processor for computation. In this case, if standard IEEE 754 float32 data is used during golden data generation, a precision mismatch may be introduced. The golden data generation must be corrected by clearing the lower 13 bits of the 23 mantissa bits of the quantization parameter before it participates in quantization computation.
- The source cube NZ format address requires 64-byte alignment. The destination cube ND format address requires 1-byte alignment, and the NZ format address requires 32-byte alignment.
- When either mSize or nSize to be moved out is 0, this instruction is not executed.
- The quantization and ReLU parameters cannot be Inf/NaN or denormalized numbers.
- The destination data must not overlap. If overlapping writes occur to the destination address, the hardware does not report any warning or error, nor does it guarantee the write order of the overlapping data.
- Enabling the unitFlag feature must be accompanied by enabling Mmad.
<!-- npu="A3,910b" id17 -->
- For the following product models, the constraints on special values/boundary values are described as follows:
    <!-- npu="A3" id18 -->
    Atlas A3 training products/Atlas A3 inference products
    <!-- end id18 -->
    <!-- npu="910b" id19 -->
    Atlas A2 training products/Atlas A2 inference products
    <!-- end id19 -->
    For floating-point Inf/NaN input and output, the CTRL\[48\] bit of the CTRL register (control register) can be used to configure the saturation mode during floating-point quantization move-out:

    - Non-saturation mode: CTRL\[48\] is set to 1'b1, and Inf/NaN are output as-is.
    - Saturation mode: CTRL\[48\] is set to 1'b0, the Inf output will be saturated to ±MAX, and the NaN output will be saturated to 0.

        ```cpp
        // Set CTRL[48] to 0 to enable the floating-point saturation mode.
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    For integer types, only the saturation mode is available.
<!-- end id17 -->
<!-- npu="950" id20 -->
- Ascend 950PR/Ascend 950DT special value/boundary value constraints:

    For floating-point Inf/NaN input and output, the CTRL\[48\] bit of the CTRL register (control register) can be used to configure the saturation mode during floating-point quantization move-out;

    For the fp8\_e4m3fn\_t type, the CTRL\[48\] bit of the CTRL register (control register) can be used for finer-grained configuration;

    - Non-saturation mode: CTRL\[48\] is set to 1'b1, and Inf/NaN are output as-is. For the fp8\_e4m3fn\_t type, if the absolute value of the result is Inf or greater than the maximum normalized value of fp8\_e4m3fn\_t, the output result is NaN.
    - Saturation mode: CTRL\[48\] is set to 1'b0, the Inf output will be saturated to ±MAX, and the NaN output will be saturated to 0. For the fp8\_e4m3fn\_t type, when CTRL\[50\] is set to 1'b0, NaN is converted to zero; when CTRL\[50\] is set to 1'b1, NaN remains unchanged. If the absolute value of the result is Inf or greater than the maximum normalized value of fp8\_e4m3fn\_t, the result is the maximum normalized value.

        ```cpp
        // Set CTRL[48] to 0 to enable the floating-point saturation mode.
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    For integer types, only the saturation mode is available.
<!-- end id20 -->

## Example<a id="zh-cn_topic_0000002542828493_section088124295117"></a>

<!-- npu="950,A3,910b" id21 -->
- Example 1: Mmad contains cube multiply bias. The data type of the left cube and the right cube is int8\_t, and the data type of the result cube is int32\_t. The quantization mode is DEQF16, and the Scalar quantization parameter is 2.0. The result calculated by Mmad is quantized from int32\_t to half and moved out. For the complete DataCopy sample, refer to [data_copy_l0c2gm](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_l0c2gm).

    ```cpp
    // Scalar quantization, with the quantization parameter set to 2.0.
    float quantScalar = 2.0;
    uint64_t deqScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&quantScalar));
    // Write the scalar of the quantization parameter to the register for use by subsequent DataCopy instructions.
    AscendC::SetFixpipePreQuantFlag(deqScalar);
    // Create the DataCopy parameters.
    AscendC::DataCopyCO12DstParams intriParams;
    intriParams.nSize = n;
    intriParams.mSize = m;
    intriParams.srcStride = CeilAlign(m, CUBE_BLOCK);
    intriParams.dstStride = n;
    intriParams.quantPre = QuantMode_t::DEQF16;
    intriParams.reluPre = 1; // Enable ReLU.
    intriParams.nz2ndEn = true; // Enable NZ2ND format conversion.
    // Perform the final data movement according to the parameters in intriParams.
    AscendC::DataCopy(cGM, cLocal, intriParams);
    ```

- Example 2: Mmad contains cube multiply bias. The data type of the left cube and the right cube is int8\_t, and the data type of the result cube is int32\_t. The quantization mode is VDEQF16, and Tensor quantization is used. The result calculated by Mmad is quantized from int32\_t to half and moved out. For the complete DataCopy sample, refer to [data_copy_l0c2gm](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_l0c2gm).

    ```cpp
    // CeilAlign is defined as follows.
    __aicore__ inline uint16_t CeilAlign(uint16_t numerator, uint16_t denominator) 
    {
        return (numerator + denominator - 1) / denominator * denominator;
    }
    // Copy the quantized data (quantAlphaGM) from GM to C1 (quantAlphaTensor).
    uint16_t burstLen = CeilAlign(n * sizeof(uint64_t), 128) / AscendC::ONE_BLK_SIZE;
    AscendC::DataCopyParams intriParams{ 1, burstLen, 0, 0 };
    AscendC::DataCopy(quantAlphaTensor, quantAlphaGM, intriParams);
    // Set synchronization to ensure that subsequent DataCopy instructions are executed after the quantized data is copied to C1.
    AscendC::SetFlag<AscendC::HardEvent::MTE2_FIX>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_FIX>(EVENT_ID0);
    // Copy the quantized data (quantAlphaTensor) from C1 to C2PIPE2GM (fbTensor).
    uint16_t fbufBurstLen = CeilAlign(deqDataSize, 128) / 128;
    AscendC::DataCopyParams dataCopyParams(1, fbufBurstLen, 0, 0);
    AscendC::DataCopy(fbTensor, quantAlphaTensor, dataCopyParams);
    // Write the quantization parameter data to registers for use by subsequent DataCopy instructions.
    AscendC::SetFixPipeConfig(fbTensor);
    // Create the DataCopy parameters.
    AscendC::DataCopyCO12DstParams intriParams;
    intriParams.nSize = CeilAlign(n, CUBE_BLOCK);
    intriParams.mSize = m;
    intriParams.srcStride = CeilAlign(m, CUBE_BLOCK);
    intriParams.dstStride = m * C0_SIZE / AscendC::ONE_BLK_SIZE; // C0_SIZE = 32
    intriParams.quantPre = QuantMode_t::VDEQF16;
    intriParams.reluPre = 1; // Enable ReLU.
    // Perform the final data movement based on the parameters in intriParams.
    AscendC::DataCopy(cGM, cLocal, intriParams);
    ```
<!-- end id21 -->
<!-- npu="310b" id22 -->
- Example 3: Mmad contains cube multiply bias, the data type of the left cube and right cube is int8\_t, and the data type of the result cube is int32\_t. The quantization mode is DEQF16, the scalar quantization parameter is 0.5, and the result calculated by Mmad is quantized from int32\_t to half and moved out. (This example applies only to Atlas 200I/500 A2 inference products.)

    ```cpp
    #ifdef ASCENDC_CPU_DEBUG
    #include "tikicpulib.h"
    #endif
    #include "kernel_operator.h"
    #include "../../instrs/common_utils/register_utils.h"
    template <typename dst_T, typename fmap_T, typename weight_T, typename dstCO1_T> class KernelCubeDataCopy{
    public:
        __aicore__ inline KernelCubeDataCopy(uint16_t CoutIn, uint8_t dilationHIn, uint8_t dilationWIn, QuantMode_t deqModeIn)
        {
            // ceiling of 16
            Cout = CoutIn;
            dilationH = dilationHIn;
            dilationW = dilationWIn;
            C0 = 32 / sizeof(fmap_T);
            C1 = channelSize / C0;
            coutBlocks = (Cout + 16 - 1) / 16;
            ho = H - dilationH * (Kh - 1);
            wo = W - dilationW * (Kw - 1);
            howo = ho * wo;
            howoRound = ((howo + 16 - 1) / 16) * 16;
            featureMapA1Size = C1 * H * W * C0;      // shape: [C1, H, W, C0]
            weightA1Size = C1 * Kh * Kw * Cout * C0; // shape: [C1, Kh, Kw, Cout, C0]
            featureMapA2Size = howoRound * (C1 * Kh * Kw * C0);
            weightB2Size = (C1 * Kh * Kw * C0) * coutBlocks * 16;
            m = howo;
            k = C1 * Kh * Kw * C0;
            n = Cout;
            biasSize = Cout;                  // shape: [Cout]
            dstSize = coutBlocks * howo * 16; // shape: [coutBlocks, howo, 16]
            dstCO1Size = coutBlocks * howoRound * 16;
            fmRepeat = featureMapA2Size / (16 * C0);
            weRepeat = weightB2Size / (16 * C0);
            deqMode = deqModeIn;
        }
        __aicore__ inline void Init(__gm__ uint8_t* fmGm, __gm__ uint8_t* weGm, __gm__ uint8_t* biasGm, __gm__ uint8_t* deqGm, __gm__ uint8_t* eleWiseGm, __gm__ uint8_t* dstGm)
        {
            fmGlobal.SetGlobalBuffer((__gm__ fmap_T*)fmGm);
            weGlobal.SetGlobalBuffer((__gm__ weight_T*)weGm);
            biasGlobal.SetGlobalBuffer((__gm__ dstCO1_T*)biasGm);
            deqGlobal.SetGlobalBuffer((__gm__ uint64_t*)deqGm);
            dstGlobal.SetGlobalBuffer((__gm__ dst_T*)dstGm);
            eleWiseGlobal.SetGlobalBuffer((__gm__ half*)eleWiseGm);
            pipe.InitBuffer(inQueueFmA1, 1, featureMapA1Size * sizeof(fmap_T));
            pipe.InitBuffer(inQueueFmA2, 1, featureMapA2Size * sizeof(fmap_T));
            pipe.InitBuffer(inQueueWeB1, 1, weightA1Size * sizeof(weight_T));
            pipe.InitBuffer(inQueueWeB2, 1, weightB2Size * sizeof(weight_T));
            pipe.InitBuffer(inQueueBiasA1, 1, biasSize * sizeof(dstCO1_T));
            pipe.InitBuffer(inQueueDeqA1, 1, dstCO1Size * sizeof(uint64_t));
            pipe.InitBuffer(inQueueDeqFB, 1, dstCO1Size * sizeof(uint64_t));
            pipe.InitBuffer(outQueueCO1, 1, dstCO1Size * sizeof(dstCO1_T));
            pipe.InitBuffer(inQueueC1, 1, dstSize * sizeof(half));
        }
        __aicore__ inline void Process()
        {
            CopyIn();
            Split();
            Compute();
            CopyOut();
        }
    private:
        __aicore__ inline void CopyIn()
        {
            AscendC::LocalTensor<fmap_T> featureMapA1 = inQueueFmA1.AllocTensor<fmap_T>();
            AscendC::LocalTensor<weight_T> weightB1 = inQueueWeB1.AllocTensor<weight_T>();
            AscendC::LocalTensor<dstCO1_T> biasA1 = inQueueBiasA1.AllocTensor<dstCO1_T>();
            AscendC::DataCopy(featureMapA1, fmGlobal, { 1, static_cast<uint16_t>(featureMapA1Size * sizeof(fmap_T) / 32), 0, 0 });
            AscendC::DataCopy(weightB1, weGlobal, { 1, static_cast<uint16_t>(weightA1Size * sizeof(weight_T) / 32), 0, 0 });
            AscendC::DataCopy(biasA1, biasGlobal, { 1, static_cast<uint16_t>(biasSize * sizeof(dstCO1_T) / 32), 0, 0 });
            inQueueFmA1.EnQue(featureMapA1);
            inQueueWeB1.EnQue(weightB1);
            inQueueBiasA1.EnQue(biasA1);
        }
        __aicore__ inline void Split()
        {
            AscendC::LocalTensor<fmap_T> featureMapA1 = inQueueFmA1.DeQue<fmap_T>();
            AscendC::LocalTensor<weight_T> weightB1 = inQueueWeB1.DeQue<weight_T>();
            AscendC::LocalTensor<fmap_T> featureMapA2 = inQueueFmA2.AllocTensor<fmap_T>();
            AscendC::LocalTensor<weight_T> weightB2 = inQueueWeB2.AllocTensor<weight_T>();
            uint8_t padList[] = {0, 0, 0, 0};
            // load3dv2
            AscendC::LoadData(featureMapA2, featureMapA1, { padList, H, W, channelSize, k, howoRound, 0, 0, 1, 1, Kw, Kh, dilationW, dilationH, false, false, 0 });
            // load2d
            AscendC::LoadData(weightB2, weightB1, { 0, weRepeat, 1, 0, 0, false, 0 });
            inQueueFmA2.EnQue<fmap_T>(featureMapA2);
            inQueueWeB2.EnQue<weight_T>(weightB2);
            inQueueFmA1.FreeTensor(featureMapA1);
            inQueueWeB1.FreeTensor(weightB1);
        }
        __aicore__ inline void Compute()
        {
            AscendC::LocalTensor<fmap_T> featureMapA2 = inQueueFmA2.DeQue<fmap_T>();
            AscendC::LocalTensor<weight_T> weightB2 = inQueueWeB2.DeQue<weight_T>();
            AscendC::LocalTensor<dstCO1_T> dstCO1 = outQueueCO1.AllocTensor<dstCO1_T>();
            AscendC::LocalTensor<dstCO1_T> biasA1 = inQueueBiasA1.DeQue<dstCO1_T>();
            // C = A * B + bias
            // m: left cube Height, k: left cube Width, n: right cube Width.
            AscendC::Mmad(dstCO1, featureMapA2, weightB2, biasA1, { m, n, k, true, 0, false, false, false });
            outQueueCO1.EnQue<dstCO1_T>(dstCO1);
            inQueueFmA2.FreeTensor(featureMapA2);
            inQueueWeB2.FreeTensor(weightB2);
        }
        __aicore__ inline void CopyOut()
        {
            AscendC::LocalTensor<dstCO1_T> dstCO1 = outQueueCO1.DeQue<dstCO1_T>();
            // Enable DEQF16 quantization and set the quantization parameter to 0.5.
            float tmp = (float)0.5;
            // Convert the float tmp to uint64_t deqScalar.
            uint64_t deqScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&tmp));
            bool nz2ndEn = false;
            // When nz2nd is disabled, nSize must be a multiple of 16.
            uint16_t nSize = coutBlocks * 16;
            uint16_t mSize = m;
            // srcStride must be a multiple of 16.
            uint16_t srcStride = (m + 16 - 1) / 16 * 16;
            // When nz2nd is disabled, dstStride is the burst head-to-head distance and is 32-byte aligned.
            uint32_t dstStride = m * sizeof(dst_T) * 16 / 32;
            if (nz2ndEn) {
                // The number of nd matrices is 1, so fill 1 into src_nd_stride and dst_nd_stride.
                AscendC::SetFixpipeNz2ndFlag(1, 1, 1);
                // When nz2nd is enabled, nSize does not need to be a multiple of 16 and stays consistent with n of Mmad.
                nSize = n;
                // When nz2nd is enabled, dstStride indicates the interval between adjacent consecutive rows of the same nd cube and stays consistent with n.
                dstStride = nSize;
            };
            // Disable ReLU and channelSplit.
            AscendC::DataCopyCO12DstParams intriParams(nSize, mSize, dstStride, srcStride, deqMode, 0, false, nz2ndEn);
           
            // mov l0c to gm, deq scalar quant
            AscendC::SetFixpipePreQuantFlag(deqScalar);  // Set the quantization parameter.
            AscendC::PipeBarrier<PIPE_FIX>();
            AscendC::DataCopy(dstGlobal, dstCO1, intriParams);
            // // mov l0c to gm, deq tensor quant
            // // Need to additionally allocate GM space for the deq tensor and move the value to workA1.
            // AscendC::LocalTensor<uint64_t> workA1 = inQueueDeqA1.AllocTensor<uint64_t>();
            // // Size of the deq tensor.
            // uint16_t deqSize = 128;
            // AscendC::DataCopy(workA1, deqGlobal, deqSize);
            // // Address of the deq tensor on fix.
            // AscendC::LocalTensor<uint64_t> deqFB = inQueueDeqFB.AllocTensor<uint64_t>();
            // // l1->fix, burst_len unit is 128Bytes
            // uint16_t fbufBurstLen = deqSize / 128;
            // AscendC::DataCopyParams dataCopyParams(1, fbufBurstLen, 0, 0);
            // AscendC::DataCopy(deqFB, workA1, dataCopyParams);
            // // Set the quantization tensor.
            // AscendC::SetFixPipeConfig(deqFB);
            // AscendC::PipeBarrier<PIPE_FIX>();
            // // Move l0c to gm, and enable the ClipReLU operation after quantization.
            // intriParams.clipReluPre = 1; 
            // // Set the ClipReLU value to the register.
            // uint64_t clipReluVal = 0x3c00; // value 1, half
            // SetFixPipeClipRelu(clipReluVal);
            // //Move l0c to gm, and set the element-wise operation to Add after quantization.
            // intriParams.eltWiseOp = 1;
            // // Apply for additional gm space for the element-wise tensor, and move the value to eleWiseTensor.
            // AscendC::LocalTensor<half> eleWiseTensor = inQueueC1.AllocTensor<half>();
            // DataCopy(eleWiseTensor, eleWiseGlobal, { 1, static_cast<uint16_t>(sizeof(half) * dstSize / 32), 0, 0 });
            // AscendC::PipeBarrier<PIPE_ALL>();
            // // Set the address that stores the element-wise tensor to the register.
            // SetFixPipeAddr(eleWiseTensor, 1);
    
            // AscendC::DataCopy(dstGlobal, dstCO1, intriParams);
            // inQueueDeqA1.FreeTensor(workA1);
            // inQueueDeqFB.FreeTensor(deqFB);
            // outQueueCO1.FreeTensor(dstCO1);
            // inQueueC1.FreeTensor(eleWiseTensor);
         }
    private:
        AscendC::TPipe pipe;
        // feature map queue
        AscendC::TQue<AscendC::TPosition::A1, 1> inQueueFmA1;
        AscendC::TQue<AscendC::TPosition::A2, 1> inQueueFmA2;
        // weight queue
        AscendC::TQue<AscendC::TPosition::B1, 1> inQueueWeB1;
        AscendC::TQue<AscendC::TPosition::B2, 1> inQueueWeB2;
        // bias queue
        AscendC::TQue<AscendC::TPosition::A1, 1> inQueueBiasA1;
        // deq tensor queue
        AscendC::TQue<AscendC::TPosition::A1, 1> inQueueDeqA1;
        // fb dst of deq tensor
        AscendC::TQue<AscendC::TPosition::C2PIPE2GM, 1> inQueueDeqFB;
        // dst queue
        AscendC::TQue<AscendC::TPosition::CO1, 1> outQueueCO1;
        // element-wise tensor
        AscendC::TQue<AscendC::TPosition::C1, 1> inQueueC1;
        AscendC::GlobalTensor<fmap_T> fmGlobal;
        AscendC::GlobalTensor<weight_T> weGlobal;
        AscendC::GlobalTensor<dst_T> dstGlobal;
        AscendC::GlobalTensor<uint64_t> deqGlobal;
        AscendC::GlobalTensor<dstCO1_T> biasGlobal;
        AscendC::GlobalTensor<half> eleWiseGlobal;
        uint16_t channelSize = 32;
        uint16_t H = 4, W = 4;
        uint8_t Kh = 2, Kw = 2;
        uint16_t Cout;
        uint16_t C0, C1;
        uint8_t dilationH, dilationW;
        uint16_t coutBlocks, ho, wo, howo, howoRound;
        uint32_t featureMapA1Size, weightA1Size, featureMapA2Size, weightB2Size, biasSize, dstSize, dstCO1Size;
        uint16_t m, k, n;
        uint8_t fmRepeat, weRepeat;
        QuantMode_t deqMode = QuantMode_t::NoQuant;
    };
    #define KERNEL_CUBE_DATACOPY(dst_type, fmap_type, weight_type, dstCO1_type, CoutIn, dilationHIn, dilationWIn, deqModeIn)  \
        extern "C" __global__ __aicore__ void cube_datacopy_kernel_##fmap_type(__gm__ uint8_t* fmGm, __gm__ uint8_t* weGm,    \
            __gm__ uint8_t* biasGm, __gm__ uint8_t* deqGm, __gm__ uint8_t* eleWiseGm, __gm__ uint8_t* dstGm)                                             \
        {                                                                                                                     \
            if (g_coreType == AscendC::AIV) {                                                                                 \
                return;                                                                                                       \
            }                                                                                                                 \
            KernelCubeDataCopy<dst_type, fmap_type, weight_type, dstCO1_type> op(CoutIn, dilationHIn, dilationWIn,            \
                deqModeIn);                                                                                                   \
            op.Init(fmGm, weGm, biasGm, deqGm, eleWiseGm, dstGm);                                                                        \
            op.Process();                                                                                                     \
        }
    KERNEL_CUBE_DATACOPY(half, int8_t, int8_t, int32_t, 128, 1, 1, QuantMode_t::DEQF16);
    ```
<!-- end id22 -->
