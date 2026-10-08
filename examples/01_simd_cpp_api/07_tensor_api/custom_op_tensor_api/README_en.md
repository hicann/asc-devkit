# Tensor API Custom Op Sample

## Overview

This sample implements a fixed-shape Query-Key-Value (QKV) projection with the Tensor API. It focuses on how to define a custom Copy Operation and connect it to the Tensor API Atom invocation flow.

In Transformer self-attention, the input hidden states are normally passed through three linear projections to obtain queries (Q), keys (K), and values (V). Each projection result has a hidden size of `H`. To combine the three computations, the results can be concatenated along the last dimension as `[B, S, 3H]`, where `3H = H(Q) + H(K) + H(V)`. This sample therefore uses `weight[H, 3H]` and `bias[1, 3H]` to perform the three projections at once. The first, middle, and last `H`-wide segments of the output correspond to Q, K, and V, respectively. The subsequent attention computation can split the output again by these three segments.

The input is the Transformer hidden state (`hidden_states`). It is represented in this sample as a three-dimensional Batch ND (N-Dimension, standard data arrangement) tensor with logical shape `[B, S, H]`. While the input is moved from Global Memory to L1 Buffer, the custom Operation flattens the Batch and Sequence dimensions, changing the logical shape from `[B, S, H]` to `[B * S, H]`, and converts ND to NZ (fractal matrix arrangement):

```text
input_flat = reshape(input, [B * S, H])
output = input_flat * weight + bias
```

`weight[H, 3H]` and `bias[1, 3H]` are shared by all batches. The Q, K, and V projection results are stored contiguously in the last dimension of the output. This sample performs only the projection and does not split Q, K, and V.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|--------------|
| Ascend 950PR&950DT products | >= CANN 9.2.0 |

## Directory Structure

```text
├── scripts
│   ├── gen_data.py             // Generate input data and CPU-side golden data
│   └── verify_result.py        // Compare sample output with the golden data
├── CMakeLists.txt              // Build project file
├── custom_op_tensor_api.asc    // Custom Operation, Kernel, and host invocation
├── data_utils.h                // Binary file read/write utilities
├── README_en.md                // English documentation
└── README.md                   // Chinese documentation
```

## Sample Description

### Sample Specification

| Tensor | Logical Shape | Global Memory Format | Data Type | Description |
|--------|---------------|-----------|-----------|-------------|
| `input` | `[B, S, H] = [16, 64, 256]` | Batch ND | `half` | Input hidden state `hidden_states` |
| `weight` | `[H, 3H] = [256, 768]` | ND | `half` | Shared projection weight |
| `bias` | `[1, 3H] = [768]` | ND | `half` | Shared bias |
| `output` | `[B*S, 3H] = [1024, 768]` | ND | `half` | Projection result |

The output can also be interpreted logically as `[B, S, 3H] = [16, 64, 768]`. Flattening changes only logical indexing and does not require an additional data movement.

The matrix multiplication parameters are:

```text
M = B * S = 1024
K = H     = 256
N = 3 * H = 768
```

### Custom Copy Operation

The core of this sample is `copy_slice_gm_to_l1`. The input requires a custom Operation because its Global Memory source tensor retains three-dimensional Batch ND strides while its L1 Buffer destination tensor is two-dimensional NZ. The weight and bias use ordinary two-dimensional ND transfers and can continue to use standard `copy_gm_to_l1`.

The matrix multiplication itself uses the existing Tensor API `mmad` and standard copy Atoms. Only the input movement from Global Memory to L1 Buffer must simultaneously flatten Batch ND data and convert ND to NZ. The custom Copy Operation therefore encapsulates this underlying transfer and connects a batched input movement that does not fit standard two-dimensional Copy semantics to the unified Tensor API `copy(atom, dst, src)` flow.

#### Operation Implementation

`copy_slice_gm_to_l1` implements a static template function named `copy`. It reads the element type, shape, stride, destination NZ layout, and cache mode from the source and destination tensors, then derives the parameters required by the underlying C API. The logical format is:

| Tensor | Location | Shape | LayoutPattern |
|--------|----------|-------|---------------|
| Source | Global Memory | `[B_tile, S, K_tile]` | Batched `nd_layout_ptn` |
| Destination | L1 Buffer | `[B_tile * S, K_tile]` | `nz_layout_ptn` |

The Kernel processes one `base_m` by `base_k` input tile at a time, so:

```text
B_tile = base_m / S = 128 / 64 = 2
K_tile = base_k = 64
[2, 64, 64] Batch ND -> [128, 64] NZ
```

Tensor API binds a Copy Operation to its execution Trait through `copy_traits`, and `make_copy` creates an invocable Atom from that Trait. To connect `copy_slice_gm_to_l1` to this flow, the sample provides two `copy_traits` specializations:

```cpp
// Preserve a caller-provided custom Trait and bind it to this Operation.
template <typename Trait>
struct copy_traits<copy_slice_gm_to_l1, Trait>
    : public copy_traits<copy_slice_gm_to_l1, Trait, copy_slice_gm_to_l1, Trait> {};

// Use the default Global Memory-to-L1 Buffer Trait when no Trait is explicitly provided.
template <>
struct copy_traits<copy_slice_gm_to_l1> : public copy_traits<copy_slice_gm_to_l1, gm_to_l1_trait_default> {};
```

This allows `copy_slice_gm_to_l1` to be created and invoked like a standard Copy Operation:

```cpp
const auto custom_copy_atom = make_copy(copy_slice_gm_to_l1{}, gm_to_l1_trait_default{});
copy(custom_copy_atom, input_l1_tensor, input_gm_slice);
```

In the Kernel, the custom Atom is used only for the input:

```cpp
copy(custom_copy_atom, input_l1_tensor, input_gm_tensor.slice(...));
copy(weight_l1_tensor, weight_gm_tensor.slice(...));
```

#### Parameter Derivation

The Operation does not hard-code the sample parameters. Instead, it derives the transfer parameters from the tensor layouts. For the current `[2, 64, 64]` input slice and `half` type, the derivation is:

| Parameter | Current Value | Derivation and Meaning |
|-----------|---------------|------------------------|
| `matrix_num` | 2 | Batch tile in the source shape, equal to `base_m / S` |
| `n_value` | 64 | Sequence length in the source shape, equal to `S` |
| `d_value` | 64 | K-direction tile in the source shape, equal to `base_k` |
| `src_d_value` | 512 bytes | Stride between adjacent sequence rows in the source, equal to `H * sizeof(half)` |
| `src_nd_matrix_stride` | 32768 bytes | Stride between adjacent batch matrices in the source, equal to `S * H * sizeof(half)` |
| `dst_nz_n_stride` | 1 | Offset between adjacent NZ rows in the destination, in 32-byte units |
| `dst_nz_c0_stride` | 128 | Destination NZ column stride divided by `c0_element<half>`, in 32-byte units |
| `dst_nz_matrix_stride` | 64 | Offset between adjacent batch matrices in the destination NZ, equal to `n_value` in 32-byte units; the current value corresponds to 2048 bytes |
| `enable_small_c0` | `false` | The current `d_value` is 64, so SmallC0 mode is not used |

`src_d_value` and `src_nd_matrix_stride` are derived from the original source tensor strides rather than assuming that the source has already been flattened. This ensures that the second batch starts from the correct Global Memory address. `dst_nz_matrix_stride` ensures that multiple batches are written sequentially into the same `[base_m, base_k]` NZ destination without overwriting each other.

#### Underlying C API Invocation

After parameter derivation, the `copy` function delegates to the private helper `copy_gm_to_cbuf_multi_nd2nz`. This helper handles data-type dispatch, architecture-specific conditional compilation, and low-level pointer conversion. It converts tensor pointers to `__gm__` and `__cbuf__` pointers according to the data type, supports `half` and `float`, and selects the corresponding C API overload at compile time. In the 3510 architecture branch, it first sets the NZ transfer parameters and then performs the ND-to-NZ transfer:

```cpp
asc_set_gm2l1_nz_para(matrix_num, dst_nz_n_stride, dst_nz_c0_stride, dst_nz_matrix_stride);
asc_copy_gm2l1_nd2nz(
    dst, src, src_d_value, l2_cache_mode, n_value, d_value, src_nd_matrix_stride, enable_small_c0);
```

The source tensor cache mode is obtained through `src.engine().get_cache_mode()` and converted to the `asc_load_l2_cache_mode` type required by the C API. `asc_set_gm2l1_nz_para` configures the destination NZ stride relationship, while `asc_copy_gm2l1_nd2nz` reads multiple batch matrices using the source strides and performs the format conversion. Both calls are required.

#### Usage Constraints

- The source tensor must use a three-dimensional batched `nd_layout_ptn` layout in Global Memory, and the destination tensor must use a two-dimensional `nz_layout_ptn` layout in L1 Buffer. Their shapes must satisfy `[B_tile, S, K_tile] -> [B_tile * S, K_tile]`.
- The source and destination element types must be the same and must be `half` or `float`, as supported by the current implementation.
- The destination row count must equal `B_tile * S`, and the K-direction size must equal `K_tile`.
- The destination NZ layout must provide a derivable C0 column stride.
- The underlying implementation calls the 3510 C APIs only in the `current_arch_version == arch_version::v3510` branch. Porting to another architecture requires an architecture-specific ND-to-NZ implementation.

## Build and Run

Run the following steps in the sample root directory.

- Configure environment variables.

  Configure the environment variables according to the CANN development kit [installation method](../../../../docs/en/quick_start.md#prepare&install).

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** ${install_path} is the CANN installation directory. If no installation directory is specified, CANN is installed under `/usr/local/Ascend` by default.

- Run the sample.

  Run the following commands in the sample directory.

  ```bash
  mkdir -p build && cd build;                                                   # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                          # Build the project; NPU mode by default
  python3 ../scripts/gen_data.py                                                # Generate test input and golden data
  ./demo                                                                        # Run the sample
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin       # Verify that the output matches the golden data
  ```

  For NPU simulation mode, add `-DCMAKE_ASC_RUN_MODE=sim`.

  For example:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Before switching build modes, clean the CMake cache. In the `build` directory, run `rm CMakeCache.txt` and run `cmake` again.

- Build options

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU or NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture; `dav-3510` corresponds to Ascend 950PR&950DT products |

  The following output indicates that the precision comparison passed:

  ```bash
  test pass!
  ```
