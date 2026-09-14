# `thread_block` Introduction

<!-- md-trans-meta sourceCommit=63bf9a47a5120d9c92d490c985eae13fcaa18fac translatedAt=2026-09-07T06:52:12.074Z pushedAt=2026-09-07T09:11:27.114Z -->

`thread_block` is an abstraction of a thread block. It represents the launch configuration of a kernel (the number of thread blocks launched, the number of threads in each thread block, and so on).

## Public Member Functions

```c++
static void sync();
static unsigned int thread_rank();
static dim3 group_index();
static dim3 thread_index();
static dim3 dim_threads();
static unsigned int num_threads();
static unsigned int size();
static dim3 group_dim();
```
