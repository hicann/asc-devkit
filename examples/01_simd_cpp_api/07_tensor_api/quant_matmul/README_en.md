# QuantMatmul Example Based on Tensor API

## Overview

This example implements a QuantMatmul operator with an AIC:AIV ratio of 1:2 using the Tensor API programming model. The example uses the `__mix__` kernel mode, in which one AIC (Cube core) and two AIVs (Vector cores) run in parallel on the same physical core. The AIC performs matrix multiplication, while the AIVs perform data type conversion, per-channel dequantization, per-token dequantization, and FastGelu activation-function computation. They are synchronized through `sync_block_arrive`/`sync_block_wait`.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR&950DT products | >= CANN 9.3.0 |

## Directory Structure

```text
├── quant_matmul
│   ├── scripts
│   │   ├── gen_data.py                 // Script for generating input and golden data
│   │   └── verify_result.py            // Golden data comparison script
│   ├── CMakeLists.txt                  // Build project file
│   ├── data_utils.h                    // Data read/write functions
│   ├── quant_matmul.asc                // Ascend C example implementation and invocation
│   ├── README.md                       // Chinese example documentation
│   └── README_en.md                    // English example documentation
```

## Example Description

- Example functionality:
  This example implements QuantMatmul computation, followed by FastGelu activation-function computation.

  The QuantMatmul formula is:

  $$
  T = x \times y^T
  $$

  $$
  D_{i,j} = T_{i,j} \times \mathrm{channel\_scale}[j] \times \mathrm{token\_scale}[i]
  $$

  $$
  C_{i,j} = \operatorname{FastGelu}\left(D_{i,j}\right)
  $$

  Here, `channel_scale[j]` applies to column j of the matrix multiplication result, and `token_scale[i]` applies to row i of the matrix multiplication result.

  FastGelu is defined as:

  $$
  \operatorname{FastGelu}(x) = \frac{x}{1 + e^{-1.702x}}
  $$

  - x: left matrix, shape [m, k], data type int8, ND layout format;
  - y: right matrix, logical shape [n, k], stored transposed and equivalent to $y^T$ in computation, data type int8, DN layout format;
  - channel_scale: per-channel dequantization scale of the right matrix, shape [n], data type float. `channel_scale[j]` applies to column j of the matrix multiplication result;
  - token_scale: per-token dequantization scale of the left matrix, shape [m], data type float. `token_scale[i]` applies to row i of the matrix multiplication result;
  - z: output matrix storing the dequantized and activated result, shape [m, n], data type float.

  Matrix multiplication result `T[i, j]` is dequantized as follows: first convert `T[i, j]` from int32 to float, then multiply it by `channel_scale[j]` and `token_scale[i]` to restore the pre-quantization numerical scale.

- Specification:

  The default shape is m=1024, n=1024, and k=256. The number of Blocks launched by the Kernel is 32.

<table>
<tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">QuantMatmul</td></tr>
<tr><td rowspan="5" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x (Matrix A)</td><td align="center">[m, k]</td><td align="center">int8</td><td align="center">ND</td></tr>
<tr><td align="center">y (Matrix B, transposed storage)</td><td align="center">[n, k]</td><td align="center">int8</td><td align="center">DN</td></tr>
<tr><td align="center">channel_scale (per-channel dequantization scale)</td><td align="center">[n]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">token_scale (per-token dequantization scale)</td><td align="center">[m]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">z (Matrix C)</td><td align="center">[m, n]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">quant_matmul_custom</td></tr>
</table>

- Invocation:
  The host derives the Block count from the fixed shape and per-block split, then calls the kernel with the invocation operator <<<>>>. With the default parameters, the Block count is `(1024 / 256) × (1024 / 128) = 4 × 8 = 32`. `__mix__(1, 2)` specifies an AIC:AIV ratio of 1:2 within each Block.

### Parameters

Default compile-time parameters:

| Parameter | Default | Description |
| --- | --- | --- |
| `base_m` | 128 | M dimension base block size |
| `base_n` | 128 | N dimension base block size |
| `base_k` | 64 | K dimension base block size |
| `step_m` | 1 | Number of base_m blocks cached in L1 along M for the left matrix |
| `step_n` | 1 | Number of base_n blocks cached in L1 along N for the right matrix |
| `step_k` | 4 | Number of base_k blocks cached in L1 along K |
| `vector_len` | 64 | Maximum number of elements in one Vector computation |
| `pipe_depth` | 2 | Number of UB slots |

The fixed example specification is as follows. These parameters are compile-time constants in `quant_matmul.asc`; after changing them, rebuild the example. Command-line parameter configuration is not supported:

| Parameter | Default | Description |
| --- | --- | --- |
| `m` | 1024 | Number of rows in Matrix A |
| `n` | 1024 | Number of columns in Matrix B |
| `k` | 256 | Number of columns in Matrix A / Number of rows in Matrix B |
| `single_core_m` | 256 | Single-core M dimension size |
| `single_core_n` | 128 | Single-core N dimension size |
| `single_core_k` | 256 | Single-core K dimension size |
| `num_blocks` | 32 | Number of Blocks launched by Kernel, derived as `(m / single_core_m) × (n / single_core_n)` |

Parameter constraints are as follows:

```text
m % single_core_m == 0
single_core_m % base_m == 0
n % single_core_n == 0
single_core_n % base_n == 0
k % base_k == 0
base_m % 2 == 0
base_n % vector_len == 0
```

`m % single_core_m == 0` and `single_core_m % base_m == 0` mean that this example supports only complete `base_m` blocks; `n % single_core_n == 0` and `single_core_n % base_n == 0` mean that it supports only complete `base_n` blocks; `k % base_k == 0` means that it supports only complete `base_k` blocks. `base_m % 2 == 0` comes from `dual_dst_mode::split_m`: the M dimension of each output tile must be split evenly between two AIVs. `base_n % vector_len == 0` ensures that the AIV processes complete Vector blocks along the column dimension.

## Example Implementation

### Overall Data Flow

The data paths are as follows:

```text
GM (A/B)                           int8
  |  copy(GM -> L1)                                     MTE2
L1 (A/B)                          int8
  |  copy(L1 -> L0A/L0B)                                MTE1
L0A / L0B                         int8
  |  mmad                                               M
L0C                               int32
  |  copy(L0C -> UB, split_m)                           FIX
UB output slot (each AIV M half)   int32
  |  load(UB slot, int32)         int32 -> reg            V
  |  cast_int32_to_float       int32 -> float             V
UB middle_vec                      float
  |  copy(GM -> UB, channel_scale/token_scale)         MTE2
  |  compute_quant_fast_gelu    dequantization and FastGelu V
UB output_vec                      float
  |  copy(UB -> GM)                                     MTE3
GM z                               float
```

Here, the AIV uses `load` to read one row of int32 data from the current UB slot by coordinate directly into registers, without first copying it to a separate UB buffer. The same MTE2 stage copies the corresponding `channel_scale` segment from GM to UB, and `token_scale` is copied to UB before the row loop starts.

### Cube-side Matrix Multiplication

1. Create GM Tensor views from the runtime `m`, `n`, and `k`.
2. Divide the output into tiles using `base_m` and `base_n`, and iterate over K in `base_k` blocks.
3. When `k_iter % step_k == 0`, transfer up to `step_k` K blocks of A/B data from GM to L1 at once, reducing the GM-to-L1 transfer count. The last transfer may contain fewer than `step_k` complete blocks; `cur_step_k` controls the actual transfer length.
4. In each K iteration, transfer the current A/B blocks from L1 to L0A/L0B.
5. Before the first K iteration, L0C contains no result for the current output tile. Set `init_with_zero=true` so `mmad` clears L0C before writing the first K block's result.
6. Set `init_with_zero=false` in the second and later K iterations so the accumulated result in L0C is retained and the operation continues as `C = A * B + C`.
7. After the K loop, use `copy` to transfer the L0C result to the UB of the two AIVs.

The key `mmad` call is:

```cpp
mmad(mm.with(mmad_params(base_m, base_n, base_k, unit_flag_mode::disable, k_iter == 0)),
     l0_c, l0_a, l0_b);
```

In this call, the last argument `k_iter == 0` maps to `mmad_params::init_with_zero`: it is `true` in the first K iteration and `false` in later iterations.

### Dual-destination L0C-to-UB Transfer

The L0C-to-UB transfer uses the following custom trait:

```cpp
constexpr asc::te::l0c_to_ub_trait copy_l0c2ub_trait = {
    asc::te::round_mode::default_round, false, false,
    asc::te::dual_dst_mode::split_m};
```

`dual_dst_mode::split_m` splits one `base_m × base_n` L0C tile along the M dimension:

- AIV 0 processes the first `base_m / 2` rows.
- AIV 1 processes the last `base_m / 2` rows.

`dual_dst_mode::split_m` reduces the L0C-to-UB transfer count and lets the two AIVs process different M halves of the same output tile in parallel.

### AIV-side Dequantization and FastGelu

Each AIV obtains its M half with `get_sub_block_id()`: AIV 0 processes the first `base_m / 2` rows, and AIV 1 processes the last `base_m / 2` rows. With the default parameters, each output tile is `128 × 128`, so each AIV processes `64 × 128` elements. After receiving the ready notification from the AIC, the AIV starts processing data as follows:

1. Use `load` to read `vector_len` int32 Cube results for the current row from the current UB slot directly into Vector registers.
2. Read the `channel_scale` segment corresponding to the current column segment from GM, that is, `channel_scale[j * base_n + column : j * base_n + column + vector_len]`, and copy it to `scale_vec` in UB.
3. Read `token_scale[local_row]` for the current row from GM and copy it to `token_scale_vec` in UB.
4. Convert the int32 Cube results in the Vector registers to float and write them to `middle_vec`.
5. Multiply the float results in `middle_vec` by `channel_scale` from `scale_vec` and `token_scale` from `token_scale_vec` to obtain the dequantized result.
6. Apply `FastGelu(x) = x / (1 + exp(-1.702x))` to the dequantized result to obtain `output_vec`.
7. Write `output_vec` back to the row and column range that the current AIV is responsible for in output matrix z in GM.

The key code in `compute_quant_fast_gelu` is:

```cpp
auto value = experimental::load(src, coord).with_mask(mask);
auto channel_value = experimental::load(channel_scale, coord).with_mask(mask);
auto token_value =
    experimental::load_broadcast<experimental::broadcast_mode::elem>(token_scale, make_coord(0))
        .with_mask(mask);

auto scaled_value = value * channel_value * token_value;
auto denominator = experimental::exp(scaled_value * fast_gelu_coefficient) + 1.0f;
auto result = scaled_value / denominator;
experimental::store(dst, coord, result);
```

The flow advances row by row. Within each row, data is processed in `vector_len` column blocks, and each column block is a complete Vector block.

### Double Buffering and Synchronization

The AIC processes output tiles in generation order. Two UB output buffer groups are reused in rotation using `slot = sequence % pipe_depth`: even-numbered tiles use slot 0, and odd-numbered tiles use slot 1. After `dual_dst_mode::split_m` is enabled for the L0C-to-UB transfer, each output tile is split along the M dimension. The first M half is written to the UB corresponding to AIV 0, and the second M half is written to the UB corresponding to AIV 1. Therefore, the same slot number identifies one M-half buffer in each AIV's UB, and the two AIVs together process one complete output tile.

Each slot group is bound to a pair of synchronization flags: slot 0 uses flags 8/10, and slot 1 uses flags 9/11. The synchronization flow is:

1. Before reusing a slot, the AIC waits on `PIPE_MTE2` until both AIVs finish consuming the previous round, preventing unread data from being overwritten.
2. After the L0C-to-UB transfer for the current slot is complete, the AIC notifies the AIVs on `PIPE_FIX` that the data is ready.
3. Each AIV waits on `PIPE_S` for the data-ready event of the corresponding slot, then reads and processes its own M half.
4. After computation and GM writeback are complete, each AIV notifies the AIC on `PIPE_MTE3` that the slot can be reused.

As a result, while the AIC computes and transfers the next tile, the AIVs can process the previous tile, forming a pipelined overlap between Cube and Vector.

### copy Interface Usage

`copy` is the transfer interface. This example uses two call forms.

With the default trait and a source/destination region specified by coordinates and a shape, the interface has five arguments, and the transfer path is inferred from the source and destination Tensor locations:

```cpp
copy(dst, src, dst_coord, src_coord, copy_shape);
```

When a non-default trait is required and the source and destination Tensors already represent the full transfer objects, the interface has three arguments:

```cpp
copy(atom, dst, src);
```

The example uses the following forms:

| Path | Trait | Call Form | Purpose |
| --- | --- | --- | --- |
| GM -> L1 | Default | `copy(dst, src, dst_coord, src_coord, copy_shape)` | Transfer A/B data along the K dimension; each transfer contains `step_k` `base_k` blocks |
| L1 -> L0A | Default | `copy(dst, src, dst_coord, src_coord, copy_shape)` | Load the left matrix |
| L1 -> L0B | Default | `copy(dst, src, dst_coord, src_coord, copy_shape)` | Load the right matrix |
| GM -> UB | Default | `copy(dst, src, dst_coord, src_coord, copy_shape)` | Transfer channel_scale and token_scale |
| UB -> GM | Default | `copy(dst, src, dst_coord, src_coord, copy_shape)` | Write back the float output |
| L0C -> UB | Custom `split_m` | `copy(atom, dst, src)` | Split the L0C result along M and write it to the UB of two AIVs |

For the L0C-to-UB path, first use `make_copy` to construct a copy atom containing the custom trait, then call the `copy` interface. This example enables `dual_dst_mode::split_m` through the custom trait:

```cpp
// Trait configuration for the L0C-to-UB transfer.
constexpr asc::te::l0c_to_ub_trait copy_l0c2ub_trait = {
    // Use the default rounding mode.
    asc::te::round_mode::default_round,
    false,
    false,
    // Enable M-axis dual-destination mode: the L0C tile is split along M
    // to the UB of two AIVs.
    asc::te::dual_dst_mode::split_m};

// A custom trait must provide the TraitType alias and a compile-time value.
// make_copy reads the trait configuration through value.
struct CopyL0C2UBTraitCustom {
    using TraitType = asc::te::l0c_to_ub_trait;
    static constexpr const TraitType value = copy_l0c2ub_trait;
};

// Construct the copy atom from the L0C-to-UB operation object and custom trait.
auto l0c_to_ub = make_copy(copy_l0c_to_ub{}, CopyL0C2UBTraitCustom{});

// ub_dst already locates the current slot and M half through slice(), so this
// uses the full-Tensor transfer form; copy_atom passes the custom trait first.
copy(l0c_to_ub, ub_dst, l0_c);
```

## Build and Run

Execute the following steps in the root directory of this example.

### Configure Environment Variables

Configure environment variables according to the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit:

```bash
source ${install_path}/cann/set_env.sh
```

`${install_path}` is the CANN package installation directory.

### Build and Execute

```bash
mkdir -p build && cd build     # Create and enter the build directory
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..  # Configure the project, default NPU mode
make -j                        # Build the example
python3 ../scripts/gen_data.py # Generate input and golden data
./demo                         # Run the example
python3 ../scripts/verify_result.py output/output.bin output/golden.bin
```

To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

Example:

```bash
cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..
make -j
```

> **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and re-run cmake.

### Build Options

| Option | Available Values | Description |
| --- | --- | --- |
| `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | NPU execution, NPU simulation |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture corresponding to Ascend 950PR&950DT products |

This example supports only the `dav-3510` architecture.

### Execution Result

The following result indicates a successful precision comparison:

```text
test pass!
```

