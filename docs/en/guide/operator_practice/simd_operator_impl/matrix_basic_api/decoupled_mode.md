# Decoupled Mode<a name="ZH-CN_TOPIC_0000002532228173"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:15:35.664Z pushedAt=2026-11-03T07:05:13.310Z nl=2 -->

> [!NOTE] Note
> This section provides programming guidance for cube multiplication using the basic APIs in decoupled mode.

For [decoupled mode](../../../programming_guide/advanced_programming/hardware_implementation/basic_architecture.md#section1574769433), due to differences in hardware architecture, the implementation of cube multiplication operators using the basic APIs differs:
- In decoupled mode, the Cube compute unit and the Vector compute unit are deployed separately. Each core has its own Scalar unit and can independently load its own code segment. When writing a kernel function, you can use the function type qualifier \_\_cube\_\_ to identify that the kernel function is executed on the Cube compute unit of the aicore on the device side.
- Decoupled mode supports Fixpipe hardware acceleration, which supports directly moving data from the L0C Buffer to the L1 Buffer/Global Memory/Unified Buffer (UB) while completing functions such as quantization, dequantization, and data layout format conversion.

Following the development process of operator analysis, kernel function definition, and operator class implementation, the following uses the Matmul operator as an example to provide the code framework of the Matmul operator in decoupled mode.

## Operator Analysis<a name="zh-cn_topic_0000002135641293_section11569817102912"></a>

The Matmul operator performs cube multiplication on cube a of shape \[m, k\] and cube b of shape \[k, n\] to obtain cube c of shape \[m, n\]. For simplicity, m = k = n = 32 is used here. The inputs and outputs, computation logic, and the Ascend C APIs to be called for the Matmul operator are listed in the following table.

**Table 1** Ascend C Matmul operator design specifications

| Item | Content |
|------|------|
| Operator type (OpType) | Matmul |
| Operator input a | shape: (m, k) = (32, 32); data type: half; format: [ND](../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md) |
| Operator input b | shape: (k, n) = (32, 32); data type: half; format: [ND](../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md) |
| Operator output c | shape: (m, n) = (32, 32); data type: half; format: [ND](../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md) |
| Kernel function name | matmul_custom |
| Main APIs used | [DataCopy (GMToL1 in-path conversion - ND2NZ movement)](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_ND2NZ.md): data movement from Global Memory to L1 Buffer + ND-to-NZ format conversion API<br>[LoadData (2D cube movement)](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_2D.md): data movement from L1 Buffer to L0 Buffer + NZ-to-ZZ/ZN format conversion API<br>[Mmad](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Mmad.md): cube multiplication computation API<br>[Fixpipe (L0C-to-GM data movement)](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md): data movement from L0C Buffer to Global Memory + NZ-to-ND format conversion + precision conversion API |

## Kernel Function Definition<a name="zh-cn_topic_0000002135641293_section434251315304"></a>

Define the kernel function according to the rules described in [Kernel Function](../../../programming_guide/programming_model/ai_core_simd_programming/kernel_function.md). The kernel function is named matmul\_custom and has three parameters: a, b, and c, where a and b are input memories and c is the output memory. The function type qualifier \_\_global\_\_ is used to identify it as a kernel function that can be called by <<<\>\>\>. The function type qualifier \_\_cube\_\_ is used to identify that the kernel function is executed on the Cube core of the aicore on the device side.

In the kernel function, the Init function of the operator class performs memory initialization, and the Process function implements the core logic of the operator. At the beginning, the kernel function calls `AscendC::InitSocState()` to initialize the hardware state, and at the end, it calls `AscendC::PipeBarrier<PIPE_ALL>()` to wait for all instructions to complete.

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
> The kernel function uses template parameters to pass the cube dimension information, so that the number of loops can be determined at compile time, which facilitates compiler optimization. M, K, and N are the total cube dimensions.

## Operator Class Implementation<a name="zh-cn_topic_0000002135641293_section1882915463510"></a>

In decoupled mode, the implementation of a Matmul cube multiplication operator consists of the following four pipeline stages.

1.  CopyIn task (Global Memory --> L1 Buffer)  
    Use the [DataCopy (GMToL1 in-path conversion - ND2NZ data movement)](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_ND2NZ.md) API to move data from Global Memory to the L1 Buffer, converting the ND format to the NZ format.

2.  Split task (L1 Buffer --> L0 Buffer)    
    Use the [LoadData (2D cube data movement)](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_2D.md) API to move data from the L1 Buffer to the L0A Buffer/L0B Buffer, while completing the format conversion from NZ to ZZ/ZN.

3.  Compute task (cube multiplication)    
    Use the [Mmad](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Mmad.md) API to complete the cube multiplication, with the result stored in the L0C Buffer.

4.  CopyOut task (L0C Buffer --> Global Memory)    
    Use the [Fixpipe (L0C to GM data movement)](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md) API to move the result from the L0C Buffer directly to Global Memory, while using the in-path format conversion capability of Fixpipe to convert the NZ format back to the ND format.

A typical code framework of a Matmul operator class is as follows.

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
    // Copy-in function that completes the CopyIn task of cube A from GM to L1
    __aicore__ inline void CopyInA(...) ()
    {
        // ...
    }
    // Copy-in function that completes the CopyIn task of cube B from GM to L1
    __aicore__ inline void CopyInB(..) ()
    {
        // ...
    }
    // Copy-in function that completes the Split task of cube A from L1 to L0A
    __aicore__ inline void DataLoadA(...) ()
    {
        // ...
    }
    // Copy-in function that completes the Split task of cube B from L1 to L0B
    __aicore__ inline void DataLoadB(...) ()
    {
        // ...
    }
    // Compute function that completes the computation task on L0
    __aicore__ inline void Compute(...) ()
    {
        // ...
    }
    // Copy-out function that completes the CopyOut task of cube C from L0C to GM
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
> The operations in the CopyIn, DataLoad, Compute, and CopyOut phases are executed on different hardware pipelines. To ensure correct data dependencies—that is, the data read in each phase must have been written by the preceding phase—a sync barrier must be set between phases. The global pipeline sync [PipeBarrier](../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/PipeBarrier_ISASI.md) is used here.
