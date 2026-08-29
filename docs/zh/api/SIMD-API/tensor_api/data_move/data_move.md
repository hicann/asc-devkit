# 数据搬运

Tensor API通过`copy`统一执行不同存储位置之间的数据搬运。Tensor表达不同存储位置和类型的数据，Layout表达Tensor的尺寸大小和排列规则。

- [Global Memory与Unified Buffer数据搬运](gm_ub_data_move/gm_ub_data_move.md)
- [Global Memory到L1 Buffer数据搬运](gm_l1_data_move.md)
- [L1 Buffer与Unified Buffer数据搬运](l1_ub_data_move/l1_ub_data_move.md)
- [L1 Buffer到L0 Buffer数据搬运](l1_l0_data_move.md)
- [L0C Buffer到Global Memory数据搬运](l0c_gm_data_move.md)
- [L0C Buffer到L1 Buffer数据搬运](l0c_l1_data_move.md)
- [L0C Buffer到Unified Buffer数据搬运](l0c_ub_data_move.md)
- [L1 Buffer到Fixpipe Buffer数据搬运](l1_fixbuf_data_move.md)
- [L1 Buffer到BiasTable Buffer数据搬运](l1_bias_table_data_move.md)
- [Unified Buffer内部数据搬运](ub_ub_data_move/ub_ub_data_move.md)

`copy`、`make_copy`、Copy Operation、Trait和Atom的统一说明参见[Algorithm](../algorithm.md)。
