# Best Practices

<!-- md-trans-meta sourceCommit=072af62184676a1bc22e7cb49c6f75e59f7fb63b translatedAt=2026-11-09T10:26:57.602Z pushedAt=2026-11-19T06:32:19.715Z nl=2 -->

- [FlashAttention Operator Performance Tuning Case](./flash_attention_tuning.md)
- [GroupedMatmul Operator Performance Tuning Case](./grouped_matmul_tuning.md)
- [MC² Operator Performance Tuning Case](./mc_operator_tuning.md)
- [Matmul Performance Tuning Case](./matmul_tuning_cases/matmul_tuning_cases.md)
  - [Matmul Performance Optimization Strategy Overview](./matmul_tuning_cases/matmul_optimization_overview.md)
  - [Matmul Operator Optimization Tiling Strategy](./matmul_tuning_cases/matmul_tiling_strategy.md)
  - [Enabling Cube-Only Mode for the Matmul Advanced API](./matmul_tuning_cases/matmul_pure_cube.md)
  - [Enabling the MDL Template for the Matmul Advanced API](./matmul_tuning_cases/matmul_mdl_template.md)
  - [Enabling UnitFlag for the Matmul Advanced API](./matmul_tuning_cases/matmul_unit_flag.md)
  - [Enabling Full Tiling Constantization for the Matmul Advanced API](./matmul_tuning_cases/matmul_tiling_constant.md)
  - [Enabling L2 Cache Split for the Matmul Advanced API](./matmul_tuning_cases/matmul_l2_cache_split.md)
  - [Enabling Multi-Core K Split for the Matmul Advanced API](./matmul_tuning_cases/matmul_multi_core_split_k.md)
  - [Enabling Staggered Multi-Core K-Axis Memory Access for the Matmul Advanced API](./matmul_tuning_cases/matmul_multi_core_k_stagger.md)
  - [Enabling the NBuffer33 Template for the Matmul Advanced API](./matmul_tuning_cases/matmul_nbuffer33.md)
  - [Enabling the IBShare Template for the Matmul Advanced API to Share B Cube Data](./matmul_tuning_cases/matmul_ibshare_b.md)
  - [Enabling the IBShare Template for the Matmul Advanced API to Share A and B Cube Data](./matmul_tuning_cases/matmul_ibshare_ab.md)
  - [ND2NZ Format Conversion on the AIV Core](./matmul_tuning_cases/aiv_nd2nz_conversion.md)
  - [Enabling MTE2 Preload for the Matmul Advanced API](./matmul_tuning_cases/matmul_mte2_preload.md)
- [SIMT Implementation Case of the InsertHashTable Operator](./simt_insert_hash_table.md)
