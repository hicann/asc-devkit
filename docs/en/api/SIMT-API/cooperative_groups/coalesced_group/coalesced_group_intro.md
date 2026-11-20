# coalesced_group Overview

<!-- md-trans-meta sourceCommit=57ffc0e98dc1ae7768d3eaadf217e1d94fdb6424 translatedAt=2026-10-23T12:21:07.578Z pushedAt=2026-10-27T09:21:55.651Z nl=2 -->

At the hardware level of the SIMT architecture, the processor executes threads in groups of 32 threads (a Warp). If a conditional branch in the kernel code causes the threads within a Warp to diverge (Warp divergence), the Warp executes each branch serially, masking the threads that are not on the instruction path when executing a particular branch. `coalesced_group` is used to obtain the subset of threads that are actually active in the current Warp.

> [!CAUTION] Caution
> When using `coalesced_group`, note that the SIMT architecture does not support independent thread scheduling.

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
int any(int predicate) const;
int all(int predicate) const;
unsigned int ballot(int predicate) const;
unsigned int size() const;
```
