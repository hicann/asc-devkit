# 数据搬运

Tensor API通过`copy`统一执行不同存储位置之间的数据搬运。Tensor表达不同存储位置和类型的数据，Layout表达Tensor的尺寸大小和排列规则。

- [copy（Global Memory到L1 Buffer数据搬运）](../matrix_compute/cube_compute_load/copy_gm_to_l1.md)
- [copy（Global Memory到Unified Buffer的数据搬运）](../reg_vector_compute/vector_compute_load/copy_gm_to_ub.md)
- [copy（L1 Buffer到Unified Buffer数据搬运）](../matrix_compute/cube_compute_load/copy_l1_to_ub.md)
- [copy（L1 Buffer到L0A Buffer数据搬运）](../matrix_compute/cube_compute_load/copy_l1_to_l0a.md)
- [copy（L1 Buffer到L0B Buffer数据搬运）](../matrix_compute/cube_compute_load/copy_l1_to_l0b.md)
- [copy（L1 Buffer到L0ScaleA Buffer数据搬运）](../matrix_compute/cube_compute_load/copy_l1_to_l0scalea.md)
- [copy（L1 Buffer到L0ScaleB Buffer数据搬运）](../matrix_compute/cube_compute_load/copy_l1_to_l0scaleb.md)
- [copy（L1 Buffer到Fixpipe Buffer数据搬运）](../matrix_compute/cube_compute_load/copy_l1_to_fixbuf.md)
- [copy（L1 Buffer到BiasTable Buffer数据搬运）](../matrix_compute/cube_compute_load/copy_l1_to_biastable.md)
- [copy（L0C Buffer到Global Memory数据搬运）](../matrix_compute/cube_compute_store/copy_l0c_to_gm.md)
- [copy（L0C Buffer到Unified Buffer数据搬运）](../matrix_compute/cube_compute_store/copy_l0c_to_ub.md)
- [copy（L0C Buffer到L1 Buffer数据搬运）](../matrix_compute/cube_compute_store/copy_l0c_to_l1.md)
- [copy（Unified Buffer到Global Memory的数据搬运）](../reg_vector_compute/vector_compute_store/copy_ub_to_gm.md)
- [copy（Unified Buffer到Unified Buffer的数据搬运）](../reg_vector_compute/vector_compute_load/copy_ub_to_ub.md)
- [copy（Unified Buffer到L1 Buffer的数据搬运）](../reg_vector_compute/vector_compute_load/copy_ub_to_l1.md)

`copy`、`make_copy`、Copy Operation、Trait和Atom的统一说明参见[Algorithm](../algorithm.md)。
