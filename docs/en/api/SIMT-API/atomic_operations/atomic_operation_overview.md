# Introduction to Atomic Operations

<!-- md-trans-meta sourceCommit=e1d69e702c9c347bfbdba7ca2ed786dab14f1ccf translatedAt=2026-09-07T09:30:58.153Z pushedAt=2026-09-08T00:54:26.293Z -->

> **Prerequisites**: This document involves SIMT thread organization concepts such as Grid, Thread Block, Thread, and Warp. It is recommended that you first read [Thread Architecture](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/guide/programming_guide/programming_model/ai_core_simd_programming/thread_architecture.md) in the programming guide to understand the relevant prerequisites.

SIMT atomic operations ensure the atomicity of a read-modify-write process on the same address, preventing lost updates when multiple threads concurrently update the same address. Atomic operations are commonly used in scenarios such as counting, state preemption, histogram statistics, parallel reduction, and shared state bit updates.

Atomic operations guarantee that a single memory update cannot be interrupted by other threads, but they do not guarantee the order in which multiple threads reach the atomic operation, nor can they replace the thread synchronization barrier.

## Access Space

Atomic operations can act on Global Memory or Unified Buffer. Different memory spaces differ in their scope, applicable scenarios, and performance characteristics.

| Access Space | Scope | Typical Use | Performance Characteristics |
| --- | --- | --- | --- |
| Global Memory | Accessible by threads within the grid | Global counting, cross-thread block aggregation, final result write-back | Longer access path; higher overhead when threads across thread blocks or cores contend for the same address |
| Unified Buffer | Accessible by threads within a thread block | Local statistics, intra-block reduction, temporary histogram | Located in on-chip memory, with access latency typically lower than Global Memory, but the scope is limited to the current thread block |

Different interfaces and data types support different memory spaces. For details, see "Supported Memory Ranges for Different Data Types" in the documentation of each interface.

## Determinism

Atomic operations can prevent update loss caused by data races, but they do not necessarily guarantee that the result is fully deterministic in all scenarios.

For integer counting operations, the final count is usually deterministic as long as no overflow occurs. For example, if all threads execute the `asc_atomic_add(counter, 1U)` interface, the final result equals the number of threads participating in the accumulation.

For scenarios that depend on the return value of an atomic operation, the output order is usually nondeterministic. For example, the following code can obtain a unique write position, but the order in which different threads obtain their positions may vary with scheduling:

```cpp
uint32_t pos = asc_atomic_add(count, 1U);
out[pos] = value;
```

For preemption scenarios, for example, using the `asc_atomic_cas` interface to change a state from 0 to the thread ID, the atomic operation guarantees that at most one thread succeeds in preemption, but it cannot guarantee which thread succeeds.

For floating-point accumulation scenarios, because floating-point addition does not satisfy strict associativity, the serialization order of different threads may cause differences in low-order rounding. If the business requires bit-for-bit consistency, avoid directly atomically accumulating a large amount of floating-point data into the same Global Memory address; instead, use hierarchical reduction with a fixed order.

## Performance Impact

The performance of atomic operations is mainly affected by the access space, the degree of contention, and the dependency on return values.

### Global Memory Atomic Operations

Global Memory atomic operations are suitable for aggregating final results but not for high-frequency hotspot updates. When multiple thread blocks or multiple cores access the same Global Memory address simultaneously, requests are queued at that address, causing a noticeable drop in throughput.

For example, when all threads access the same `counter`:

```cpp
asc_atomic_add(counter, 1U);
```

That address becomes a hotspot, and the program may degrade from parallel execution to serial execution of a large number of atomic requests.

### Unified Buffer Atomic Operations

Unified Buffer atomic operations are suitable for local aggregation within a thread block. Compared with writing directly to Global Memory, they can reduce global memory access and cross-thread block contention. However, the scope of Unified Buffer is limited, and it cannot directly replace the Global Memory results shared across thread blocks.

Common optimization approaches are as follows:

1. Each thread first computes a local result in registers.
2. Within the thread block, use Unified Buffer for local accumulation or reduction.
3. Each thread block performs only a small number of atomic operations on Global Memory.

### Multiple Threads in the Same Warp Accessing the Same Address

Multiple threads in the same warp performing atomic operations on the same address simultaneously is a common performance bottleneck. Although the threads in a warp issue requests in parallel under the execution model, atomic read-modify-write operations on the same address must be completed serially.

For example, 32 threads in a warp execute the following simultaneously:

```cpp
asc_atomic_add(counter, 1U);
```

These requests are serialized on the `counter` address. If other warps or thread blocks also access the same address, the scope of serialization expands further.

If the business logic allows, you can first combine the increments within a warp or thread block, and then have a few threads perform the atomic operation. For example, in a counting scenario, you can first count the number of threads that meet the condition within the current warp, and then have one thread execute the `asc_atomic_add` interface once.

## Usage Recommendations

-   Use atomic operations only when concurrent writes to the same address actually exist and updates must not be lost.
-   Avoid having all threads frequently perform atomic updates to the same Global Memory address.
-   Prefer local aggregation using registers or Unified Buffer before writing back to Global Memory.
-   Do not rely on the return value of an atomic operation to express a stable thread execution order.
-   When floating-point accumulation requires bit-exact consistency, use fixed-order reduction instead of direct high-concurrency atomic accumulation.
-   When using `asc_atomic_cas` to implement preemption logic, ensure that the business only requires "one thread to succeed" rather than a specific thread to succeed.
