# ND2NZ Bank Conflict Optimization Example Based on Tensor API

## Overview

Based on the static Tensor API programming model, this example converts an `8192 x 8192` half matrix from ND layout to a NZ layout. Two scenarios compare bank conflicts during the UB Vector write stage and the corresponding avoidance method.

Data movement between GM and UB uses the Tensor API `copy` interface. Register accesses for the ND-to-NZ rearrangement use the C APIs `asc_loadalign` and `asc_storealign`. Each `asc_storealign` writes eight DataBlocks, and its `block_stride` controls the distance between adjacent DataBlocks so that the bank-conflict behavior can be compared directly.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|------------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```text
├── bank_conflict
│   ├── scripts
│   │   ├── gen_data.py                    // Input data and ground truth generation script
│   │   └── verify_result.py               // Ground truth comparison file
│   ├── figures                            // Figures
│   ├── CMakeLists.txt                     // Build project file
│   ├── data_utils.h                       // Data read/write functions
│   ├── bank_conflict.asc                  // Ascend C example implementation
│   └── README.md                          // Example documentation
```

## Example Description

- Example functionality:

  This example converts the input matrix `x` from ND layout to the NZ layout of the output matrix `z`. 

  A C0 block contains 16 half elements, that is, one 32-byte DataBlock. Each row of a `144 x 128` tile contains eight C0 blocks. The Vector write stage writes these eight DataBlocks to the corresponding eight C0 columns in the NZ region.

  - `x`: input matrix, logical shape `[M, N]`, half, ND layout
  - `z`: output matrix, logical shape `[M, N]`, half, NZ layout

- Example specifications:

  The example uses `M=8192` and `N=8192` and launches 64 Vector blocks. Each block processes a row split and a column split using `144 x 128` tiles. The valid height of the final row tile is passed through `actualTileH`.

  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">ND2NZ</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[M, N]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[M, N]</td><td align="center">float16</td><td align="center">NZ</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">bank_conflict_kernel</td></tr>
  </table>

  Compile-time parameters are listed below:

  | Parameter | Value | Description |
  | :--- | :---: | :--- |
  | `TOTAL_M` | 8192 | Number of rows in the input matrix |
  | `TOTAL_N` | 8192 | Number of columns in the input matrix |
  | `ROW_SPLITS` | 8 | Number of row splits |
  | `COL_SPLITS` | 8 | Number of column splits |
  | `TOTAL_BLOCKS` | 64 | Number of launched blocks |
  | `TILE_H` | 144 | Maximum number of rows in a tile |
  | `TILE_W` | 128 | Number of columns in a tile |
  | `C0_ELEMS` | 16 | Number of half elements in one C0 block |
  | `TILE_C0_COLS` | 8 | Number of C0 columns in one tile |
  | `SCENARIO_NUM` | 1, 2 | Bank-conflict comparison scenario |
  | `dstNzC0Stride` | S1=144, S2=145 | Distance between adjacent C0-column start positions, in DataBlocks |

  > **Terminology:** A DataBlock is the data unit processed by a Vector instruction and is 32 bytes in size. One DataBlock contains 16 half elements.

  **Figure: ND and NZ data layout**

  <img src="figures/nd2nz.png" width="80%">

- Example implementation:

  - Implementation process:

    <table>
    <tr><th align="left">Step</th><th align="left">Tensor API/C API Operation</th><th align="left">Function</th><th align="left">Layout or Execution Unit</th></tr>
    <tr><td align="left">1</td><td align="left">Compile-time constants</td><td align="left">Define matrix, tile, partitioning, and scenario parameters</td><td align="left">Not applicable</td></tr>
    <tr><td align="left">2</td><td align="left">make_tensor + layout</td><td align="left">Create GM, UB source, and UB destination Tensor views</td><td align="left">GM uses ND; NZ write strides are expressed by the Vector interface and the MTE3 layout respectively</td></tr>
    <tr><td align="left">3</td><td align="left">copy(copy_gm_to_ub)</td><td align="left">Move the valid-height ND tile from GM to UB</td><td align="left">ND to ND</td></tr>
    <tr><td align="left">4</td><td align="left">slice + asc_loadalign</td><td align="left">Slice by row and read 128 half elements from the UB ND tile</td><td align="left">Vector</td></tr>
    <tr><td align="left">5</td><td align="left">asc_storealign</td><td align="left">Write eight DataBlocks to the UB NZ region with `dstNzC0Stride`</td><td align="left">Vector, ND to NZ rearrangement</td></tr>
    <tr><td align="left">6</td><td align="left">copy(copy_ub_to_gm)</td><td align="left">Write the NZ tile back to the compact NZ layout in GM, skipping the padding used by S2</td><td align="left">NZ to NZ</td></tr>
    </table>

  - Tensor API core interfaces:

    1. **Tensor construction interface:** Use `make_frame_layout` and `make_pattern_layout` to describe GM and UB Tensor shapes and layouts, and use `make_tensor` and `make_mem_ptr` to create the GM and UB Tensors.

    2. **Slicing interface:** Use `slice(make_coord(...), make_shape(...))` to obtain the row view of the current tile. Both source and destination addresses are obtained from Tensor views.

    3. **Data movement interface:** Use `make_copy` to construct `copy_gm_to_ub` and `copy_ub_to_gm`, completing data movement between GM and UB. The NZ destination Tensor layout in the MTE3 stage describes the actual stride between adjacent C0 columns in UB.

    4. **Register movement interface:** Use `asc_loadalign` to read one row of 128 half elements and the `block_stride` parameter of `asc_storealign` to control the non-contiguous writes of eight DataBlocks. `block_stride` is the direct control parameter for the bank-conflict comparison in this example.

    5. **Pipeline interface:** Use `asc_lock` and `asc_unlock` to protect the MTE2, Vector, and MTE3 stages, and use two UB buffers to process tiles in ping-pong mode.

  - Core ND-to-NZ implementation:

    `srcTensor` and `dstTensor` are first constructed with the Tensor API. `slice` only obtains the starting address of each row; the non-contiguous destinations of the eight DataBlocks are specified by the `dstNzC0Stride` argument of `asc_storealign`.

    ```cpp
    template <uint32_t dstNzC0Stride>
    __simd_vf__ inline void NdToNz(__ubuf__ half* dst, __ubuf__ half* src)
    {
        vector_bool mask = asc_create_mask_b16(PAT_ALL);
        vector_half value;
        asc_loadalign(value, src);
        asc_storealign(dst, value, dstNzC0Stride, 0, mask);
    }
    ```

    The scenario difference is exposed at the final call site in the kernel:

    ```cpp
    if constexpr (SCENARIO_NUM == 1) {
        NdToNz<TILE_H>(dstRow.data().get(), srcRow.data().get());
    } else {
        NdToNz<TILE_H + BANK_CONFLICT_OFFSET>(dstRow.data().get(), srcRow.data().get());
    }
    ```

  - Invocation implementation:

    The host-side invocation uses the kernel call operator `<<<>>>` to launch `bank_conflict_kernel`. The `SCENARIO_NUM` parameter selects the comparison scenario.

  - UB bank structure and conflicts in this example:

    The Unified Buffer of Ascend 950PR/Ascend 950DT is divided into 16 physical banks and organized into eight bank groups. Banks `i` and `i+8` belong to the same bank group, so `bank group = bank % 8`. UB addresses use low-order interleaving: for every 32-byte DataBlock traversed by a consecutive address, the physical bank number increases by one modulo 16.

    This example focuses on write-write conflicts when one `asc_storealign` writes eight DataBlocks. The distance between adjacent destinations is controlled by `dstNzC0Stride`; when multiple destinations fall into the same bank group, the writes must be serialized.

    **Figure: UB bank structure of Ascend 950PR/Ascend 950DT**

    <img src="figures/ubBankStruct3510.png" width="80%">

    **Figure: UB bank memory layout of Ascend 950PR/Ascend 950DT**

    <img src="figures/UB-3510.png" width="80%">

    **Scenario 1: Compact placement**

    Build with `SCENARIO_NUM=1`. In this scenario, `dstNzC0Stride=144`, which is `16 x 9` DataBlocks. For one row, the physical bank number of all eight write destinations remains unchanged, so the eight accesses use the same bank and bank group, causing write-write conflicts.

    **Figure: ND-to-NZ layout with compact placement in Scenario 1**

    <img src="figures/datand2nzS1.png" width="80%">

    Taking the first row of a tile as an example, the eight destinations are DataBlocks `0, 144, 288, ..., 1008`, corresponding to bank 0, bank 0, ..., bank 0. The starting bank changes with the row address, but the eight destinations of one store still fall into the same bank.

    **Figure: UB bank conflicts in Scenario 1**

    <img src="figures/s1bank3510.png" width="80%">

    **Scenario 2: Increased stride and shifted NZ buffer base**

    Build with `SCENARIO_NUM=2`. In this scenario, `dstNzC0Stride=145`, which is `16 x 9 + 1` DataBlocks. The physical bank number increases by one at every jump, so the eight write destinations enter eight different bank groups.

    The code also shifts the initial `nzPtr` address by eight DataBlocks (`NZ_BASE_SHIFT_DATA_BLOCKS=8`). For the first row, `asc_loadalign` reads banks 0-7 and `asc_storealign` writes banks 8-15, separating the physical banks used by the initial read and write and reducing same-cycle bank overlap. The additional stride is only used to change UB destinations; the UB-to-GM transfer uses the NZ Tensor layout to skip the padding, so the output remains in compact NZ format.

    **Figure: ND-to-NZ layout with increased stride in Scenario 2**

    <img src="figures/datand2nzS2.png" width="80%">

    **Figure: Distributed UB bank destinations in Scenario 2**

    <img src="figures/s2bank3510.png" width="80%">

## Tensor API Implementation Features

| Feature | Example Implementation |
|---------|------------------------|
| Tensor representation | Use Tensor objects to describe GM, UB ND tiles, and UB NZ tiles |
| Tensor slicing | Use `slice` to obtain each tile and row start address |
| Data movement | Use `copy` for GM-to-UB and UB-to-GM transfers |
| ND-to-NZ rearrangement | Use `asc_loadalign` and `asc_storealign` for Vector register movement |
| Bank-conflict comparison | Compare compact and distributed placement with `dstNzC0Stride=144/145` |
| Address offset | Scenario 2 shifts the NZ base by eight DataBlocks to separate the physical banks used by the initial ND read and NZ write |
| Double buffering | Use ping-pong UB buffers to process tiles alternately |
| Tail-tile handling | Use `actualTileH` to limit the valid rows in the final tile |

## Performance Data

The following data was measured on Ascend 950PR/Ascend 950DT with `144 x 128` tiles and 64 launched blocks.

| Scenario | `dstNzC0Stride` | Task Duration (us) | `aiv_time` (us) | `aiv_total_cycles` | `aiv_vec_time` (us) | `aiv_vec_ratio` | `aiv_scalar_time` (us) | `aiv_scalar_ratio` | `aiv_mte2_time` (us) | `aiv_mte2_ratio` | `aiv_mte3_time` (us) | `aiv_mte3_ratio` | `icache_miss_rate` |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| S1 | 144 | 262.628 | 261.96 | 14112428 | **254.637** | 0.972 | 3.851 | 0.015 | 93.445 | 0.357 | 47.817 | 0.183 | 0.006 |
| S2 | 145 | 222.964 | 222.18 | 12052109 | **214.199** | 0.964 | 3.823 | 0.017 | 111.712 | 0.503 | 41.139 | 0.185 | 0.009 |

The `aiv_vec_time` of S2 decreases from 254.637 us to 214.199 us, indicating that the increased stride significantly reduces write-write bank conflicts. 

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables

  Configure environment variables according to the CANN development kit [installation method](../../../../docs/en/quick_start.md#prepare&install) in the current environment. **Currently only [CANN master](../../../../docs/en/quick_start.md#cann-install) is supported.**

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default is `/usr/local/Ascend`.

- Run the example

  The following commands use Scenario 1 as an example. Run them in this example directory.

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;                                                        # Create and enter the build directory
  cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # Build the project (default NPU mode)
  python3 ../scripts/gen_data.py                                                    # Generate test input data
  ./demo                                                                               # Run the compiled executable
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin            # Verify output correctness
  ```

  To build Scenario 2, set `SCENARIO_NUM=2`.

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter:

  ```bash
  cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Clear the CMake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then run CMake again.

- Build option description

  | Option | Available Values | Description |
  |--------|------------------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution or NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture; dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | 1, 2 | 1: compact placement; 2: increased stride and shifted NZ buffer base |

  > **Note:** This example only supports the dav-3510 architecture (Ascend 950PR/Ascend 950DT).

- Execution result

  The following output indicates that the precision comparison succeeded.

  ```bash
  test pass!
  ```
