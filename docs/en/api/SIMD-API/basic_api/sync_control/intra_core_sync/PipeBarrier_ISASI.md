# PipeBarrier\(ISASI\)<a name="ZH-CN_TOPIC_0000001835308877"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T19:04:44.402Z pushedAt=2026-10-26T07:41:44.831Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/intra_core_sync/PipeBarrier_ISASI_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_block_sync_intf.h"`.

As shown in Figure 1, this API implements synchronization control within the same pipeline to constrain the execution order inside the pipeline. It ensures that all data read and write operations of the preceding instructions are complete before the subsequent instructions can be executed.

**Figure 1**  PipeBarrier API function diagram<a name="zh-cn_topic_0000002549838971_fig7681194617307"></a>  
![](../../../../figures/pipebarrier_function_diagram.png "PipeBarrier API function diagram")

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
template <pipe_t pipe>
__aicore__ inline void PipeBarrier()
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| -------- | -------- |
| pipe | Template parameter, indicating the pipeline type to be blocked.<br>For supported pipelines, see [hardware pipeline types](intra_core_sync_overview.md#zh-cn_topic_0000002542725361_section1272612276459), where PIPE_S is not supported.<br>If you do not care about the pipeline type and want to block all pipelines, pass PIPE_ALL. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- Synchronization between Scalar pipelines is automatically guaranteed by the hardware. The PipeBarrier API does not support PIPE\_S synchronization of a single pipeline.

- `PipeBarrier<PIPE_ALL>()` waits for all previously submitted APIs in all pipelines to complete, which affects performance. If blocking only a single pipeline can resolve the issue, avoid calling `PipeBarrier<PIPE_ALL>()` arbitrarily.

- For PIPE\_MTE2/PIPE\_MTE3, when the copy addresses overlap, developers need to insert synchronization. For example, when multiple DataCopy instructions need to be executed and their destination addresses overlap, call the PipeBarrier API to insert a single-pipeline synchronization instruction to ensure serialization of multiple DataCopy instructions and prevent data anomalies. As shown in the left diagram below, when two DataCopy instructions are executed and their destination GM addresses overlap, call `PipeBarrier<PIPE_MTE3>()` between the two copy instructions to add synchronization for the MTE3 copy-out pipeline. As shown in the right diagram below, when the destination Unified Buffer (UB) addresses overlap, call `PipeBarrier<PIPE_MTE2>()` between the two copy instructions to add synchronization for the MTE2 copy-in pipeline.

![](../../../../figures/datacopy_address_overlap_pipebarrier.png "DataCopy address overlap PipeBarrier synchronization diagram")

## Example<a name="section837496171220"></a>

In the following example, the input **dst0Local** of the **Mul** instruction is the output of the **Add** instruction. The two vector operation instructions have a dependency, so **PipeBarrier** must be inserted to ensure the execution order of the two instructions.

Note: This is only an example for reference. In the following cases, PIPE\_V synchronization does not need to be manually inserted by developers:
<!-- npu="A3,910b" id10 -->
- When the [cce-auto-sync compilation option](key_features.md#cce-compiler-auto-sync) is enabled, the BiSheng Compiler automatically inserts synchronization between PIPE\_V.
<!-- end id10 -->
<!-- npu="950" id11 -->
- For Ascend 950PR & 950DT products, synchronization between PIPE\_V is guaranteed by hardware.
<!-- end id11 -->

**Figure 2** The **Mul** instruction and the **Add** instruction are in a serial relationship. The **Mul** instruction can be executed only after the **Add** instruction is completed.<a name="fig1359216580459"></a>  
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
