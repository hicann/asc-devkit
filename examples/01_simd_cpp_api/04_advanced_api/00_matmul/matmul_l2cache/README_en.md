# Matmul L2 Cache Split Direct Invocation Example
## Overview
A Matmul example that enables the L2 Cache split feature to improve L2 Cache utilization.

The Matmul example splits the input matrix along the M or N direction, dividing the matrix into multiple blocks, and performs computation in multiple passes based on the number of data blocks. Before each computation pass, when the first core first accesses the matrix in Global Memory, it loads one block of the split matrix data into the L2 Cache. Subsequent data access by other cores or the first core can hit the L2 Cache, improving example performance.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.2.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.2.0 |

## Directory Structure
```
├── matmul_l2cache
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Ground truth verification file
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── l2_cache_optimizer.h    // L2Cache split algorithm implementation functions
│   ├── matmul_l2cache.asc      // Ascend C example implementation & invocation example
│   └── README.md               // Example description document
```
## Example Description
- Example Function:  
  This example uses an AI processor with an L2 Cache size of 192M. Based on the input and output shapes, the total data volume of input and output is calculated as ((30720 * 4096) + (4096 * 1024) + (1024) + (30720 * 1024)) * 2 = 322963456 bytes (approximately 308M), which exceeds the L2 Cache (192M). This means the data read before computation cannot hit the L2 Cache, and since Global Memory bandwidth is lower than L2 Cache bandwidth with a significant gap between them, data transfer becomes a performance bottleneck. Therefore, the input data needs to be split into multiple blocks so that the computation data volume of each block (including input and output) can fit in the L2 Cache. This example provides the L2CacheOptimizer class, where the GetTileNum interface automatically obtains the total L2 split count for the left and right matrices based on their shapes, the GetBlockShape interface obtains the M, N, K axis lengths after L2 splitting, and the GetBlockCoord interface returns the position coordinates of the corresponding block, that is, the offset relative to the matrix start position in the M, N, K directions.

- Example Specifications:  
  In this example: M = 30720, N = 1024, K = 4096.
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="5" align="center">Matmul</td></tr>
  <tr><td rowspan="4" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td>
  <td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">matmul_l2cache_custom</td></tr>
  </table>
- Example Implementation: 
  - Key Kernel Steps
    - Set the left matrix A, right matrix B, and Bias. Obtain the total L2 split count from the L2CacheOptimizer class GetTileNum interface and loop through multiple computations.
      ```cpp
      L2CacheOpt l2Opt(shapes, blockNum);
      matmulObj.SetOrgShape(shapes.m, shapes.n, shapes.k);
      for (int64_t tileIdx = curBlockIdx; tileIdx < l2Opt.GetTileNum(); tileIdx += blockNum) {
          auto blockShape = l2Opt.GetBlockShape(tileIdx);  // Get the L2 split block size for single computation
          if (Get<0>(blockShape) <= 0 || Get<1>(blockShape) <= 0) {
              return;
          }
          auto blockCoord = l2Opt.GetBlockCoord(tileIdx);  // Get the current computation index blockCoord
          matmulObj.SetTail(Get<0>(blockShape), Get<1>(blockShape), Get<2>(blockShape));
          const auto& offsetCoord = CalcOffset(shapes, blockCoord); // Calculate matrix offset based on index
          int64_t offsetA = Get<0>(offsetCoord);
          int64_t offsetB = Get<1>(offsetCoord);
          int64_t offsetC = Get<2>(offsetCoord);
          matmulObj.SetTensorA(aGlobal[offsetA], false);
          matmulObj.SetTensorB(bGlobal[offsetB], false);
          if (shapes.isBias) {
              matmulObj.SetBias(biasGlobal);
          }
          matmulObj.IterateAll(cGlobal[offsetC]);  // Compute L2 split block
      }
      matmulObj.End();
      ```

  - Key Tiling Steps
    - This example uses constant Tiling computation. On the Kernel side, a set of fixed basic block information is configured, and other Tiling information is derived through constant deduction on the Kernel side. The Kernel side no longer needs runtime Tiling information. Based on this set of optimal basic block information, it can handle scenarios where M and N in the input Shape are large. The example provides an L2Cache split algorithm (refer to the L2CacheOptimizer class). This algorithm currently computes the L2 split count on the Kernel side, and the code can also be migrated to the Host side for computation.
    - L2CacheOptimizer specific computation steps:
      - Determine whether L2 splitting is needed
        ```cpp
        bool smallDim = mTileNum_ < L1_MIN_UST_DIM && nTileNum_ < L1_MIN_UST_DIM;
        if (smallDim || (!EnableL2Tile())) { // Check if total computation data is below L2 Cache threshold
            mL2TileNum_ = mTileNum_;
            nL2TileNum_ = nTileNum_;
            mL2BlockNum_ = 1;
            nL2BlockNum_ = 1;
            return; // No splitting needed, return early
        }
        InitL2TileTail(); // Compute L2 split
        ```
      - Calculate optimal L2 blocking based on load balancing
        ```cpp
        int64_t mConflict = INT64_MAX;
        int64_t nConflict = INT64_MAX;
        constexpr bool isNMajor = l1N > l1M; // Determine the major dimension based on shape size
        for (int64_t i = maxMajor; i >= L1_MIN_UST_DIM; i--) {
            for (int64_t j = maxMinor; j >= minMinor; j--) {
                if (GetTotalSize(j * l1M, i * l1N, k_) <= L2_TILE_THRESHOLD) { // Ensure block is below L2 Cache threshold
                    uint64_t mConflictTmp = AscendC::Std::ceil_div(blockNum_, mL2TileNumTailTmp); // Calculate load conflict value
                    uint64_t nConflictTmp = AscendC::Std::ceil_div(blockNum_, nL2TileNumTailTmp);
                    if (mConflict >= mConflictTmp && nConflict >= nConflictTmp) { // If conflict value is smaller, update block count
                        mConflict = mConflictTmp;
                        nConflict = nConflictTmp;
                        mL2TileNum_ = curMajorDim;
                        nL2TileNum_ = curMinorDim;
                    }
                }
            }
        }
        ```

  - Invocation Implementation  
    Use the kernel invocation operator <<<>>> to call the kernel function.

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
