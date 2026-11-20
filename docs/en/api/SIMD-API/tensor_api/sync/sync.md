# Synchronization Control

<!-- md-trans-meta sourceCommit=8d12fdc1392b422cc5d95088e74075f8f24a6135 translatedAt=2026-11-19T03:23:35.838Z pushedAt=2026-11-19T05:54:06.700Z nl=2 -->

Tensor API provides three types of synchronization control capabilities:

- **[block, intra, inter, and subblock synchronization](block_sync.md)**: Functional API for arrival/wait synchronization between cores or subblocks.
- **[mutex](mutex.md)**: Object-based pipeline lock for resource handover between asynchronous pipelines within a core.
- **[lock_guard](lock_guard.md)**: Scoped lock protection object that automatically releases the referenced mutex upon destruction.
- **[sync_data_barrier](data_barrier.md)**: Data access barrier that waits for preceding memory accesses to complete.
- **[sync_pipe](sync_pipe.md)**: Pipeline synchronization that waits for the preceding instructions of a specified pipeline or all pipelines to complete.
