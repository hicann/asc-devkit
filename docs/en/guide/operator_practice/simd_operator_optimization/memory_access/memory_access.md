# Memory Access

<!-- md-trans-meta sourceCommit=072af62184676a1bc22e7cb49c6f75e59f7fb63b translatedAt=2026-11-09T10:36:10.685Z pushedAt=2026-11-19T06:32:19.775Z nl=2 -->

- [Transferring Larger Data Blocks in One Batch If Possible](./transfer_larger_data_blocks.md)
- [Aligning GM Addresses to 512B Wherever Possible](./gm_address_512b_alignment.md)
- [Using Movement APIs Efficiently](./efficient_transfer_api.md)
- [Reducing Invalid Data Transfer in Non-Aligned Scenarios](./reduce_invalid_data_transfer.md)
- [Reducing Transfer Count in Non-Contiguous Transfer Scenarios](./reduce_non_contiguous_transfer.md)
- [Avoiding Same-Address Access](./avoid_same_address_access.md)
- [Set a Reasonable L2 Cache Mode](./l2_cache_mode_config.md)
- [Sharing Temporary Buffers Between Operators and Advanced APIs](./shared_temp_buffer.md)
- [Reusing VECIN and VECOUT for Transfer-Only Operators](./vecin_vecout_reuse.md)
- [Optimizing Stack Space by Reducing Tensor ShapeInfo Dimensions](./reduce_shapeinfo_optimize_stack.md)
- [Avoiding UB Bank Conflicts](./avoid_ub_bank_conflict/avoid_ub_bank_conflict.md)
  - [Overview](./avoid_ub_bank_conflict/overview.md)
  - [Avoiding Bank Conflicts (NPU Architecture Version 2201)](./avoid_ub_bank_conflict/avoid_bank_conflict_npu_arch_2201.md)
  - [Avoiding Bank Conflicts (NPU Architecture Version 3510)](./avoid_ub_bank_conflict/avoid_bank_conflict_npu_arch_3510.md)
- [L2 Cache Splitting](./l2_cache_split.md)
