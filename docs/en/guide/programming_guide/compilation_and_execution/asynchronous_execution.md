# Asynchronous Execution

<!-- md-trans-meta sourceCommit=198cbfdf29f72f78c8de735d88a5244dc258f2e7 translatedAt=2026-08-26T10:51:31.214Z pushedAt=2026-08-31T06:12:17.014Z -->

After a developer finishes writing a kernel function, they need to submit it to a specified stream (compute stream) for execution using specific dispatch/start syntax. For example:

```c++
     kernel<<<numBlocks, ubufDynSize, stream>>>(Parameter list);
     // Or use the corresponding API: aclrtLaunchKernel*(...)
```

These kernel functions are loaded and run through CANN Runtime. CANN Runtime is the core component in the CANN software stack responsible for driving hardware execution and managing AI compute tasks. By providing APIs for device, memory, context, stream, and kernel management, it enables upper-layer applications and frameworks to efficiently utilize the hardware compute resources of AI processors.

The main process of loading and running a kernel consists of the following steps:

```txt
1. Initialization: aclInit.
2. Runtime resource allocation: Allocate device and stream runtime management resources through aclrtSetDevice and aclrtCreateStream, respectively.
3. If device memory is involved in computation, use aclrtMalloc to allocate device memory, and copy the memory from the host side to the device side through aclrtMemcpy or aclrtMemcpyAsync.
4. Call the kernel function through kernel<<<numBlocks, ubufDynSize, stream>>>(argument list) to complete the asynchronous execution of the kernel function.
5. After executing the kernel function, you need to synchronize and wait for the kernel function execution to complete.
6. If the host needs the computation result of the device memory, use aclrtMemcpy or aclrtMemcpyAsync to copy the memory from the device side to the host side.
7. Resource release: Release the stream and device runtime management resources through aclrtDestroyStream and aclrtResetDevice, respectively.
8. De-initialization: aclFinalize.
```

![](../../figures/npu_side_run_verification_steps.png)

In this process, we mentioned stream creation and destruction, memory allocation and copy, asynchronous calls, and synchronization waiting. In the rest of this section, we will explain these CANN Runtime elements and their corresponding APIs.

## Stream Management

A stream (compute stream) is essentially an ordered task queue. It provides developers with an abstract mechanism for representing "ordered execution". A program can submit various operations to the queue, such as memory copy and kernel function dispatch, and all tasks are executed sequentially in the order in which they are enqueued. Within a single compute stream, the task at the head of the queue is executed first and is automatically dequeued upon completion; subsequent tasks move forward in the queue and wait to be scheduled for execution. Tasks in the same compute stream are executed serially and strictly follow the order in which they are enqueued.

An application can create and use multiple compute streams simultaneously. In a multi-stream scenario, the Runtime component selects tasks from compute streams that contain pending tasks for execution based on the hardware resource occupancy of the AI processor. Developers can configure a priority for a compute stream. This priority serves as a reference for runtime scheduling and is used to adjust the task scheduling policy, but it cannot enforce a fixed execution order.

API calls and kernel function dispatch operations bound to a specified compute stream are executed asynchronously with respect to the host CPU thread. An application can achieve stream-level synchronization by waiting for all tasks in the target compute stream to be cleared, or it can achieve global synchronization at the device level.

CANN Runtime provides a built-in default compute stream. All compute operations and kernel function dispatch tasks that do not explicitly specify a compute stream are enqueued into this stream by default. Most business code that does not manually specify a compute stream implicitly uses the default compute stream.

- **Creating and destroying a stream**: `aclrtCreateStream(&stream)` and `aclrtDestroyStream(stream)`.

- **Specifying a stream to execute a kernel function**: `kernel<<<numBlocks, 0, stream>>>(...)`.

- **Specifying a stream for asynchronous copy**: `aclrtMemcpyAsync(..., stream)`.

- **Specifying a stream for synchronization**: `aclrtSynchronizeStream(stream)`.

## Memory Management

In a heterogeneous compute architecture, the system consists of a host and a device. The host and the device each have independent memory. Host memory refers to the host memory (that is, CPU memory) of the server where the AI processor resides, while device memory refers to the device memory built into the AI processor.

Two things must be done well in memory management:

1. **Memory accessibility**: Runtime provides a set of memory management APIs, allowing developers to write memory management code in applications efficiently and conveniently. Since host memory and device memory are independent of each other, Runtime provides dedicated APIs to allocate and free host memory and device memory separately. For example, the APIs for allocating and freeing host memory are **aclrtMallocHost** and **aclrtFreeHost**, while the APIs for allocating and freeing device memory are **aclrtMalloc** and **aclrtFree**.

2. **Efficient memory access**: When an operator runs on the device, accessing device memory data on the device delivers higher performance. To this end, Runtime provides APIs for copying memory between the host and the device, supporting both synchronous and asynchronous modes, such as **aclrtMemcpy** and **aclrtMemcpyAsync**, so that developers can better plan data storage and access.

## Asynchronous Call

To better utilize the hardware of the CPU and AI processor, CANN Runtime follows the "return upon dispatch" asynchronous execution policy. After the host submits an H2D/D2H copy or kernel computation task, it immediately continues executing subsequent logic. This allows host-side scheduling, device-side computation, and data transfer between the host and device to overlap as much as possible, hiding transfer and scheduling overhead and improving end-to-end throughput.

A typical <<<\>\>\> asynchronous call scenario is to overlap host and device computation operations, thereby improving the utilization of host-side and device-side compute resources.
![Kernel asynchronous call](../../figures/asynchronous_execution.png)

Similarly, replacing **aclrtMemcpy** with the **aclrtMemcpyAsync** API allows memory transfer between the host and device to overlap with computation operations, thereby reducing or even eliminating the performance overhead introduced by memory transfer.
![Concurrent asynchronous execution of memory transfer](../../figures/asynchronous_execution_async.png)

## Synchronization Wait

Kernel function calls are asynchronous. After a kernel function call ends, the control permission is immediately returned to the host. To wait for the kernel function to finish executing on the host, you can use the following APIs to force the host program to wait until the kernel function execution is complete:

|Synchronous API |Description|
|-|-|
|`aclrtSynchronizeDevice()` | Blocks the current thread on the host until all operations on the device corresponding to the context bound to the current thread are complete.|
|`aclrtSynchronizeStream()` | Blocks the current thread on the host until all operations in the specified stream are complete. |

## More Advanced Runtime Features

For more Runtime programming methods and APIs, see [*Application Development (C&C++)*](https://hiascend.com/document/redirect/CannCommunityadev).