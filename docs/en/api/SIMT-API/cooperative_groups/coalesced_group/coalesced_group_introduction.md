# Introduction to coalesced_group 

<!-- md-trans-meta sourceCommit=63bf9a47a5120d9c92d490c985eae13fcaa18fac translatedAt=2026-09-07T06:40:44.283Z pushedAt=2026-09-07T09:11:27.074Z -->

At the hardware level of the SIMT architecture, the processor executes threads in groups of 32 threads (one Warp). If a conditional branch in the kernel function code causes the threads within a Warp to diverge (Warp Divergence), the Warp executes each branch serially, masking out threads that are not on the current instruction path when executing a particular branch. `coalesced_group` is used to obtain the subset of threads that are actually active in the current Warp.

> [!CAUTION]Note
> When using **coalesced_group**, note that the SIMT architecture does not support independent thread scheduling.

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
int any(int predicate) const;
int all(int predicate) const;
unsigned int ballot(int predicate) const;
unsigned long long size() const;
```
