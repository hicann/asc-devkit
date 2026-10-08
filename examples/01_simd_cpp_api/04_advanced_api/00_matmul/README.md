# Matmul API样例介绍
## 概述
本样例集介绍了Matmul API不同特性的典型用法，给出了对应的端到端实现。

## 样例列表
| 目录名称 | 功能描述 | 支持的产品 |
|--------------------------------------------------------------------------------------------| ---------------------------------------------------- | --- |
| [batch_matmul](./batch_matmul) |  批量处理Matmul计算的样例 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [batch_matmul_bias_reuse](./batch_matmul_bias_reuse) |  批量处理Matmul计算时，每个Batch复用同一个Bias矩阵的Matmul样例 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [batch_matmul_iterate_n_batch](./batch_matmul_iterate_n_batch) |  多次批量处理Matmul计算的样例，包括非异步场景和异步场景的实现 | Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul](./matmul) |  调用Matmul API实现matmul样例 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_a2b2_share](./matmul_a2b2_share) |  开启 A2(L0A Buffer) 和 B2(L0B Buffer) 全局管理的Matmul样例 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_async_iterate](./matmul_async_iterate) |  异步场景下的Matmul样例，实现方式为调用Iterate和GetTensorC输出到VECIN | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_async_iterate_all](./matmul_async_iterate_all) |  异步场景下的Matmul样例，实现方式为调用IterateAll输出到GM | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_callback](./matmul_callback) |  Matmul API模板参数MatmulCallbackFunc的自定义使用方式 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_channelsplit_output](./matmul_channelsplit_output) |  矩阵乘输出Channel拆分功能的Matmul样例 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_co1_output](./matmul_co1_output) |  用户自主管理CO1（L0C Buffer）的Matmul样例 | Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_constant_tiling](./matmul_constant_tiling) |  Tiling常量化的Matmul样例，在tiling参数固定的场景下，减少运行时的Scalar开销 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_format_column_major](./matmul_format_column_major) |  输入输出矩阵为COLUMN_MAJOR（列优先）格式排布的Matmul样例 | Ascend 950PR&950DT系列产品 |
| [matmul_format_gemv](./matmul_format_gemv) |  实现矩阵向量乘GEMV（General Matrix-Vector multiplication）的Matmul样例 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_format_nd_align](./matmul_format_nd_align) |  在输入矩阵的N方向非对齐场景下，矩阵乘输出时使能N方向对齐的Matmul样例 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_fp8](./matmul_fp8) |  A、B矩阵为hifloat8、fp8_e4m3fn、fp8_e5m2数据类型输入的Matmul样例 | Ascend 950PR&950DT系列产品 |
| [matmul_fused](./matmul_fused) |  分核AIC、AIV融合编程实现，主要介绍了Matmul高阶API的MIX模式，Matmul API内部会自动控制AIC、AIV的核间同步 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_fused_manual](./matmul_fused_manual) |  分核AIC、AIV融合编程实现，主要介绍了Matmul高阶API的纯Cube模式，需要调用相关接口实现手动控制AIC、AIV的核间同步 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_ibshareAB](./matmul_ibshareAB) |  开启IBShare功能，复用L1 Buffer上相同的A矩阵或者B矩阵数据的样例，本样例为A矩阵和B矩阵同时复用场景 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_ibshareB](./matmul_ibshareB) |  调开启IBShare功能，复用L1 Buffer上相同的A矩阵或者B矩阵数据的样例，本样例为仅B矩阵复用场景 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_int4](./matmul_int4) |  A、B矩阵为int4b_t数据类型输入的Matmul样例 | Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_k_reorder_load](./matmul_k_reorder_load) |  使能K轴错峰加载数据特性的Matmul样例，减少多核对Global Memory地址访问冲突概率 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_l0cache](./matmul_l0cache) |  使能L0缓存特性的Matmul样例，减少MTE1重复搬运 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_l2cache](./matmul_l2cache) |  使能L2 Cache切分特性的Matmul样例，提高L2 Cache的利用率 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_mixdualmaster](./matmul_mixdualmaster) |  使能双主模式（MixDualMaster）的Matmul样例，AIC和AIV独立运行代码，不依赖消息驱动，用于提升性能 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_mn_double_buffer](./matmul_mn_double_buffer) |  M/N轴方向的流水并行的Matmul样例 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_multi_core_unaligned](./matmul_multi_core_unaligned) |  多核非对齐切分，即多核切分尾块的实际计算量小于tiling中对应参数的Matmul样例 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_mx](./matmul_mx) |  在MXFP4/MXFP8数据格式下的带有量化系数的矩阵乘法，即MxMatmul样例 | Ascend 950PR&950DT系列产品 |
| [matmul_mx_scale_cache](./matmul_mx_scale_cache) |  在MXFP4/MXFP8数据格式下，量化系数矩阵scale在L1 Buffer上开启多倍缓存的MxMatmul样例 | Ascend 950PR&950DT系列产品 |
| [matmul_mx_ub_tscm_nz](./matmul_mx_ub_tscm_nz) |  在MXFP4/MXFP8数据格式下，使用用户自定义TSCM、VECOUT的输入的MxMatmul样例 | Ascend 950PR&950DT系列产品 |
| [matmul_nbuffer33](./matmul_nbuffer33) |  NBuffer33算法的Matmul样例，实现搬入搬出带宽平衡从而提升效率 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_partial_output](./matmul_partial_output) |  Matmul高阶API实现开启Partial Output功能的样例 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_preload](./matmul_preload) |  M/N方向预加载的Matmul样例，可以减少MTE2间隙 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_quant](./matmul_quant) |  输出随路反量化的Matmul样例，支持同一系数的反量化模式和向量的反量化模式 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_sparse](./matmul_sparse) |  4:2稀疏矩阵乘（Sparse Matmul）的Matmul样例，可以减少矩阵乘计算时的内存占用和计算量 | Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_splitk](./matmul_splitk) |  多核切K场景下的Matmul样例，将输入矩阵沿K轴切分后分配到多核上并行处理 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_splitm](./matmul_splitm) |  多核切M场景下的Matmul样例，将输入矩阵沿M轴切分后分配到多核上并行处理 | Ascend 950PR&950DT系列产品 |
| [matmul_triangle](./matmul_triangle) |  TrianUpperMatmulPolicy（上三角模板策略）和TrianLowerMatmulPolicy（下三角模板策略）的Matmul样例 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_tscm](./matmul_tscm) |  使用数据来源为GM的用户自定义TSCM的输入的Matmul样例，开发者可以自主管理L1 Buffer以高效利用硬件资源 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_tscm_src_vecout](./matmul_tscm_src_vecout) |  使用数据来源为VECOUT的用户自定义TSCM的输入的Matmul样例，开发者可以自主管理L1 Buffer以高效利用硬件资源 | Ascend 950PR&950DT系列产品 |
| [matmul_unitflag](./matmul_unitflag) |  使能UnitFlag功能的Matmul样例，使样例中的CUBE计算流水与FIXPIPE数据搬出流水并行 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [matmul_vecout](./matmul_vecout) |  使用用户自定义VECOUT的输入的Matmul样例，开发者可以自主管理Unified Buffer以高效利用硬件资源 | Ascend 950PR&950DT系列产品 |
