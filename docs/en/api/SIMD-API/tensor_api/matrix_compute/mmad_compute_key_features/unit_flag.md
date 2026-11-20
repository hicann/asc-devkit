# unit_flag

<!-- md-trans-meta sourceCommit=31a38264724303fd61aceda1dcc05c8407b0c930 translatedAt=2026-10-27T03:14:24.074Z pushedAt=2026-10-27T13:03:35.357Z nl=2 -->

## Description

The core function of unit_flag is as follows: the mmad and copy APIs introduce a unit-flag mechanism that implements fine-grained data synchronization at the granularity of memory blocks, thereby effectively reducing synchronization latency and improving overall system performance. When the unit_flag switch is enabled, a unit flag is provided for each memory block (512B) in the L0C Buffer to indicate whether the block is readable or writable.

After the mmad and copy APIs set unit_flag to `unit_flag_mode::enable_keep` or `unit_flag_mode::enable_update`, the system enables the unit flag.

When `params.unit_flag` is `unit_flag_mode::enable_keep` (with an underlying value of 2), the unit_flag function is enabled, and the unit flag is not changed after the hardware finishes executing the instruction.

- For a write operation (mmad API), if the unit flag is 0, the hardware directly writes to the L0C Buffer; otherwise, if the unit flag is 1, the write operation waits until the unit flag changes to 0, and after execution completes, the unit flag is kept at 0.
- For a read operation (copy API), if the unit flag is 1, the hardware directly reads from the L0C Buffer; otherwise, if the unit flag is 0, the read operation waits until the unit flag changes to 1, and after execution completes, the unit flag is kept at 1.

When `params.unit_flag` is `unit_flag_mode::enable_update` (with an underlying value of 3), the unit_flag function is enabled, and the unit flag is changed after the hardware finishes executing the instruction.

- For a write operation (mmad API), if the unit flag is 0, the hardware directly writes to the L0C Buffer; otherwise, if the unit flag is 1, the write operation waits until the unit flag changes to 0, and after execution completes, the unit flag is set to 1.
- For a read operation (copy API), if the unit flag is 1, the hardware directly reads from the L0C Buffer; otherwise, if the unit flag is 0, the read operation waits until the unit flag changes to 1, and after execution completes, the unit flag is set to 0.

Based on the above characteristics, when a user performs cube multiplication with an A cube of dimension 128×1024 and a B cube of dimension 1024×128, an iterative loop along the K axis is required. Assuming the K length of each iteration is 128, eight iterations are required, in which case eight mmad instructions correspond to one Fixpipe instruction.

- For the first seven mmad operations, set the unit flag to `unit_flag_mode::enable_keep`. After writing, the unit flag remains 0, ensuring that subsequent mmad operations can write to the L0C Buffer.
- For the last mmad operation, set the unit flag to `unit_flag_mode::enable_update`. After writing, the unit flag is set to 1, ensuring that Fixpipe can read the L0C Buffer.
- Set the unit_flag of Fixpipe to `unit_flag_mode::enable_update`. After reading, the unit_flag bit is set to 0, ensuring that subsequent mmad APIs can write data to the L0C Buffer smoothly.

If the result of a single mmad operation needs to be moved out in multiple passes, for example, when the L0C Buffer of the mmad computation result is M\(128\) × N\(256\) and it is moved out in two passes along the N axis, one mmad operation corresponds to two Fixpipe operations.

- For the mmad operation, set the unit flag to `unit_flag_mode::enable_update` to ensure that Fixpipe can read the L0C Buffer data.
- Set the unit flag of each Fixpipe operation to `unit_flag_mode::enable_update`. After reading, the unit flag is set to 0, ensuring that other subsequent mmad APIs can write data smoothly when reusing this L0C Buffer address.

When the unit flag is enabled, mmad and Fixpipe perform read and write operations on the same tiled L0C Buffer. Therefore, keeping the read/write order of mmad computation and Fixpipe consistent helps achieve better performance.

When calling the mmad API, set the computation direction of mmad through [asc_set_mmad_direction_m](../../../c_api/cube_compute/mmad_compute_aux_config/asc_set_mmad_direction_m.md) or [asc_set_mmad_direction_n](../../../c_api/cube_compute/mmad_compute_aux_config/asc_set_mmad_direction_n.md). When Fixpipe enables layout transformations such as NZ2ND or ChannelMerge, set the computation direction of mmad to N-direction first, that is, call `asc_set_mmad_direction_n`. Otherwise, if these features are not used, set the computation direction to M-direction first, that is, call `asc_set_mmad_direction_m`.

**Figure 1** mmad and Fixpipe write/read along the M direction simultaneously

![mmad-Fixpipe read/write diagram](../../../../figures/mmad_unitflag.png)

## Constraints

- Both the mmad and copy APIs provide the unit_flag parameter to control whether this feature is enabled. Ensure that both are enabled simultaneously for the feature to take effect.
- To keep the same L0C Buffer memory space continuously operated only by multiple mmad or multiple Fixpipe instructions, set the unit_flag value of the first n-1 instructions to `unit_flag_mode::enable_keep` to maintain the continuous occupation of the operated memory space, and set the last instruction to `unit_flag_mode::enable_update` to release the occupied state.
- After the unit flag feature is enabled, it is recommended that the amount of data computed by mmad be consistent with the amount of data moved out by Fixpipe. If mmad computes a large block of data (M × N = 128 × 128) but Fixpipe moves out only part of it (M × N = 64 × 64), execution exceptions may occur. In this case, you can reset the L0C Buffer state through the `SetFixPipeConfig` API.

## Related APIs

- [mmad](../mmad_compute/mmad.md)
- [copy (L0C to GM movement)](../cube_compute_store/copy_l0c_to_gm.md)
- [copy (L0C to UB movement)](../cube_compute_store/copy_l0c_to_ub.md)
- [asc_set_mmad_direction_m](../../../c_api/cube_compute/mmad_compute_aux_config/asc_set_mmad_direction_m.md)
- [asc_set_mmad_direction_n](../../../c_api/cube_compute/mmad_compute_aux_config/asc_set_mmad_direction_n.md)

## Example Snippet of Iterative Loop Along the K Axis

The following example only shows how `unit_flag` is set in the iterative loop along the K axis, omitting tensor construction and parameter initialization.

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

// Call mmad k_round times.
for (auto k_index = 0; k_index < k_round; ++k_index) {
    if (k_index != k_round - 1) {
        // Keep the unit flag for the first k_round-1 iterations to ensure that mmad can keep writing to the L0C Buffer throughout the K iteration loop.
        params.unit_flag = unit_flag_mode::enable_keep;
    } else {
        // Update the unit flag in the last iteration to ensure that Fixpipe can read the L0C Buffer.
        params.unit_flag = unit_flag_mode::enable_update;
    }
    auto mmad_atom = make_mmad(mmad_operation{}).with(params);
    mmad(mmad_atom, l0c, l0a, l0b);
}
// Fixpipe moves the data out at once.
// Fixpipe updates the unit flag to ensure that subsequent mmad APIs can write data to the L0C Buffer successfully.
l0c_to_gm_params fixpipe_params;
fixpipe_params.unit_flag = unit_flag_mode::enable_update;
auto fixpipe_atom = make_copy(copy_l0c_to_gm{}).with(fixpipe_params);
copy(fixpipe_atom, gm, l0c);

```

**Table 1** Performance example description (taking the input data type half as an example; "no-service measurement" means that the operations before and after the instruction are ignored, and only the single-instruction performance is considered, including overheads such as bandwidth latency)

| M | N | K | unit_flag | LOOP_COUNT | mmad Theoretical Value (cycle) | mmad No-Service Measurement Value (cycle) |
| --- | --- | --- | --- | --- | --- | --- |
| 128 | 256 | 512 | Enable | 8 | 4156 | 4209 |
| 128 | 256 | 512 | Disable | 8 | 4156 | 4225 |
| 128 | 128 | 512 | Enable | 8 | 2108 | 2172 |
| 128 | 128 | 512 | Disable | 8 | 2108 | 2172 |
