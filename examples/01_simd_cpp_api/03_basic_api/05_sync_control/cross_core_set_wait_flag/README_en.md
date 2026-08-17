# CrossCoreSetFlag and CrossCoreWaitFlag Inter-Core Synchronization Example

## Overview
This example first introduces the three synchronization modes supported by the inter-core synchronization interfaces CrossCoreSetFlag and CrossCoreWaitFlag (see the table below), and then demonstrates the specific usage of these three synchronization modes in two practical scenarios: a pure Vector computation scenario and a Cube and Vector fused computation scenario.
<table border="1" align="center">
  <tr bgcolor="lightgray">
    <td>Synchronization Control Mode</td>
    <td align="center">Description</td>
  </tr>
  <tr>
    <td rowspan="2">mode 0</td>
    <td>For AIC scenarios, synchronize all AIC cores. Instructions after CrossCoreWaitFlag execute only when all AIC cores have reached CrossCoreSetFlag.</td>
  </tr>
  <tr>
    <td>For AIV scenarios, synchronize all AIV cores. Instructions after CrossCoreWaitFlag execute only when all AIV cores have reached CrossCoreSetFlag.</td>
  </tr>
  <tr>
    <td>mode 1</td>
    <td>Synchronization control between AIV cores within a single AI Core. Instructions after CrossCoreWaitFlag execute only when both AIV cores have run CrossCoreSetFlag.</td>
  </tr>
  <tr>
    <td rowspan="2">mode 2</td>
    <td>After the AIC core executes CrossCoreSetFlag, instructions after CrossCoreWaitFlag on the two AIVs continue to execute.</td>
  </tr>
  <tr>
    <td>After both AIVs execute CrossCoreSetFlag, instructions after CrossCoreWaitFlag on the AIC can execute.</td>
  </tr>
</table>

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── cross_core_set_wait_flag
│   ├── scripts
│   │   ├── gen_data.py              // Script for generating input data and ground truth data
│   │   └── verify_result.py         // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt               // Build project file
│   ├── data_utils.h                 // Data read/write functions
│   ├── figures                      // Illustrations
│   ├── cross_core_set_wait_flag.h   // Ascend C example implementation
│   ├── cross_core_set_wait_flag.asc // Invocation example and result verification
│   └── README.md                    // Example documentation
```

## Example Description
<table border="1" style="text-align: center;">
  <tr>
    <td>SCENARIO_NUM Value</td>
    <td>Business Scenario</td>
    <td>Synchronization Mode Used</td>
  </tr>
  <tr>
    <td>0</td>
    <td>Pure Vector computation scenario (16 AIVs)</td>
    <td>mode 0 (all AIV core synchronization)</td>
  </tr>
  <tr>
    <td>1</td>
    <td>Pure Vector computation scenario (2 AIVs)</td>
    <td>mode 1</td>
  </tr>
  <tr>
    <td>2</td>
    <td>Cube and Vector fused computation scenario</td>
    <td>mode 2 (AIC waits for AIV), mode 2 (AIV waits for AIC), mode 0 (all AIC core synchronization)</td>
  </tr>
</table>
This example controls execution branches through SCENARIO_NUM. Different values of this variable correspond to different business scenarios and synchronization modes. As shown in the table above, when SCENARIO_NUM takes different values, it demonstrates the specific usage of the three synchronization modes in pure Vector computation scenarios and Cube and Vector fused computation scenarios respectively.

### Computation Formula and Example Specifications

#### SCENARIO_NUM=0 (Pure Vector Computation Scenario, Mode 0)
- Computation formula:  
  $$
  z = \sum_{i=0}^{15} (x \times i)
  $$
  - x is the input vector, all ones
  - i is the BlockIdx of each AIV (range 0-15)
  - z is the accumulated value of computation results from all AIVs

- Example specifications:
  <table border="1" align="center">
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">CrossCoreSetFlagMode0</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[32]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[32]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">cross_core_set_wait_flag_custom</td></tr>
  </table>

#### SCENARIO_NUM=1 (Pure Vector Computation Scenario, Mode 1)
- Computation formula:  
  $$
  z = (x \times 2) + (x \times 3)
  $$
  - x is the input vector, all ones
  - Only AIVs with BlockIdx=2 and 3 participate in computation
  - z is the accumulated value of computation results from these two AIVs

- Example specifications:
  <table border="1" align="center">
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">CrossCoreSetFlagMode1</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[32]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[32]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">cross_core_set_wait_flag_custom</td></tr>
  </table>

#### SCENARIO_NUM=2 (Cube and Vector Fused Computation Scenario)
- Computation formula:  
  $$
  C = \text{LeakyRelu}(Cast(A) \times Cast(B))
  $$
  - A is the left matrix with shape [M, K] and data type uint8
  - B is the right matrix with shape [K, N] and data type uint8
  - First convert the data types of A and B from uint8 to half
  - Then perform matrix multiplication: A x B
  - Finally perform LeakyRelu operation on the result
  - C is the final result with shape [M, N] and data type float32

- Example specifications:
  <table border="1" align="center">
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="5" align="center">CrossCoreSetFlagMode2</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[32, 32]</td><td align="center">uint8</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[32, 64]</td><td align="center">uint8</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[32, 64]</td><td align="center">float32</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">mmad_custom</td></tr>
  </table>
## Example Implementation
### 1. Cube and Vector Fused Computation Scenario
#### 1.1 Overall Logic
<p align="center">
  <img src="figures/融合场景_示意图.png" width="100%">
   </p>
<p align="center">
Figure 1: Cube and Vector fused computation scenario, overall computation logic diagram
</p>

This example focuses on the fused operator scenario (configured with __mix__(1,2)), where each AI Core contains 1 AIC and 2 AIVs. The configured logical core count numBlocks = 8, corresponding to 8 AICs and 16 AIVs. As shown in Figure 1, the overall computation logic is divided into three core phases: precision conversion phase, block matrix multiplication and atomic accumulation phase, and LeakyRelu operation and result write-back phase.
#### 1.2 Precision Conversion Phase (Mode 2, AIC Waits for AIV Within a Single AI Core)
Since the data type of the left and right matrices on GM is uint8, which does not meet the input data type requirements of the mmad instruction, the data on GM must first be transferred to AIV for precision conversion before block matrix multiplication can be performed in AIC. Therefore,
within each AI Core, one AIC needs to wait for the other 2 AIVs within the same AI Core to complete data precision conversion before starting block matrix multiplication computation.
Specifically: the left matrix (A matrix) data in GM is split into 8 parts along the K axis, assigned to AIVs with even BlockIdx for uint8 to half precision conversion; the right matrix (B matrix) data in GM is split into 8 parts along the K axis, assigned to AIVs with odd BlockIdx for uint8 to half precision conversion. As shown in Figure 2, based on the above description, inter-core synchronization mode 2 is required.
The code segment corresponding to the above description is as follows:

        if (blockIdx % 2 == 0) {
            ...
            AscendC::Cast(castALocal, aLocal, AscendC::RoundMode::CAST_NONE, A_BLOCKS_LENGTH);
            ...
            AscendC::DataCopy(AVectorGM, castALocal, A_BLOCKS_LENGTH);
        } else {
            ...
            AscendC::Cast(castBLocal, bLocal, AscendC::RoundMode::CAST_NONE, B_BLOCKS_LENGTH);
            ...
            AscendC::DataCopy(BVectorGM, castBLocal, B_BLOCKS_LENGTH);
        }
        // Mode 2: within each AI Core, one AIC waits for 2 AIVs
        AscendC::CrossCoreSetFlag<2, PIPE_MTE3>(SYNC_AIV_AIC_FLAG);

<p align="center">
  <img src="figures/融合场景_精度转换阶段.png" width="100%">
   </p>
<p align="center">
Figure 2: Precision conversion phase, mode 2 diagram
</p>

#### 1.3 Block Matrix Multiplication and Atomic Accumulation Phase (Mode 0, All AIC Core Synchronization)
After each AIC performs block matrix multiplication computation, it enables the atomic accumulation summation mechanism, transfers the computation results to the same GM region, and accumulates the block matrix multiplication results from 8 AICs on GM to obtain the complete C matrix. To obtain the correct C matrix, it is necessary to wait for all 8 AICs to complete block matrix multiplication and transfer results to GM via FixPipe. As shown in Figure 3, based on the above description, inter-core synchronization mode 0 (all AIC core synchronization) is required. The code segment corresponding to the above description is as follows:
$$
C = \sum_{i=1}^{8} A_i \cdot B_i
$$

        // Mode 2: within each AI Core, AIC waits for 2 AIVs
        AscendC::CrossCoreWaitFlag(SYNC_AIV_AIC_FLAG);

        CopyIn(a1Local, b1Local);
        SplitA(a1Local, a2Local);
        SplitBTranspose(b1Local, b2Local);
        Compute(a2Local, b2Local, c1Local);
        CopyOut(c1Local);
        // Mode 0: 8 AICs from 8 AI Cores synchronize
        AscendC::CrossCoreSetFlag<0, PIPE_FIX>(SYNC_AIC_FLAG);  
        AscendC::CrossCoreWaitFlag(SYNC_AIC_FLAG);  

        // Mode 2: within each AI Core, 2 AIVs wait for AIC
        AscendC::CrossCoreSetFlag<2, PIPE_FIX>(SYNC_AIC_AIV_FLAG);  

<p align="center">
  <img src="figures/融合场景_分块矩阵乘与原子累加阶段.png" width="100%">
   </p>
<p align="center">
Figure 3: Block matrix multiplication and atomic accumulation phase, mode 0 diagram
</p>

#### 1.4 LeakyRelu and Result Write-Back Phase (Mode 2, 2 AIVs Wait for AIC)
Within each AI Core, 2 AIVs need to wait for AIC to complete block matrix multiplication and atomic accumulation operations before performing LeakyRelu operation on C matrix blocks.
Specifically, the accumulated C matrix is split into 16 parts along the M axis, assigned to 16 AIVs for LeakyRelu computation respectively. As shown in Figure 4, based on the above description, inter-core synchronization mode 2 (2 AIVs wait for one AIC within a single AI Core) is required. The code segment corresponding to the above description is as follows:

        // Mode 2: within each AI Core, 2 AIVs wait for AIC
        AscendC::CrossCoreWaitFlag(SYNC_AIC_AIV_FLAG);

        // Perform LeakyRelu operation
        float alpha = 0.001;
        ...
        AscendC::LeakyRelu(reluCLocal, cLocal, alpha, C_AIV_BLOCKS_LENGTH);
        ...

<p align="center">
  <img src="figures/融合场景_LeakyRelu运算与结果回写阶段.png" width="100%">
   </p>
<p align="center">
Figure 4: LeakyRelu operation and result write-back phase, mode 2 diagram
</p>

### 2. Pure Vector Computation Scenario
#### 2.1 Comparison of Mode 0 and Mode 1
This example sets NUM\_BLOCKS to 8 (8 AI Cores), with each AI Core having an AIC to AIV ratio of 1:2, meaning this example launches 8 AICs and 16 AIVs in total, with AIV BlockIdx ranging from 0 to 15.
As shown in Figure 5 below, the computation logic of mode 0 and mode 1 in this example is nearly identical, with the only difference being the number of AIVs participating in synchronization: in mode 0, all 16 AIVs participate in synchronization; in mode 1, only 2 AIVs (BlockIdx=2 and 3) in the second AI Core participate in synchronization. Therefore, the next section will detail the overall logic of mode 0, and mode 1 will not be described in detail.
<p align="center">
  <img src="figures/纯aiv_模式1和模式0的区别.png" width="100%">
</p>
<p align="center">
Figure 5: Pure AIV scenario, difference between mode 0 and mode 1 diagram
</p>

#### 2.2 Overall Logic of Mode 0
The GM used in this example is divided into 2 blocks: one for storing input data (initialDataGm) and one for storing the accumulated results from all AIVs (atomicResultGm).
As shown in Figure 6 below, the overall logic of mode 0 is divided into the following steps:
(1) Each AIV transfers all-ones data from initialDataGm to UB, which is a PIPE\_MTE2 pipeline operation.

(2) Each AIV performs vector computation on the data in UB: multiply by the BlockIdx corresponding to each core using the Muls instruction, which is a PIPE\_V pipeline operation.

(3) Step 3 corresponds to the following code snippet. atomicResultGm is used to store the accumulated results after all 16 AIVs complete transfer. By calling CrossCoreSetFlag and CrossCoreWaitFlag interfaces, synchronization control is achieved: ensuring that after all 16 AIVs complete the PIPE_MTE3 transfer instruction, instructions after CrossCoreWaitFlag can execute.

         // Enable atomic accumulation for UB to GM transfer: data transferred to atomicResult is accumulated with the original value and overwrites the original value
        AscendC::SetAtomicAdd<float>(); 
        // DataCopy is a PIPE_MTE3 pipeline operation
        AscendC::DataCopy(atomicResultGm, xLocal, this->blockLength);   
        // After this AIV completes the preceding PIPE_MTE3 (DataCopy) pipeline operation, notify other AIV cores that this AIV has completed
        AscendC::CrossCoreSetFlag<0, PIPE_MTE3>(0);  
        // Block this AIV from continuing to execute instructions until all other AIVs complete the PIPE_MTE3 pipeline operation, then unblock and continue execution
        AscendC::CrossCoreWaitFlag(0); 

After the above synchronization is complete, atomicResultGm already contains the accumulated value of vector computation results from 16 AIVs.
If the synchronization in the previous step is not inserted correctly, the data transferred from atomicResultGm to AIV may be the accumulated value of vector computation results from only some AIVs, resulting in inaccurate results.

        if (AscendC::GetBlockIdx() == 0) {
            AscendC::DataCopy(yLocal, atomicResultGm, this->blockLength);   // PIPE_MTE2
            return;
        }
<p align="center">
  <img src="figures/纯aiv_模式0示意图.png" width="100%">
</p>
<p align="center">
Figure 6: Pure AIV scenario, mode 0 computation logic diagram
</p>

### 3. Notes
#### 3.1 Cube and Vector Fused Computation Scenario
(1) In the Cube and Vector fused computation scenario, the fused operator (configured with __mix__(1,2)) requires ASCEND_IS_AIV/ASCEND_IS_AIC for code isolation between AIV and AIC cores.
```
KernelMmad op;
if ASCEND_IS_AIC {
    op.InitAIC(A, B, c);
    op.ProcessAIC();
} 
if ASCEND_IS_AIV {
    op.InitAIV(a, b, A, B, c);
    op.ProcessAIV();
}
```
(2) GetBlockIdx (obtaining the index of the current core) has different value ranges in AIC and AIV, and its values are related to the logical core count set by the operator and the AIC to AIV ratio in one AI Core. In this example, NUM_BLOCKS=8 and the AIC to AIV ratio is 1:2, so GetBlockIdx ranges from 0-7 in AIC and 0-15 in AIV.

(3) The example uses the static Tensor programming paradigm, which requires manually inserting intra-core synchronization. Additionally, in the static Tensor programming mode, developers need to manually call the InitSocState() interface to initialize the global state register.

#### 3.2 Pure Vector Computation Scenario
(1) When using the CrossCoreSetFlag and CrossCoreWaitFlag inter-core synchronization interfaces, even in pure Vector computation scenarios, the kernel function cannot use the __vector__ modifier.
The kernel function in this example uses __mix__(1,2) decoration, but in the pure Vector scenario, since only vector computation is performed, the ASCEND_IS_AIV macro must be used to ensure the program runs only on AIV cores, otherwise the program will hang.
```
if ASCEND_IS_AIV {
        op.Init(x, z, dataLength);
        op.Process();
}
```
 (2) Mode 1 requires that the 2 AIVs participating in synchronization must belong to the same AI Core, otherwise the program will hang. In this example, the two AIVs participating in synchronization have GetBlockIdx=2, 3, which belong to the 2nd AI Core (index starting from 1); if GetBlockIdx is changed to 3 and 4 (belonging to two different AI Cores), the program will hang.

 (3) The example uses the static Tensor programming paradigm, which requires manually inserting intra-core synchronization. Additionally, in the static Tensor programming mode, developers need to manually call the InitSocState() interface to initialize the global state register.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables  
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.
    
- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=0  # Set the scenario number (values: 0, 1, 2)
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # Build the project
  python3 ../scripts/gen_data.py -scenarioNum $SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin  # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  Examples:
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # CPU debug mode
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU simulation mode
  ```
  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0` (default), `1`, `2` | Scenario number: 0 (pure Vector mode 0), 1 (pure Vector mode 1), 2 (Cube+Vector fusion) |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.
  ```bash
  test pass!
  ```
