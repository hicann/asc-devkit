# lock_guard

<!-- md-trans-meta sourceCommit=8d12fdc1392b422cc5d95088e74075f8f24a6135 translatedAt=2026-11-19T03:20:50.992Z pushedAt=2026-11-19T05:54:06.698Z nl=2 -->

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

`lock_guard` is a scoped lock protection object that holds a mutex reference. It acquires the lock at construction and releases the lock when leaving scope and being destructed.

`lock_guard` is typically used together with [`mutex<Pipe>`](mutex.md). It acquires and releases the lock resource through the `lock` and `unlock` methods of the mutex.

## Prototype

```cpp
template <typename Mutex>
class lock_guard {
public:
    __aicore__ inline explicit lock_guard(Mutex& mutex)
    __aicore__ inline ~lock_guard()

    lock_guard(const lock_guard&) = delete;
    lock_guard& operator=(const lock_guard&) = delete;
    lock_guard(lock_guard&&) = delete;
    lock_guard& operator=(lock_guard&&) = delete;
};
```

## Parameters

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| Mutex | - | Compile-time template parameter, the managed mutex type. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| mutex | Input | Reference to the managed mutex object. |

## Return Value

None

## Constraints

- `lock_guard` manages only the same mutex object passed in at construction. When entering the `lock_guard` scope, it calls the `lock` method of this mutex, and when leaving scope, it calls its `unlock` method.
- During the lifetime of `lock_guard`, you must not manually call `lock` or `unlock` on this mutex. Otherwise, the use-in-pairs constraint is violated.
- The mutex used by `lock_guard` must still satisfy the `Pipe` value, `mutex_id` management, use-in-pairs, and no-nesting constraints in [mutex](mutex.md).
- Do not rely on `lock_guard` for ordering control of multiple instructions in the same pipeline. Use [sync_pipe](sync_pipe.md) instead.

## Example

The following example uses lock_guard to manage the mutex during the UB-to-GM copy-out stage: the lock is automatically acquired when entering the scope and automatically released when leaving scope.

```cpp
#include "tensor_api/tensor.h"

constexpr uint32_t total_length = 64;
constexpr uint8_t buffer_lock_id = 0;

__global__ __vector__ void lock_guard_kernel(__gm__ float* dst)
{
    using namespace asc::te;
    __ubuf__ float buffer[total_length];

    auto layout = make_layout(make_shape(total_length));
    auto dst_gm = make_tensor(make_mem_ptr<location::gm>(dst), layout);
    auto ub_tensor = make_tensor(make_mem_ptr(buffer), layout);

    mutex<pipe_t::PIPE_MTE3> copy_out_mutex(buffer_lock_id);
    {
        lock_guard guard(copy_out_mutex);
        copy(make_copy(copy_ub_to_gm{}), dst_gm, ub_tensor);
    }
}
```
