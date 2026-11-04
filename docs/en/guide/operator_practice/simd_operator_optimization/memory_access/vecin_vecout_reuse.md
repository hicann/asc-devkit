# Reusing Pure Data Movement Operators VECIN and VECOUT<a name="ZH-CN_TOPIC_0000001893038937"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:44:39.486Z pushedAt=2026-11-03T07:05:13.339Z nl=2 -->

[Priority] High

[Description] Pure data movement operators do not involve actual vector computation during execution. If redundant vector instructions exist, the overall execution time of the operator increases. In this scenario, you can use the TQueBind API provided by Ascend C for pure data movement operators. This API binds Unified Buffer (UB, VECIN) to UB (VECOUT), eliminating the step of copying data from UB (VECIN) to UB (VECOUT), thereby avoiding unnecessary vector consumption.

[Negative example]

To ensure pipeline synchronization between data movement in and data movement out, this code contains a LocalTensor -\> LocalTensor DataCopy instruction.

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
                 DataCopy(oLocal, iLocal, size); // LocalTensor -> LocalTensor DataCopy instruction for moving data from UB (VECIN) to UB (VECOUT)
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

[Positive example]

Replace the LocalTensor -\> LocalTensor DataCopy instruction with the TQueBind API to reduce the step of copying UB (VECIN) to UB (VECOUT), thereby avoiding redundant copying.

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

[Performance comparison]

**Figure 1**  aiv\_vec\_time Comparison before and after optimization<a name="fig74881227195511"></a>  

![](../../../figures/fig_1.png)

As shown in the preceding figure, replacing the **DataCopy** instruction in the negative example with **TQueBind** yields significant optimization. Since the step of copying data from UB (VECIN) to UB (VECOUT) is omitted, **aiv\_vec\_time** is reduced to almost 0.
