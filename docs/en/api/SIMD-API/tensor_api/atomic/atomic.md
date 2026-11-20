# Atomic Operations

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T02:17:05.323Z pushedAt=2026-11-19T05:54:06.625Z nl=2 -->

DMA (Direct Memory Access) atomic operations perform an atomic read-modify-write on the existing data in Global Memory during data movement, and then write the movement result. The Tensor API provides the following APIs to configure the subsequent supported movement instructions whose destination address is Global Memory:

- **[set_atomic_add](set_atomic_add.md)**: Sums the data to be moved with the existing data in Global Memory, and writes the result into Global Memory.
- **[set_atomic_max](set_atomic_max.md)**: Compares the data to be moved with the existing data in Global Memory, and writes the greater of the two into Global Memory.
- **[set_atomic_min](set_atomic_min.md)**: Compares the data to be moved with the existing data in Global Memory, and writes the smaller of the two into Global Memory.
- **[disable_dma_atomic](disable_dma_atomic.md)**: Disables the DMA atomic mode to prevent the atomic configuration from affecting subsequent movement.

`set_atomic_add`, `set_atomic_max`, and `set_atomic_min` specify the data type that participates in the atomic operation through template parameters.

Scalar atomic operations perform an atomic read-modify-write on a single element at a specified address in Global Memory:

- **[atomic_add](atomic_add.md)**: Performs atomic addition and returns the old value.
- **[atomic_sub](atomic_sub.md)**: Performs atomic subtraction and returns the old value.
- **[atomic_min](atomic_min.md)**: Performs atomic minimum and returns the old value.
- **[atomic_max](atomic_max.md)**: Performs atomic maximum and returns the old value.
- **[atomic_inc](atomic_inc.md)**: Performs atomic increment and returns the old value.
- **[atomic_dec](atomic_dec.md)**: Performs atomic decrement and returns the old value.
- **[atomic_and](atomic_and.md)**: Performs atomic bitwise AND and returns the old value.
- **[atomic_or](atomic_or.md)**: Performs atomic bitwise OR and returns the old value.
- **[atomic_xor](atomic_xor.md)**: Performs atomic bitwise XOR and returns the old value.
- **[atomic_exch](atomic_exch.md)**: Performs atomic exchange and returns the old value.
- **[atomic_cas](atomic_cas.md)**: Performs atomic compare-and-swap and returns the old value.
