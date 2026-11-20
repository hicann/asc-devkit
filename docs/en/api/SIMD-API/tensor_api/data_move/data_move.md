# Movement

<!-- md-trans-meta sourceCommit=8d12fdc1392b422cc5d95088e74075f8f24a6135 translatedAt=2026-11-19T02:28:47.474Z pushedAt=2026-11-19T05:54:06.636Z nl=2 -->

The Tensor API uses `copy` to uniformly perform movement between different storage locations. A Tensor represents data at different storage locations and types, and a Layout represents the size and arrangement rules of a Tensor.

- [copy (movement from Global Memory to L1 Buffer)](../matrix_compute/cube_compute_load/copy_gm_to_l1.md)
- [copy (movement from Global Memory to Unified Buffer)](../reg_vector_compute/vector_compute_load/copy_gm_to_ub.md)
- [copy (movement from L1 Buffer to Unified Buffer)](../matrix_compute/cube_compute_load/copy_l1_to_ub.md)
- [copy (movement from L1 Buffer to L0A Buffer)](../matrix_compute/cube_compute_load/copy_l1_to_l0a.md)
- [copy (movement from L1 Buffer to L0B Buffer)](../matrix_compute/cube_compute_load/copy_l1_to_l0b.md)
- [copy (movement from L1 Buffer to L0ScaleA Buffer)](../matrix_compute/cube_compute_load/copy_l1_to_l0scalea.md)
- [copy (movement from L1 Buffer to L0ScaleB Buffer)](../matrix_compute/cube_compute_load/copy_l1_to_l0scaleb.md)
- [copy (movement from L1 Buffer to Fixpipe Buffer)](../matrix_compute/cube_compute_load/copy_l1_to_fixbuf.md)
- [copy (movement from L1 Buffer to BiasTable Buffer)](../matrix_compute/cube_compute_load/copy_l1_to_biastable.md)
- [copy (movement from L0C Buffer to Global Memory)](../matrix_compute/cube_compute_store/copy_l0c_to_gm.md)
- [copy (movement from L0C Buffer to Unified Buffer)](../matrix_compute/cube_compute_store/copy_l0c_to_ub.md)
- [copy (movement from L0C Buffer to L1 Buffer)](../matrix_compute/cube_compute_store/copy_l0c_to_l1.md)
- [copy (movement from Unified Buffer to Global Memory)](../reg_vector_compute/vector_compute_store/copy_ub_to_gm.md)
- [copy (movement from Unified Buffer to Unified Buffer)](../reg_vector_compute/vector_compute_load/copy_ub_to_ub.md)
- [copy (movement from Unified Buffer to L1 Buffer)](../reg_vector_compute/vector_compute_load/copy_ub_to_l1.md)
- [set_copy_pad_val](set_copy_pad_val.md)

For description of `copy`, `make_copy`, Movement Operation, Trait, and Atom, see [Algorithm](../algorithm.md).
