# Decoupled Mode<a name="ZH-CN_TOPIC_0000002532228173"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-26T13:45:06.376Z -->

>[!NOTE] Note
>This section provides programming guidance for matrix multiplication using basic APIs in decoupled mode.

For [decoupled mode](../../../programming_guide/advanced_programming/hardware_implementation/basic_architecture.md#section1574769433), due to hardware architecture differences, there are differences in implementing the matrix multiplication operator using basic APIs:
- In decoupled mode, the Cube compute unit and the Vector compute unit are deployed separately. Each core has its own Scalar unit and can independently load its own code segment. When writing a kernel function, you can use the function type qualifier \_\_cube\_\_ to indicate that the kernel function is executed on the Cube compute unit of the aicore on the device side.
- The decoupled mode supports Fixpipe hardware acceleration, which supports moving data directly from the L0C Buffer to the L1 Buffer/Global Memory/Unified Buffer while completing functions such as quantization, dequantization, and data layout format conversion.

Following the development process of operator analysis, kernel function definition, and operator class implementation, the following uses the Matmul operator as an example to provide the code framework of the Matmul operator in decoupled mode.

## Operator Analysis<a name="zh-cn_topic_0000002135641293_section11569817102912"></a>

The Matmul operator performs matrix multiplication on matrix a with the shape \[m, k\] and matrix b with the shape \[k, n\] to obtain matrix c with the shape \[m, n\]. For simplicity, m = k = n = 32 is used here. The inputs and outputs, computation logic, and Ascend C APIs to be called for the Matmul operator are shown in the following table.

**Table 1** Ascend C Matmul operator design specifications

| Item | Content |
|------|------|
| Operator type (OpType) | Matmul |
| Operator input a | shape: (m, k) = (32, 32); data type: half; format: [ND](../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout_format.md) |
| Operator input b | shape: (k, n) = (32, 32); data type: half; format: [ND](../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout_format.md) |
| Operator output c | shape: (m, n) = (32, 32); data type: half; format: [ND](../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout_format.md) |
| Kernel function name | matmul_custom |
| Main APIs used | [DataCopy](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_ND2NZ.md): data transfer from Global Memory to L1 Buffer + ND-to-NZ format conversion<br>[Load2D](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/Load2D.md): data transfer from L1 Buffer to L0 Buffer + NZ-to-ZZ/ZN format conversion<br>[Mmad](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Mmad.md): Cube computation API<br>[Fixpipe](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md): data transfer from L0C Buffer to Global Memory + NZ-to-ND format conversion + precision conversion API |

## Kernel Function Definition<a name="zh-cn_topic_0000002135641293_section434251315304"></a>

Define the kernel function according to the rules described in [Kernel Function](../../../programming_guide/programming_model/ai_core_simd_programming/kernel_function.md). The kernel function is named matmul\_custom and has three parameters: a, b, and c, where a and b are input memory and c is output memory. Use the function type qualifier \_\_global\_\_ to identify it as a kernel function that can be invoked by <<<\>\>\>, and use the function type qualifier \_\_cube\_\_ to identify that the kernel function is executed on the Cube core of the device-side aicore.

In the kernel function, the Init function of the operator class completes memory initialization, and the Process function implements the core logic of the operator. At the beginning, the kernel function calls `AscendC::InitSocState()` to initialize the hardware state, and at the end it calls `AscendC::PipeBarrier<PIPE_ALL>()` to wait for all instructions to complete.

```cpp
template <uint32_t M, uint32_t K, uint32_t N>
__global__ __cube__ void mmad_custom(__gm__ uint8* a, __gm__ uint8* b, __gm__ uint8* c)
{
    AscendC::InitSocState();
    KernelMatmul<M, K, N> op;
    op.Init(a, b, c);
    op.Process();
    AscendC::PipeBarrier<PIPE_ALL>();
}
```

> [!NOTE] Note
>
> The kernel function uses template parameters to pass in the matrix dimension information, so that the number of loops can be determined at compile time, which facilitates compiler optimization. Here, M/K/N are the total matrix dimensions.

## Operator Class Implementation<a name="zh-cn_topic_0000002135641293_section1882915463510"></a>

In decoupled mode, a Matmul matrix multiplication operator is implemented through the following four-stage pipeline.

1.  CopyIn task (Global Memory --> L1 Buffer)  
    Use the [DataCopy](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_ND2NZ.md) API to copy data from Global Memory to the L1 Buffer, converting the ND format to the NZ format.

2.  Split task (L1 Buffer --> L0 Buffer)    
    Use the [Load2D](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/Load2D.md) API to copy data from the L1 Buffer to the L0A Buffer/L0B Buffer, completing the NZ-to-ZZ/ZN format conversion at the same time.

3.  Compute task (matrix multiplication computation)    
    Use the [Mmad](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Mmad.md) API to complete the matrix multiplication computation, with the result stored in the L0C Buffer.

4.  CopyOut task (L0C Buffer --> Global Memory)    
    Use the [Fixpipe](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md) API to copy the result from the L0C Buffer directly to Global Memory, while using Fixpipe's on-the-fly format conversion to convert the NZ format back to the ND format.

The code framework of a typical Matmul operator class is as follows.

```cpp
template <uint32_t M, uint32_t K, uint32_t N>
class KernelMatmul {
public:
    __aicore__ inline KernelMatmul() {}
    __aicore__ inline void Init(__gm__ uint8_t* a, __gm__ uint8_t* b, __gm__ uint8_t* c)
    {
        aGM.SetGlobalBuffer((__gm__ half*)a);
        bGM.SetGlobalBuffer((__gm__ half*)b);
        cGM.SetGlobalBuffer((__gm__ half*)c);
    }

    __aicore__ inline void Process()
    {
        AscendC::LocalTensor<half> a1Local(AscendC::TPosition::A1, a1Addr, M * K);
        AscendC::LocalTensor<half> a2Local(AscendC::TPosition::A2, a2Addr, M * K);
        AscendC::LocalTensor<half> b1Local(AscendC::TPosition::B1, b1Addr, K * N);
        AscendC::LocalTensor<half> b2Local(AscendC::TPosition::B2, b2Addr, K * N);
        AscendC::LocalTensor<float> cLocal(AscendC::TPosition::CO1, cAddr, M * N);

        CopyInA(a1Local);
        CopyInB(b1Local);

        AscendC::PipeBarrier<PIPE_ALL>();

        DataLoadA(a1Local, a2Local);
        DataLoadB(b1Local, b2Local);

        AscendC::PipeBarrier<PIPE_ALL>();

        Compute(cLocal, a2Local, b2Local);

        AscendC::PipeBarrier<PIPE_ALL>();
        CopyOut(cLocal);
    }

private:
    // Copy-in function that completes the CopyIn task of matrix A from GM to L1
    __aicore__ inline void CopyInA(...) ()
    {
        // ...
    }
    // Copy-in function that completes the CopyIn task of matrix B from GM to L1
    __aicore__ inline void CopyInB(..) ()
    {
        // ...
    }
    // Copy-in function that completes the Split task of matrix A from L1 to L0A
    __aicore__ inline void DataLoadA(...) ()
    {
        // ...
    }
    // Copy-in function that completes the Split task of matrix B from L1 to L0B
    __aicore__ inline void DataLoadB(...) ()
    {
        // ...
    }
    // Compute function that completes the computation task on L0
    __aicore__ inline void Compute(...) ()
    {
        // ...
    }
    // Copy-out function that completes the CopyOut task of matrix C from L0C to GM
    __aicore__ inline void CopyOut(...) ()
    {
        // ...
    }

private:
    // Private member variables
    // ...
};
```

> [!NOTE] Note
> The operations in the CopyIn, DataLoad, Compute, and CopyOut stages are executed on different hardware pipelines. To ensure correct data dependency, that is, the data read in each stage must have been written by the previous stage, a synchronization barrier must be set between stages. Here, the global pipeline synchronization [PipeBarrier](../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/PipeBarrier_ISASI.md) is used.
