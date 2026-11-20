# mutex

<!-- md-trans-meta sourceCommit=8d12fdc1392b422cc5d95088e74075f8f24a6135 translatedAt=2026-11-19T03:21:06.307Z pushedAt=2026-11-19T05:54:06.699Z nl=2 -->

## Supported Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Function Description

The header file path is `tensor_api/tensor.h`.

`mutex` is an object-based pipeline synchronization handle. The template parameter `Pipe` specifies the pipeline at compile time, the constructor stores the runtime hardware lock ID, and `lock` and `unlock` are used to get and release the lock resource, respectively.

`lock` gets the Mutex based on `mutex_id`. If the Mutex is already locked, it blocks the execution of subsequent instructions on the specified pipeline until all Mutexes with the same `mutex_id` across all pipelines have been released. `unlock` releases the corresponding Mutex based on `mutex_id` after the preceding instructions on the specified pipeline have completed execution.

mutex is used for resource handover between asynchronous pipelines within a core. It is not a CPU thread lock, nor a cross-core mutex lock.

## Prototype

```cpp
using mutex_id = uint8_t;

template <pipe_t Pipe>
class mutex {
public:
    __aicore__ inline explicit mutex(mutex_id id)
    __aicore__ inline void lock()
    __aicore__ inline void unlock()

    mutex(const mutex&) = delete;
    mutex& operator=(const mutex&) = delete;
    mutex(mutex&&) = delete;
    mutex& operator=(mutex&&) = delete;
};
```

## Parameters

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| Pipe | - | Compile-time template parameter of type `pipe_t`, specifying the pipeline associated with the Mutex. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| id | Input | Constructor parameter, hardware lock ID of type `mutex_id`, that is, `uint8_t`. |

## Return Value

The constructor has no return value. `lock` and `unlock` have no return value.

## Constraints

- The `Pipe` values supported by AIC are `PIPE_S`, `PIPE_M`, `PIPE_MTE1`, `PIPE_MTE2`, and `PIPE_FIX`; the `Pipe` values supported by AIV are `PIPE_S`, `PIPE_MTE2`, `PIPE_MTE3`, and `PIPE_V`.
- `mutex_id` is managed by you. The same group of synchronization dependencies must use the same `mutex_id`, and different synchronization dependencies are recommended to use different `mutex_id` values; otherwise, the actual synchronization behavior may not match expectations.
- `lock` and `unlock` must be used strictly in pairs and use the same `Pipe` and `mutex_id`. The corresponding `unlock` must always follow `lock`; otherwise, it is undefined behavior.
- For `lock` and `unlock` combinations with the same `mutex_id`, nesting is not allowed regardless of whether the `Pipe` is the same; otherwise, it is undefined behavior.
- When two pairs of `lock` and `unlock` with the same `mutex_id` and `Pipe` are called consecutively, the second `lock` cannot block the same pipeline again. Therefore, this usage cannot implement order control between multiple instructions within the same pipeline. For order control between multiple instructions within a single pipeline, use [sync_pipe](sync_pipe.md).

## Example

The following example uses mutex to protect three pipeline stages: GM-to-UB copy-in, in-UB computation, and UB-to-GM copy-out, ensuring that each stage executes in dependency order.

```cpp
#include "tensor_api/tensor.h"

constexpr uint32_t total_length = 64;
constexpr uint8_t buffer_lock_id = 0;

__global__ __vector__ void mutex_kernel(__gm__ float* src, __gm__ float* dst)
{
    using namespace asc::te;
    __ubuf__ float buffer[total_length];

    auto layout = make_layout(make_shape(total_length));
    auto src_gm = make_tensor(make_mem_ptr<location::gm>(src), layout);
    auto dst_gm = make_tensor(make_mem_ptr<location::gm>(dst), layout);
    auto ub_tensor = make_tensor(make_mem_ptr(buffer), layout);

    mutex<pipe_t::PIPE_MTE2> copy_in_mutex(buffer_lock_id);
    copy_in_mutex.lock();
    copy(make_copy(copy_gm_to_ub{}), ub_tensor, src_gm);
    copy_in_mutex.unlock();

    mutex<pipe_t::PIPE_MTE3> copy_out_mutex(buffer_lock_id);
    copy_out_mutex.lock();
    copy(make_copy(copy_ub_to_gm{}), dst_gm, ub_tensor);
    copy_out_mutex.unlock();
}
```
