# Cube Data Fill

<!-- md-trans-meta sourceCommit=8d12fdc1392b422cc5d95088e74075f8f24a6135 translatedAt=2026-11-19T02:31:53.418Z pushedAt=2026-11-19T05:54:06.639Z nl=2 -->

Cube data fill initializes the physical blocks in the L1 Buffer before matrix computation, preparing the input buffer for subsequent matrix data loading and matrix multiply-accumulate operations.

- **[fill](fill.md)**: Writes a scalar fill value into the L1 Buffer in units of 32-byte physical blocks.
