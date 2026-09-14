# Efficient Matrix Multiplication Result Accumulation Through L0C Buffer Data Staging<a name="ZH-CN_TOPIC_0000001918025254"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:15:05.664Z -->

[Priority] High  __

[Description] When accumulating the results of matrix multiplications in an operator implementation (for example, the result accumulation of matrix A1 \* B1 + A2 \* B2...), the result of the previous matrix multiplication can be staged in CO1 (L0C), and the Mmad API can be called to implement matrix multiplication result accumulation. Compared with copying the result of each matrix multiplication from CO1 to GM and then to UB for accumulation, this approach reduces the number of data copy operations and improves memory usage efficiency.

**Figure 1**  Negative example data flow diagram<a name="fig1739115131882"></a>  
![](../../../figures/negative_sample_data_flow_diagram_80.png "negative-example-data-flow-diagram-80")

**Figure 2**  Positive example data flow diagram<a name="fig1831565712115"></a>  
![](../../../figures/positive_sample_data_flow_diagram_81.png "positive-example-data-flow-diagram-81")

[Negative Example]

Before optimization, the process of accumulating the results of two matrix multiplications is as follows:

-   Copy the result of the previous matrix multiplication from CO1 to workspace, and then from workspace to UB.
-   For the next matrix multiplication, repeat the preceding steps to copy the result to UB.
-   Add the results of the two matrix multiplications in UB.

When n sub-matrix multiplications need to be accumulated, n CO1->workspace copies, n workspace->UB copies, and n Add operations are added respectively.

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
    __aicore__ inline void Init(__gm__ uint8_t *a, __gm__ uint8_t *b, __gm__ uint8_t *c)
    {
        aGM.SetGlobalBuffer((__gm__ half *)a);
        bGM.SetGlobalBuffer((__gm__ half *)b);
        cGM.SetGlobalBuffer((__gm__ float *)c);
        pipe.InitBuffer(inQueueA1, 1, aSize * sizeof(half));
        pipe.InitBuffer(inQueueA2, 1, aSize * sizeof(half));
        pipe.InitBuffer(inQueueB1, 1, bSize * sizeof(half));
        pipe.InitBuffer(inQueueB2, 2, bSize * sizeof(half));
        pipe.InitBuffer(outQueueCO1, 1, cSize * sizeof(float));
        pipe.InitBuffer(inQueueSrc0, 1, cSize * sizeof(float));
        pipe.InitBuffer(inQueueSrc1, 1, cSize * sizeof(float));
        pipe.InitBuffer(outQueueDst, 1, cSize * sizeof(float));

    }
    __aicore__ inline void Process()
    {
        // Perform the first matrix multiplication
        CopyIn();
        SplitA();
        SplitB();
        Compute();
        // Copy out the result of the first matrix multiplication
        CopyOut();
        // Copy the result of the first matrix multiplication to UB
        CopyIn1();
        // Perform the second matrix multiplication
        Compute1();
        // Copy out the result of the first matrix multiplication
        CopyOut1();
        // Copy the result of the second matrix multiplication to UB
        CopyIn1();
        // Accumulate the results of the two matrix multiplications
        Compute2();
        CopyOut2();
    }
private:
    __aicore__ inline void CopyIn()
    {
        LocalTensor<half> a1Local = inQueueA1.AllocTensor<half>();
        LocalTensor<half> b1Local = inQueueB1.AllocTensor<half>();

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

        inQueueA1.EnQue<half>(a1Local);
        inQueueB1.EnQue<half>(b1Local);
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
        Mmad(c1Local, a2Local, b2Local, mmadParams);
        outQueueCO1.EnQue<float>(c1Local);
        inQueueA2.EnQue<half>(a2Local);
        inQueueB2.EnQue<half>(b2Local);
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
        // Copy the matrix multiplication result from CO1 to workspace.
        Fixpipe(xGm, c1Local, fixpipeParams);
        outQueueCO1.EnQue<float>(c1Local);
    }
    __aicore__ inline void CopyIn1()
    {
        LocalTensor<float> src0Local = inQueueSrc0.AllocTensor<float>();
        // Copy the matrix multiplication result from workspace to UB.
        DataCopy(src0Local, xGm, cSize);
        inQueueSrc0.EnQue<float>(src0Local);
    }
    __aicore__ inline void Compute1()
    {
        LocalTensor<half> a2Local = inQueueA2.DeQue<half>();
        LocalTensor<half> b2Local = inQueueB2.DeQue<half>();
        LocalTensor<float> c1Local = outQueueCO1.DeQue<float>();
        MmadParams mmadParams;
        mmadParams.m = m;
        mmadParams.n = n;
        mmadParams.k = k;
        // Matrix multiplication
        Mmad(c1Local, a2Local, b2Local, mmadParams);
        outQueueCO1.EnQue<float>(c1Local);
        inQueueA2.FreeTensor(a2Local);
        inQueueB2.FreeTensor(b2Local);
    }
    __aicore__ inline void CopyOut1()
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
        // Copy the matrix multiplication result from CO1 to workspace.
        Fixpipe(xGm, c1Local, fixpipeParams);
        outQueueCO1.FreeTensor(c1Local);
    }
    __aicore__ inline void CopyIn2()
    {
        PipeBarrier<PIPE_ALL>();
        LocalTensor<float> src1Local = inQueueSrc1.AllocTensor<float>();
        // Copy the matrix multiplication result from workspace to UB.
        DataCopy(src1Local, xGm, cSize);
        inQueueSrc1.EnQue<float>(src1Local);
    }
    __aicore__ inline void Compute2()
    {
        LocalTensor<float> src0Local = inQueueSrc0.DeQue<float>();
        LocalTensor<float> src1Local = inQueueSrc1.DeQue<float>();
        LocalTensor<float> dstLocal = outQueueDst.AllocTensor<float>();
        // Add the results of the two matrix multiplications.
        Add(dstLocal, src0Local, src1Local, cSize);
        outQueueDst.EnQue<float>(dstLocal);
        inQueueSrc0.FreeTensor(src0Local);
        inQueueSrc1.FreeTensor(src1Local);
    }
    __aicore__ inline void CopyOut2()
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
    TQue<TPosition::VECIN, 1> inQueueSrc0;
    TQue<TPosition::VECIN, 1> inQueueSrc1;
    TQue<TPosition::VECOUT, 1> outQueueDst;

    GlobalTensor<half> aGM;
    GlobalTensor<half> bGM;
    GlobalTensor<float> cGM;
    uint16_t m = 32, k = 32, n = 32;
    uint16_t aSize, bSize, cSize;  
...
```

[Positive Example]

With this optimization, when the operator accumulates matrix multiplication results, it can temporarily store the result of the previous matrix multiplication on L0C, configure the initial value of the C matrix through the Mmad interface parameters cmatrixInitVal and cmatrixSource, and call the Mmad interface only twice to accumulate the results of two matrix multiplications.

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
    __aicore__ inline void Init(__gm__ uint8_t *a, __gm__ uint8_t *b, __gm__ uint8_t *c)
    {
        aGM.SetGlobalBuffer((__gm__ half *)a);
        bGM.SetGlobalBuffer((__gm__ half *)b);
        cGM.SetGlobalBuffer((__gm__ float *)c);
        pipe.InitBuffer(inQueueA1, 1, aSize * sizeof(half));
        pipe.InitBuffer(inQueueA2, 1, aSize * sizeof(half));
        pipe.InitBuffer(inQueueB1, 1, bSize * sizeof(half));
        pipe.InitBuffer(inQueueB2, 2, bSize * sizeof(half));
        pipe.InitBuffer(outQueueCO1, 1, cSize * sizeof(float));
    }
    __aicore__ inline void Process()
    {
        CopyIn();
        SplitA();
        SplitB();
        Compute();
        CopyOut();
    }
private:
    __aicore__ inline void CopyIn()
    {
        LocalTensor<half> a1Local = inQueueA1.AllocTensor<half>();
        LocalTensor<half> b1Local = inQueueB1.AllocTensor<half>();

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

        inQueueA1.EnQue(a1Local);
        inQueueB1.EnQue(b1Local);
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
        // First matrix multiplication
        Mmad(c1Local, a2Local, b2Local, mmadParams);
        PipeBarrier<PIPE_M>();
        // Accumulate the result of the first matrix multiplication in the second matrix multiplication.
        mmadParams.cmatrixInitVal = false;
        Mmad(c1Local, a2Local, b2Local, c1Local, mmadParams);
        outQueueCO1.EnQue<float>(c1Local);
        inQueueA2.FreeTensor(a2Local);
        inQueueB2.FreeTensor(b2Local);
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

    GlobalTensor<half> aGM;
    GlobalTensor<half> bGM;
    GlobalTensor<dst_T> cGM;
    uint16_t m = 32, k = 32, n = 32;
    uint16_t aSize, bSize, cSize; 
```

