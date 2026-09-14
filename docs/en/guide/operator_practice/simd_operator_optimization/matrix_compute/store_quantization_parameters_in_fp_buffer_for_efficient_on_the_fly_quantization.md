# Efficient Inline Quantization by Storing Quantization Parameters in FP-Buffer<a name="ZH-CN_TOPIC_0000001947984233"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:14:53.674Z -->

[Priority] High

[Description] When performing quantization on the matrix multiplication result in an operator implementation, you can move the quantization parameters to C2PIPE2GM (Fixpipe Buffer) and call the Fixpipe API once to implement quantization of the matrix multiplication result. Compared with the process of moving the matrix multiplication result from CO1 (L0C) to GM, then from GM to UB, and performing quantization on UB, this approach involves fewer data movement operations and higher memory usage efficiency.

>[!NOTE] Note
>This performance optimization method takes effect only for Atlas A2 Training Series products/Atlas A2 Inference Series products.

**Figure 1**  Negative example data flow<a name="fig1775910202351"></a>  
![](../../../figures/negative_sample_data_flow_diagram_78.png "negative-example-data-flow-78")

**Figure 2**  Positive example data flow<a name="fig14909105811118"></a>  
![](../../../figures/positive_sample_data_flow_diagram_79.png "positive-example-data-flow-79")

[Negative Example]

The process of performing quantization on the matrix multiplication result is as follows:

-   Move the matrix multiplication result from CO1 to the workspace;
-   Then move it from the workspace to UB;
-   Move the quantization parameters to the UB and perform a series of quantization calculations on the UB together with the matrix multiplication result;
-   Move the final quantization result from the UB to the GM.

Compared with the positive example, this adds the CO1->workspace and workspace->UB data movement processes and the vector calculation for quantization.

```
...
// This sample is for illustration only and is not complete code. Some synchronization control code is omitted.
public:
    __aicore__ inline KernelSample()
    {
        aSize = m * k;
        bSize = k * n;
        cSize = m * n;
    }
    __aicore__ inline void Init(__gm__ uint8_t *a, __gm__ uint8_t *b, __gm__ uint8_t *c, __gm__ uint8_t *deqTensor)
    {
        aGM.SetGlobalBuffer((__gm__ half *)a);
        bGM.SetGlobalBuffer((__gm__ half *)b);
        cGM.SetGlobalBuffer((__gm__ float *)c);
        deqGM.SetGlobalBuffer((__gm__ half *)deqTensor);
        pipe.InitBuffer(inQueueA1, 1, aSize * sizeof(half));
        pipe.InitBuffer(inQueueA2, 1, aSize * sizeof(half));
        pipe.InitBuffer(inQueueB1, 1, bSize * sizeof(half));
        pipe.InitBuffer(inQueueB2, 2, bSize * sizeof(half));
        pipe.InitBuffer(outQueueCO1, 1, cSize * sizeof(float));
        pipe.InitBuffer(inQueueSrc0, 1, cSize * sizeof(float));
        pipe.InitBuffer(inQueueTmp, 1, cSize * sizeof(half));
        pipe.InitBuffer(inQueueDeq, 1, cSize * sizeof(half));
        pipe.InitBuffer(outQueueDst, 1, cSize * sizeof(int8_t));
    }
    __aicore__ inline void Process()
    {
        CopyIn();
        SplitA();
        SplitB();
        Compute();
        CopyOut();
        CopyIn1();
        Compute1();
        CopyOut1();
    }
private:
    __aicore__ inline void CopyIn()
    {
        LocalTensor<half> a1Local = inQueueA1.AllocTensor<half>();
        LocalTensor<half> b1Local = inQueueB1.AllocTensor<half>();
        LocalTensor<half> deqLocal = inQueueDeq.AllocTensor<half>();

        Nd2NzParams dataCopyA1Params;
        dataCopyA1Params.ndNum = 1;
        dataCopyA1Params.nValue = m;
        dataCopyA1Params.dValue = k;
        dataCopyA1Params.srcNdMatrixStride = 0;
        dataCopyA1Params.srcDValue = k;
        dataCopyA1Params.dstNzC0Stride = m;
        dataCopyA1Params.dstNzNStride = 1;
        dataCopyA1Params.dstNzMatrixStride = 0;
        DataCopy(a1Local, aGM, dataCopyA1Params);

        Nd2NzParams dataCopyB1Params;
        dataCopyB1Params.ndNum = 1;
        dataCopyB1Params.nValue = k;
        dataCopyB1Params.dValue = n;
        dataCopyB1Params.srcNdMatrixStride = 0;
        dataCopyB1Params.srcDValue = n;
        dataCopyB1Params.dstNzC0Stride = k;
        dataCopyB1Params.dstNzNStride = 1;
        dataCopyB1Params.dstNzMatrixStride = 0;
        DataCopy(b1Local, bGM, dataCopyB1Params);
        // Move the quantization parameters to the UB
        DataCopy(deqLocal, deqGM, cSize);

        inQueueA1.EnQue(a1Local);
        inQueueB1.EnQue(b1Local);
        inQueueDeq.EnQue(deqLocal);
    }
    __aicore__ inline void SplitA()
    {
        ...
    }
    __aicore__ inline void SplitB()
    {
        ...
    }
    __aicore__ inline void Compute()
    {
        LocalTensor<half> a2Local = inQueueA2.DeQue<half>();
        LocalTensor<half> b2Local = inQueueB2.DeQue<half>();
        LocalTensor<float> c1Local = outQueueCO1.AllocTensor<float>();
        MmadParams mmadParams;
        mmadParams.m = m;
        mmadParams.n = n;
        mmadParams.k = k;
        // Matrix multiplication
        Mmad(c1Local, a2Local, b2Local, mmadParams); // m*n
        outQueueCO1.EnQue<float>(c1Local);
        inQueueA2.FreeTensor(a2Local);
        inQueueB2.FreeTensor(b2Local);
    }
    __aicore__ inline void CopyOut()
    {
        LocalTensor<float> c1Local = outQueueCO1.DeQue<float>();
        GM_ADDR usrWorkspace = AscendC::GetUserWorkspace(workspace);
        xGm.SetGlobalBuffer((__gm__ float *)(usrWorkspace));
        FixpipeParamsV220 fixpipeParams;
        fixpipeParams.nSize = n;
        fixpipeParams.mSize = m;
        fixpipeParams.srcStride = m;
        fixpipeParams.dstStride = n;
        fixpipeParams.ndNum = 1;
        fixpipeParams.srcNdStride = 0;
        fixpipeParams.dstNdStride = 0;
        // Move the matrix multiplication result from CO1 to the workspace
        Fixpipe(xGm, c1Local, fixpipeParams);
        outQueueCO1.FreeTensor(c1Local);
    }
    __aicore__ inline void CopyIn1()
    {
        // Move the matrix multiplication result from the workspace to the UB
        LocalTensor<float> src0Local = inQueueSrc0.AllocTensor<float>();
        DataCopy(src0Local, xGm, cSize);
        inQueueSrc0.EnQue(src0Local);
    }
    __aicore__ inline void Compute1()
    {
        LocalTensor<float> src0Local = inQueueSrc0.DeQue<float>();
        LocalTensor<half> tmpLocal = inQueueTmp.AllocTensor<half>();
        LocalTensor<half> deqLocal = inQueueDeq.DeQue<half>();
        LocalTensor<int8_t> dstLocal = outQueueDst.AllocTensor<int8_t>();
        // Quantization calculation
        Cast(tmpLocal, src0Local, RoundMode::CAST_NONE, cSize);
        LocalTensor<half> tmpHalfBuffer = src0Local.ReinterpretCast<half>();
        Mul(tmpHalfBuffer, tmpLocal, deqLocal, cSize);
        Cast(dstLocal, tmpHalfBuffer, RoundMode::CAST_NONE, cSize);
        outQueueDst.EnQue<int8_t>(dstLocal);
        inQueueSrc0.FreeTensor(src0Local);
        inQueueTmp.FreeTensor(tmpLocal);
        inQueueDeq.FreeTensor(deqLocal);
    }
    __aicore__ inline void CopyOut1()
    {
        ...
    }
private:
    TPipe pipe;
    TQue<TPosition::A1, 1> inQueueA1;
    TQue<TPosition::A2, 1> inQueueA2;
    TQue<TPosition::B1, 1> inQueueB1;
    TQue<TPosition::B2, 1> inQueueB2;
    TQue<TPosition::CO1, 1> outQueueCO1;
    TQue<TPosition::VECIN, 1> inQueueDeq;
    TQue<TPosition::VECIN, 1> inQueueSrc0;
    TQue<TPosition::VECCALC, 1> inQueueTmp;
    TQue<TPosition::VECOUT, 1> outQueueDst;

    GlobalTensor<half> aGM;
    GlobalTensor<half> bGM;
    GlobalTensor<float> cGM;
    GlobalTensor<float> biasGM;
    uint16_t m = 32, k = 32, n = 32;
    uint16_t aSize, bSize, cSize;
    ...
```

[Positive Example]

When this operator performs quantization on the matrix multiplication result, it can move the quantization parameters to the FB (Fixpipe Buffer) and call the Fixpipe API once to implement quantization of the matrix multiplication result.

```
...
public:
    __aicore__ inline KernelSample()
    {
        aSize = m * k;
        bSize = k * n;
        cSize = m * n;
        QuantSize = n;
    }
    __aicore__ inline void Init(__gm__ uint8_t *a, __gm__ uint8_t *b, __gm__ uint8_t *c, __gm__ uint8_t *deqTensor)
    {
        aGM.SetGlobalBuffer((__gm__ half *)a);
        bGM.SetGlobalBuffer((__gm__ half *)b);
        cGM.SetGlobalBuffer((__gm__ float *)c);
        deqGM.SetGlobalBuffer((__gm__ uint64_t *)deqTensor);
        pipe.InitBuffer(inQueueA1, 1, aSize * sizeof(half));
        pipe.InitBuffer(inQueueA2, 1, aSize * sizeof(half));
        pipe.InitBuffer(inQueueB1, 1, bSize * sizeof(half));
        pipe.InitBuffer(inQueueB2, 2, bSize * sizeof(half));
        pipe.InitBuffer(outQueueCO1, 1, cSize * sizeof(float));
        pipe.InitBuffer(inQueueDeq1, 1, QuantSize * sizeof(uint64_t));
        pipe.InitBuffer(inQueueDeq, 1, QuantSize * sizeof(uint64_t));
    }
    __aicore__ inline void Process()
    {
        CopyIn();
        SplitA();
        SplitB();
        SplitDeq();
        Compute();
        CopyOut();
    }
private:
    __aicore__ inline void CopyIn()
    {
        LocalTensor<half> a1Local = inQueueA1.AllocTensor<half>();
        LocalTensor<half> b1Local = inQueueB1.AllocTensor<half>();
        LocalTensor<uint64_t> deq1Local = inQueueDeq1.AllocTensor<uint64_t>();

        Nd2NzParams dataCopyA1Params;
        dataCopyA1Params.ndNum = 1;
        dataCopyA1Params.nValue = m;
        dataCopyA1Params.dValue = k;
        dataCopyA1Params.srcNdMatrixStride = 0;
        dataCopyA1Params.srcDValue = k;
        dataCopyA1Params.dstNzC0Stride = m;
        dataCopyA1Params.dstNzNStride = 1;
        dataCopyA1Params.dstNzMatrixStride = 0;
        DataCopy(a1Local, aGM, dataCopyA1Params);

        Nd2NzParams dataCopyB1Params;
        dataCopyB1Params.ndNum = 1;
        dataCopyB1Params.nValue = k;
        dataCopyB1Params.dValue = n;
        dataCopyB1Params.srcNdMatrixStride = 0;
        dataCopyB1Params.srcDValue = n;
        dataCopyB1Params.dstNzC0Stride = k;
        dataCopyB1Params.dstNzNStride = 1;
        dataCopyB1Params.dstNzMatrixStride = 0;
        DataCopy(b1Local, bGM, dataCopyB1Params);
        // Move the quantization parameters to L1
        DataCopy(deq1Local, deqGM, QuantSize);

        inQueueA1.EnQue(a1Local);
        inQueueB1.EnQue(b1Local);
        inQueueDeq.EnQue(deq1Local);
    }
    __aicore__ inline void SplitA()
    {
        ...
    }
    __aicore__ inline void SplitB()
    {
        ...
    }
    __aicore__ inline void SplitDeq()
    {
        LocalTensor<uint64_t> deq1Local = inQueueDeq1.DeQue<uint64_t>();
        LocalTensor<uint64_t> deqLocal = inQueueDeq.AllocTensor<uint64_t>();
        // Move the quantization parameters from L1 to FB
        DataCopy(deqLocal, deq1Local, { 1, (uint16_t)(QuantSize * sizeof(uint64_t) / 128), 0, 0 });
        inQueueDeq.EnQue<uint64_t>(deqLocal);
        inQueueDeq1.FreeTensor(deq1Local);
    }
    __aicore__ inline void Compute()
    {
        LocalTensor<half> a2Local = inQueueA2.DeQue<half>();
        LocalTensor<half> b2Local = inQueueB2.DeQue<half>();
        LocalTensor<float> c1Local = outQueueCO1.AllocTensor<float>();
        MmadParams mmadParams;
        mmadParams.m = m;
        mmadParams.n = n;
        mmadParams.k = k;
        // Matrix multiplication
        Mmad(c1Local, a2Local, b2Local, mmadParams); // m*n
        outQueueCO1.EnQue<float>(c1Local);
        inQueueA2.FreeTensor(a2Local);
        inQueueB2.FreeTensor(b2Local);
    }
    __aicore__ inline void CopyOut()
    {
        LocalTensor<float> c1Local = outQueueCO1.DeQue<float>();
        LocalTensor<uint64_t> deqLocal = inQueueDeq.DeQue<uint64_t>();
        SetFixpipeNz2ndFlag(1, 0, 0);
        DataCopyCO12DstParams dataCopyParams;
        dataCopyParams.nSize = n;
        dataCopyParams.mSize = m;
        dataCopyParams.srcStride = m;
        dataCopyParams.dstStride = n;
        dataCopyParams.quantPre = QuantMode_t::VQF322B8_PRE;
        dataCopyParams.nz2ndEn = true;
        // Move out the quantized matrix multiplication result
        DataCopy(cGM, c1Local, DataCopyCO12DstParams);
        outQueueCO1.FreeTensor(c1Local);
    }

private:
    TPipe pipe;
    TQue<QuePosition::A1, 1> inQueueA1;
    TQue<QuePosition::A2, 1> inQueueA2;
    TQue<QuePosition::B1, 1> inQueueB1;
    TQue<QuePosition::B2, 1> inQueueB2;
    TQue<QuePosition::C1, 1> inQueueDeq1;
    TQue<QuePosition::C2PIPE2GM, 1> inQueueDeq;
    TQue<QuePosition::CO1, 1> outQueueCO1;
    GlobalTensor<half> aGM;
    GlobalTensor<half> bGM;
    GlobalTensor<float> cGM;
    GlobalTensor<uint64_t> deqTensorGM;
    uint16_t m = 32, k = 32, n = 32;
    uint16_t aSize, bSize, cSize, QuantSize;
    ...
```

