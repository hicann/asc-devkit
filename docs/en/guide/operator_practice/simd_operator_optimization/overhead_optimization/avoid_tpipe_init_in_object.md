# Avoiding Creating and Initializing TPipe Inside an Object<a name="ZH-CN_TOPIC_0000001848187442"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:46:30.356Z pushedAt=2026-11-03T07:05:13.343Z nl=2 -->

[Priority] Medium

[Compiler Background] When a class object is created, memory space is allocated to store the related member variables or functions in the class. When a variable in the class needs to participate in computation, the variable value is loaded from memory into a register, and after the computation is complete, the variable is stored back from the register to memory. Scalar constant folding and constant propagation are compiler optimizations performed at compile time. Before optimization, the compiler determines whether a variable has been initialized or assigned only once. If this precondition for compiler optimization is met, the variable value stays in the register as much as possible, so that subsequent uses of the variable reduce memory read operations and improve runtime performance.

[Description] TPipe is a framework used to manage global memory and synchronization. You can call TPipe APIs to allocate memory for TQue/TBuf. When writing an Ascend C operator, a class is often used to store the related variables required for computation. This class is named KernelExample here. When a TPipe object is defined and initialized in the implementation of the KernelExample class, the memory space of the TPipe object is inside the memory space of the entire KernelExample object. Note that when a TPipe object is created, object initialization sets the TPipe pointer of a global variable, which causes the risk that the memory of the KernelExample object may be polluted externally. In this case, the compiler adopts a conservative strategy for compilation optimization and does not perform constant folding and constant propagation on the Scalar variables in the KernelExample object. Therefore, in all scenarios, we recommend that you create the TPipe object outside the KernelExample class so that the memory space of the TPipe object is independent of the memory space of the KernelExample class object. This triggers the compiler optimization on the Scalar variables in the KernelExample class and reduces the time consumed by Scalar instructions of the operator.

[Negative example]

In the code, the TPipe object is created and initialized inside the KernelExample class, which affects the Scalar folding optimization of the compiler and causes unnecessary Scalar increase on the NPU side.

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

[Positive example]

Change to creating the TPipe object in the kernel entry function, and store the TPipe pointer in the KernelExample class for use.

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

[Performance comparison]

**Figure 1**  aiv\_scalar\_time comparison before and after optimization<a name="fig31681942161513"></a>  
![](../../../figures/aiv_time.png "aiv_scalar_time comparison before and after optimization")

**Figure 2**  aiv\_scalar\_ratio before and after optimization<a name="fig105241506161"></a>  
![](../../../figures/aiv_ratio.png "aiv_scalar_ratio before and after optimization")

The performance data comparison shows that the scalar optimization delivers significant improvement: the average time decreases from 281 us to 236 us, a reduction of 17%; the average scalar\_time latency ratio decreases from 21% to 17%. Therefore, this optimization measure can be used in Scalar-bound (upper limit reached) scenarios.
