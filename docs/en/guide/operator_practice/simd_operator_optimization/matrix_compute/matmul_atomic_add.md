# Enabling the AtomicAdd Option for Matmul<a name="ZH-CN_TOPIC_0000001925216954"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:35:45.064Z pushedAt=2026-11-03T07:05:13.325Z nl=2 -->

[Priority] Medium

[Description] __For the result cube C\(m, n\) obtained by Matmul, if an Add operation with the cube D\(m, n\) on GM is subsequently required, you can set the enAtomic parameter to 1 on the GM path of the **GetTensorC** API or the **IterateAll** API to enable the AtomicAdd accumulation operation. When cube C is moved out to GM, its result is directly accumulated to the GM address of cube D, thereby implementing the Add operation with cube D.

[Negative example]

Move the result cube C of Matmul and cube D on GM to the Unified Buffer (UB) separately, perform the Add operation, and then move the result out to GM. This requires allocating at least one additional UB memory block for cube D. Assuming execution on a processor with a decoupled architecture, three additional data movement operations are performed (moving cube C from GM to UB, moving cube D from GM to UB, and moving the Add result from UB to GM).

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

[Positive example]

When computing the Matmul result, call the IterateAll API or the GetTensorC API to move the result to the GM address of cube D, and set the enAtomic parameter in the API to 1. When the result is moved out to GM, the Matmul result cube C is accumulated to cube D, thereby obtaining the result of adding the two cubes.

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

    mm.IterateAll(gm_d, 1); // Set enAtomic to 1 in the IterateAll API
    // while (mm. Iterate()) {
        // mm.GetTensorC(gm_d, 1);     // Set enAtomic to 1 in the GetTensorC API
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

[Performance comparison]

**Figure 1**  Performance comparison before and after enabling the AtomicAdd option for Matmul<a name="fig1924944205516"></a>  
![](../../../figures/mm_atomic.png "Performance comparison before and after enabling the AtomicAdd option for Matmul")

Taking cube dimensions M=64, N=256, K=256, and cube D as \(64, 256\) as an example, the performance comparison before and after enabling the AtomicAdd option for Matmul is shown in the preceding figure. The average number of cycles changes from 154181 before enabling the AtomicAdd option to 135054 after enabling it, achieving a performance improvement of 12.4%. Therefore, in this scenario, enabling the AtomicAdd option delivers better performance.
