# Reusing VECIN and VECOUT for Pure Data-Movement Operators<a name="ZH-CN_TOPIC_0000001893038937"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:17:49.220Z -->

[Priority] High

[Description] Pure data-movement operators do not involve actual vector computation during execution. If redundant vector instructions exist, the overall execution time of the operator increases. In this scenario, you can use the TQueBind API provided by Ascend C for pure data-movement operators. This API binds VECIN and VECOUT, omitting the step of copying data from VECIN to VECOUT, thereby avoiding unnecessary vector consumption.

[Negative Example]

To ensure pipeline synchronization between data loading and data unloading, this code segment contains a LocalTensor -> LocalTensor DataCopy instruction.

```
template <typename ComputeT> class KernelExample {
 public:
     ...
     __aicore__ inline void Process(...)
     {
         for (int i = 0; i < iLen; ++i) {
             ... 
             auto iLocal = QueI.AllocTensor<ComputeT>();
             DataCopy(iLocal, inGm[i * 32], size);
             QueI.EnQue(iLocal);
             iLocal = QueI.DeQue<ComputeT>();
             for (int j = 0; j < jLen; ++j) { 
                 ...
                 auto oLocal = QueO.AllocTensor<ComputeT>();
                 DataCopy(oLocal, iLocal, size); // DataCopy instruction from LocalTensor to LocalTensor to move data from VECIN to VECOUT
                 QueO.EnQue(oLocal);

                 auto oLocal = QueO.DeQue<ComputeT>();
                 DataCopyPad(outGm[j], oLocal, ...);
                 QueO.FreeTensor(oLocal);
             }
             QueI.FreeTensor(iLocal);
         }
     }

 private:
     ... 
     TQue<TPosition::VECIN, BUFFER_NUM> QueI;
     TQue<TPosition::VECOUT, BUFFER_NUM> QueO;
     ...
 };

 extern "C" __global__ __aicore__ void example_kernel(...)
 {
     ...
     op.Process(...);
 }
```

[Positive Example]

Replace the LocalTensor -> LocalTensor DataCopy instruction with the TQueBind API to reduce the step of copying VECIN to VECOUT, thereby avoiding redundant copying.

```
template <typename ComputeT> class KernelExample {
 public:
     ...
     __aicore__ inline void Process(...)
     {
         for (int i = 0; i < iLen; ++i) {
             ... 
             auto bindLocal = queBind.AllocTensor<ComputeT>();
             DataCopy(bindLocal, inGm[i * 32], size);
             queBind.EnQue(bindLocal);
             bindLocal = queBind.DeQue<ComputeT>();
             for (int j = 0; j < jlen; ++j) {
                 ...
                 DataCopyPad(outGm[j], bindLocal, ...);
             }
             queBind.FreeTensor(bindLocal);
         }
     }

 private:
     ... 
     TQueBind<TPosition::VECIN, TPosition::VECOUT, BUFFER_NUM> queBind; // Use TQueBind to replace the original QueI and QueO
     ...
 };

 extern "C" __global__ __aicore__ void example_kernel(...)
 {
     ...
     op.Process(...);
 }
```

[Performance Comparison]

**Figure 1**  aiv\_vec\_time comparison before and after optimization<a name="fig74881227195511"></a>  

![](../../../figures/1.png)

As shown in the preceding figure, replacing the DataCopy instruction in the counterexample with TQueBind yields significant optimization. Because the step of copying data from VECIN to VECOUT is omitted, aiv\_vec\_time is reduced to almost 0.

