# Matmul and LeakyRelu Fusion Computation Example

## Operator Overview

This example implements Matmul and LeakyRelu fusion computation based on the static Tensor programming mode (a programming approach where tensor shapes and memory layouts are determined at compile time). It demonstrates the collaborative computing programming pattern between the Cube unit (matrix computation unit that executes matrix operations such as Matmul) and the Vector unit (vector computation unit that executes element-wise operations). The Cube core completes the matrix multiplication computation, and the Vector core completes the LeakyRelu activation computation.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── matmul_leakyrelu_basic_api
│   ├── scripts
│   │   ├── gen_data.py                 // Input data and golden data generation script
│   │   └── verify_result.py            // Golden value comparison file
│   ├── CMakeLists.txt                  // Compilation project file
│   ├── data_utils.h                    // Data read and write functions
│   ├── matmul_leakyrelu_basic_api.asc  // Ascend C example implementation & invocation example
│   └── README.md                       // Example documentation
```

## Operator Description

- Example Function:
  Implements Matmul and LeakyRelu fusion computation. The computation formula is as follows:

  Matmul Computation:
  $$
  C_{ij} = \sum_{k} A_{ik} \times B_{kj}
  $$

  LeakyRelu Computation:
  $$
  C_{ij} = \begin{cases}
  C_{ij} & \text{if } C_{ij} \geq 0 \\
  C_{ij} \times 0.001 & \text{if } C_{ij} < 0
  \end{cases}
  $$

  Where A is the left matrix with shape [M, K]; B is the right matrix with shape [K, N]; C is the output matrix with shape [M, N].

- Example Specifications:
  This example has parameters M = 512, K = 128, N = 128. It invokes 2 Cube cores and 4 Vector cores to complete the computation. The input specifications are shown in the following table:

  <table>
  <tr><td rowspan="1" align="center">Example Type(OpType)</td><td colspan="4" align="center">Matmul+LeakyRelu Fusion</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">A (left matrix)</td><td align="center">[512, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">B (right matrix)</td><td align="center">[128, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">C</td><td align="center">[512, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">mmad_vec_custom</td></tr>
  </table>

  **Core Distribution Logic**:

  The output matrix C is divided into 2 blocks along the M direction (shape first dimension, row direction), while the N direction (shape second dimension, column direction) is not divided. Two Cube cores complete the Matmul computation. Each Cube core corresponds to 2 Vector cores, so 4 Vector cores complete the LeakyRelu computation. Each Cube core produces a baseM×baseN result block, corresponding to 2 Vector cores each processing baseM/2×baseN data.

  **M/N/K Parameter Illustration**:

  In matrix multiplication C = A × B, M/N/K correspond to different dimensions of the matrices, and core distribution offsets also revolve around these dimensions:

  ```
           K                N
       ┌─────────┐      ┌──────────┐
       │         │      │          │
     M │    A    │  × K │    B     │  =  C (M × N)
       │         │      │          │
       └─────────┘      └──────────┘
  ```

  - M direction block count: M / singleCoreM = 512 / 256 = 2
  - N direction block count: N / singleCoreN = 128 / 128 = 1
  - Total Cube cores: 2 × 1 = 2
  - Total Vector cores: 2 × 2 = 4

- Operator Implementation:
  - **Overall Computation Flow**:

    The overall flow of the example is as follows (showing Cube core and Vector core collaborative computation):

      > **Preliminary Note: Ascend AI Core Memory Hierarchy and Data Pathways**
      >
      > Ascend AI Core has multiple levels of on-chip cache. Data moves from external DDR (Global Memory, GM) to computation units level by level:
      > - **GM (Global Memory)**: External DDR on the chip, storing input/output data, with the largest capacity but highest access latency
      > - **L1**: On-chip cache (mainly stores A/B matrix data on the Cube side), moved from GM by the MTE2 engine
      > - **L0A/L0B**: Input cache for Cube matrix computation unit, moved from L1 by the MTE1 engine
      > - **L0C**: Output cache for Cube matrix computation unit, storing Mmad computation results
      > - **UB (Unified Buffer, corresponding to TPosition::VECCALC in AscendC)**: On-chip cache on the Vector side, storing input/output data required for Vector computation
      >
      > Data requires format conversion (ND→Nz/Zn) during the transfer process because the Cube computation unit requires a specific Fractal layout format rather than ND layout.
      >
      > Synchronization Mechanism Description:
      > - **Intra-core Synchronization**: [SetFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md)/[WaitFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) are used for dependency synchronization between different pipeline engines within the same core, such as notifying MTE1 that it can start transferring after MTE2 completes
      > - **Inter-core Synchronization**: [CrossCoreSetFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核间同步/CrossCoreSetFlag(ISASI).md)/[CrossCoreWaitFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核间同步/CrossCoreWaitFlag(ISASI).md) are used for dependency synchronization between different cores, such as notifying Vector cores that they can start reading data after Cube core completes computation

      **1) Data Flow Path**

      Cube side data flow:
      ```
      GM(A:ND,half) -> L1(A:Nz,half) -> L0A(A:Nz,half) -
                  │                  │                 │
                DataCopy            LoadData            │
                ND->Nz              Nz->Zz/Nz           │
                                                        │--->L0C(Nz,float) -> GM(C:ND,half)
                                                        │  │                  │
                                                        │ Mmad             Fixpipe
                                                        │ C=A×B       Nz->ND, float32->half
                                                        │
      GM(B:ND,half) -> L1(B:Nz,half) -> L0B(B:Zn,half) -
                  │                  │
                DataCopy            LoadData
                ND->Nz              Nz->Zn(transpose)
      ```

      > **Note**: L0A fractal format differs across products:
      > - Ascend 950PR/Ascend 950DT products: L0A fractal format is Nz
      > - Atlas A2/A3 series products: L0A fractal format is Zz

      Inter-core synchronization and Vector side data flow (1 Cube core's result is processed by 2 Vector cores):
      ```
      Cube core:  Fixpipe -> GM(C:ND,half)
                          │
                          │ CrossCoreSetFlag / CrossCoreWaitFlag
                          ▼
      Vector core: GM(C:ND,half) -> UB(VECCALC,half) -> UB(VECCALC,half) -> GM(C:ND,half)
                            │                   │                   │
                        DataCopyPad          LeakyRelu          DataCopyPad
                  MTE2 transfer(baseM/2×baseN)    VEC compute       MTE3 write out
      ```

      **2) Flow Details**:

      1. **Cube Core Computation Phase**:

          **GlobalTensor Definition and Core Distribution Offset**: Use [GlobalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/GlobalTensor/GlobalTensor简介.md) (global memory tensor) to access input/output data in the external DDR of the chip, and use [GetBlockIdx](../../../../../docs/zh/api/SIMD-API/基础API/工具接口/系统资源与变量/GetBlockIdx.md) (get the logical index of the current core) to calculate the core distribution offset:

          ```cpp
          class KernelMatmul {
          public:
              __aicore__ inline void Init(__gm__ uint8_t* xMatrix, __gm__ uint8_t* yMatrix, __gm__ uint8_t* zMatrix)
              {
                  // Set GlobalTensor start address, pointing to the corresponding input/output matrix in GM
                  aGM.SetGlobalBuffer((__gm__ half*)xMatrix);
                  bGM.SetGlobalBuffer((__gm__ half*)yMatrix);
                  cGM.SetGlobalBuffer((__gm__ half*)zMatrix);

                  // Cube cores are distributed along M direction, each Cube core is responsible for singleCoreM * N output block
                  if ASCEND_IS_AIC {
                      aGM = aGM[AscendC::GetBlockIdx() * singleCoreM * K];  // Offset to the A matrix row assigned to current core
                      cGM = cGM[AscendC::GetBlockIdx() * singleCoreM * N];  // Offset to the C matrix row assigned to current core
                  }
                  // Vector core count is 2x Cube core count, 1 Cube core's result is processed by 2 Vector cores
                  // GetBlockIdx() / 2 maps Vector core index back to the corresponding Cube core output block start address
                  if ASCEND_IS_AIV {
                      cGM = cGM[AscendC::GetBlockIdx() / 2 * singleCoreM * N];
                  }
              }
          // ...
          private:
              AscendC::GlobalTensor<half> aGM;  // Left matrix A, [M, K]
              AscendC::GlobalTensor<half> bGM;  // Right matrix B, [K, N]
              AscendC::GlobalTensor<half> cGM;  // Output matrix C, [M, N]
          };
          ```

          **Memory Transfer and Computation Flow**:

          - **LocalTensor Creation**: Use [LocalMemAllocator](../../../../../docs/zh/api/SIMD-API/基础API/资源管理/LocalMemAllocator/LocalMemAllocator简介.md) (on-chip memory allocator that automatically allocates in application order, avoiding manual address offset maintenance) to create [LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md) (on-chip memory tensor) for each on-chip cache. The temporary space for A and B matrices in L1 is allocated by the same L1 allocator in application order to avoid manual L1 address offset maintenance
          - **GM → L1**: Use [DataCopy](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（GMToL1随路转换-ND2NZ搬运）.md) to transfer A and B matrices from GM to L1, completing ND to Nz format conversion (Cube computation unit requires Nz fractal layout, so ND format must be converted to Nz format during transfer)
          - **L1 → L0A/L0B**: Use [LoadData](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md) to transfer data to L0A and L0B. B matrix requires transpose (Nz→Zn)
          - **L0A/L0B → L0C**: Use [Mmad](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md) (matrix multiply-accumulate instruction) to execute matrix multiply-add, accumulating all data blocks along the K axis
          - **L0C → GM**: Use [Fixpipe](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md) to transfer results out to GM, completing Nz to ND format conversion and float32 to half type conversion

          Code example:

          ```cpp
          __aicore__ inline void Process()
          {
              if ASCEND_IS_AIC {
                  // ==================== Local Tensor Allocation ====================
                  // Use LocalMemAllocator to automatically allocate on-chip memory in application order, avoiding manual address offset maintenance
                  AscendC::LocalMemAllocator<AscendC::Hardware::L1> l1Allocator;
                  AscendC::LocalMemAllocator<AscendC::Hardware::L0A> l0aAllocator;
                  AscendC::LocalMemAllocator<AscendC::Hardware::L0B> l0bAllocator;
                  AscendC::LocalMemAllocator<AscendC::Hardware::L0C> l0cAllocator;
                  // L1: temporary storage for A/B matrices; L0A/L0B: Cube computation unit input; L0C: Cube computation unit output
                  // The TPosition template parameter of Alloc can be omitted, the Alloc function will provide default value based on the physical location corresponding to the allocator
                  AscendC::LocalTensor<half> a1Local = l1Allocator.Alloc<half>(baseM * baseK);
                  AscendC::LocalTensor<half> b1Local = l1Allocator.Alloc<half>(baseK * baseN);
                  AscendC::LocalTensor<half> a2Local = l0aAllocator.Alloc<half>(baseM * baseK);
                  AscendC::LocalTensor<half> b2Local = l0bAllocator.Alloc<half>(baseK * baseN);
                  AscendC::LocalTensor<float> cLocal = l0cAllocator.Alloc<float>(baseM * baseN);

                  // ==================== GM → L1: DataCopy + ND→Nz Format Conversion ====================
                  // DataCopy completes ND to Nz format conversion during transfer (Cube computation unit requires Nz fractal layout)
                  AscendC::DataCopy(a1Local, aGM, AscendC::Nd2NzParams{1, baseM, baseK, 0, K, baseM, 1, 0});
                  AscendC::DataCopy(b1Local, bGM, AscendC::Nd2NzParams{1, baseK, baseN, 0, N, baseK, 1, 0});
                  // Intra-core synchronization: notify MTE1 that it can start transfer after MTE2 transfer completes
                  AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE1>(EVENT_ID0);
                  AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE1>(EVENT_ID0);

                  // ==================== L1 → L0A/L0B: LoadData ====================
                  // ⚠️ Architecture constraint: L0A layout format differs across products, LoadData parameters need to be adjusted accordingly
                  // A2/A3(2201) uses Zz layout requiring block-by-block transfer, 950PR(3510) uses Nz layout allowing one-time completion
                  // A2/A3 architecture (dav-2201): L0A layout is Zz format, requires block-by-block transfer
          #if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
                  for (int i = 0; i < baseM / CUBE_BLOCK; ++i) {
                      AscendC::LoadData(a2Local[i * baseK * CUBE_BLOCK], a1Local[i * 512 / sizeof(half)],
                          AscendC::LoadData2DParams{0, baseK / CUBE_BLOCK, baseM / CUBE_BLOCK, 0, 0, false, 0});
                  }
                  // B matrix Nz→Zn transpose transfer
                  for (int i = 0; i < baseK / CUBE_BLOCK; ++i) {
                      AscendC::LoadData(b2Local[i * baseN * CUBE_BLOCK], b1Local[i * 512 / sizeof(half)],
                          AscendC::LoadData2DParams{0, baseN / CUBE_BLOCK, baseK / CUBE_BLOCK, 0, 0, true, 0});
                  }
                  // Ascend 950PR architecture (dav-3510): L0A layout is Nz format, one-time transfer completion
          #elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
                  AscendC::LoadData(a2Local, a1Local,
                      AscendC::LoadData2DParamsV2{0, 0, baseM / CUBE_BLOCK, baseK / CUBE_BLOCK,
                          baseM / CUBE_BLOCK, baseM / CUBE_BLOCK, false, 0});
                  AscendC::LoadData(b2Local, b1Local,
                      AscendC::LoadData2DParamsV2{0, 0, baseK / CUBE_BLOCK, baseN / CUBE_BLOCK,
                          baseK / CUBE_BLOCK, baseN / CUBE_BLOCK, true, 0});
          #endif
                  // Intra-core synchronization: notify M pipeline that Mmad computation can start after MTE1 transfer completes
                  AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID0);
                  AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID0);

                  // ==================== L0A/L0B → L0C: Mmad Matrix Multiply-Accumulate ====================
                  // cmatrixInitVal=true: Initialize L0C accumulator to 0 for first computation
                  AscendC::Mmad(cLocal, a2Local, b2Local,
                      AscendC::MmadParams{baseM, baseN, baseK, 0, false, true});

                  // ==================== L0C → GM: Fixpipe Transfer Out + Nz→ND + fp32→fp16 ====================
                  // Intra-core synchronization: notify FIX pipeline that Fixpipe can start after M pipeline completes
                  AscendC::SetFlag<AscendC::HardEvent::M_FIX>(EVENT_ID0);
                  AscendC::WaitFlag<AscendC::HardEvent::M_FIX>(EVENT_ID0);
                  // Fixpipe transfers L0C results out to GM, simultaneously completing NZ→ND format conversion and float32→half precision conversion
                  AscendC::Fixpipe(cGM, cLocal,
                      AscendC::FixpipeParamsV220{baseN, baseM, baseM, N, false, QuantMode_t::F322F16, 0, 1, 0, 0, 0});

                  // ==================== Inter-core Synchronization: Notify Vector Cores Data is Ready ====================
                  AscendC::CrossCoreSetFlag<0x2, PIPE_FIX>(0);
              }
              // ... Vector core part see below
          }
          ```

      2. **Vector Core Computation Phase**:
          - **Inter-core Synchronization**: Vector cores use [CrossCoreWaitFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核间同步/CrossCoreWaitFlag(ISASI).md) (inter-core synchronization flag wait, blocks until the flag is set) to wait for Cube core to complete Fixpipe writeback, ensuring LeakyRelu starts only after Matmul computation completes
          - **LocalTensor Creation**: Use UB allocator to create [LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md) at the VECCALC position to carry the half-block result processed by the current Vector core
          - **GM → UB**: Use [DataCopyPad](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopyPad（GMToUB非对齐数据搬运）.md) (data transfer between GM and UB with padding) to transfer Matmul results to UB. Each Vector core processes baseM/2×baseN data
          - **UB Computation**: Use [LeakyRelu](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/基础算术/LeakyRelu.md) (LeakyReLU activation function, negative values are multiplied by negativeSlope) to execute activation computation, with negative values multiplied by 0.001
          - **UB → GM**: Use [DataCopyPad](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopyPad（UBToGM非对齐数据搬运）.md) to write results back to GM, completing the fusion computation

          Code example:

          ```cpp
              if ASCEND_IS_AIV {
                  // ==================== UB Local Tensor Allocation ====================
                  AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
                  // Each Vector core processes baseM/2 × baseN elements (1 Cube core's result is split evenly between 2 Vector cores)
                  AscendC::LocalTensor<half> vecLocal = ubAllocator.Alloc<half>(baseM / 2 * baseN);

                  // ==================== Inter-core Synchronization: Wait for Cube Core Fixpipe Completion ====================
                  AscendC::CrossCoreWaitFlag(0);

                  // ==================== GM → UB: DataCopyPad Read Matmul Results ====================
                  // Calculate the starting offset of current Vector core in GM
                  // GetBlockIdx() % 2 distinguishes between 2 AIVs within the same logical core, processing different half-block data
                  uint32_t gmOffset = AscendC::GetBlockIdx() % 2 * (baseM / 2 * N);
                  uint32_t blockLen = baseN * sizeof(half);
                  uint32_t srcStride = (N - baseN) * sizeof(half);  // Jump stride between adjacent rows in GM
                  AscendC::DataCopyPad<half>(
                      vecLocal, cGM[gmOffset],
                      AscendC::DataCopyExtParams{static_cast<uint16_t>(baseM / 2), blockLen, srcStride, 0, 0},
                      AscendC::DataCopyPadExtParams<half>{true, 0, 0, 0});
                  // Intra-core synchronization: notify V pipeline that computation can start after MTE2 transfer completes
                  AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
                  AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

                  // ==================== UB Computation: LeakyRelu Activation ====================
                  // LeakyRelu(x) = x >= 0 ? x : x * 0.001
                  AscendC::LeakyRelu(vecLocal, vecLocal, (half)0.001, baseM / 2 * baseN);
                  // Intra-core synchronization: notify MTE3 that writeback can start after V pipeline completes
                  AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
                  AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);

                  // ==================== UB → GM: DataCopyPad Write Back Results ====================
                  AscendC::DataCopyPad<half>(cGM[gmOffset], vecLocal,
                      AscendC::DataCopyExtParams{static_cast<uint16_t>(baseM / 2), blockLen, 0, srcStride, 0});
              }
          }
          ```

  - **Constraints**:
    1. baseM/baseK/baseN satisfy 16-alignment
    2. M/N are divisible by singleCoreM/singleCoreN
    3. singleCoreM/singleCoreN are divisible by baseM/baseN, K is divisible by baseK, non-integer division scenarios are not supported
    4. Vector core count is 2 times the Cube core count

  - **Invocation Implementation**:
    The kernel call operator `__mix__(1, 2)` implements collaborative invocation of Cube and Vector cores, where the parameter `(1, 2)` indicates that each logical core consists of 1 AIC (Cube core) and 2 AIVs (Vector cores), with a Cube:Vector core ratio of 1:2.

    ```cpp
    // __global__: kernel function declaration modifier
    // __mix__(1,2): Mixed kernel function, 1 AIC(Cube) + 2 AIV(Vector) form a logical core
    __global__ __mix__(1, 2) void mmad_vec_custom(__gm__ uint8_t* xMatrix, __gm__ uint8_t* yMatrix, __gm__ uint8_t* zMatrix)
    {
        AscendC::InitSocState();  // Initialize hardware state
        KernelMatmul<M, K, N, singleCoreM, singleCoreK, singleCoreN,
                     baseM, baseK, baseN> op;
        op.Init(xMatrix, yMatrix, zMatrix);   // GlobalTensor initialization and core distribution offset
        op.Process();        // AIC side executes Matmul, AIV side executes LeakyRelu
        AscendC::PipeBarrier<PIPE_ALL>();  // Wait for all pipeline stages to complete
    }
    ```

- **Interface Parameter Description**

  The following structures are all passed using curly braces `{}`. The meaning of each field is as follows (field order is consistent with API documentation; actual struct declaration may have different field order):

  **[AscendC::Nd2NzParams](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（GMToL1随路转换-ND2NZ搬运）.md)** — Used by the `DataCopy` interface to describe ND→Nz format conversion parameters:
  ```cpp
  struct Nd2NzParams {
      int32_t  ndNum;              // Number of ND matrices to transfer, [0, 4095]
      uint16_t nValue;             // Number of rows in ND matrix, [0, 16384]
      int32_t  dValue;             // Number of columns in ND matrix, [0, 65535]
      int32_t  srcNdMatrixStride;  // Offset between starting addresses of adjacent ND matrices, unit: elements, [0, 65535]
      int32_t  srcDValue;          // Offset between adjacent rows of the same ND matrix, unit: elements, [1, 65535]
      uint16_t dstNzC0Stride;      // Adjacent offset of multiple rows converted from the same source row in destination Nz, unit: C0_SIZE(32B), [1, 16384]
      uint16_t dstNzNStride;       // Adjacent row offset of Z-shaped matrix in destination Nz, unit: C0_SIZE(32B), [1, 16384]
      int32_t  dstNzMatrixStride;  // Starting address offset between adjacent Nz matrices in destination Nz, unit: elements, [1, 65535]
  };
  ```
  For example, when transferring matrix A with `{1, baseM, baseK, 0, K, baseM, 1, 0}`, it converts baseM×baseK ND data to Nz format.

  **[AscendC::LoadData2DParams](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md)** — Used by the `LoadData` interface to describe data transfer parameters for matrix A from L1 to L0A and matrix B from L1 to L0B in Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products:
  ```cpp
  struct LoadData2DParams {
      int32_t startIndex;   // Fractal matrix ID (0 is the first), unit: 512B, [0, 65535]
      int32_t repeatTimes;  // Number of iterations, each iteration processes 512B, [1, 255]
      int32_t srcStride;    // Interval between starting addresses of source fractals in adjacent iterations, unit: 512B, [0, 65535]
      int32_t sid;          // Reserved, set to 0
      int32_t dstGap;       // Interval between destination fractals in adjacent iterations, unit: 512B, [0, 65535]
      bool    ifTranspose;  // Whether to transpose each fractal, default false
      bool    addrMode;     // Address update mode, false=increment, true=decrement, default false
  };
  ```
  For example: In Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, the layout format on L0A is Zz. When transferring matrix A, use `{0, baseK / CUBE_BLOCK, baseM / CUBE_BLOCK, 0, 0, false, 0}`;<br>
  When transferring matrix B, set `ifTranspose=true` to complete Nz to Zn transpose transfer.

  **[AscendC::LoadData2DParamsV2](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D_V2.md)** — Used by the `LoadData` interface to describe data transfer parameters for matrix A from L1 to L0A and matrix B from L1 to L0B in Ascend 950PR/Ascend 950DT products:
  ```cpp
  struct LoadData2DParamsV2 {
      uint32_t mStartPosition;  // Starting position in M direction, unit: 512B
      uint32_t kStartPosition;  // Starting position in K direction, unit: 512B
      uint16_t mStep;           // Number of fractals to transfer in M direction
      uint16_t kStep;           // Number of fractals to transfer in K direction
      int32_t  srcStride;       // Interval between adjacent K-direction fractals at source, unit: 512B
      uint16_t dstStride;       // Interval between adjacent K-direction fractals at destination, unit: 512B
      bool     ifTranspose;     // Whether to transpose each fractal, default false
      uint8_t  sid;             // Reserved, set to 0
  };
  ```
  In Ascend 950PR/Ascend 950DT products, the layout format on L0A is Nz. When transferring matrix A, use `{0, 0, baseM / CUBE_BLOCK, baseK / CUBE_BLOCK, baseM / CUBE_BLOCK, baseM / CUBE_BLOCK, false, 0}` to complete A matrix Nz to Nz transfer in one step; when transferring matrix B, use `{0, 0, baseK / CUBE_BLOCK, baseN / CUBE_BLOCK, baseK / CUBE_BLOCK, baseN / CUBE_BLOCK, true, 0}` to complete B matrix Nz to Zn transfer in one step.

  **[AscendC::MmadParams](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md)** — Used by the `Mmad` interface to describe matrix multiplication parameters:
  ```cpp
  struct MmadParams {
      uint16_t m;               // Left matrix Height (M dimension), [0, 4095]
      uint16_t n;               // Right matrix Width (N dimension), [0, 4095]
      uint16_t k;               // Left matrix Width/Right matrix Height (K dimension), [0, 4095]
      uint16_t unitFlag;        // Fine-grained parallel control between Mmad and Fixpipe, default 0
      bool     cmatrixSource;   // C matrix initial value source, false=CO1, true=C2, default false
      bool     cmatrixInitVal;  // Whether C matrix initial value is 0, default true
  };
  ```
  For example, `{baseM, baseN, baseK, 0, false, true}` computes a baseM×baseN output block and accumulates along the K direction for baseK length.

  **[AscendC::FixpipeParamsV220](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)** — Used by the `Fixpipe` interface to describe data transfer and precision conversion parameters from L0C to GM:
  ```cpp
  struct FixpipeParamsV220 {
      int32_t     nSize;        // Source Nz matrix N dimension size, [1, 4095]
      uint16_t    mSize;        // Source Nz matrix M dimension size (for Nz2ND [1, 8192])
      uint16_t    srcStride;    // Starting offset of adjacent Z layout in source Nz, unit: C0_SIZE, [0, 65535]
      int32_t     dstStride;    // Number of elements per row in destination ND matrix for Nz2ND, unit: element
      bool        reluEn;       // Whether to enable ReLU
      QuantMode_t quantPre;     // Quantization mode, F322F16 indicates float→half
      uint64_t    deqScalar;    // Scalar quantization parameter, single scale value
      int32_t     ndNum;        // Number of source Nz matrices, [1, 65535]
      int32_t     srcNdStride;  // Starting address interval between different Nz matrices, unit: 16×C0_SIZE, [1, 512]
      int32_t     dstNdStride;  // Offset between adjacent destination ND matrices, unit: element, [1, 65535]
      int32_t     unitFlag;     // Parallel control between Mmad and Fixpipe
  };
  ```
  For example, `{baseN, baseM, baseM, N, false, F322F16, 0, 1, 0, 0, 0}` converts the baseM×baseN float32 result in L0C to half and writes it back to GM.

  **[AscendC::DataCopyExtParams](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopyPad（GMToUB非对齐数据搬运）.md)** — Used by the `DataCopyPad` interface to describe block transfer parameters between GM and UB:
  ```cpp
  struct DataCopyExtParams {
      uint16_t blockCount;  // Number of consecutive data blocks to transfer
      uint32_t blockLen;    // Length of each data block, unit: Byte
      uint32_t srcStride;   // Interval between adjacent data blocks at source, unit: Byte
      uint32_t dstStride;   // Interval between adjacent data blocks at destination, unit: Byte
      uint32_t rsv;         // Reserved field, set to 0
  };
  ```
  For example, when transferring from GM to UB with `{static_cast<uint16_t>(baseM / 2), blockLen, srcStride, 0, 0}`, each Vector core reads baseM/2 rows of results.

  **[AscendC::DataCopyPadExtParams\<half\>](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopyPad（GMToUB非对齐数据搬运）.md)** — Used by the `DataCopyPad` interface to describe tail block padding parameters:
  ```cpp
  template <typename T>
  struct DataCopyPadExtParams {
      bool isPad;       // Whether to enable padding
      uint8_t leftPad;  // Left padding length
      uint8_t rightPad; // Right padding length
      T paddingValue;   // Padding value
  };
  ```
  For example, `{true, 0, 0, 0}` indicates enabling padding capability, but no additional padding on either left or right side.

## Compilation and Execution

Execute the following steps in the root directory of this example to compile and run the example.

- Configure Environment Variables

  Configure the environment variables according to the [installation method](../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit package in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, it defaults to `/usr/local/Ascend`.

- Example Execution

  Execute the following commands in this example directory.
  ```bash
  mkdir -p build && cd build;                                               # Create and enter build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # Compile project (default npu mode)
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Execute the compiled executable program to run the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify output results for correctness, confirm algorithm logic is correct
  ```

  When using CPU debugging or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debugging mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Before switching compilation modes, you need to clean the cmake cache. You can execute `rm CMakeCache.txt` in the build directory and then run cmake again.

- Compilation Option Description

  | Option | Possible Values | Description |
  | -------| ----------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debugging, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution Result

  The execution result is as follows, indicating precision comparison passed.
  ```bash
  test pass!
  ```

## Implementation Flow Analysis

The following table details each step of the Cube core and Vector core operations in chronological order:

### Cube Core Flow

| Stage | Data Flow/Behavior | Implementation Purpose/Reason |
|:---|:---|:---|
| Initialization | Use [LocalMemAllocator](../../../../../docs/zh/api/SIMD-API/基础API/资源管理/LocalMemAllocator/LocalMemAllocator简介.md) to allocate [LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md) for on-chip caches such as L1, L0A/L0B, L0C | Automatically allocate on-chip memory in application order, avoiding manual address offset maintenance |
| GM → L1 | [DataCopy](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（GMToL1随路转换-ND2NZ搬运）.md) transfers A/B matrices from GM to L1, simultaneously completing ND→Nz format conversion | Cube computation unit requires Nz fractal layout format, so ND format must be converted to Nz format during transfer to avoid additional conversion overhead |
| L1 → L0A/L0B | [LoadData](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md) transfers A matrix from L1 to L0A (Nz→Zz/Nz), B matrix from L1 to L0B (Nz→Zn transpose) | B matrix requires transpose because the Mmad instruction requires B matrix to be input in Zn (transposed Nz) format |
| L0A/L0B → L0C | [Mmad](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md) executes matrix multiply-add, accumulating all data blocks along the K direction | Completes A×B matrix multiplication computation, K-direction block accumulation ensures correctness |
| Intra-core Synchronization | [SetFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md)/[WaitFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) ensures data transfer completes before starting the next operation | Avoids LoadData reading data that has not finished transferring, avoids Mmad reading data that has not finished loading |
| L0C → GM | [Fixpipe](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md) transfers L0C results out to GM, simultaneously completing Nz→ND format conversion and fp32→fp16 precision conversion | Output results need to return to GM for Vector cores to read, format needs to be converted to ND layout, precision needs to be reduced from fp32 to fp16 to match output requirements |
| Inter-core Synchronization | [CrossCoreSetFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核间同步/CrossCoreSetFlag(ISASI).md) notifies Vector cores that data is ready | Ensures Vector cores do not start reading GM data before Fixpipe completes |

### Vector Core Flow

| Stage | Data Flow/Behavior | Implementation Purpose/Reason |
|:---|:---|:---|
| Inter-core Synchronization | [CrossCoreWaitFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核间同步/CrossCoreWaitFlag(ISASI).md) waits for Cube core Fixpipe to complete | Blocks until Cube core notifies data is ready, ensuring complete Matmul results are read |
| Initialization | Use UB allocator to allocate [LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md) at VECCALC position | Allocates UB buffer for GM→UB transfer and Vector computation |
| GM → UB | [DataCopyPad](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopyPad（GMToUB非对齐数据搬运）.md) transfers Matmul results from GM to UB, each Vector core reads baseM/2 rows, with baseN elements per row | Vector cores read Matmul results written back by Cube cores from GM, each Vector core processes half-block data |
| UB Computation | [LeakyRelu](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/基础算术/LeakyRelu.md) executes activation computation, negative values are multiplied by 0.001 | Applies LeakyRelu activation function to Matmul results, completing fusion computation |
| UB → GM | [DataCopyPad](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopyPad（UBToGM非对齐数据搬运）.md) writes LeakyRelu results back to GM | Outputs final computation results to GM for subsequent use |

## Optimization Directions Analysis

| Optimization Direction | Issue with Current Implementation | Expected Optimization Benefit |
|:---|:---|:---|
| L1/L0 Double Buffering Pipeline | Cube core executes GM→L1→L0A/L0B→L0C→GM stages serially, transfer and computation cannot overlap | Ping-Pong double buffering enables parallel MTE2 transfer and MTE1/Mmad computation, significantly improving throughput |
| Fixpipe and Mmad Fine-grained Parallelism | Current Mmad and Fixpipe are block-serial, Fixpipe is idle during Mmad computation, Mmad is idle during Fixpipe transfer out | Split into finer-grained blocks, Mmad and Fixpipe execute alternately, reducing pipeline bubbles |
| UB Double Buffering | Vector core executes GM→UB→computation→GM serially | UB Ping-Pong double buffering enables parallel MTE2 transfer and VEC computation |
| Vector Core VF Fusion | Currently uses MemBase API, intermediate results need to be written back to UB | Use RegBase + [asc_vf_call](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/VF调用/asc_vf_call.md) for VF fusion, intermediate computation completes in registers, reducing UB read/write count |
| Large Block Transfer | Currently only transfers single block of baseM×baseN data | Increase singleCoreM/singleCoreN, reduce transfer count, improve bandwidth utilization |
| GM Transfer Optimization | Cube core results need to be transferred to Vector cores via GM (L0C→GM→UB), two transfer operations have high overhead | Ascend 950PR supports Fixpipe direct write to UB (L0C→UB), eliminating GM transfer overhead |

## Functional Debugging

### printf

This interface provides formatted output functionality in CPU domain/NPU domain debugging scenarios.

Call the printf interface at the location in the operator kernel-side implementation code where log information needs to be output to print relevant content.

Example:

```cpp
AscendC::printf("matmul blockIdx=%d\n", AscendC::GetBlockIdx());
```

> **Note:** The printf (PRINTF) interface printing functionality has a certain impact on the actual performance of the operator and is typically used during the debugging phase. Developers can disable the printing functionality by setting ASCENDC_DUMP=0 as needed.

### DumpTensor

For operators developed based on operator projects, you can use this interface to Dump the contents of a specified [LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md). It also supports printing custom additional information (only supports uint32\_t data type information), such as printing the current line number.

Call the DumpTensor interface at the location in the operator kernel-side implementation code where Tensor data needs to be printed. Example:

```cpp
// Vector core LeakyRelu result
AscendC::LeakyRelu(zLocal, xLocal, ..., len);
AscendC::DumpTensor(zLocal, 1, 16);
```

> **Note:** The [DumpTensor](../../../../../docs/zh/api/SIMD-API/基础API/调试接口/上板打印/DumpTensor.md) interface printing functionality has a certain impact on the actual performance of the operator and is typically used during the debugging phase. Developers can disable the printing functionality by setting ASCENDC_DUMP=0 as needed.

## Performance Debugging

### Introduction to the msOpProf Tool

msOpProf is a single-operator performance analysis tool with two usage modes: `msopprof` and `msopprof simulator`. It helps users identify anomalies in operator memory, code, and instructions for comprehensive operator tuning. It currently supports performance data collection and automatic parsing for different run modes (on-device or simulation) and file types (executables or operator binary `.o` files).

- On-device performance collection

    On-device performance collection directly measures the operator's execution time on the Ascend AI Processor. This method is suitable for quickly locating operator performance issues in an on-device environment.

    Run msopprof on the `demo` executable for operator tuning:
    ```
    msopprof ./demo
    ```

    - Performance data description  
      After the command completes, a folder named "OPPROF_{timestamp}_XXX" will be generated in the default directory. The performance data folder structure is as follows:

      ```bash
      ├──dump                       # Raw performance data; users do not need to inspect it
      ├──ArithmeticUtilization.csv  # Cube/Vector instruction cycle proportions
      ├──L2Cache.csv                # L2 Cache hit rate; affects MTE2. Plan data transfer logic properly to increase the hit rate
      ├──Memory.csv                 # Read/write bandwidth rates of UB, L1, and main memory
      ├──MemoryL0.csv               # Read/write bandwidth rates of L0A, L0B, and L0C
      ├──MemoryUB.csv               # Read/write bandwidth rates from Vector and Scalar to UB
      ├──OpBasicInfo.csv            # Basic operator information
      ├──PipeUtilization.csv        # Durations and proportions of computation and data transfer units
      ├──ResourceConflictRatio.csv  # Proportions of UB bank groups, bank conflicts, and resource conflicts among all instructions
      └──visualize_data.bin         # MindStudio Insight presentation file
      ```

View the detailed performance analysis results:

```bash
# View Task Duration and other metrics
cat ./OPPROF_*/PipeUtilization.csv
```
