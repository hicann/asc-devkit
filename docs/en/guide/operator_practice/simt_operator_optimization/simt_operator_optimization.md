# SIMT Operator Performance Optimization

<!-- md-trans-meta sourceCommit=1fa0fc4f3ef074ae5db86a4cd38e1778da419dc2 translatedAt=2026-11-09T10:42:42.563Z pushedAt=2026-11-19T06:32:19.834Z nl=2 -->

- [Overview](./overview.md)
- [Memory Access](./memory_access/memory_access.md)
  - [Memory Access Merging](./memory_access/memory_access_merge.md)
  - [Avoiding UB Bank Conflicts](./memory_access/avoid_ub_bank_conflict_simt.md)
  - [DCache Access Optimization](./memory_access/dcache_access.md)
  - [Improving Transfer Efficiency Through Type Alignment](./memory_access/type_alignment_transfer.md)
  - [Improving Efficiency with Short Vector Types](./memory_access/short_vector_efficiency.md)
  - [Improving Hash Table Lookup Efficiency with SoA Layout](./memory_access/soa_layout_efficiency.md)
- [Execution Configuration](./execution_config/execution_config.md)
  - [Thread Block Count Configuration Optimization](./execution_config/thread_block_config.md)
  - [Configuring the Thread Count Properly to Avoid Register Overflow](./execution_config/thread_count_register_overflow.md)
- [Control Flow](./control_flow/control_flow.md)
  - [Avoiding Branch Divergence Within the Same Warp](./control_flow/avoid_warp_branch.md)
  - [Branch Handling](./control_flow/branch_handing.md)
- [Instruction Optimization](./instruction_optimization/instruction_optimization.md)
  - [Atomic Operation Instruction Optimization](./instruction_optimization/atomic_instruction_optimization.md)
  - [In-place Add Instruction Optimization](./instruction_optimization/inplace_add_instruction_optimization.md)
