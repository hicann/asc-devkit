# Overall Constraints<a name="ZH-CN_TOPIC_0000002565840545"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-02T05:03:34.833Z pushedAt=2026-11-18T01:25:06.869Z nl=2 -->

All data movement APIs share several common constraint rules. Before performing any transfer operation, ensure that these constraints are satisfied; otherwise, undefined behavior or data anomalies may occur.

## Address Alignment Constraints<a name="section136711917101710"></a>

Different storage units on the AI Core have different address alignment requirements. The start address of an Ascend C API operand must satisfy the alignment constraint of the corresponding storage unit. **DataBlock** (32 bytes) is the basic addressing granularity for data movement operations, and the **blockLen** parameter of most data movement APIs is calculated in units of **DataBlock**. The alignment requirements of each storage unit are shown in the following table.

**Table 1** **Address alignment requirements of each storage unit**<a name="table16278354141117"></a>

| Storage Unit | Alignment Requirement | Description |
|----------|----------|------|
| Global Memory | 1-byte alignment | The most relaxed alignment requirement; any byte address is allowed. |
| UB | 32-byte alignment | Consistent with the **DataBlock** size; the address must be a multiple of 32. |
| L1 Buffer | 32-byte alignment | Same as UB; the address must be a multiple of 32. |
| L0A Buffer/L0B Buffer | 512-byte alignment | High alignment requirement for matrix computation inputs. |
| L0C Buffer | 64-byte alignment | Matrix computation result buffer. |
| BiasTable Buffer | 64-byte alignment | Bias data buffer. |
| Fixpipe Buffer | 128-byte alignment | Temporary storage area for quantization parameters. |

> [!NOTE]Note
  > - Note that if the constraint description of a specific API explicitly specifies an alignment requirement different from the table above, the description of that API prevails.
  > - For scenarios that do not satisfy the alignment requirement, use the non-aligned data movement capability of the **DataCopyPad** API.

## Multi-Instruction Synchronization<a name="section711085812111"></a>

If multiple DataCopy instructions need to be executed and their destination addresses overlap, you need to call [PipeBarrier](../../sync_control/intra_core_sync/PipeBarrier_ISASI.md) to insert a synchronization instruction, ensuring that multiple DataCopy instructions are serialized to prevent abnormal data. As shown in the left diagram below, when two DataCopy instructions are executed and their destination GM addresses overlap, the two copy instructions need to call PipeBarrier<PIPE\_MTE3>\(\) to synchronize the MTE3 pipeline. As shown in the right diagram below, when the destination addresses in the Unified Buffer (UB) overlap, the two copy instructions need to call PipeBarrier<PIPE\_MTE2>\(\) to synchronize the MTE2 pipeline. The core code example of multi-instruction synchronization is as follows:

```cpp
AscendC::DataCopy(src2Local, src2Global, srcDataSize);
// Call PipeBarrier<PIPE_MTE2>() between the two copy instructions to synchronize the MTE2 copy-in pipeline.
AscendC::PipeBarrier<PIPE_MTE2>();
AscendC::DataCopy(src1Local, src1Global, srcDataSize);
// The address ranges allocated for the input src1 and src2 on the UB overlap.
// To ensure that the data of src2 does not overwrite the value of src1 in the overlapping range, copy in src2 first and then src1.
```

**Figure 1**  Overlapping destination addresses of DataCopy<a name="fig333145263318"></a>  

![](../../../../figures/datacopy_address_overlap_sync_diagram.png "Overlapping destination addresses of DataCopy")
