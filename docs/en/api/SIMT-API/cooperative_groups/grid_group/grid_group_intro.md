# grid_group Overview

<!-- md-trans-meta sourceCommit=cd233cc88e0461543b11943c95e5c293570eb6fb translatedAt=2026-10-23T12:34:43.069Z pushedAt=2026-10-26T06:23:22.643Z nl=2 -->

`grid_group` is an abstraction of all SIMT threads launched by a kernel. Its thread scope covers all thread blocks and their threads in the current grid. Through `grid_group`, you can query the grid dimensions, thread block rank, and global thread rank, and perform cross-block synchronization within the grid by calling `sync()`.

## Public Member Functions

```cpp
bool is_valid() const;
void sync() const;
static unsigned long long thread_rank();
static unsigned long long block_rank();
static unsigned long long num_threads();
static unsigned long long size();
static unsigned long long num_blocks();
static dim3 dim_blocks();
static dim3 group_dim();
static dim3 block_index();
```

## Constraints

`grid_group` supports only the SIMT programming scenario and does not support mixed SIMD and SIMT programming.
