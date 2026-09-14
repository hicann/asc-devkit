# Introduction to `thread_block_tile` 

<!-- md-trans-meta sourceCommit=d527ad95eb60550038fb5b9a21039d8e30ec687f translatedAt=2026-09-07T07:01:57.182Z pushedAt=2026-09-07T09:11:27.135Z -->

`thread_block_tile` is a template class used to manage a thread subgroup of a specified size.

> [!CAUTION] Note
> The SIMT architecture does not support independent thread scheduling. Data dependencies between cooperative groups within a Warp should be avoided; otherwise, a deadlock may occur.

## Public Member Functions

```c++
void sync() const;
unsigned long long num_threads() const;
unsigned long long thread_rank() const;
unsigned long long meta_group_size() const;
unsigned long long meta_group_rank() const;
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
unsigned long long size() const;
```
