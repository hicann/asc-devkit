# Introduction to Atomic Operations

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:13:14.531Z pushedAt=2026-10-26T06:23:22.600Z nl=2 -->

SIMT atomic operations ensure that a read-modify-write sequence on the same address is atomic, preventing lost updates when multiple threads concurrently update the same address. Atomic operations are commonly used in scenarios such as counting, state preemption, histogram statistics, reduction computation, and shared state bit updates.

Atomic operations guarantee that a single memory update cannot be interrupted by other threads, but they do not guarantee the order in which multiple threads reach the atomic operation, nor can they replace the thread synchronization barrier.

## Access Space

Atomic operations can act on Global Memory or Unified Buffer (UB). Different memory spaces differ in their scope, applicable scenarios, and performance characteristics.

| Access Space | Scope | Typical Use Cases | Performance Characteristics |
| --- | --- | --- | --- |
| Global Memory | Accessible by threads within the grid | Global counting, cross-thread block aggregation, final result write-back | Long access path; high overhead when threads across thread blocks or cores contend for the same address |
| UB | Accessible by threads within a thread block | Local statistics, intra-block reduction, temporary histograms | Located in on-chip memory; access latency is typically lower than Global Memory, but the scope is limited to the current thread block |

Different APIs and data types support different memory spaces. For details, see "Memory Ranges Supported by Different Data Types" in the documentation of each API.

## Atomic Operation Mechanism

The mechanism described in this section applies to all SIMT atomic operations listed in this chapter. Different APIs perform different determinations or computations, but the basic process for guaranteeing atomicity is the same.

### Basic Execution Process

An atomic operation involves the following steps:

1. Read the old value from the destination address.
2. Perform the determination or computation according to the API semantics. For example, arithmetic APIs perform addition, subtraction, maximum, or minimum; exchange APIs determine the value to be written; compare-and-swap APIs first compare the old value and then decide whether to write the new value; bitwise APIs perform the corresponding bitwise computation.
3. Write back the result according to the APItics.

The hardware executes the above steps as an indivisible whole. Before the current atomic operation completes, atomic operations initiated by other threads on the same address cannot modify that address. Therefore, when multiple threads perform atomic operations on the same address, the requests can only be executed sequentially and cannot be completed in parallel. An atomic operation guarantees that a single read-modify-write process is not interfered with by other threads, but it does not guarantee the execution order of requests from different threads.

### Global Memory Atomic Operation Principle

Global Memory (GM) is located outside the AI Core and is device-side memory accessible to all threads in the Grid. The L2 Cache is an off-core shared cache used to cache data from GM, reducing direct access to GM. The data stored in the L2 Cache comes from GM: on a cache hit, the data in the L2 Cache can be used directly; on a miss, the corresponding data must be loaded from GM. Data consistency between the L2 Cache and GM is guaranteed by hardware.

A Cache Line is the minimum management granularity for loading, tagging, replacing, and writing back data in the cache. The cache does not cache only a single variable or a single data address; instead, it loads a contiguous segment of GM data containing the destination address into the L2 Cache. The cached copy of this segment in the L2 Cache is called a Cache Line. For the GM atomic operation processing path described in this document, the Cache Line size is 512B, and each Cache Line contains four 128B Sectors. GM atomic operations are performed at the granularity of a 128B address range.

When the destination addresses of multiple atomic operations are concentrated within the same 128B address range, access contention is strong and throughput is low. When the destination addresses are distributed across more 128B address ranges, access contention is reduced and throughput is higher. Therefore, when evaluating GM atomic operation performance, pay attention to the distribution of destination addresses within 128B address ranges.

Data width affects the number of consecutive destination addresses that a single 128B address range can accommodate:

| Data Width | Space Occupied by a Single Address | Number of Consecutive Addresses per 128B Address Range |
| --- | --- | --- |
| 32-bit | 4B | 32 |
| 64-bit | 8B | 16 |

For example, 32 consecutive 32-bit addresses fall within one 128B address range, while 32 consecutive 64-bit addresses are distributed across two 128B address ranges. With the same number of atomic operations and addresses, the latter has a wider distribution of destination addresses, resulting in relatively weaker access contention and typically higher performance. However, this does not mean that 64-bit atomic operations are always faster than 32-bit atomic operations: if all threads access the same address, the destination addresses are concentrated within the same 128B address range regardless of data width. If the address distribution, operation type, or the way return values are used differs, the performance results also change accordingly. Therefore, when evaluating GM atomic operation performance, consider the number of threads contending for the same address, the distribution of destination addresses within 128B address ranges, and the execution overhead of the specific API.

### UB Atomic Operation Principle

When multiple threads perform atomic operations on the UB, the requests are executed serially following the basic execution process described above and are not affected by bank conflicts.


## Determinism

Atomic operations can prevent update loss caused by data races, but they do not necessarily guarantee that results are fully deterministic in all scenarios.

For integer counting operations, the final count is usually deterministic as long as no overflow occurs. For example, if all threads execute the `asc_atomic_add(counter, 1U)` API, the final result equals the number of threads participating in the accumulation.

For scenarios that depend on the return value of an atomic operation, the output order is usually nondeterministic. For example, the following code can obtain a unique write position, but the order in which different threads obtain positions may vary with scheduling:

```cpp
uint32_t pos = asc_atomic_add(count, 1U);
out[pos] = value;
```

For preemption scenarios, such as using the `asc_atomic_cas` API to change a state from 0 to the thread ID, the atomic operation guarantees that at most one thread succeeds in preemption, but it cannot guarantee which specific thread succeeds.

For floating-point accumulation scenarios, because floating-point addition does not satisfy strict associativity, the update order of different threads may cause differences in low-order rounding. If the business requires bit-exact consistency, avoid directly atomically accumulating a large amount of floating-point data into the same Global Memory address; instead, use fixed-order hierarchical reduction.

## Performance Impact

The performance of atomic operations is primarily affected by the access space, the degree of contention on the same address, the distribution of destination addresses within the 128B address range, and return value dependencies.

### Global Memory Atomic Operation

Global Memory atomic operations are suitable for final result aggregation but not for high-frequency hotspot updates. When multiple thread blocks or multiple cores access the same Global Memory address simultaneously, access contention is strong and throughput drops significantly. For specific performance characteristics, see [Global Memory Atomic Operation Principle](#global-memory-atomic-operation-principle).

For example, when all threads access the same `counter`:

```cpp
asc_atomic_add(counter, 1U);
```

This address becomes a hotspot, and the atomic operation throughput drops significantly. Even if different threads access different addresses, avoid concentrating these addresses excessively within a small number of 128B address ranges.

### UB Atomic Operation

UB atomic operations are suitable for local aggregation within a thread block. Compared with writing directly to Global Memory, they can reduce global memory accesses and cross-thread block contention. However, the scope of UB is limited, and it cannot directly replace Global Memory results shared across thread blocks.

Common optimization methods are as follows:

1. Each thread first computes a local result in registers.
2. Within the thread block, use UB for local accumulation or reduction.
3. Each thread block performs only a small number of atomic operations on Global Memory.

## How to Use

-   Use atomic operations only when concurrent writes to the same address actually exist and updates must not be lost.
-   Avoid having all threads frequently perform atomic updates to the same Global Memory address.
-   Prefer local aggregation in registers or UB before writing back to Global Memory.
-   Do not rely on the return values of atomic operations to express a stable thread execution order.
-   When floating-point accumulation requires bit-exact consistency, use fixed-order reduction instead of direct highly concurrent atomic accumulation.
-   When using `asc_atomic_cas` to implement preemption logic, ensure that the business only requires that "one thread succeeds" rather than that a specific thread succeeds.
