# PipeBarrier\(ISASI\)<a name="ZH-CN_TOPIC_0000001835308877"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T13:53:46.363Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_block_sync_intf.h"`.

As shown in Figure 1, this API implements synchronization control within the same pipeline to constrain the execution order inside the pipeline. It ensures that all data read and write operations of preceding instructions are completed before subsequent instructions can be executed.

**Figure 1**  PipeBarrier API function diagram<a name="zh-cn_topic_0000002549838971_fig7681194617307"></a>  
![](../../../../figures/pipebarrier_function_diagram.png "PipeBarrier_API_function_diagram")

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <pipe_t pipe>
__aicore__ inline void PipeBarrier()
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| -------- | -------- |
| pipe | Template parameter, indicating the pipeline type to be blocked.<br>For supported pipelines, see [hardware pipeline types](intra_core_synchronization_capability_overview.md#zh-cn_topic_0000002542725361_section1272612276459), where **PIPE_S** is not supported.<br>If you do not care about the pipeline type and want to block all pipelines, pass **PIPE_ALL**. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- Synchronization between Scalar pipelines is automatically guaranteed by the hardware. The PipeBarrier API does not support synchronization of the **PIPE_S** single pipeline.

- `PipeBarrier<PIPE_ALL>()` waits for all previously submitted APIs in all pipelines to complete, which affects performance. If blocking only a single pipeline can resolve the issue, avoid calling `PipeBarrier<PIPE_ALL>()` arbitrarily.

- When the addresses moved by **PIPE_MTE2**/**PIPE_MTE3** overlap, developers need to insert synchronization. For example, when multiple DataCopy instructions need to be executed and their destination addresses overlap, call `PipeBarrier<PIPE_MTE3>()` to insert a PipeBarrier synchronization instruction to ensure serialization of the multiple DataCopy instructions and prevent abnormal data. As shown in the left diagram below, when two DataCopy instructions are executed and the destination GM addresses being moved overlap, call `PipeBarrier<PIPE_MTE3>()` between the two move instructions to add synchronization for the MTE3 move-out pipeline. As shown in the right diagram below, when the destination UB addresses being moved overlap, call `PipeBarrier<PIPE_MTE2>()` between the two move instructions to add synchronization for the MTE2 move-in pipeline.

![](../../../../figures/datacopy_address_overlap_pipebarrier.png "DataCopy address overlap PipeBarrier synchronization diagram")

## Example<a name="section837496171220"></a>

In the following example, the input **dst0Local** of the **Mul** instruction is the output of the **Add** instruction. The two vector instructions have a dependency, so **PipeBarrier** must be inserted to ensure the execution order of the two instructions.

Note: This is for reference only. When automatic synchronization is enabled (it is enabled by default in Kernel direct-call operator projects and custom operator development projects), the compiler automatically inserts PIPE\_V synchronization, so developers do not need to insert it manually.

**Figure 2** The **Mul** instruction and the **Add** instruction are in a serial relationship. The **Mul** instruction can be executed only after the **Add** instruction completes execution.<a name="fig1359216580459"></a>  
![](../../../../figures/mul_add_pipebarrier_sync_diagram.png "Mul_Add_instruction_serial_dependency_PipeBarrier_synchronization_diagram")

```cpp
AscendC::LocalTensor<half> src0Local;
AscendC::LocalTensor<half> src1Local;
AscendC::LocalTensor<half> src2Local;
AscendC::LocalTensor<half> dst0Local;
AscendC::LocalTensor<half> dst1Local;

AscendC::Add(dst0Local, src0Local, src1Local, 512);
AscendC::PipeBarrier<PIPE_V>();
AscendC::Mul(dst1Local, dst0Local, src2Local, 512);
```
