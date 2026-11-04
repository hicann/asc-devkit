# Atomic Operations

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-28T09:19:44.691Z pushedAt=2026-11-03T07:08:03.565Z nl=2 -->

In the SIMT programming model, a grid consists of multiple thread blocks, and each thread block contains multiple threads. Different threads can concurrently access the same segment of Global Memory or Unified Buffer (UB). When multiple threads perform read-modify-write operations on the same address simultaneously, a data race may occur without synchronization protection, causing the final result to be inconsistent with expectations.

Atomic operations ensure that a single read-modify-write process on the same address cannot be interrupted by other threads. Typical scenarios include counting, state preemption, histogram statistics, and parallel reduction.

## Basic Semantics

An atomic operation guarantees the atomicity of a single memory update. For example, when multiple threads execute `asc_atomic_add` on the same counter, each addition is completed atomically, so the problem of multiple threads reading the same old value and then overwriting each other's results does not occur.

An atomic operation does not guarantee the order in which multiple threads reach that atomic operation, nor is it a thread synchronization barrier. When multiple threads, warps, or thread blocks access the same address simultaneously, the hardware serializes these atomic requests, but the serialization order is usually not specified by the program.

Ascend C SIMT provides atomic operation APIs such as `asc_atomic_add`, `asc_atomic_sub`, and `asc_atomic_cas`, which can be used to update data in Global Memory or UB. Different APIs support different data types, access spaces, and return value constraints. For details, see [Atomic Operation APIs](../../../../api/SIMT-API/atomic_operations/atomic_operations_intro.md).

## Usage Recommendations

- Use atomic operations only when concurrent writes to the same address actually occur and updates must not be lost.
- Do not treat atomic operations as equivalent to thread synchronization. When threads need to reach the same execution point, use synchronization APIs instead.
- Avoid frequent atomic updates to the same Global Memory address by a large number of threads. If necessary, perform local aggregation within the thread block first.
- Do not rely on the return values of atomic operations to express a stable thread execution order.
