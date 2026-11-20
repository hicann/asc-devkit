# Tensor

<!-- md-trans-meta sourceCommit=46a50d69bd092786e44495d519df57743eefbfc6 translatedAt=2026-10-27T04:24:33.874Z pushedAt=2026-10-27T13:03:35.488Z nl=2 -->

A Tensor consists of a Pointer and a Layout. The Pointer describes the data address and storage location, and the Layout describes the Shape and Stride of the data. Developers can use a Tensor to uniformly access data in Global Memory or on-chip Buffers, and create sub-Tensors that share the underlying storage through slicing.

## Tensor-Related Data Structures

- **[Tensor](tensor.md)**
- **[Pointer](pointer.md)**
- **[view_engine](view_engine.md)**

## Tensor-Related APIs

- **[make_tensor](make_tensor.md)**
- **[make_mem_ptr](make_mem_ptr.md)**
- **[slice](slice.md)**
