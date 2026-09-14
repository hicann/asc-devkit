# Enabling AtomicAdd for Matmul<a name="ZH-CN_TOPIC_0000001925216954"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:15:33.179Z -->

[Priority] Medium

[Description] For the result matrix C(m, n) obtained by Matmul, if an Add operation with matrix D(m, n) on GM is subsequently required, you can set the enAtomic parameter to 1 on the GM path of the GetTensorC or IterateAll API to enable the AtomicAdd accumulation operation. When matrix C is moved out to GM, its result is directly accumulated to the GM address of matrix D, thereby implementing the Add operation with matrix D.

[Negative Example]

Move the result matrix C of Matmul and matrix D on GM to UB separately, perform the Add operation, and then move the result out to GM. This requires allocating at least one additional UB memory block for matrix D. Assuming execution on a processor with a separate architecture, three additional data movement operations are performed (moving matrix C from GM to UB, moving matrix D from GM to UB, and moving the Add result from UB to GM).

```
 template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE>
 __aicore__ inline void MatMulKernel(...)
 {
    ...
    AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_MDL> mm;
    TPipe pipe;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm);

    mm.SetTensorA(gm_a);
    mm.SetTensorB(gm_b);
    mm.SetBias(gm_bias);
    mm.IterateAll(gm_c);
    
    DataCopy(local_c, gm_c, c_size);
    DataCopy(local_d, gm_d, d_size);
    event_t eventIdMTE2ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_V));
    SetFlag<HardEvent::MTE2_V>(eventIdMTE2ToV);
    WaitFlag<HardEvent::MTE2_V>(eventIdMTE2ToV);
    Add(local_d, local_d, local_c, d_size);
    DataCopy(gm_d, local_d, d_size);
    ...
 }

 extern "C" __global__ __aicore__ void example_kernel(...)
 {
     ...
     typedef AscendC::MatmulType<TPosition::GM, CubeFormat::ND, half> aType; 
     typedef AscendC::MatmulType<TPosition::GM, CubeFormat::ND, half> bType; 
     typedef AscendC::MatmulType<TPosition::GM, CubeFormat::ND, float> cType; 
     typedef AscendC::MatmulType<TPosition::GM, CubeFormat::ND, float> biasType;
     MatMulKernel<aType, bType, cType, biasType>(...);
     ...
 }
```

[Positive Example]

When computing the Matmul result, call the IterateAll or GetTensorC API to move the result to the GM address of matrix D, and set the enAtomic parameter in the API to 1. When moving the result out to GM, the Matmul result matrix C is accumulated to matrix D, thereby obtaining the result of the Add operation on the two matrices.

```
 template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE>
 __aicore__ inline void MatMulKernel(...)
 {
    ...
    AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_MDL> mm;
    TPipe pipe;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm);

    mm.SetTensorA(gm_a);
    mm.SetTensorB(gm_b);
    mm.SetBias(gm_bias);

    mm.IterateAll(gm_d, 1); // Set enAtomic to 1 in the IterateAll API.
    // while (mm. Iterate()) {
        // mm.GetTensorC(gm_d, 1);     // Set enAtomic to 1 in the GetTensorC API.
    // }
    ...
 }

 extern "C" __global__ __aicore__ void example_kernel(...)
 {
     ...
     typedef AscendC::MatmulType<TPosition::GM, CubeFormat::ND, half> aType; 
     typedef AscendC::MatmulType<TPosition::GM, CubeFormat::ND, half> bType; 
     typedef AscendC::MatmulType<TPosition::GM, CubeFormat::ND, float> cType; 
     typedef AscendC::MatmulType<TPosition::GM, CubeFormat::ND, float> biasType;
     MatMulKernel<aType, bType, cType, biasType>(...);
     ...
 }
```

[Performance Comparison]

**Figure 1**  Performance comparison before and after enabling the AtomicAdd option for Matmul<a name="fig1924944205516"></a>  
![](../../../figures/matmul_atomicadd_option_performance_comparison.png "Performance comparison before and after enabling the AtomicAdd option for Matmul")

Taking the matrix dimensions M=64, N=256, and K=256, with matrix D of (64, 256) as an example, the performance comparison before and after enabling the AtomicAdd option for Matmul is shown in the preceding figure. The average number of cycles changes from 154181 before enabling the AtomicAdd option to 135054 after enabling it, achieving a performance improvement of 12.4%. Therefore, in this scenario, enabling the AtomicAdd option delivers better performance.
