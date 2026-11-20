# Introduction to thread_block_tile

<!-- md-trans-meta sourceCommit=1f4a2b944d6a3be0dcda8e236e323fd4ad48808a translatedAt=2026-10-23T12:55:52.458Z pushedAt=2026-10-26T06:23:22.698Z nl=2 -->

`thread_block_tile` is a template class used to manage a thread subgroup of a specified size.

> [!CAUTION] Caution 
> The SIMT architecture does not support independent thread scheduling. Data dependencies between cooperative groups within a Warp should be avoided; otherwise, a deadlock may occur.

## Public Member Functions

```cpp
void sync() const;
unsigned int num_threads() const;
unsigned int thread_rank() const;
unsigned int meta_group_size() const;
unsigned int meta_group_rank() const;
template <typename T>
T shfl(T var, int src_rank) const;
template <typename T>
T shfl_up(T var, unsigned int delta) const;
template <typename T>
T shfl_down(T var, unsigned int delta) const;
template <typename T>
T shfl_xor(T var, unsigned int lane_mask) const;
int any(int predicate) const;
int all(int predicate) const;
unsigned int ballot(int predicate) const;
unsigned int size() const;
```

## Interface Support Scope

**Table 1** `thread_block_tile` API support scope

| API | `Size <= 32` (thread subgroup within a single warp) | `Size > 32` (thread subgroup across warps) |
| --- | --- | --- |
| `sync()` | Supported | Supported |
| `thread_rank()` | Supported | Supported |
| `num_threads()` | Supported | Supported |
| `size()` | Supported | Supported |
| `meta_group_rank()` | Supported | Supported |
| `meta_group_size()` | Supported | Supported |
| `shfl()` | Supported | Supported |
| `any()` | Supported | Supported |
| `all()` | Supported | Supported |
| `ballot()` | Supported | Not supported |
| `shfl_up()` | Supported | Not supported |
| `shfl_down()` | Supported | Not supported |
| `shfl_xor()` | Supported | Not supported |
