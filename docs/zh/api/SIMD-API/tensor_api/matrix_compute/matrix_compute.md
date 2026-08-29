# 矩阵计算（Tensor API）

矩阵计算由数据搬入、`mmad`计算和结果搬出组成。各阶段通过Tensor的存储位置和Layout选择对应Arch通路，通过Atom配置Trait及运行时Params。

- **[概述](overview.md)**
- **[矩阵计算分形介绍](cube_compute_fractal_intro.md)**
- **[矩阵计算的搬入](cube_compute_load/cube_compute_load.md)**
- **[mmad计算](mmad_compute/mmad_compute.md)**
- **[mmad计算关键特性说明](mmad_compute_key_features/mmad_compute_key_features.md)**
- **[mmad计算辅助配置接口](mmad_compute_aux_config.md)**
- **[矩阵计算的搬出](cube_compute_store/cube_compute_store.md)**
- **[矩阵搬出关键特性说明](cube_store_key_features/cube_store_key_features.md)**

Operation、Trait和Atom的通用组合方式参见[Algorithm](../algorithm.md)。
