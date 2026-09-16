# SetFixPipeClipRelu<a name="ZH-CN_TOPIC_0000002054401800"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:08:48.097Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
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
## Description<a name="section618mcpsimp"></a>

After in-line quantization is performed during DataCopy data movement ([L0C Buffer->GM](../cube_compute_store/DataCopy_L0CToGM.md)), call this API to set the maximum value for the ClipReLU operation.

The ClipReLU calculation formula is `min(clipReluMaxVal, srcData)`, where `clipReluMaxVal` is the maximum value set through this API and `srcData` is the source data.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetFixPipeClipRelu(uint64_t config)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| config | Input | **clipReluMaxVal**, the maximum value in the ClipReLU operation. **clipReluMaxVal** occupies only bits 0 to 15, must be greater than 0, and cannot be Inf/NaN. |

## Data Types

The supported data type is **uint64_t**.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

When ReLU is enabled, the ReLU operation is performed first, followed by ClipReLU.

## Example<a name="section6461234123118"></a>

```cpp
uint64_t clipReluMaxVal = 0x3c00;
SetFixPipeClipRelu(clipReluMaxVal); // When ReLU is enabled, the ReLU operation is performed first, followed by ClipReLU. clipReluMaxVal is the maximum value set through this API.
```

- For Atlas 200I/500 A2 inference products, the complete example is as follows. Data is moved with in-path format conversion, and the path is L0C Buffer-\>GM.

    Example: Mmad contains a cube multiplication bias. The data type of the left cube and the right cube is int8\_t, and the data type of the result cube is int32\_t. The quantization mode is DEQF16, and the scalar quantization parameter is 0.5. The result computed by Mmad is quantized from int32\_t to half and moved out.

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
            // m: height of the left cube; k: width of the left cube; n: width of the right cube.
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
            // When nz2nd is disabled, dstStride is the distance from burst head to head and is 32-byte aligned.
            uint32_t dstStride = m * sizeof(dst_T) * 16 / 32;
            if (nz2ndEn) {
                // The number of nd matrices is 1, so src_nd_stride and dst_nd_stride are set to 1.
                AscendC::SetFixpipeNz2ndFlag(1, 1, 1);
                // When nz2nd is enabled, nSize does not need to be a multiple of 16 and stays consistent with n of Mmad.
                nSize = n;
                // When nz2nd is enabled, dstStride indicates the interval between adjacent consecutive rows of the same nd cube and stays consistent with n.
                dstStride = nSize;
            };
            // ReLU and channelSplit are not enabled.
            AscendC::DataCopyCO12DstParams intriParams(nSize, mSize, dstStride, srcStride, deqMode, 0, false, nz2ndEn);
           
            // mov l0c to gm, deq scalar quant
            AscendC::SetFixpipePreQuantFlag(deqScalar);  // Set the quantization parameters.
            AscendC::PipeBarrier<PIPE_FIX>();
            AscendC::DataCopy(dstGlobal, dstCO1, intriParams);
            // // mov l0c to gm, deq tensor quant
            // // Additionally allocate GM space for the deq tensor and move the value to workA1.
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
            // //Move l0c to gm. After the quantization operation, set the element-wise operation to Add.
            // intriParams.eltWiseOp = 1;
            // // Apply for additional GM space for the element-wise tensor and move the value to eleWiseTensor.
            // AscendC::LocalTensor<half> eleWiseTensor = inQueueC1.AllocTensor<half>();
            // DataCopy(eleWiseTensor, eleWiseGlobal, { 1, static_cast<uint16_t>(sizeof(half) * dst_size / 32), 0, 0 });
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
