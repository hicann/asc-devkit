# **thread_block** Introduction

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:44:23.152Z pushedAt=2026-10-26T06:23:22.671Z nl=2 -->

`thread_block` is an abstraction of a thread block. It represents the launch configuration of a kernel (Kernel), including the number of thread blocks to launch, the number of threads in each thread block, and so on.

## Public Member Functions

```cpp
static void sync();
static unsigned int thread_rank();
static dim3 group_index();
static dim3 thread_index();
static dim3 dim_threads();
static unsigned int num_threads();
static unsigned int size();
static dim3 group_dim();
```
