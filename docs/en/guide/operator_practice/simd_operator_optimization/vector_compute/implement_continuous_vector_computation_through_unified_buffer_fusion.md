# Implementing Contiguous Vector Computation Through Unified Buffer Fusion<a name="ZH-CN_TOPIC_0000001918025250"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:25:45.477Z -->

[Priority] High

[Description] When an operator implementation involves multiple vector computations and the output of a previous computation serves as the input of the next computation, you can temporarily store the output of the previous computation in the UB (Unified Buffer) and directly use it as the input of the next computation, without moving the output from the UB to the GM and then back from the GM to the UB. This Unified Buffer fusion approach reduces the number of data transfers, implements continuous vector computation, and improves memory usage efficiency. The data flow graph comparison is as follows:

**Figure 1**  Data flow graph comparison<a name="fig9257105941218"></a>  
![](../../../figures/data_flow_graph_comparison.png "data flow graph comparison")

[Negative Example]

The computation logic of this operator is to perform an Exp computation followed by an Abs computation. During the computation, the source operand is first moved from the GM to the UB for the Exp computation. After the Exp computation is complete, the Exp result is moved from the UB to the GM. Then, the Exp result is moved from the GM to the UB as the input of the Abs computation. After the Abs computation is complete, the destination operand result is moved from the UB to the GM. The entire process involves four data transfers between the GM and the UB. When n vector computations are required, a total of 2n data transfers between the GM and the UB are needed.

```
class KernelSample {
public:
    __aicore__ inline KernelSample() {}
    __aicore__ inline void Init(__gm__ uint8_t* src0Gm, __gm__ uint8_t* dstGm)
    {
        src0Global.SetGlobalBuffer((__gm__ float*)src0Gm);
        dstGlobal.SetGlobalBuffer((__gm__ float*)dstGm);
        pipe.InitBuffer(inQueueSrc0, 1, 1024 * sizeof(float));
        pipe.InitBuffer(outQueueDst, 1, 1024 * sizeof(float));
    }
    __aicore__ inline void Process()
    {
        CopyIn();
        Compute();
        CopyOut();
        CopyIn1();
        Compute1();
        CopyOut1();
    }

private:
    __aicore__ inline void CopyIn()
    {
        LocalTensor<float> src0Local = inQueueSrc0.AllocTensor<float>();
        DataCopy(src0Local, src0Global, 1024);
        inQueueSrc0.EnQue(src0Local);
    }
    __aicore__ inline void Compute()
    {
        LocalTensor<float> src0Local = inQueueSrc0.DeQue<float>();
        LocalTensor<float> dstLocal = outQueueDst.AllocTensor<float>();
        Exp(dstLocal, src0Local, 1024);
        outQueueDst.EnQue<float>(dstLocal);
        inQueueSrc0.FreeTensor(src0Local);
    }
    __aicore__ inline void CopyOut()
    {
        LocalTensor<float> dstLocal = outQueueDst.DeQue<float>();
        DataCopy(dstGlobal, dstLocal, 1024);
        outQueueDst.FreeTensor(dstLocal);
    }
    __aicore__ inline void CopyIn1()
    {
        LocalTensor<float> src0Local = inQueueSrc0.AllocTensor<float>();
        DataCopy(src0Local, dstGlobal, 1024);
        inQueueSrc0.EnQue(src0Local);
    }
    __aicore__ inline void Compute1()
    {
        LocalTensor<float> src0Local = inQueueSrc0.DeQue<float>();
        LocalTensor<float> dstLocal = outQueueDst.AllocTensor<float>();
        Abs(dstLocal, src0Local, 1024);
        outQueueDst.EnQue<float>(dstLocal);
        inQueueSrc0.FreeTensor(src0Local);
    }
    __aicore__ inline void CopyOut1()
    {
        LocalTensor<float> dstLocal = outQueueDst.DeQue<float>();
        DataCopy(dstGlobal, dstLocal, 1024);
        outQueueDst.FreeTensor(dstLocal);
    }

private:
    TPipe pipe;
    TQue<TPosition::VECIN, 1> inQueueSrc0;
    TQue<TPosition::VECOUT, 1> outQueueDst;
    GlobalTensor<float> src0Global, dstGlobal;
};
```

[Positive Example]

With the Unified Buffer fusion approach, when continuous vector computations are performed on the UB, the result of a previous computation can be directly used as the input of the next computation and continue to be computed on the UB, without intermediate data transfers. The source operand only needs to be moved to the UB at the beginning of the computation, and the final result is moved from the UB to the GM after all computations are complete, involving two data transfers in total.

```
class KernelSample {
public:
    __aicore__ inline KernelSample() {}
    __aicore__ inline void Init(__gm__ uint8_t* src0Gm, __gm__ uint8_t* dstGm)
    {
        src0Global.SetGlobalBuffer((__gm__ float*)src0Gm);
        dstGlobal.SetGlobalBuffer((__gm__ float*)dstGm);
        pipe.InitBuffer(inQueueSrc0, 1, 1024 * sizeof(float));
        pipe.InitBuffer(outQueueDst, 1, 1024 * sizeof(float));
    }
    __aicore__ inline void Process()
    {
        CopyIn();
        Compute();
        CopyOut();
    }

private:
    __aicore__ inline void CopyIn()
    {
        LocalTensor<float> src0Local = inQueueSrc0.AllocTensor<float>();
        DataCopy(src0Local, src0Global, 1024);
        inQueueSrc0.EnQue(src0Local);
    }
    __aicore__ inline void Compute()
    {
        LocalTensor<float> src0Local = inQueueSrc0.DeQue<float>();
        LocalTensor<float> dstLocal = outQueueDst.AllocTensor<float>();
        Exp(dstLocal, src0Local, 1024);
        Abs(dstLocal, dstLocal, 1024);
        outQueueDst.EnQue<float>(dstLocal);
        inQueueSrc0.FreeTensor(src0Local);
    }
    __aicore__ inline void CopyOut()
    {
        LocalTensor<float> dstLocal = outQueueDst.DeQue<float>();
        DataCopy(dstGlobal, dstLocal, 1024);
        outQueueDst.FreeTensor(dstLocal);
    }

private:
    TPipe pipe;
    TQue<TPosition::VECIN, 1> inQueueSrc0;
    TQue<TPosition::VECOUT, 1> outQueueDst;
    GlobalTensor<float> src0Global, dstGlobal;
};
```

