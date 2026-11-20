# UnitFlag<a name="ZH-CN_TOPIC_00000025690709788"></a>

<!-- md-trans-meta sourceCommit=13a3d8128166649557d8b2f24fc74d561e042dfb translatedAt=2026-11-02T04:44:31.363Z pushedAt=2026-11-18T01:25:06.821Z nl=2 -->

**Description:**

The core function of unitFlag is as follows: the Mmad and FixPipe APIs introduce a unit-flag mechanism that implements fine-grained data synchronization at the granularity of memory blocks, thereby effectively reducing synchronization latency and improving overall system performance. When the UnitFlag switch is enabled, for the memory blocks in the L0C Buffer, the hardware provides a unit flag bit to indicate whether the block is readable or writable. The specific size of a memory block is controlled by hardware behavior, and users do not need to be aware of it.

After the Mmad and Fixpipe APIs set the unitFlag value to 2/3, the system enables the unit flag bit.

When mmadParams.unitFlag == 2 (0b10), the unitFlag function is enabled. After the hardware finishes executing the instruction, the unit flag bit is not changed.

- For a write operation (Mmad API), if the unit flag bit is 0, the hardware directly writes to the L0C Buffer; otherwise, if the unit flag bit is 1, the write operation waits until the unit flag changes to 0. After execution is complete, the unit flag bit is kept at 0.
- For a read operation (Fixpipe API), if the unit flag bit is 1, the hardware directly reads from the L0C Buffer; otherwise, if the unit flag bit is 0, the read operation waits until the unit flag changes to 1. After execution is complete, the unit flag bit is kept at 1.

When mmadParams.unitFlag == 3 (0b11), the unitFlag function is enabled. After the hardware finishes executing the instruction, the unit flag bit is changed.

- For a write operation (Mmad API), if the unit flag bit is 0, the hardware directly writes to the L0C Buffer; otherwise, if the unit flag bit is 1, the write operation waits until the unit flag changes to 0. After execution is complete, the unit flag bit is set to 1.
- For a read operation (Fixpipe API), if the unit flag bit is 1, the hardware directly reads from the L0C Buffer; otherwise, if the unit flag bit is 0, the read operation waits until the unit flag changes to 1. After execution is complete, the unit flag bit is set to 0.

Based on the above characteristics, when users perform matrix multiplication where matrix A is \[128, 1024\] and matrix B is \[1024, 128\], they need to iterate along the K axis. Assuming each iteration has a K length of 128, eight iterations are required, in which case eight Mmad operations correspond to one Fixpipe operation.

- For the first seven Mmad operations, set the unitFlag to 2, so that the unit flag bit remains 0 after writing, ensuring that subsequent Mmad operations can write to the L0C Buffer.
- For the last Mmad operation, set the unitFlag to 3, so that the unit flag bit is set to 1 after writing, ensuring that Fixpipe can read the L0C Buffer.
- Set the unitFlag of Fixpipe to 3, so that the unit flag bit is set to 0 after reading, ensuring that subsequent Mmad APIs can successfully write data to the L0C Buffer.

If users need to move out the result of a single Mmad operation in multiple batches, for example, when the L0C Buffer of the Mmad computation result is M\(128\) x N\(256\), it is moved out in two batches along the N axis; in this case, one Mmad operation corresponds to two Fixpipe operations.

- For Mmad, set unitFlag = 3 to ensure that Fixpipe can read the L0C Buffer data.
- Set the unitFlag of each Fixpipe to 3, and set the unit flag bit to 0 after reading, ensuring that other subsequent Mmad APIs can successfully write data when reusing this L0C Buffer address.

When unitFlag is enabled, Mmad and Fixpipe perform read and write operations on the same fractal L0C Buffer. Therefore, keeping the read/write order of Mmad computation and Fixpipe consistent helps achieve better performance.

When calling the Mmad API, the computation direction of Mmad must be set through the [SetMMColumnMajor/SetMMRowMajor](../mmad_compute_aux_config/SetMMColumnMajor-SetMMRowMajor.md) API. When Fixpipe enables layout transformations such as NZ2ND or ChannelMerge, the Mmad computation direction must be set to N-direction first, that is, call SetMMRowMajor(); otherwise, if these features are not used, the computation direction should be set to M-direction first, that is, call SetMMColumnMajor().

**Figure 1** Mmad and Fixpipe writing/reading simultaneously along the M direction

![Mmad and Fixpipe writing/reading along the M direction simultaneously](../../../../figures/mmad_unitflag.png "Mmad and Fixpipe writing/reading along the M direction simultaneously")

**Constraints:**

- Both the **Mmad** and **Fixpipe** APIs provide the **unitFlag** parameter to control whether this feature is enabled. Ensure that both are enabled simultaneously for the feature to take effect.
- To keep the same L0C Buffer memory space continuously operated only by multiple **Mmad** or multiple **Fixpipe** instructions, set the **unitFlag** value of the first n-1 instructions to 2 to keep the operated memory space continuously occupied, and set the last instruction to 3 to release the occupied state.
- After the unitFlag feature is enabled, it is recommended that the amount of data computed by **Mmad** be consistent with the amount of data moved out by **Fixpipe**. If **Mmad** computes a large block of data (M × N = 128 × 128) but **Fixpipe** moves out only part of it (M × N = 64 × 64), execution exceptions may occur. You can reset the L0C Buffer state by calling the **SetFixPipeConfig()** API. For details, see the example below.

**Example snippet of iterative looping along the K axis:**

```cpp
// Call Mmad kRound times.
for (auto kIndex = 0; kIndex < kRound; ++kIndex) {
    if (kIndex != kRound - 1) {
        // Set the first kRound-1 iterations to 2 to ensure that Mmad can keep writing to the L0C Buffer during the K-axis iteration loop.
        mmadParams.unitFlag = 2;
    } else {
        // Set the last iteration to 3 to set the unit flag bit to 1, ensuring that Fixpipe can read the L0C Buffer.
        mmadParams.unitFlag = 3;
    }
    AscendC::Mmad(c, a, b, mmadParams);
    if ((m / ALIGN_NUM) * (n / ALIGN_NUM) < LIMIT_MNSIZE) {
        AscendC::PipeBarrier<PIPE_M>();
    }
}
// Fixpipe moves data out once.
// Set Fixpipe's unitFlag to 3. After reading, set the unit flag bit to 0 to ensure that subsequent Mmad API calls can write data to the L0C Buffer successfully.
fixpipeParams.unitFlag = 3;
AscendC::Fixpipe(cGM, c, fixpipeParams);
```

For a complete example with and without the unitFlag feature, see [UnitFlag example](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/mmad_unitflag).

Note that enabling the unitFlag feature does not improve the performance of the Mmad and Fixpipe instructions. The performance gain comes from the fine-grained parallelism of the Mmad and Fixpipe instructions, which reduces synchronization latency. The following table shows the impact of enabling or disabling the unitFlag feature on Mmad instruction performance. The measured data shows that enabling the unitFlag feature has no impact on performance.

**Table 1** Performance example description (using the half input data type as an example; "Measured Value Without Service" means that the preceding and subsequent operations of the instruction are ignored, and only the single-instruction performance is considered, including overheads such as bandwidth latency)

| M | N | K | UnitFlag | LOOP_COUNT | Mmad Theoretical Value (Cycle) | Mmad Measured Value Without Service (Cycle) |
| --- | --- | --- | --- | --- | --- | --- |
| 128 | 256 | 512 | Enable | 8 | 4156 | 4209 |
| 128 | 256 | 512 | Disable | 8 | 4156 | 4225 |
| 128 | 128 | 512 | Enable | 8 | 2108 | 2172 |
| 128 | 128 | 512 | Disable | 8 | 2108 | 2172 |
