# Synchronization Control API Samples

## Overview

This sample collection introduces multiple API samples related to synchronization control, including intra-core synchronization, inter-core synchronization, inter-task synchronization, and RegBase synchronization.

## Sample List

### Intra-core Synchronization

| Directory Name | Description | Supported Products |
|----------|----------| --- |
| [data_sync_barrier](./data_sync_barrier) |  Implements scalar pipeline GM access write ordering synchronization using DataSyncBarrier | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [mutex](./mutex) |  Demonstrates the usage of Mutex::Lock, Mutex::Unlock, AllocMutexID, and ReleaseMutexID intra-core pipeline synchronization interfaces | Ascend 950PR&950DT products |

### Inter-core Synchronization

| Directory Name | Description | Supported Products |
|----------|----------| --- |
| [cross_core_set_wait_flag](./cross_core_set_wait_flag) |  Implements inter-core synchronization using CrossCoreSetFlag and CrossCoreWaitFlag, supporting three modes: all-core synchronization, AIV synchronization within a single AI Core, and AIC-AIV synchronization | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [group_barrier](./group_barrier) |  Implements synchronization between two groups of AIVs with dependencies using GroupBarrier; after group A AIVs complete computation, group B AIVs proceed with subsequent calculations dependent on those results | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [ib_set_wait](./ib_set_wait) |  Implements inter-core synchronization using IBSet and IBWait, applicable to scenarios where different cores operate on the same global memory with data dependency issues | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [sequential_block_sync](./sequential_block_sync) |  Implements inter-core sequential synchronization using a combination of InitDetermineComputeWorkspace, WaitPreBlock, and NotifyNextBlock interfaces, ensuring multi-core execution in ascending blockIdx order | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [sync_all](./sync_all) |  Implements inter-core synchronization using SyncAll, applicable to scenarios where different cores operate on the same global memory with data dependency issues such as read-after-write, write-after-read, and write-after-write | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |

### Inter-task Synchronization

| Directory Name | Description | Supported Products |
|----------|----------| --- |
| [task_sync](./task_sync) |  Implements Superkernel sub-kernel parallelism using WaitPreTaskEnd and SetNextTaskStart; when used together, maximizes parallelism between sub-kernels and improves overall performance | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |

### RegBase Synchronization

| Directory Name | Description | Supported Products |
|----------|----------| --- |
| [reg_sync](./reg_sync) |  Demonstrates UB read/write operation synchronization under Reg programming interface, including register ordering preservation (optimizing synchronization instructions between reads and writes) and LocalMemBar interface ordering preservation scenarios | Ascend 950PR&950DT products |