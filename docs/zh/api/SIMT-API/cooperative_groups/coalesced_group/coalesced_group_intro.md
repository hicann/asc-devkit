# coalesced_group简介

在SIMT架构的硬件层面上，处理器以32个线程为一组（一个Warp）来执行线程。如果核函数（Kernel）代码中的条件分支导致Warp内的线程出现发散（Warp Divergence），那么Warp会串行执行每个分支，在执行某个分支时会屏蔽不在该指令路径上的线程。`coalesced_group`用于获取当前Warp中实际活跃的线程子集。

> [!CAUTION]注意 
> 使用`coalesced_group`时需关注SIMT架构不支持独立线程调度。

## Public成员函数

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
