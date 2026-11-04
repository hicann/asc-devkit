# Keeping the Smaller Cube Resident in L1 Buffer and Transferring the Larger Cube in Multiple Batches<a name="ZH-CN_TOPIC_0000001918185186"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-10-23T12:37:11.164Z pushedAt=2026-11-03T07:05:13.328Z nl=2 -->

[Priority] High  __

[Description] During cube computation, when L1 cannot hold both the left and right cubes at once, keep the smaller cube resident in L1 and transfer only the larger cube in multiple batches, thereby reducing the number of transfers.

[Negative example]

Assume that the L1 size is 512K, the left and right cubes are 992K and 16K respectively, and the data type is half. The left and right cubes cannot be fully loaded into L1 at once. The developer plans the following split strategy: do not split the K axis; split the left cube evenly into two blocks A1 and A2, each with a shape of \[992, 256\]; split the right cube evenly into two blocks, each with a shape of \[256, 16\]. The loading order during computation is as follows: first load cube A1 into L1, then load and compute B1 and B2 in sequence; then load A2 into L1, and load and compute B1 and B2 in sequence.

**Figure 1**  Negative example split strategy<a name="fig9917043192214"></a>  
![](../../../figures/bad_split.png "Negative example split strategy")

```
...
public:
    __aicore__ inline KernelSample()
    {
        aSize = baseM * baseK;
        bSize = baseK * baseN;
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
        for (uint32_t i = 0; i < 2; i++) {
            CopyInA1(i);
            SplitA();
            for (uint32_t j = 0; j < 2; j++) {
                CopyInB1(j);
                SplitB();
                Compute(i, j);
            }
        }
        CopyOut();
    }
private:
    __aicore__ inline void CopyInA1(uint32_t i)
    {
        LocalTensor<half> a1Local = inQueueA1.AllocTensor<half>();
        // Load the left cube blocks a1/a2 into the L1 buffer (A1)
        Nd2NzParams dataCopyA1Params;
        dataCopyA1Params.ndNum = 1;
        dataCopyA1Params.nValue = baseM;
        dataCopyA1Params.dValue = baseK;
        dataCopyA1Params.srcNdMatrixStride = 0;
        dataCopyA1Params.srcDValue = baseK;
        dataCopyA1Params.dstNzC0Stride = baseM;
        dataCopyA1Params.dstNzNStride = 1;
        dataCopyA1Params.dstNzMatrixStride = 0;
        DataCopy(a1Local, aGM[i * baseM * baseK], dataCopyA1Params);
        inQueueA1.EnQue(a1Local);
    }
    __aicore__ inline void SplitA()
    {
        LocalTensor<half> a1Local = inQueueA1.DeQue<half>();
        LocalTensor<half> a2Local = inQueueA2.AllocTensor<half>();
        // Transfer the left cube blocks a1/a2 from the L1 buffer (A1) to the L0A buffer (A2)
        LoadData2dParams loadL0AParams;
        loadL0AParams.repeatTimes = baseM * baseK * sizeof(half) / 512;
        loadL0AParams.srcStride = 1;
        loadL0AParams.dstGap = 0;
        LoadData(a2Local, a1Local, loadL0AParams);
        inQueueA2.EnQue(a2Local);
        inQueueA1.FreeTensor(a1Local);
    }
    __aicore__ inline void CopyInB1(uint32_t j)
    {
        LocalTensor<half> b1Local = inQueueB1.AllocTensor<half>();
        // Load the right cube blocks b1/b2 into the L1 buffer (B1)
        Nd2NzParams dataCopyB1Params;
        dataCopyB1Params.ndNum = 1;
        dataCopyB1Params.nValue = baseK;
        dataCopyB1Params.dValue = baseN;
        dataCopyB1Params.srcNdMatrixStride = 0;
        dataCopyB1Params.srcDValue = n;
        dataCopyB1Params.dstNzC0Stride = baseK;
        dataCopyB1Params.dstNzNStride = 1;
        dataCopyB1Params.dstNzMatrixStride = 0;
        DataCopy(b1Local, bGM[j * baseN], dataCopyB1Params);
        inQueueB1.EnQue(b1Local);
    }
    __aicore__ inline void SplitB()
    {
        LocalTensor<half> b1Local = inQueueB1.DeQue<half>();
        LocalTensor<half> b2Local = inQueueB2.AllocTensor<half>();
        // Transfer the right cube blocks b1/b2 from the L1 buffer (B1) to the L0B buffer (B2)
        LoadData2dTransposeParams loadL0BParams;
        loadL0BParams.startIndex = 0;
        loadL0BParams.repeatTimes = baseK / nBlockSize;
        loadL0BParams.srcStride = 1;
        loadL0BParams.dstGap = 1;
        LoadDataWithTranspose(b2Local, b1Local, loadL0BParams);
        inQueueB2.EnQue(b2Local);
        inQueueB1.FreeTensor(b1Local);
    }
    __aicore__ inline void Compute(uint32_t i, uint32_t j)
    {
        LocalTensor<half> a2Local = inQueueA2.DeQue<half>();
        LocalTensor<half> b2Local = inQueueB2.DeQue<half>();
        LocalTensor<float> c1Local = outQueueCO1.AllocTensor<float>();
        // Cube multiplication
        mmadParams.m = baseM;
        mmadParams.n = baseN;
        mmadParams.k = baseK;
        Mmad(c1Local[i * baseM * baseN + j * m * baseN], a2Local, b2Local, mmadParams);
        outQueueCO1.EnQue<float>(c1Local);
        inQueueA2.FreeTensor(a2Local);
        inQueueB2.FreeTensor(b2Local);
    }
    __aicore__ inline void CopyOut()
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

    GlobalTensor<half> aGM;
    GlobalTensor<half> bGM;
    GlobalTensor<float> cGM;
    uint16_t m = 1984, k = 256, n = 32;
    uint16_t baseM = 992, baseK = 256, baseN = 16;
    uint16_t aSize, bSize, cSize;
    uint16_t nBlockSize = 16;
...
```

[Positive example]

In this example, the smaller right cube is loaded into L1 at once and kept resident in L1. Cube A is continuously transferred within the loop. When the loop count is 2, a total of 3 transfers are required.

```
...
public:
    __aicore__ inline KernelSample()
    {
        aSize = baseM * baseK;
        bSize = baseK * n;
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
        CopyInB1();
        SplitB();
        for (uint32_t i = 0; i < 2; i++) {
            CopyInA1(i);
            SplitA();
            for (uint32_t j = 0; j < 2; j++) {
                Compute(i, j);
            }
        }
        CopyOut();
    }
private:
    __aicore__ inline void CopyInB1()
    {
        LocalTensor<half> b1Local = inQueueB1.AllocTensor<half>();
        // Load the entire right cube into B1
        Nd2NzParams dataCopyB1Params;
        dataCopyB1Params.ndNum = 1;
        dataCopyB1Params.nValue = baseK;
        dataCopyB1Params.dValue = n;
        dataCopyB1Params.srcNdMatrixStride = 0;
        dataCopyB1Params.srcDValue = n;
        dataCopyB1Params.dstNzC0Stride = baseK;
        dataCopyB1Params.dstNzNStride = 1;
        dataCopyB1Params.dstNzMatrixStride = 0;
        DataCopy(b1Local, bGM, dataCopyB1Params);
        inQueueB1.EnQue(b1Local);
    }
    __aicore__ inline void SplitB()
    {
        LocalTensor<half> b1Local = inQueueB1.DeQue<half>();
        LocalTensor<half> b2Local = inQueueB2.AllocTensor<half>();
        // Transfer the entire right cube from the L1 buffer (B1) to the L0B buffer (B2)
        LoadData2dTransposeParams loadL0BParams;
        loadL0BParams.startIndex = 0;
        loadL0BParams.repeatTimes = baseK / nBlockSize;
        loadL0BParams.srcStride = 1;
        loadL0BParams.dstGap = 1;
        for (int blockNum = 0; blockNum < (n / nBlockSize); blockNum++) {
            LoadDataWithTranspose(b2Local[blockNum * 16 * nBlockSize], b1Local[blockNum * baseK * nBlockSize], loadL0BParams);
        }
        inQueueB2.EnQue(b2Local);
        inQueueB1.FreeTensor(b1Local);
    }
    __aicore__ inline void CopyInA1(uint32_t i)
    {
        LocalTensor<half> a1Local = inQueueA1.AllocTensor<half>();
        // Load the left cube blocks a1/a2 into the L1 buffer (A1)
        Nd2NzParams dataCopyA1Params;
        dataCopyA1Params.ndNum = 1;
        dataCopyA1Params.nValue = baseM;
        dataCopyA1Params.dValue = baseK;
        dataCopyA1Params.srcNdMatrixStride = 0;
        dataCopyA1Params.srcDValue = baseK;
        dataCopyA1Params.dstNzC0Stride = baseM;
        dataCopyA1Params.dstNzNStride = 1;
        dataCopyA1Params.dstNzMatrixStride = 0;
        DataCopy(a1Local, aGM[i * baseM * baseK], dataCopyA1Params);
        inQueueA1.EnQue(a1Local);
    }
    __aicore__ inline void SplitA()
    {
        LocalTensor<half> a1Local = inQueueA1.DeQue<half>();
        LocalTensor<half> a2Local = inQueueA2.AllocTensor<half>();
        // Transfer the left cube blocks a1/a2 from the L1 buffer (A1) to the L0A buffer (A2)
        LoadData2dParams loadL0AParams;
        loadL0AParams.repeatTimes = baseM * baseK * sizeof(half) / 512;
        loadL0AParams.srcStride = 1;
        loadL0AParams.dstGap = 0;
        LoadData(a2Local, a1Local, loadL0AParams);
        inQueueA2.EnQue(a2Local);
        inQueueA1.FreeTensor(a1Local);
    }
    __aicore__ inline void Compute(uint32_t i, uint32_t j)
    {
        LocalTensor<half> a2Local = inQueueA2.DeQue<half>();
        LocalTensor<half> b2Local = inQueueB2.DeQue<half>();
        LocalTensor<float> c1Local = outQueueCO1.AllocTensor<float>();
        // Cube multiplication
        mmadParams.m = baseM;
        mmadParams.n = baseN;
        mmadParams.k = baseK;
        Mmad(c1Local[i * baseM * baseN + j * m * baseN], a2Local, b2Local, mmadParams);
        outQueueCO1.EnQue<float>(c1Local);
        inQueueA2.FreeTensor(a2Local);
        inQueueB2.FreeTensor(b2Local);
    }
    __aicore__ inline void CopyOut()
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

    GlobalTensor<half> aGM;
    GlobalTensor<half> bGM;
    GlobalTensor<float> cGM;
    uint16_t m = 1984, k = 256, n = 32;
    uint16_t baseM = 992, baseK = 256, baseN = 16;
    uint16_t aSize, bSize, cSize;
    uint16_t nBlockSize = 16;
...
```
