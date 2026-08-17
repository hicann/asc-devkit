# Batch Matmul IterateNBatch Direct Invocation Example
## Overview
An example for multiple batch Matmul computations, including both synchronous and asynchronous scenario implementations.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── batch_matmul_iterate_n_batch
│   ├── scripts
│   │   ├── gen_data.py                    // Script for generating input data and ground truth data
│   │   └── verify_result.py               // Ground truth verification file
│   ├── CMakeLists.txt                     // Build project file
│   ├── data_utils.h                       // Data read/write functions
│   ├── batch_matmul_iterate_n_batch.asc   // Ascend C example implementation & invocation example
│   └── README.md                          // Example description document
```
## Example Description
- Example Function:  
  This example splits the BatchMatmul computation batches into nNum main batches, where each main batch includes BatchNum sub-batches of Matmul computation. By modifying the IS_SYNCH parameter in the batch_matmul_iterate_n_batch.asc code, you can control whether to use the asynchronous computation flow.

- Constraints
  - Computation within a single Batch Matmul follows the IterateBatch constraints;
  - For BSNGD, SBNGD, and BNGS1S2 Layout formats, the total multi-batch data of input A and B matrices should be smaller than the L1 Buffer size;
- Example Specifications:  
    In this example: BatchNum = 3, M = 32, N = 256, K = 64.
    <table>
    <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="6" align="center">BatchMatmulCustom</td></tr>
    <tr><td rowspan="4" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td><td align="center">layout</td></tr>
    <tr><td align="center">a</td><td align="center">[BatchNum, M, K]</td><td align="center">half</td><td align="center">ND</td><td align="center">true</td><td align="center">BSNGD</td></tr>
    <tr><td align="center">b</td><td align="center">[BatchNum, K, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td><td align="center">BSNGD</td></tr>
    <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td><td align="center">-</td></tr>
    <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[BatchNum, M, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td><td align="center">BSNGD</td></tr>
    <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="6" align="center">batch_matmul_iterate_n_batch_custom</td></tr>
    </table>

- Example Implementation: 
  - Key Kernel Steps
    - When creating the Matmul object, customize the MatmulConfig parameters, set the isNBatch parameter to true to enable multi-batch input and multi-batch output, and set the isBiasBatch parameter to false to enable the Bias reuse feature of BatchMatmul, obtaining a custom Matmul object using the NORM template.
      ```cpp
      constexpr MatmulConfigMode configMode = MatmulConfigMode::CONFIG_NORM;
      constexpr MatmulBatchParams batchParams = {
        true, BatchMode::BATCH_LESS_THAN_L1, false /* isNBatch, batchMode, isBiasBatch */
      };
      constexpr MatmulConfig CFG_MM = GetMMConfig<configMode>(batchParams);
      AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_MM> matmulObj;
      ```
    - Calculate the nNum size based on the Layout information and BatchNum of the A and B matrices.
      ```cpp
      int32_t g_lay = tiling.ALayoutInfoG > tiling.BLayoutInfoG ? tiling.ALayoutInfoG : tiling.BLayoutInfoG;
      int32_t nNum = tiling.ALayoutInfoB * tiling.ALayoutInfoN * g_lay / tiling.BatchNum;
      ```
    - The current source code defaults to `IS_SYNCH=true`, where computation results are first written to `gm_workspace` and then transferred to the output GM.
      ```cpp
      matmulObj.SetWorkspace(gm_workspace);
      matmulObj.IterateNBatch(nNum, batchA, batchB, false);
      DataCopy(bufferC, gm_workspace, sizeC / sizeof(cType));
      ```
    - When `IS_SYNCH=false`, complete the multi-batch matrix multiplication operation and call the GetBatchTensorC interface to obtain results.
      ```cpp
      matmulObj.template IterateNBatch<false>(nNum, batchA, batchB, false);
      for(int32_t j = 0; j < nNum; ++j){
          matmulObj.template GetBatchTensorC<false>(batchA, batchB, false);
      }
      ```

  - Key Tiling Steps
    - Call SetALayout, SetBLayout, SetCLayout, and SetBatchNum to set the Layout axis information and maximum BatchNum for A/B/C.
      ```cpp
      cubeTiling.SetALayout(1, M, 1, batchNum, K);
      cubeTiling.SetBLayout(1, N, 1, batchNum, K);
      cubeTiling.SetCLayout(1, M, 1, batchNum, N);
      cubeTiling.SetBatchNum(batchNum);
      ```

## Build and Run
Run the following steps in the root directory of this example to build and run the example.
- Configure Environment Variables  
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;    # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, npu mode by default
  python3 ../scripts/gen_data.py    # Generate test input data
  ./demo                        # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin    # Verify whether the output is correct and confirm the algorithm logic
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build Option Description

  | Option | Values | Description |
  | ----------------| -----------------------------| ---------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution Result

  The following output indicates a successful accuracy comparison:
  ```bash
  test pass!
  ```
