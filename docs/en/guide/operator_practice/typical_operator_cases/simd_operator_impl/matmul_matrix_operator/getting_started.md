# Entry-Level Feature Implementation<a name="ZH-CN_TOPIC_000000_ADD_ENTRY"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-11-09T10:44:04.832Z pushedAt=2026-11-19T06:32:19.844Z nl=2 -->

This section provides programming guidance for cube multiplication using basic APIs. It introduces the basic method of implementing a cube operator based on the Ascend C programming model and the Cube computing unit. In the operator, the data transfer and compute pipelines are coordinated through event synchronization, which reduces programming complexity.

## Details

For details, see the [matmul sample](../../../../../../../examples/01_simd_cpp_api/00_introduction/02_matrix/matmul_basic_api), which covers:

- Operator implementation: use of Global Tensor and Local Tensor, DataCopy data transfer, and Matmul computation
- Function debugging: printf formatted output and DumpTensor tensor data printing
- Compilation and running: BiSheng compilation and execution
- Performance debugging: on-board and simulation performance collection using the msOpProf tool

### Optimization Direction Analysis

The preceding implementation correctly implements cube multiplication, but it still has the following optimization opportunities in terms of performance.

| Optimization Direction | Problem in the Current Implementation | Expected Optimization Gain |
|-----------|---------------|-------------|
| **L1/L0 single buffer** | The A and B cubes each have only one buffer in L1 and L0, so the next round of data cannot be loaded in until the previous round is fully consumed | Switch to Ping-Pong double buffering to overlap data transfer and computation |
| **Small MTE2 transfer granularity** | Each DataCopyIn moves in one baseM x baseK block, resulting in dense MTE2 instruction issue | Increase the movement granularity to reduce the number of MTE2 instructions and improve bandwidth utilization |
| **Serial pipeline execution** | The current implementation uses PipeBarrier and SetFlag/WaitFlag for pipeline synchronization, but does not adopt a double buffering mechanism, so the MTE2, MTE1, M, and FIX hardware units cannot fully run in parallel | Use double buffering and hardware event synchronization to run the four units in parallel, significantly improving throughput |
| **MTE1 instruction queue pressure** | Under dav-2201, LoadData2D must be issued multiple times in a for loop, and the MTE1 instruction queue has limited depth | Use a more efficient load method to reduce MTE1 queue pressure |
| **Fixed multi-core partitioning** | The core count allocation is computed by hardcoding `M / singleCoreM`, which applies only to fixed cube sizes and core counts | A general partitioning strategy that supports automatic allocation for any core count and cube size |
| **Runtime computation of tiling parameters** | Scalar is used inside the core to compute parameters of each dimension level by level, introducing extra instruction overhead | Determine parameters at compile time to reduce runtime overhead |
| **Serial Fixpipe and cube** | The L0C write-back must wait until all Mmad operations are complete, leaving the Cube idle during write-back | Use fine-grained Fixpipe issue so that write-back and computation overlap in the pipeline |
| **Movement order of cube B** | Cubes A and B are moved in equal amounts sequentially; in larger cube scenarios, cube B arriving late may become a pipeline bottleneck | Move cube B first to alleviate data shortage |
| **Underutilized L2Cache** | In the current implementation, the cube B is read from GM every time, and repeated GM access by multiple cores puts high pressure on MTE2 bandwidth; the pure read bandwidth of L2Cache is about 3 to 4 times that of GM, but the current implementation does not leverage this feature | Partition the cube A along the M direction so that the cube B stays in L2Cache across rounds, reducing GM access and effectively lowering MTE2 time |

> For the detailed implementation of each optimization direction above, see [Intermediate Performance Optimization](./intermediate_optimization.md) and [Advanced Extreme Optimization](./advanced_extreme_optimization.md).
