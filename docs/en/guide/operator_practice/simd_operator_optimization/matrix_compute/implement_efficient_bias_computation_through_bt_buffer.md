# Efficient Bias Computation Through BT Buffer<a name="ZH-CN_TOPIC_0000001918185182"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:13:28.037Z -->

[Priority] High

[Description] When an operator performs matrix multiplication with bias, you can move the bias data to C2 (Bias Table Buffer) and call the Mmad API once to implement matrix multiplication with bias, or directly call the high-level Matmul API to complete the function. Compared with the process of first moving the matrix multiplication result from CO1 (L0C) to GM and then moving it to UB to add the bias, this approach reduces the number of data movements and improves memory usage efficiency. The data flow diagrams are compared as follows:

**Figure 1**  Negative sample data flow diagram<a name="fig1598718171213"></a>  
![](../../../figures/negative_sample_data_flow_diagram.png "Negative sample data flow diagram")

**Figure 2**  Positive sample data flow diagram<a name="fig12618184193220"></a>  
![](../../../figures/positive_sample_data_flow_diagram.png "Positive sample data flow diagram")

[Negative Example]

When this operator performs matrix multiplication with bias, the process is as follows:

-   Move the matrix multiplication result from CO1 (L0C) to workspace (GM).
-   Move it from workspace to UB.
-   Perform the bias addition operation on UB.
-   Finally, move the result to GM.

When this computation process is looped n times, n additional CO1->workspace and workspace->UB data movements are added respectively.

```
// This sample is for illustration only and is not complete code. Some synchronization control code is omitted.
public:
    __aicore__ inline KernelSample()
    {
        aSize = m * k;
        bSize = k * n;
        cSize = m * n;
    }
    __aicore__ inline void Init(__gm__ uint8_t *a, __gm__ uint8_t *b, __gm__ uint8_t *bias, __gm__ uint8_t *c)
    {
        aGM.SetGlobalBuffer((__gm__ half *)a);
        bGM.SetGlobalBuffer((__gm__ half *)b);
        cGM.SetGlobalBuffer((__gm__ float *)c);
        biasGM.SetGlobalBuffer((__gm__ float *)bias);
        pipe.InitBuffer(inQueueA1, 1, aSize * sizeof(half));
        pipe.InitBuffer(inQueueA2, 1, aSize * sizeof(half));
        pipe.InitBuffer(inQueueB1, 1, bSize * sizeof(half));
        pipe.InitBuffer(inQueueB2, 2, bSize * sizeof(half));
        pipe.InitBuffer(outQueueCO1, 1, cSize * sizeof(float));
        pipe.InitBuffer(inQueueBias, 1, n * sizeof(float));
        pipe.InitBuffer(inQueueSrc0, 1, cSize * sizeof(float));
        pipe.InitBuffer(outQueueDst, 1, cSize * sizeof(float));

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
        LocalTensor<float> biasLocal = inQueueBias.AllocTensor<float>();

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
        // Move the bias to UB
        DataCopy(biasLocal, biasGM, n);

        inQueueA1.EnQue(a1Local);
        inQueueB1.EnQue(b1Local);
        inQueueBias.EnQue(biasLocal);
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
        // Move the matrix multiplication result from CO1 to workspace
        Fixpipe(xGm, c1Local, fixpipeParams);
        outQueueCO1.FreeTensor(c1Local);
    }
    __aicore__ inline void CopyIn1()
    {
        PipeBarrier<PIPE_ALL>();
        // Move the matrix multiplication result from workspace to UB
        LocalTensor<float> src0Local = inQueueSrc0.AllocTensor<float>();
        DataCopy(src0Local, xGm, cSize);
        inQueueSrc0.EnQue(src0Local);
    }
    __aicore__ inline void Compute1()
    {
        LocalTensor<float> src0Local = inQueueSrc0.DeQue<float>();
        LocalTensor<float> biasLocal = inQueueBias.DeQue<float>();
        LocalTensor<float> dstLocal = outQueueDst.AllocTensor<float>();
        BinaryRepeatParams addRepeatParams;
        addRepeatParams.dstRepStride = 8;
        addRepeatParams.src0RepStride = 8;
        addRepeatParams.src1RepStride = 0;
        // Add the bias
        Add(dstLocal, src0Local, biasLocal, 32, m, addRepeatParams);
        outQueueDst.EnQue<float>(dstLocal);
        inQueueSrc0.FreeTensor(src0Local);
        inQueueBias.FreeTensor(biasLocal);
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
    TQue<TPosition::VECIN, 1> inQueueBias;
    TQue<TPosition::VECIN, 1> inQueueSrc0;
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

When this operator performs matrix multiplication with bias, it first moves the bias to BT, and then calls the Mmad API once to implement matrix multiplication and bias addition.

```
...
// This sample is for illustration only and is not complete code. Some synchronization control code is omitted
public:
    __aicore__ inline KernelSample()
    {
        aSize = m * k;
        bSize = k * n;
        cSize = m * n;
    }
    __aicore__ inline void Init(__gm__ uint8_t *a, __gm__ uint8_t *b, __gm__ uint8_t *bias, __gm__ uint8_t *c)
    {
        aGM.SetGlobalBuffer((__gm__ half *)a);
        bGM.SetGlobalBuffer((__gm__ half *)b);
        cGM.SetGlobalBuffer((__gm__ float *)c);
        biasGM.SetGlobalBuffer((__gm__ float *)bias);
        pipe.InitBuffer(inQueueA1, 1, aSize * sizeof(half));
        pipe.InitBuffer(inQueueA2, 1, aSize * sizeof(half));
        pipe.InitBuffer(inQueueB1, 1, bSize * sizeof(half));
        pipe.InitBuffer(inQueueB2, 2, bSize * sizeof(half));
        pipe.InitBuffer(outQueueCO1, 1, cSize * sizeof(float));
        pipe.InitBuffer(inQueueC1, 1, n * sizeof(float));
        pipe.InitBuffer(outQueueC2, 1, n * sizeof(float));
    }
    __aicore__ inline void Process()
    {
        CopyIn();
        SplitA();
        SplitB();
        SplitBias();
        Compute();
        CopyOut();
    }
private:
    __aicore__ inline void CopyIn()
    {
        LocalTensor<half> a1Local = inQueueA1.AllocTensor<half>();
        LocalTensor<half> b1Local = inQueueB1.AllocTensor<half>();
        LocalTensor<float> bias1Local = inQueueC1.AllocTensor<float>();

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
        // Copy bias from GM to L1
        DataCopy(bias1Local, biasGM, n);

        inQueueA1.EnQue(a1Local);
        inQueueB1.EnQue(b1Local);
        inQueueC1.EnQue(bias1Local);
    }
    __aicore__ inline void SplitA()
    {
        ...
    }
    __aicore__ inline void SplitB()
    {
        ...
    }
    __aicore__ inline void SplitBias()
    {
        LocalTensor<float> bias1Local = inQueueC1.DeQue<float>();
        LocalTensor<float> bias2Local = outQueueC2.AllocTensor<float>();
        // Copy bias from L1 to BT
        DataCopy(bias2Local, bias1Local, { 1, (uint16_t)(n * sizeof(float) / 64), 0, 0 });
        outQueueC2.EnQue<float>(bias2Local);
        inQueueC1.FreeTensor(bias1Local);
    }
    __aicore__ inline void Compute()
    {
        LocalTensor<half> a2Local = inQueueA2.DeQue<half>();
        LocalTensor<half> b2Local = inQueueB2.DeQue<half>();
        LocalTensor<float> bias2Local = outQueueC2.DeQue<float>();
        LocalTensor<float> c1Local = outQueueCO1.AllocTensor<float>();
        MmadParams mmadParams;
        mmadParams.m = m;
        mmadParams.n = n;
        mmadParams.k = k;
        mmadParams.cmatrixInitVal = false;
        // Matrix multiplication
        Mmad(c1Local, a2Local, b2Local, bias2Local, mmadParams);
        outQueueCO1.EnQue<float>(c1Local);
        inQueueA2.FreeTensor(a2Local);
        inQueueB2.FreeTensor(b2Local);
        outQueueC2.FreeTensor(bias2Local);
    }
    __aicore__ inline void CopyOut()
    {
        LocalTensor<float> c1Local = outQueueCO1.DeQue<float>();
        FixpipeParamsV220 fixpipeParams;
        fixpipeParams.nSize = n;
        fixpipeParams.mSize = m;
        fixpipeParams.srcStride = m;
        fixpipeParams.dstStride = n;

        fixpipeParams.ndNum = 1;
        fixpipeParams.srcNdStride = 0;
        fixpipeParams.dstNdStride = 0;
        Fixpipe(cGM, c1Local, fixpipeParams);
        outQueueCO1.FreeTensor(c1Local);
    }
private:
    TPipe pipe;
    TQue<TPosition::A1, 1> inQueueA1;
    TQue<TPosition::A2, 1> inQueueA2;
    TQue<TPosition::B1, 1> inQueueB1;
    TQue<TPosition::B2, 1> inQueueB2;
    TQue<TPosition::CO1, 1> outQueueCO1;
    TQue<TPosition::C1, 1> inQueueC1;
    TQue<TPosition::C2, 1> outQueueC2;

    GlobalTensor<half> aGM;
    GlobalTensor<half> bGM;
    GlobalTensor<float> cGM;
    GlobalTensor<float> biasGM;
    uint16_t m = 32, k = 32, n = 32;
    uint16_t aSize, bSize, cSize; 
```

