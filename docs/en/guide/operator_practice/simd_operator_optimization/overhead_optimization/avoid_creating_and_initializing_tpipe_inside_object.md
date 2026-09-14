# Avoid Creating and Initializing TPipe Inside an Object<a name="ZH-CN_TOPIC_0000001848187442"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:30:44.263Z -->

[Priority] Medium

[Compiler Background] When a class object is created, memory space is allocated to store the related member variables or functions of the class. When a variable in the class needs to participate in computation, its value is loaded from memory into a register; after the computation is complete, the variable is stored back from the register to memory. Scalar constant folding and constant propagation are compiler optimizations performed at compile time. Before optimization, the compiler checks whether a variable has been initialized or assigned only once. If this precondition is met, the variable value is kept in registers as much as possible, so that subsequent uses of the variable reduce memory read operations and improve runtime performance.

[Description] TPipe is a framework used to manage global memory and synchronization. You can call TPipe APIs to allocate memory for TQue/TBuf. When writing an Ascend C operator, a class is often used to store the variables required for computation; this class is referred to as KernelExample here. When a TPipe object is defined and initialized inside the implementation of the KernelExample class, the memory space of the TPipe object lies within the memory space of the entire KernelExample object. Note that when a TPipe object is created, its initialization sets the TPipe pointer of a global variable, which risks the memory of the KernelExample object being polluted externally. In this case, the compiler adopts a conservative optimization strategy and does not perform constant folding and constant propagation on the Scalar variables inside the KernelExample object. Therefore, in all scenarios, we recommend creating the TPipe object outside the KernelExample class so that the memory space of the TPipe object is independent of that of the KernelExample object. This triggers the compiler to optimize the Scalar variables inside the KernelExample class and reduces the time consumed by Scalar instructions in the operator.

[Negative Example]

In the code, the TPipe object is created and initialized inside the KernelExample class, which affects the compiler's Scalar folding optimization and causes unnecessary Scalar instructions on the NPU side.

```
template <typename ComputeT> class KernelExample {
 public:
     __aicore__ inline KernelExample() {}

     __aicore__ inline void Init(...)
     {
         ...
         pipe.InitBuffer(xxxBuf, BUFFER_NUM, xxxSize);
         ...
     }

 private:
     ...
     TPipe pipe;
     ...
 };

 extern "C" __global__ __aicore__ void example_kernel(...)
 {
     ...
     KernelExample<float> op;
     op.Init(...);
     ...
 }
```

[Positive Example]

Instead, create the TPipe object in the Kernel entry function and store the TPipe pointer in the KernelExample class for use.

```
template <typename ComputeT> class KernelExample {
 public:
     __aicore__ inline KernelExample() {}

     __aicore__ inline void Init(..., TPipe* pipeIn)
     {
         ...
         pipe = pipeIn;
         pipe->InitBuffer(xxxBuf, BUFFER_NUM, xxxSize);
         ...
     }

 private:
     ...
     TPipe* pipe;
     ...
 };

 extern "C" __global__ __aicore__ void example_kernel(...)
 {
     ...
     TPipe pipe;
     KernelExample<float> op;
     op.Init(..., &pipe);
     ...
 }
```

[Performance Comparison]

**Figure 1**  aiv\_scalar\_time before and after optimization<a name="fig31681942161513"></a>  
![](../../../figures/aiv_scalar_time_before_and_after_optimization.png "aiv_scalar_time before and after optimization")

**Figure 2**  aiv\_scalar\_ratio before and after optimization<a name="fig105241506161"></a>  
![](../../../figures/aiv_scalar_ratio_before_and_after_optimization.png "aiv_scalar_ratio before and after optimization")

The performance data shows that Scalar optimization delivers significant improvement. The average time decreases from 281 us to 236 us, a reduction of 17%, and the average scalar\_time latency ratio drops from 21% to 17%. Therefore, this optimization measure can be used in Scalar bound (upper limit reached) scenarios.

