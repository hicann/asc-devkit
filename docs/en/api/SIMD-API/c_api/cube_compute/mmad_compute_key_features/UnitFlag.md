---
okftype: reference.concept
description: "Introduces the unit flag mechanism shared by asc_mmad and the L0C Buffer move-out interface: at memory block granularity, hardware flag bits indicate readability and writability to reduce synchronization latency; explains the read/write wait behavior when ENABLE_KEEP retains the flag and ENABLE_UPDATE toggles the flag; provides configuration combinations for iterating along the K-axis and moving out in multiple passes, the constraint that the computation amount and move-out amount must be consistent, and the method of resetting the state using asc_set_l0c_copy_config."
---
# UnitFlag

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:23:12.270Z pushedAt=2026-11-18T08:15:34.237Z nl=2 -->

**Feature description:**

The core function of unitFlag is as follows: asc_mmad and the L0C Buffer move-out interface introduce a unit flag mechanism, which implements fine-grained data synchronization at the granularity of memory blocks, thereby effectively reducing synchronization latency and improving overall system performance. When the UnitFlag switch is enabled, for memory blocks in the L0C Buffer, the hardware provides a unit flag bit to indicate whether the block is readable or writable. The specific size of a memory block is controlled by hardware behavior and is transparent to users.

asc_mmad enables the unit flag through `asc_unit_flag_mode::ENABLE_KEEP` or `ENABLE_UPDATE`.

When `unit_flag_mode == asc_unit_flag_mode::ENABLE_KEEP`, the unitFlag function is enabled, and after the hardware completes executing the instruction, the unit flag bit is not changed;

- For a write operation (asc_mmad interface), if the unit flag bit is 0, the hardware directly writes to the L0C Buffer; otherwise, if the unit flag bit is 1, the write operation waits until the unit flag changes to 0; after execution completes, the unit flag bit is kept at 0;
- For a read operation (L0C Buffer move-out interface), if the unit flag bit is 1, the hardware directly reads from the L0C Buffer; otherwise, if the unit flag bit is 0, the read operation waits until the unit flag changes to 1; after execution completes, the unit flag bit is kept at 1;

When `unit_flag_mode == asc_unit_flag_mode::ENABLE_UPDATE`, the unitFlag function is enabled, and after the hardware completes executing the instruction, the unit flag bit is changed;

- For a write operation (asc_mmad interface), if the unit flag bit is 0, the hardware directly writes to the L0C Buffer; otherwise, if the unit flag bit is 1, the write operation waits until the unit flag changes to 0; after execution completes, the unit flag bit is set to 1;
- For a read operation (L0C Buffer move-out interface), if the unit flag bit is 1, the hardware directly reads from the L0C Buffer; otherwise, if the unit flag bit is 0, the read operation waits until the unit flag changes to 1; after execution completes, the unit flag bit is set to 0;

Based on the above characteristics, if the user needs to iterate along the K axis when performing cube multiplication where cube A is \[128, 1024\] and cube B is \[1024, 128\], assuming each iteration has a K length of 128, then 8 iterations are required. In this case, 8 asc_mmad operations correspond to 1 move-out operation;

- The first 7 asc_mmad operations are all set to `asc_unit_flag_mode::ENABLE_KEEP`, so that the unit flag bit remains 0 after writing, ensuring that subsequent asc_mmad operations can write to the L0C Buffer.
- The last asc_mmad operation is set to `asc_unit_flag_mode::ENABLE_UPDATE`, so that the unit flag bit is set to 1 after writing, ensuring that Fixpipe can read the L0C Buffer;
- The move-out interface is set to `asc_unit_flag_mode::ENABLE_UPDATE`, so that the unit flag bit is set to 0 after reading, ensuring that subsequent asc_mmad interfaces can write L0C Buffer data smoothly;

If the user needs to move out the result of a single asc_mmad operation in multiple passes, for example, when the L0C Buffer of the asc_mmad computation result is M\(128\) x N\(256\) and it is moved out in two passes along the N axis, then one asc_mmad operation corresponds to two move-out operations;

- For asc_mmad, `asc_unit_flag_mode::ENABLE_UPDATE` needs to be set to ensure that the L0C Buffer data can be read during the move-out;
- Each move-out interface is set to `asc_unit_flag_mode::ENABLE_UPDATE`, so that the unit flag bit is set to 0 after reading, ensuring that other subsequent asc_mmad interfaces can write data smoothly when reusing this L0C Buffer address;

When unitFlag is enabled, the asc_mmad and L0C Buffer move-out interfaces perform read and write operations on the same tiled L0C Buffer. Therefore, keeping the read/write order of the asc_mmad computation and the L0C Buffer move-out interface consistent helps achieve better performance.

When calling the asc_mmad interface, configure the computation direction through [asc_set_mmad_direction_m](../mmad_compute_aux_config/asc_set_mmad_direction_m.md) or [asc_set_mmad_direction_n](../mmad_compute_aux_config/asc_set_mmad_direction_n.md). For details on the computation direction and recommended scenarios, refer to the corresponding interface documentation.

**Figure 1** asc_mmad and L0C Buffer move-out interface writing/reading simultaneously along the M direction

![asc_mmad and L0C Buffer move-out interface writing/reading along the M direction simultaneously](../../../../figures/mmad_unitflag.png "asc_mmad and L0C Buffer move-out interface writing/reading along the M direction simultaneously")

For the compute-side interface, see [asc_mmad](../mmad_compute/asc_mmad.md). For the move-out-side interfaces, see [asc_copy_l0c2gm](../../cube_datamove/cube_compute_store/asc_copy_l0c2gm.md) and [asc_copy_l0c2l1](../../cube_datamove/cube_compute_store/asc_copy_l0c2l1.md). For the mode definition, see [asc_unit_flag_mode](../../defs/enum/asc_unit_flag_mode.md). For state initialization and configuration override behavior, see [asc_set_l0c_copy_config](../../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md).

<!-- npu="950" id1 -->
For Ascend 950PR and 950DT products, also see [asc_copy_l0c2ub](../../cube_datamove/cube_compute_store/asc_copy_l0c2ub.md).
<!-- end id1 -->

**Feature constraints:**

- Both asc_mmad and the L0C Buffer move-out interface provide a UnitFlag control parameter to enable this feature. Ensure that both are enabled synchronously for the feature to take effect.
- When you want to control the same L0C Buffer memory space so that it can be continuously operated only by multiple asc_mmad instructions or multiple move-out instructions, set the unit_flag_mode of the first n-1 instructions to `asc_unit_flag_mode::ENABLE_KEEP` to maintain the occupied state of the operated memory space, and set the last instruction to `asc_unit_flag_mode::ENABLE_UPDATE` to release the occupied state.
- After the unitFlag feature is enabled, it is recommended that the amount of data computed by asc_mmad be consistent with the amount of data moved out. If asc_mmad computes a large block of data (M × N = 128 × 128) but only a portion of it is moved out (M × N = 64 × 64), execution exceptions may occur. In this case, you can reset the L0C Buffer state through the `asc_set_l0c_copy_config` interface.

**Example snippet for iterative reuse along the K axis:**

```cpp
asc_set_mmad_direction_n();
// Call asc_mmad k_round times.
for (uint16_t i = 0; i < k_round; ++i) {
    // Set the first k_round-1 iterations to ENABLE_KEEP to ensure that asc_mmad can keep writing to the L0C Buffer throughout the K iteration loop.
    // For the last iteration, set to ENABLE_UPDATE to ensure asc_copy_l0c2gm can read the L0C Buffer.
    const auto flag = (i + 1 == k_round)
        ? asc_unit_flag_mode::ENABLE_UPDATE
        : asc_unit_flag_mode::ENABLE_KEEP;
    asc_mmad(c, a, b, m, k_tile, n, flag, false, false, i == 0 /* clear only on the first iteration */);
    if ((m / 16) * (n / 16) < 10) {
        asc_sync_pipe(PIPE_M);
    }
}

asc_set_l0c_copy_nz_para(1, 0, 0);
asc_copy_l0c2gm(c_gm, c, n, m, n, m,
    asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM,
    asc_unit_flag_mode::ENABLE_UPDATE, asc_quant_mode::NoQuant,
    asc_relu_pre_mode::NONE, false, true, false, false);
```
