# Overview

<!-- md-trans-meta sourceCommit=1398d4baddf82d4b412922bb4a32a05abf2ada8a translatedAt=2026-11-16T09:14:51.031Z pushedAt=2026-11-18T08:15:34.158Z nl=2 -->

## Atomic Operation Overview

### Inline Atomic Operation for Data Movement

The inline atomic operation interface for data movement is used to enable atomic operations for subsequent data movement instructions whose destination address is GM. For the involved interfaces, see [Table 1](#datamove_atomic_operation_interface). As shown in the left subfigure of Figure 1 below, when atomic operations are not enabled, after the data movement that writes to GM is complete, the original data in GM is completely overwritten by the newly moved data. As shown in the right subfigure of Figure 1, after the inline atomic operation interface for data movement is called, the system enables atomic operations for subsequent data movement that writes to GM. In this case, after the data movement is complete, the final data in GM is determined jointly by the original GM data and the newly moved data.

**Figure 1** Inline atomic accumulation effect for data movement

![](../../../figures/atomic_operation_pipelined_data_movement_effect.png "Inline atomic accumulation effect for data movement")

### Scalar Atomic Operations

The scalar atomic operation interface is used to perform an atomic read-modify-write operation on a single element at a specified address in Global Memory (GM): it reads the old value `old_value` at that address, computes a new value according to the interface semantics and writes it back to that address, and returns `old_value` at the same time. The entire process is guaranteed by hardware to be an atomic operation, and no other atomic update can be inserted into it. Therefore, when multiple AI Cores or multiple instructions within the same AI Core concurrently access the same GM address, they do not overwrite each other, and the final result meets expectations. For a comparison of the effects with and without atomic operations, see [Scalar Atomic Accumulation Effect Diagram](#scalar_atomic_operation_diagram).

Scalar atomic operations are suitable for scenarios where multiple cores need to update the same GM address, such as multiple cores accumulating the same statistic in parallel, maintaining a counter shared by multiple cores, and comparing and updating a state at a specified address.

The differences between scalar atomic operations and inline atomic operations for data movement are as follows:

- The object of an inline atomic operation for data movement is the tensor being moved. You need to first call an inline atomic operation API for data movement to enable atomic operations. Once enabled, the operation takes effect on the entire tensor subsequently moved to GM through the MTE unit, and affects the behavior of subsequent data move instructions.
- The object of a scalar atomic operation is a single element at the passed GM address. No configuration is required. The operation takes effect only on the single element at the GM address passed for this call, and does not affect subsequent data move instructions.

As shown in the left subfigure of Figure 2, when the `asc_atomic_add` API is not used, multiple AI Cores performing accumulation operations on the same GM address simultaneously overwrite each other, the operations are not atomic, and the final result is unpredictable. As shown in the right subfigure, after the `asc_atomic_add` API is used, the accumulation operations of each AI Core are executed serially, ensuring the atomicity of each accumulation operation, and the final result meets expectations.

**Figure 2** Scalar atomic accumulation effect<a id="scalar_atomic_operation_diagram"></a>

![](../../../figures/scalar_atomic_operation_diagram.png "Scalar atomic accumulation effect")

For the deterministic computation and synchronization solution of atomic operations, see [Key Features](key_features.md).

## API List

### Data Move Atomic Operations

**Table 1** Data movement inline atomic operation interfaces<a id="datamove_atomic_operation_interface"></a>

| API | Interface Description | Finest-grained Common Header File |
| --- | --- | --- |
| [asc_set_atomic_add](datamove_atomic/asc_set_atomic_add.md) | Enables atomic accumulation for subsequent data movement whose destination address is GM. Atomic accumulation process: sums the content to be copied and the existing content on GM, and then writes the sum result to GM. | `c_api/atomic/datamove_atomic.h` |
| [asc_set_atomic_max](datamove_atomic/asc_set_atomic_max.md) | Sets whether atomic comparison is performed on the data subsequently moved to GM: compares the content to be copied with the existing content on GM, and then writes the maximum value to GM. | `c_api/atomic/datamove_atomic.h` |
| [asc_set_atomic_min](datamove_atomic/asc_set_atomic_min.md) | Sets whether atomic comparison is performed on the data subsequently moved to GM: compares the content to be copied with the existing content on GM, and then writes the minimum value to GM. | `c_api/atomic/datamove_atomic.h` |
| [asc_disable_dma_atomic](datamove_atomic/asc_disable_dma_atomic.md) | Disables inline atomic operations for data movement. | `c_api/atomic/datamove_atomic.h` |

### Scalar Atomic Operations

The scalar atomic operation interfaces are shown in Table 2, and the data types supported by each interface are shown in Table 3. Here, `old_value` is the old value at the GM address pointed to by `address`, and `new_value` is the new value written back to that address. All atomic computation interfaces in Table 2 return `old_value`.

**Table 2** Scalar atomic operation interfaces

| API | Interface Description | Calculation Formula | Finest-grained Common Header File |
| --- | --- | --- | --- |
| [asc_atomic_add](scalar_atomic/asc_atomic_add.md) | Atomic add: sums the old value at the GM address pointed to by `address` and the input scalar value `val`, and writes the result back to that address. | $new\_value = old\_value + val$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_sub](scalar_atomic/asc_atomic_sub.md) | Atomic subtract: subtracts the input scalar value `val` from the old value at the GM address pointed to by `address`, and writes the result back to that address. | $new\_value = old\_value - val$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_max](scalar_atomic/asc_atomic_max.md) | Atomic maximum: compares the old value at the GM address pointed to by `address` with the input scalar value `val`, and writes the larger value back to that address. | $new\_value = Max(old\_value, val)$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_min](scalar_atomic/asc_atomic_min.md) | Atomic minimum: compares the old value at the GM address pointed to by `address` with the input scalar value `val`, and writes the smaller value back to that address. | $new\_value = Min(old\_value, val)$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_inc](scalar_atomic/asc_atomic_inc.md) | Atomic increment: when the old value at the GM address pointed to by `address` is greater than or equal to `val`, sets that address to 0; otherwise, adds 1 to the old value and writes it back. | $new\_value = (old\_value >= val) ? 0 : (old\_value + 1)$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_dec](scalar_atomic/asc_atomic_dec.md) | Atomic decrement: when the old value at the GM address pointed to by `address` is equal to 0 or greater than `val`, sets that address to `val`; otherwise, subtracts 1 from the old value and writes it back. | $new\_value = (old\_value == 0 \vert\vert old\_value > val) ? val : (old\_value - 1)$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_and](scalar_atomic/asc_atomic_and.md) | Atomic AND: performs a bitwise AND between the old value at the GM address pointed to by `address` and the input scalar value `val`, and writes the result back to that address. | $new\_value = old\_value \& val$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_or](scalar_atomic/asc_atomic_or.md) | Atomic OR: performs a bitwise OR between the old value at the GM address pointed to by `address` and the input scalar value `val`, and writes the result back to that address. | $new\_value = old\_value \vert val$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_xor](scalar_atomic/asc_atomic_xor.md) | Atomic XOR: performs a bitwise XOR between the old value at the GM address pointed to by `address` and the input scalar value `val`, and writes the result back to that address. | $new\_value = old\_value \oplus val$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_exch](scalar_atomic/asc_atomic_exch.md) | Atomic exchange: writes the input scalar value `val` to the GM address pointed to by `address` to replace the old value. | $new\_value = val$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_cas](scalar_atomic/asc_atomic_cas.md) | Atomic compare-and-swap: when the old value at the GM address pointed to by `address` is equal to `compare`, writes `val` to that address; when not equal, keeps the value at that address unchanged. | $new\_value = (old\_value == compare) ? val : old\_value$ | `c_api/atomic/scalar_atomic.h` |
| [asc_set_store_atomic_config_v1](scalar_atomic/asc_set_store_atomic_config_v1.md) | Sets the scalar atomic operation configuration: operation type (only the sum operation is supported) and data type. | - | `c_api/atomic/scalar_atomic.h` |
| [asc_get_store_atomic_config](scalar_atomic/asc_get_store_atomic_config.md) | Obtains the scalar atomic operation configuration set by `asc_set_store_atomic_config_v1`. | - | `c_api/atomic/scalar_atomic.h` |

**Table 3** Data types supported by scalar atomic operation interfaces

| API | Data Types Supported by `dtype` |
| --- | --- |
| [asc_atomic_add](scalar_atomic/asc_atomic_add.md) | `int32_t`, `uint32_t`, `float`, `int64_t`, `uint64_t` |
| [asc_atomic_sub](scalar_atomic/asc_atomic_sub.md) | `int32_t`, `uint32_t`, `float`, `int64_t`, `uint64_t` |
| [asc_atomic_max](scalar_atomic/asc_atomic_max.md) | `int32_t`, `uint32_t`, `float`, `int64_t`, `uint64_t` |
| [asc_atomic_min](scalar_atomic/asc_atomic_min.md) | `int32_t`, `uint32_t`, `float`, `int64_t`, `uint64_t` |
| [asc_atomic_inc](scalar_atomic/asc_atomic_inc.md) | `uint32_t`, `uint64_t` |
| [asc_atomic_dec](scalar_atomic/asc_atomic_dec.md) | `uint32_t`, `uint64_t` |
| [asc_atomic_and](scalar_atomic/asc_atomic_and.md) | `int32_t`, `uint32_t`, `int64_t`, `uint64_t` |
| [asc_atomic_or](scalar_atomic/asc_atomic_or.md) | `int32_t`, `uint32_t`, `int64_t`, `uint64_t` |
| [asc_atomic_xor](scalar_atomic/asc_atomic_xor.md) | `int32_t`, `uint32_t`, `int64_t`, `uint64_t` |
| [asc_atomic_exch](scalar_atomic/asc_atomic_exch.md) | `int32_t`, `uint32_t`, `float`, `int64_t`, `uint64_t` |
| [asc_atomic_cas](scalar_atomic/asc_atomic_cas.md) | `int32_t`, `uint32_t`, `float`, `int64_t`, `uint64_t` |
| [asc_set_store_atomic_config_v1](scalar_atomic/asc_set_store_atomic_config_v1.md) | `float`, `half`, `int16_t`, `int32_t`, `int8_t`, `bfloat16_t` |
| [asc_get_store_atomic_config](scalar_atomic/asc_get_store_atomic_config.md) | - |
