# FlashAttention Tensor API Sample

## Overview

This sample implements the FlashAttention forward pass and shows how to use the Tensor API for attention with a single-row Query. Inputs and output use `bfloat16_t` data; softmax intermediate state uses `float` data.

Attention computation produces a contextual representation for each Query head. Within one head, Query is one row of query features, Key is a set of index features, and Value is a set of content features aligned with Key row by row. The Query is first compared with every Key row to obtain similarity scores. Softmax converts these scores into attention weights whose sum is 1. Each weight determines how much the corresponding Value row contributes to the result. The final output is the weighted combination of all Value rows and represents the Query in the current context.

The sample has 32 Query heads and 4 KV heads. GQA (Grouped Query Attention) reuses Key and Value: the Key and Value of each KV head are shared by 8 Query heads. Each Query head has only one Query row, while each KV head has 4160 Key rows and 4160 Value rows, so the sample targets the single-row Query forward scenario. The sample does not include backward computation, causal masking, attention bias, dropout, or variable sequence length.

A direct implementation first materializes the complete `Query × Key` similarity matrix and then applies softmax to each row. FlashAttention divides the 4160 KV rows into KV splits and tiles. Each tile only updates the current maximum, softmax denominator, and unnormalized output; these states are merged at the end. This avoids storing the complete similarity matrix or weight matrix in GM while keeping the softmax result mathematically unchanged.

## Calculation Principle

### Attention for One Query Head

Consider Query head `h`, where `h` ranges from 0 to 31. The GQA mapping in this sample is `g(h) = h / 8`: Query heads 0 through 7 use KV head 0, Query heads 8 through 15 use KV head 1, and so on. Each Query head only accesses Key and Value from its mapped KV head.

In the formulas, `T` denotes matrix transpose. Q is the `1 × 128` Query row of Query head `h`; K is the `4160 × 128` Key matrix of the mapped KV head; and V is the `4160 × 128` Value matrix of the same KV head. `softmax_scale` is the scaling coefficient. This sample uses `1 / sqrt(128)` to keep dot products in a numerical range suitable for `float` exponentials.

The score between Query head `h` and Key row `t` is:

$$
S_h = \mathit{softmax\_scale} \times Q_h K_{g(h)}^T
$$

The matrix multiplication produces a `1 × 4160` result. Element `t` is the similarity between the Query and Key row `t`. A larger similarity gives the corresponding Value row a larger contribution in the following weighted sum.

Softmax first finds the maximum score so that large scores do not enter the exponential directly and cause overflow:

$$
m_h = \max_t S_{h,t}
$$

Here, `t` iterates over all 4160 rows of the current KV head. Each score is then shifted by this maximum before the exponential:

$$
w_{h,t} = \exp(S_{h,t} - m_h)
$$

The resulting `w` values are non-negative weights, but they are not yet final softmax weights. Only after the sum of all `w` values divides them do the 4160 weights sum to 1. The final output is:

$$
O_h = \frac{\sum_{t=0}^{L-1} w_{h,t} V_{g(h),t}}{\sum_{t=0}^{L-1} w_{h,t}}
$$

`L` is the KV sequence length, which is 4160 in this sample. Term `t` in the numerator multiplies unnormalized weight `t` by Value row `t`; the denominator sums all unnormalized weights. After division, O is the `1 × 128` attention-weighted output of Query head `h`.

### Online Softmax

Standard softmax must obtain the scores between the current Query head and all 4160 Key rows before it knows the global maximum and can compute the normalization denominator. Online softmax changes the execution order: it processes KV rows in blocks and updates the state after each block, so it never stores the complete score matrix.

This sample divides the 4160 KV rows into 13 KV splits. Each split is further divided into five `64 × 128` tiles. For each Query head and KV split, the split kernel maintains three `float` states: current maximum `m`, softmax denominator `l`, and unnormalized output `o`. When a new tile starts, the maximum is updated first:

$$
m_{\mathrm{new}} = \max(m_{\mathrm{old}}, \max_t S_t)
$$

Here, `t` iterates only over the 64 rows in the current tile. If the maximum changes, historical `l` and `o`, which were computed relative to the old maximum, must be converted to the new reference. The conversion factor is:

$$
\alpha = \exp(m_{\mathrm{old}} - m_{\mathrm{new}})
$$

Historical states are scaled by `α`, and the weights of the current tile are accumulated:

$$
l_{\mathrm{new}} = \alpha l_{\mathrm{old}} + \sum_t \exp(S_t - m_{\mathrm{new}})
$$

$$
o_{\mathrm{new}} = \alpha o_{\mathrm{old}} + \sum_t \exp(S_t - m_{\mathrm{new}}) V_t
$$

After all five tiles in a KV split are processed, the kernel has the local `m/l/o` state for that split. The merge kernel combines the 13 local states with the same relation and produces:

$$
O = \frac{o_{\mathrm{total}}}{l_{\mathrm{total}}}
$$

Therefore, online softmax and standard softmax have the same mathematical result. Standard softmax normalizes once after all scores are available; online softmax updates the state tile by tile and stores only fixed-size `m/l/o` state.

## Supported Products and CANN Software Version

<table border="2">
  <caption>Supported Products and CANN Software Version</caption>
  <tr><td>Product</td><td>CANN Software Version</td></tr>
  <tr><td>Ascend 950PR&950DT products</td><td>>= CANN 9.3.0</td></tr>
</table>

## Directory Structure

```text
├── CMakeLists.txt                  // Build project
├── README.md                       // Chinese description
├── README_en.md                    // English description
├── data_utils.h                    // Host-side binary file utilities
├── flash_attention_tensor_api.asc  // Kernel and host invocation code
└── scripts
    ├── gen_data.py                 // Input and golden data generation
    └── verify_result.py            // Output verification
```

## Sample Description

### Inputs and Output

<table border="2">
  <caption>Inputs and Output</caption>
  <tr><td align="left">Tensor</td><td align="left">Shape</td><td align="left">Data Type</td><td align="left">Description</td></tr>
  <tr><td align="left">query</td><td align="left">[32, 128]</td><td align="left">bfloat16_t</td><td align="left">32 Query heads, one row each</td></tr>
  <tr><td align="left">key</td><td align="left">[4, 4160, 128]</td><td align="left">bfloat16_t</td><td align="left">Keys of 4 KV heads</td></tr>
  <tr><td align="left">value</td><td align="left">[4, 4160, 128]</td><td align="left">bfloat16_t</td><td align="left">Values of 4 KV heads</td></tr>
  <tr><td align="left">output</td><td align="left">[32, 128]</td><td align="left">bfloat16_t</td><td align="left">Attention output of each Query head</td></tr>
</table>

### Sample Specifications

<table border="2">
  <caption>Sample Specifications</caption>
  <tr><td align="left">Item</td><td align="right">Value</td><td align="left">Description</td></tr>
  <tr><td align="left">`query_heads`</td><td align="right">32</td><td align="left">Number of Query heads</td></tr>
  <tr><td align="left">`kv_heads`</td><td align="right">4</td><td align="left">Number of KV heads</td></tr>
  <tr><td align="left">`kv_length`</td><td align="right">4160</td><td align="left">Sequence length of each KV head</td></tr>
  <tr><td align="left">`head_dim`</td><td align="right">128</td><td align="left">Feature dimension of each head</td></tr>
  <tr><td align="left">`tile_rows`</td><td align="right">64</td><td align="left">Number of KV rows processed at a time</td></tr>
  <tr><td align="left">`splits`</td><td align="right">13</td><td align="left">Number of KV sequence splits; each split is called a KV split</td></tr>
  <tr><td align="left">`tiles_per_split`</td><td align="right">5</td><td align="left">Number of 64-row tiles in each KV split; 13 × 5 × 64 = 4160</td></tr>
  <tr><td align="left">`head_group`</td><td align="right">2</td><td align="left">Number of Query heads processed by one split block</td></tr>
  <tr><td align="left">`partial_size`</td><td align="right">256</td><td align="left">float state size for each head and KV split, in floats</td></tr>
  <tr><td align="left">`split_blocks`</td><td align="right">208</td><td align="left">Number of Vector blocks launched by the split kernel, 32 / 2 × 13</td></tr>
  <tr><td align="left">`merge_blocks`</td><td align="right">32</td><td align="left">Number of Vector blocks launched by the merge kernel, one per Query head</td></tr>
  <tr><td align="left">`softmax_scale`</td><td align="right">1/sqrt(128)</td><td align="left">Scaling factor after the QK dot product</td></tr>
</table>

A Vector block is a parallel execution instance of a launched kernel. Each Vector block in this sample independently handles its Query heads and KV range.

### Computation Flow

The sample runs in two phases: split kernels compute local softmax states for different KV ranges in parallel, and merge kernels combine the 13 local states of each Query head to produce the final output.

1. The host program reads the three input tensors from `input/query.bin`, `input/key.bin`, and `input/value.bin`.
2. The host program calls `aclrtMemcpy` to copy query, key, and value from host memory to device GM.
3. The host program submits `flash_attention_split` to the stream with 208 Vector blocks. Each block derives 2 Query heads and 1 KV split from `block_idx` and determines its KV head with `g(h) = h / 8`.
4. The split kernel creates Tensor views for GM inputs, the GM workspace, UB input buffers, and the UB state buffer.
5. The split kernel uses `copy<copy_gm_to_ub>` to copy the 2 Query rows handled by the block from device GM to the Query Tensor in UB.
6. For each of the 5 `64 × 128` tiles in the current KV split, the split kernel performs the following steps:
   1. The split kernel computes the device GM addresses of the current Key and Value tiles from the split index, tile index, and GQA mapping.
   2. The split kernel uses `copy<copy_gm_to_ub>` to copy the current Key and Value tiles from device GM to the Key Tensor and Value Tensor in UB.
   3. The `compute_decode_pair_tile` function reads `bfloat16_t` elements from the UB Query Tensor and Key Tensor with `load<unpack>` and converts them to `float` with `cast`.
   4. For every row in the Key tile, the kernel computes QK dot products for both Query heads with multiplication and `fma`, then reduces each dot product to one score with `reduce_sum`.
   5. The kernel writes the 64 scores to the Score Tensor in UB and maintains the maximum score of the current tile for each Query head.
   6. If the tile is not the first tile, the kernel reads the previous `m/l/o` state from the UB State Tensor and rescales historical `l/o` to the new maximum using the old and new maxima.
   7. The kernel reads a `bfloat16_t` Value row from the UB Value Tensor, converts it to `float` with `cast`, computes current-tile weights with `exp_diff`, accumulates `weight × Value` with `fma`, and updates softmax denominator `l` and unnormalized output `o`.
   8. The kernel saves the updated `m/l/o` state in the UB State Tensor for the next tile.
7. After all 5 tiles in the KV split are processed, the split kernel uses `copy<copy_ub_to_gm>` to write the local `m/l/o` states of both Query heads from UB to the device GM workspace. Each `(head, split)` occupies 256 floats: offset 0 is the 128-value unnormalized output, offset 128 is the maximum, and offset 192 is the sum. The workspace size is `32 × 13 × 256 × 4 = 425984 bytes`.
8. The host program submits `flash_attention_merge` on the same stream with 32 Vector blocks, one for each Query head.
9. The merge kernel determines the Query head index from `block_idx` and uses `copy<copy_gm_to_ub>` to copy the 13 local states of that head from device GM workspace to the UB Partial Tensor.
10. The merge kernel reads the 13 local maxima from the UB Partial Tensor and computes the global maximum.
11. For each KV split, the merge kernel reads the local sum and 128 unnormalized output values from the UB Partial Tensor, computes `exp(local_max - global_max)` with `exp_diff`, rescales the local sum and output to the global-maximum reference, and accumulates the global sum and global unnormalized output.
12. The merge kernel divides the global unnormalized output by the global sum to obtain the `float` attention output.
13. The merge kernel converts the `float` output to `bfloat16_t` with `cast`, extracts the valid results with `deinterleave`, writes them to the UB Output Tensor with `store`, and copies the 128 `bfloat16_t` output elements to the output Tensor in device GM with `copy<copy_ub_to_gm>`.
14. The host program synchronizes the stream, calls `aclrtMemcpy` to copy the output from device GM back to host memory, and writes it to `output/output.bin`.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables
  Configure environment variables according to the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** ${install_path} is the CANN package installation directory. When no installation directory is specified, the default installation path is /usr/local/Ascend.

- Run the example

  Run the following commands in the example directory.

  ```bash
  mkdir -p build && cd build;                                               # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # Build the project (default NPU mode)
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic is correct
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then run cmake again.

- Build option description

  <table border="2">
    <caption>Build Options</caption>
    <tr><td align="left">Option</td><td align="left">Values</td><td align="left">Description</td></tr>
    <tr><td align="left">`CMAKE_ASC_RUN_MODE`</td><td align="left">`npu` (default), `sim`</td><td align="left">Run mode: NPU execution, NPU simulation</td></tr>
    <tr><td align="left">`CMAKE_ASC_ARCHITECTURES`</td><td align="left">`dav-3510`</td><td align="left">NPU architecture: dav-3510 corresponds to Ascend 950PR&950DT products</td></tr>
  </table>

  > **Note:** This example only supports dav-3510 architecture (corresponding to Ascend 950PR&950DT products).

- Execution result

  The following execution result indicates that the precision comparison is successful.

  ```bash
  test pass!
  ```
