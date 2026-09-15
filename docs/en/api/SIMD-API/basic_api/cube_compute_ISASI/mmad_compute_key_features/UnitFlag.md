# UnitFlag<a name="ZH-CN_TOPIC_00000025690709788"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-27T17:06:43.470Z -->

**Description:**

The core function of **unitFlag** is as follows: the **Mmad** and **FixPipe** APIs introduce a unit-flag mechanism that implements fine-grained data synchronization at the granularity of memory blocks, thereby effectively reducing synchronization latency and improving overall system performance. When the **UnitFlag** switch is enabled, a unit flag bit is provided for each memory block (512B) in the **L0C Buffer** to indicate whether the block is readable or writable.

After the **Mmad** and **Fixpipe** APIs set the **unitFlag** value to 2/3, the system enables the unit flag bit.

When **mmadParams.unitFlag** == 2 (0b10), the **unitFlag** function is enabled, and the unit flag bit is not changed after the hardware completes instruction execution.

- For a write operation (**Mmad** API), if the unit flag bit is 0, the hardware directly writes to the **L0C Buffer**; otherwise, if the unit flag bit is 1, the write operation waits until the unit flag changes to 0. After execution completes, the unit flag bit remains 0.
- For a read operation (**Fixpipe** API), if the unit flag bit is 1, the hardware directly reads from the **L0C Buffer**; otherwise, if the unit flag bit is 0, the read operation waits until the unit flag changes to 1. After execution completes, the unit flag bit remains 1.

When **mmadParams.unitFlag** == 3 (0b11), the **unitFlag** function is enabled, and the unit flag bit is changed after the hardware completes instruction execution.

- For a write operation (**Mmad** API), if the unit flag bit is 0, the hardware directly writes to the **L0C Buffer**; otherwise, if the unit flag bit is 1, the write operation waits until the unit flag changes to 0. After execution completes, the unit flag bit is set to 1.
- For a read operation (**Fixpipe** API), if the unit flag bit is 1, the hardware directly reads from the **L0C Buffer**; otherwise, if the unit flag bit is 0, the read operation waits until the unit flag changes to 1. After execution completes, the unit flag bit is set to 0.

Based on the preceding characteristics, when a user performs cube multiplication where cube A is [128, 1024] and cube B is [1024, 128], the computation must iterate along the K axis. Assuming each iteration has a K length of 128, eight iterations are required, in which case eight **Mmad** operations correspond to one **FixPipe** operation.

- For the first seven **Mmad** operations, set **unitFlag** to 2. After writing, the unit flag bit remains 0, ensuring that subsequent **Mmad** operations can write to the L0C Buffer.
- For the last **Mmad** operation, set **unitFlag** to 3. After writing, the unit flag bit is set to 1, ensuring that **FixPipe** can read the L0C Buffer.
- Set **unitFlag** of **FixPipe** to 3. After reading, the unit flag bit is set to 0, ensuring that subsequent **Mmad** APIs can write to the L0C Buffer smoothly.

If a user needs to move out the result of a single **Mmad** operation in multiple batches, for example, when the L0C Buffer of the **Mmad** computation result is M(128) x N(256) and it is moved out in two batches along the N axis, one **Mmad** operation corresponds to two **FixPipe** operations.

- When calling **Mmad**, set **unitFlag** to 3 to ensure that **FixPipe** can read the data in the **L0C Buffer**.
- Set **unitFlag** to 3 for each **FixPipe** call, and reset the unit flag bit to 0 after reading, to ensure that subsequent **Mmad** APIs can write data smoothly when reusing this **L0C Buffer** address.

When **unitFlag** is enabled, **Mmad** and **FixPipe** perform read and write operations on the same fractal **L0C Buffer**. Therefore, keeping the read/write order of **Mmad** computation and **FixPipe** consistent helps achieve better performance.

When calling the **Mmad** API, set the computation direction of **Mmad** through the [SetMMColumnMajor/SetMMRowMajor](../mmad_compute_aux_config/SetMMColumnMajor-SetMMRowMajor.md) API. When **FixPipe** enables layout transformations such as **NZ2ND** or **ChannelMerge**, set the computation direction of **Mmad** to N-direction first, that is, call **SetMMRowMajor()**. Otherwise, if these features are not used, set the computation direction to M-direction first, that is, call **SetMMColumnMajor()**.

**Figure 1** Mmad and FixPipe writing/reading along the M direction simultaneously

![Mmad and FixPipe writing/reading along the M direction simultaneously](../../../../figures/mmad_unitflag.png "Mmad and FixPipe writing/reading along the M direction simultaneously")

**Constraints:**

- Both the **Mmad** and **FixPipe** APIs provide the **unitFlag** parameter to control whether this feature is enabled. Ensure that both are enabled synchronously for the feature to take effect.
- To keep the same L0C Buffer memory space continuously occupied by multiple **Mmad** or multiple **FixPipe** instructions, set the **unitFlag** value of the first n-1 instructions to 2 to maintain the occupied state of the operated memory space, and set the last instruction to 3 to release the occupied state.
- After the **unitFlag** feature is enabled, it is recommended that the amount of data computed by **Mmad** be consistent with the amount of data moved out by **FixPipe**. If **Mmad** computes a large block of data (M × N = 128 × 128) but **FixPipe** moves out only part of it (M × N = 64 × 64), execution exceptions may occur. You can reset the state of the L0C Buffer through the **SetFixPipeConfig()** API. For details, see the example below.

**Example snippet for iterative loop along the K axis:**

```cpp
// Call Mmad kRound times.
for (auto kIndex = 0; kIndex < kRound; ++kIndex) {
    if (kIndex != kRound - 1) {
        // Set the value to 2 for the first kRound-1 iterations to ensure that Mmad can keep writing to the L0C Buffer throughout the K iteration loop.
        mmadParams.unitFlag = 2;
    } else {
        // Set the value to 3 for the last iteration to set the unit flag bit to 1, ensuring that Fixpipe can read the L0C Buffer.
        mmadParams.unitFlag = 3;
    }
    AscendC::Mmad(c, a, b, mmadParams);
    if ((m / ALIGN_NUM) * (n / ALIGN_NUM) < LIMIT_MNSIZE) {
        AscendC::PipeBarrier<PIPE_M>();
    }
}
// Fixpipe moves out the data at once.
// Set FixPipe's unitFlag to 3. After reading, set the unit flag bit to 0 to ensure that the subsequent Mmad API can write data to the L0C Buffer smoothly.
fixpipeParams.unitFlag = 3;
AscendC::Fixpipe(cGM, c, fixpipeParams);
```

Complete example of enabling and disabling the unitFlag feature: [UnitFlag example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_cube_compute/mmad_unitflag)

**Table 1** Performance example description (using the half input data type as an example; "no business measurement" means that the preceding and following operations of the instruction are ignored, and only the single-instruction performance is considered, including overheads such as bandwidth latency)

| M | N | K | UnitFlag | LOOP_COUNT | Mmad theoretical value (cycle) | Mmad measured value without business (cycle) |
| --- | --- | --- | --- | --- | --- | --- |
| 128 | 256 | 512 | Enable | 8 | 4156 | 4209 |
| 128 | 256 | 512 | Disable | 8 | 4156 | 4225 |
| 128 | 128 | 512 | Enable | 8 | 2108 | 2172 |
| 128 | 128 | 512 | Disable | 8 | 2108 | 2172 |
