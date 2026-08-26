# thread_block_tile简介

`thread_block_tile`是一个模板类，用于管理指定大小的线程子组。

> [!CAUTION]注意 
> SIMT架构不支持独立线程调度，一个Warp内的各协作组间应避免存在数据依赖，否则可能出现卡死的情况。

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
template <typename T>
T shfl_xor(T var, unsigned int lane_mask) const;
int any(int predicate) const;
int all(int predicate) const;
unsigned int ballot(int predicate) const;
unsigned int size() const;
```

## 接口支持范围

**表1**  `thread_block_tile`接口支持范围

| 接口 | `Size <= 32`（单Warp内线程子组） | `Size > 32`（跨Warp线程子组） |
| --- | --- | --- |
| `sync()` | 支持 | 支持 |
| `thread_rank()` | 支持 | 支持 |
| `num_threads()` | 支持 | 支持 |
| `size()` | 支持 | 支持 |
| `meta_group_rank()` | 支持 | 支持 |
| `meta_group_size()` | 支持 | 支持 |
| `shfl()` | 支持 | 支持 |
| `any()` | 支持 | 支持 |
| `all()` | 支持 | 支持 |
| `ballot()` | 支持 | 不支持 |
| `shfl_up()` | 支持 | 不支持 |
| `shfl_down()` | 支持 | 不支持 |
| `shfl_xor()` | 支持 | 不支持 |
