# Overview

<!-- md-trans-meta sourceCommit=1398d4baddf82d4b412922bb4a32a05abf2ada8a translatedAt=2026-11-16T15:37:41.919Z pushedAt=2026-11-18T03:12:16.719Z nl=2 -->

## Experimental API Overview

The experimental APIs provide register computation in dual-return-value form, data movement, and instruction issue queue status query capabilities. To use them, you need to define the `CANN_ASC_USE_EXPERIMENTAL` macro and explicitly include the corresponding header file. Compatibility is not guaranteed in later versions.

## API List

### Experimental API

**Table 1** Experimental APIs

| API | Interface Description | Finest-grained Common Header File |
| --- | --- | --- |
| [asc_get_pipe_mte2_idle_slot_count, asc_get_pipe_mte3_idle_slot_count, asc_get_pipe_v_idle_slot_count, asc_get_pipe_m_idle_slot_count, asc_get_pipe_mte1_idle_slot_count, asc_get_pipe_fix_idle_slot_count](asc_get_pipe_idle_slot_count.md) | Obtains the current number of idle slots in the issue queue corresponding to the specified pipeline. | `c_api/experimental/asc_get_pipe_idle_slot_count.h` |
| [asc_add_carry](asc_add_carry.md) | Performs 32-bit element-wise addition by mask, returning both the addition result and the carry flag, and supports operations with an input carry. | `c_api/experimental/reg_arith_exp.h` |
| [asc_sub_carry](asc_sub_carry.md) | Performs 32-bit element-wise subtraction by mask, returning both the subtraction result and the carry flag; a carry of 1 indicates no borrow, and 0 indicates a borrow. | `c_api/experimental/reg_arith_exp.h` |
| [asc_mull](asc_mull.md) | Multiplies 32-bit elements by mask, returning the low 32 bits and high 32 bits of the 64-bit product separately. | `c_api/experimental/reg_arith_exp.h` |
| [asc_intlv](asc_intlv.md) | Interleaves two source vector data registers or mask registers, returning two register results. | `c_api/experimental/reg_permute_sel_exp.h` |
| [asc_deintlv](asc_deintlv.md) | De-interleaves two source vector data registers or mask registers, returning two register results. | `c_api/experimental/reg_permute_sel_exp.h` |
| [asc_loadalign_deintlv](asc_loadalign_deintlv.md) | Reads 2×VL length data from a 32-byte aligned address in UB, returning even-indexed and odd-indexed elements separately. | `c_api/experimental/loadalign_exp.h` |

### Related Types

**Table 2** Related types

| Name | Description | Finest-grained Common Header File |
| --- | --- | --- |
| [Dual-return-value struct](pair.md) | Carries the two register results returned by the experimental API through the `first` and `second` members. | `c_api/experimental/pair.h` |
