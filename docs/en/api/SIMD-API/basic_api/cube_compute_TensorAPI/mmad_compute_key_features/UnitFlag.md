# UnitFlag

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T17:28:47.593Z -->

## Description

The core function of unitFlag is as follows: the Mmad and Copy APIs introduce a unit-flag mechanism that implements fine-grained data synchronization at the memory block granularity, thereby effectively reducing synchronization latency and improving overall system performance. When the UnitFlag switch is enabled, a unit flag bit is provided for each memory block (512B) in the L0C Buffer to indicate whether the block is readable or writable.

After the Mmad and Copy APIs set the unitFlag value to 2/3, the system enables the unit flag bit.

When `mmadParams.unitFlag` is `2 (0b10)`, the unitFlag function is enabled, and the unit flag bit remains unchanged after the hardware completes executing the instruction.

- For write operations (Mmad API), if the unit flag bit is 0, the hardware writes directly to the L0C Buffer; otherwise, if the unit flag bit is 1, the write operation waits until the unit flag changes to 0, and after execution completes, the unit flag bit remains 0.
- For read operations (Copy API), if the unit flag bit is 1, the hardware reads directly from the L0C Buffer; otherwise, if the unit flag bit is 0, the read operation waits until the unit flag changes to 1, and after execution completes, the unit flag bit remains 1.

When `mmadParams.unitFlag` is `3 (0b11)`, the unitFlag function is enabled, and the unit flag bit changes after the hardware completes executing the instruction.

- For write operations (Mmad API), if the unit flag bit is 0, the hardware writes directly to the L0C Buffer; otherwise, if the unit flag bit is 1, the write operation waits until the unit flag changes to 0, and after execution completes, the unit flag bit is set to 1.
- For read operations (Copy API), if the unit flag bit is 1, the hardware reads directly from the L0C Buffer; otherwise, if the unit flag bit is 0, the read operation waits until the unit flag changes to 1, and after execution completes, the unit flag bit is set to 0.

Based on the preceding characteristics, when a user performs cube multiplication with cube A of dimension 128×1024 and cube B of dimension 1024×128, an iterative loop along the K axis is required. Assuming each iteration has a K length of 128, eight iterations are required, in which case eight Mmad instructions correspond to one Fixpipe instruction.

- For the first seven Mmad operations, set unitFlag to 2. After writing, the unit flag bit remains 0, ensuring that subsequent Mmad operations can write to the L0C Buffer.
- For the last Mmad operation, set unitFlag to 3. After writing, the unit flag bit is set to 1, ensuring that Fixpipe can read the L0C Buffer.
- Set the Fixpipe unitFlag to 3. After reading, the unit flag bit is set to 0, ensuring that subsequent Mmad APIs can write L0C Buffer data successfully.

If the user needs to move the result of a single Mmad operation out in multiple passes, for example, when the L0C Buffer of the Mmad computation result is M\(128\) × N\(256\) and it is moved out in two passes along the N axis, one Mmad operation corresponds to two Fixpipe operations.

- For the Mmad operation, set unitFlag = 3 to ensure that Fixpipe can read the L0C Buffer data.
- Set the unitFlag of each Fixpipe operation to 3. After reading, the unit flag bit is set to 0, ensuring that other subsequent Mmad APIs can write data successfully when reusing this L0C Buffer address.

When unitFlag is enabled, Mmad and Fixpipe perform read and write operations on the same fractal L0C Buffer. Therefore, keeping the read/write order of Mmad computation and Fixpipe consistent helps achieve better performance.

When calling the Mmad API, use the [SetMMColumnMajor/SetMMRowMajor](../../cube_compute_ISASI/mmad_compute_aux_config/SetMMColumnMajor-SetMMRowMajor.md) API to set the computation direction of Mmad. When Fixpipe enables layout transformations such as NZ2ND or ChannelMerge, set the Mmad computation direction to N-direction first, that is, call `SetMMRowMajor`. Otherwise, if these features are not used, set the computation direction to M-direction first, that is, call `SetMMColumnMajor`.

**Figure 1**  Mmad and Fixpipe writing/reading along the M direction simultaneously

![Mmad-Fixpipe read/write diagram](../../../../figures/mmad_unitflag.png)

## Constraints

- Both the Mmad and Copy APIs provide the `unitFlag` parameter to control whether this feature is enabled. The two must be enabled synchronously for the feature to take effect.
- To keep the same L0C Buffer memory space continuously operated only by multiple Mmad or multiple Fixpipe instructions, set the `unitFlag` value of the first n-1 instructions to 2 to maintain the occupied state of the operated memory space, and set the value of the last instruction to 3 to release the occupied state.
- After the unitFlag feature is enabled, it is recommended that the amount of data computed by Mmad be consistent with the amount of data moved out by Fixpipe. If Mmad computes a large block of data (M × N = 128 × 128) but Fixpipe moves out only part of it (M × N = 64 × 64), execution exceptions may occur. In this case, you can reset the L0C Buffer state through the `SetFixPipeConfig` API.

## Related APIs

- [Mmad](../mmad_compute/Mmad.md)
- [L0C-to-GM data copy (Copy)](../cube_compute_store/Copy_L0CToGM.md)
- [L0C-to-UB data copy (Copy)](../cube_compute_store/Copy_L0CToUB.md)
- [SetFixPipeConfig](../../cube_compute_ISASI/cube_store_aux_config/SetFixPipeConfig.md)
- [SetMMColumnMajor/SetMMRowMajor](../../cube_compute_ISASI/mmad_compute_aux_config/SetMMColumnMajor-SetMMRowMajor.md)

## Example Snippet of Iterative Loop Along the K Axis

The following example only shows how `unitFlag` is set in the iterative loop along the K axis, omitting tensor construction and parameter initialization.

```cpp
using namespace AscendC::Te;

// Call Mmad kRound times.
for (auto kIndex = 0; kIndex < kRound; ++kIndex) {
    if (kIndex != kRound - 1) {
        // Set the value to 2 for the first kRound-1 iterations to ensure that Mmad can keep writing to the L0C Buffer throughout the K iteration loop.
        mmadParams.unitFlag = 2;
    } else {
        // Set the value to 3 for the last iteration to set the unit flag bit to 1, ensuring that Fixpipe can read the L0C Buffer.
        mmadParams.unitFlag = 3;
    }
    auto mmadAtom = MakeMmad(MmadOperation{}).with(mmadParams);
    Mmad(mmadAtom, l0C, l0A, l0B);
}
// Move out once with Fixpipe.
// Set Fixpipe's unitFlag to 3. After reading, set the unit flag bit to 0 to ensure that subsequent Mmad APIs can write L0C Buffer data smoothly.
FixpipeParams fixpipeParams;
fixpipeParams.unitFlag = 3;
auto fixpipeAtom = MakeCopy(CopyL0C2GM{}).with(fixpipeParams);
Copy(fixpipeAtom, gm, l0C);

```

**Table 1** Performance example description (using the input data type half as an example; "no business measurement" means ignoring the preceding and following operations of the instruction and considering only the single-instruction performance, including overheads such as bandwidth latency)

| M | N | K | UnitFlag | LOOP_COUNT | Mmad theoretical value (cycle) | Mmad measured value without business operations (cycle) |
| --- | --- | --- | --- | --- | --- | --- |
| 128 | 256 | 512 | Enable | 8 | 4156 | 4209 |
| 128 | 256 | 512 | Disable | 8 | 4156 | 4225 |
| 128 | 128 | 512 | Enable | 8 | 2108 | 2172 |
| 128 | 128 | 512 | Disable | 8 | 2108 | 2172 |
