# grid_group简介

`grid_group`是对一个核函数（Kernel）启动的所有SIMT线程的抽象，线程范围覆盖当前Grid中的所有线程块及其线程。通过`grid_group`可以查询Grid维度、线程块排名、全局线程排名，并通过`sync()`在Grid范围内进行跨线程块同步。

## Public成员函数

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

## 需包含的头文件

```cpp
#include "simt_api/cooperative_groups.h"
```

## 约束说明

`grid_group`仅支持SIMT编程场景，不支持SIMD与SIMT混合编程场景。
