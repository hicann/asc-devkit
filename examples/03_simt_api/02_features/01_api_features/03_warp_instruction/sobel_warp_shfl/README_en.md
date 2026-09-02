# SIMT Sobel Edge Detection Example

## Overview

This example uses Sobel edge detection to demonstrate how to reuse data loaded by adjacent threads through Warp shuffle instructions to complete 3x3 convolution computation, and how to implement compact output of edge pixels in combination with [asc_ballot()](../../../../../../docs/zh/api/SIMT-API/Warp_functions/Warp_vote_functions/asc_ballot.md).

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version

- \>= CANN 9.1.0

## Directory Structure

```text
├── sobel_warp_shfl
│   ├── figures                // Image resources for README
│   ├── CMakeLists.txt         // CMake build file
│   ├── sobel_warp_shfl.asc    // Sobel edge detection implementation & invocation example
│   └── README.md
```

## Example Description

### Example Function

This example computes the edge magnitude of an input grayscale image and outputs edge pixels that meet the threshold condition to a compact list `edge_list`. Each row in `edge_list` contains the x coordinate, y coordinate, and edge magnitude (`strength`) of an edge pixel.

This example uses the **Sobel edge detection algorithm** to compute edge magnitude. Sobel is a commonly used image edge detection algorithm that computes the edge magnitude of a pixel based on the grayscale changes around the pixel. For a non-boundary pixel at coordinates (x, y) in the image, Sobel uses the 3x3 neighborhood around the pixel to compute the horizontal gradient `Gx` and vertical gradient `Gy` respectively:

<p align="center">
  <img src="./figures/sobel.png" width="45%">
</p>

`Gx` and `Gy` are computed as follows:

```text
Gx = -top_left + top_right - 2 * middle_left + 2 * middle_right - bottom_left + bottom_right
Gy = -top_left - 2 * top - top_right + bottom_left + 2 * bottom + bottom_right
```

The edge magnitude of the pixel is:

```text
strength = |Gx| + |Gy|
```

For boundary pixels of the image (where a complete 3x3 neighborhood cannot be formed), the magnitude is directly set to 0. For non-boundary pixels, after computing `strength`, only edge pixels whose `strength` exceeds the threshold (200 in this example) have their coordinates and magnitude written to edge_list.

### Example Specifications

<table>
<tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">sobel_edge_detect</td></tr>
<tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">input</td><td align="center">[1025,1023]</td><td align="center">uint8_t</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">edge_list</td><td align="center">[N,3]</td><td align="center">uint16_t</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">sobel_edge_detect</td></tr>
</table>

### Example Implementation

**Obtaining Adjacent Pixels within a Warp**

In the SIMT programming model, a Warp consists of 32 threads. This example needs to obtain adjacent pixel values in the same row through Warp shuffle. Since shuffle can only transfer data between threads within the same Warp, it must be ensured that all threads in the same Warp process 32 consecutive pixels in the same row of the image, with each thread responsible for one pixel.

To ensure that the 32 threads in the same Warp always process pixels in the same row, the following coordinate mapping is used:

```cpp
int tiles_x = (width + warpSize - 1) / warpSize;    // Number of Warp-sized blocks in the width direction (32 columns per block)
int x = (warp_id % tiles_x) * warpSize + lane_id;   // Pixel column: current Warp's horizontal start position + intra-Warp offset, ensuring same Warp same row
int y = warp_id / tiles_x;                          // Pixel row: which Warp row
```

<p align="center">
  <img src="./figures/shfl.png" width="80%">
</p>


As shown in the figure above, computing the Sobel magnitude requires obtaining 9 pixel values from the 3x3 neighborhood around the current pixel. When adjacent threads compute their respective Sobel magnitudes, they also need the pixel values from their 3x3 neighborhoods, so the 3x3 neighborhoods of different threads overlap. Leveraging this characteristic, each thread first reads three pixel values at the same x coordinate (top, middle, bottom) from Global Memory, and saves them to register variables top, mid, and bot, corresponding to the middle column of the current pixel's 3x3 neighborhood. Then, [asc_shfl_up()](../../../../../../docs/zh/api/SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl_up.md) and [asc_shfl_down()](../../../../../../docs/zh/api/SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl_down.md) are used to obtain the left and right column data from adjacent thread registers, avoiding additional reads from Global Memory.



```cpp
// Each thread first reads the top, middle, and bottom pixels in the current pixel's column.
// in_image indicates whether the current thread coordinate is within the valid image range; when false, pixel value is set to 0.
int top = in_image ? input[(y - 1) * width + x] : 0;
int mid = in_image ? input[y * width + x] : 0;
int bot = in_image ? input[(y + 1) * width + x] : 0;

// Obtain left and right column data from adjacent threads.
int shfl_top_left = asc_shfl_up(top, 1, warpSize);
int shfl_top_right = asc_shfl_down(top, 1, warpSize);
int shfl_mid_left = asc_shfl_up(mid, 1, warpSize);
int shfl_mid_right = asc_shfl_down(mid, 1, warpSize);
int shfl_bot_left = asc_shfl_up(bot, 1, warpSize);
int shfl_bot_right = asc_shfl_down(bot, 1, warpSize);
```

For non-boundary pixels that need to participate in Sobel computation, threads at Warp boundaries require special handling: the left neighbor of thread 0 and the right neighbor of thread 31 are not in the current Warp, so data must be read from Global Memory:

```cpp
// At Warp boundaries, adjacent pixels cannot be obtained through shuffle and must be read from Global Memory.
int top_left = (lane_id == 0) ? input[(y - 1) * width + (x - 1)] : shfl_top_left;
int top_right = (lane_id == 31) ? input[(y - 1) * width + (x + 1)] : shfl_top_right;
int mid_left = (lane_id == 0) ? input[y * width + (x - 1)] : shfl_mid_left;
int mid_right = (lane_id == 31) ? input[y * width + (x + 1)] : shfl_mid_right;
int bot_left = (lane_id == 0) ? input[(y + 1) * width + (x - 1)] : shfl_bot_left;
int bot_right = (lane_id == 31) ? input[(y + 1) * width + (x + 1)] : shfl_bot_right;
```

After obtaining all 9 pixels of the 3x3 neighborhood, compute the horizontal gradient `Gx` and vertical gradient `Gy` according to the Sobel formula to obtain the edge magnitude.

**asc_ballot for Compact Output**

Compact output requires writing edge pixels that meet the threshold condition consecutively to edge_list. For the same Warp, only some threads may have edge magnitudes that meet the threshold condition. If each edge thread independently executes [asc_atomic_add()](../../../../../../docs/zh/api/SIMT-API/atomic_operations/asc_atomic_add.md) to request an output position, up to 32 atomic operations would be generated within one Warp.

This example uses `asc_ballot()` to collect whether each thread in the Warp meets the threshold condition and generate edge_mask. The i-th bit in edge_mask being 1 indicates that thread i in the Warp has an edge magnitude that meets the threshold condition:

```cpp
uint32_t edge_mask = asc_ballot(strength >= THRESHOLD);
```

edge_mask is used to complete two operations:

- Thread 0 counts the number of edge threads in this Warp through [__popc(edge_mask)](../../../../../../docs/zh/api/SIMT-API/math_functions/integer_math_functions/__popc.md), and requests consecutive output positions for the entire Warp through a single `asc_atomic_add()`.
- Each edge thread computes its local offset within the Warp based on edge_mask and writes to the corresponding position in edge_list.

After thread 0 requests the output position, it broadcasts the base position to all threads in the Warp through [asc_shfl()](../../../../../../docs/zh/api/SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl.md):

```cpp
uint32_t warp_base = 0;
if (lane_id == 0) {
    // Thread 0 requests output positions for all edge threads in this Warp at once.
    warp_base = asc_atomic_add(edge_count, static_cast<uint32_t>(__popc(edge_mask)));
}
warp_base = asc_shfl(warp_base, 0, warpSize);
```

Each edge thread then computes its local offset within the Warp based on edge_mask and writes `{x, y, strength}` to edge_list:

```cpp
if (edge_mask & lanemask_eq()) {
    // Count how many edge threads precede the current thread to obtain the compact write offset within this Warp.
    uint32_t lower_mask = edge_mask & lanemask_lt();
    int local_idx = __popc(lower_mask);
    edge_list[warp_base + local_idx] = {
        static_cast<uint16_t>(x),
        static_cast<uint16_t>(y),
        strength
    };
}
```

Where `lanemask_eq()` generates only the mask corresponding to the current thread, used to detect whether the current thread's edge magnitude meets the threshold condition; `lanemask_lt()` generates masks for all threads preceding the current thread, used to count the number of threads meeting the threshold condition before the current thread, that is, the compact write offset of the current edge thread within this Warp.

Compared to each edge thread independently executing `asc_atomic_add()`, this approach compresses up to 32 atomic operations within one Warp into 1 atomic operation.

## Build and Run

Run the following steps in the root directory of this example to build and execute the operator.
- Configure Environment Variables  
  Configure environment variables based on the [installation method](../../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example

  Run the following commands in this example directory.

  ```bash
  mkdir -p build && cd build;                              # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # Build the project
  ./demo                                                   # Run the example
  ```

  Build Options Description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |

  The following output indicates that the accuracy verification is successful.
  ```
  [Success] Case accuracy verification passed.
  ```
