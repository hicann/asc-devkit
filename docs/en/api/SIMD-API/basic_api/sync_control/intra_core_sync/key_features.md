# Key Features

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T19:00:36.782Z pushedAt=2026-10-26T07:41:44.824Z nl=2 -->

## Automatic Synchronization Overview

The automatic synchronization feature reduces the workload of manually inserting synchronization. The following points about automatic synchronization need to be understood:

- The scope of automatic insertion of synchronization is **intra-core synchronization of the SIMD API (including single-pipeline synchronization and multi-pipeline synchronization)**.
- Synchronization between some APIs is guaranteed by hardware, requiring no manual or automatic insertion of synchronization. For details, see [Hardware-Guaranteed Synchronization](#hardware-guaranteed-synchronization).
- Both the [TPipe-TQue framework programming paradigm](../../../../../../en/guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_paradigm.md) and enabling the [cce-auto-sync compilation option](../../../../../../en/guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#common-compilation-options) can automatically insert synchronization (the latter is automatically inserted by the BiSheng compiler). The two approaches are not mutually exclusive, and each inserts different synchronization types, so they can take effect simultaneously.
- The prerequisite for automatic insertion of synchronization by the TPipe-TQue framework programming paradigm and by enabling the cce-auto-sync compilation option is that their respective usage constraints must be satisfied.

## Automatic Synchronization Decision Tree

As shown in Figure 1, developers can follow the flow in the figure to determine whether synchronization needs to be manually inserted between a pair of APIs. For the supported automatic synchronization cases, see [Table 1](#table-aiv-tpipe-sync), [Table 2](#table-aic-tpipe-sync), and [Table 3](#table-aiv-cce-sync).

Ascend C provides [three-tier gradient SIMD programming APIs](../../../../../../en/guide/programming_guide/programming_model/ai_core_simd_programming/overview.md#ascend-c-multi-level-programming-interfaces). The following lists the specific synchronization management for each type of API:

- [TPipe-TQue framework programming - synchronization mechanism](../../../../../../en/guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_principles.md#data-dependency-and-synchronization-how-the-two-apis-work-together).
- [Pointer-based C programming - synchronization mechanism](../../../../../../en/guide/programming_guide/programming_model/ai_core_simd_programming/c_pointer_programming/c_programming_overview.md#synchronization-mechanism).
- [Tensor-based CPP programming - synchronization mechanism](../../../../../../en/guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/cpp_tensor_programming_overview.md#synchronization-mechanism).

The Tensors operated by Tensor-based CPP programming can be divided into basic Tensors and extended Tensors. For the specific meanings and differences between the two types of Tensors, see [Tensor memory abstraction](../../../../../../en/guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/cpp_tensor_programming_overview.md#tensor-memory-abstraction).

The usage constraints and supported synchronization types of the two approaches are described in the subsequent sections: [TPipe-TQue framework paradigm automatic synchronization](#tpipe-tque-auto-sync) and [cce-auto-sync compilation option automatic synchronization](#cce-compiler-auto-sync).

**Figure 1**  Automatic synchronization decision tree  
![](../../../../figures/auto_sync_decision_tree.png)

## Hardware-Guaranteed Synchronization

<!-- npu="950" id1 -->
- For the Ascend 950PR&950DT products, the single-pipeline synchronization between PIPE\_V and PIPE\_V is guaranteed by hardware.
<!-- end id1 -->

<!-- npu="A3,910b" id2 -->
- The following APIs all belong to PIPE_S. The multi-pipeline synchronization between these APIs and APIs of other pipelines is guaranteed by hardware:

    <!-- npu="A3" id3 -->
    - Atlas A3 products
    <!-- end id3 -->
    <!-- npu="910b" id4 -->
    - Atlas A2 products
    <!-- end id4 -->

    The specific APIs are as follows:
    - Mask operation APIs: [SetVectorMask](../../memory_vector_compute/mask_operations/SetVectorMask.md), [SetMaskCount](../../memory_vector_compute/mask_operations/SetMaskCount.md), [SetMaskNorm](../../memory_vector_compute/mask_operations/SetMaskNorm.md), [ResetMask](../../memory_vector_compute/mask_operations/ResetMask.md).
    - Data movement piggyback atomic operation APIs: [SetAtomicAdd](../../atomic_operations/SetAtomicAdd.md), [SetAtomicMax](../../atomic_operations/SetAtomicMax_ISASI.md), [SetAtomicMin](../../atomic_operations/SetAtomicMin_ISASI.md), [DisableDmaAtomic](../../atomic_operations/DisableDmaAtomic.md), [SetAtomicType](../../atomic_operations/SetAtomicType.md).
    - [APIs related to calling CTRL registers](../../../basic_api/special_register_access/special_register_access.md).
    <!-- end id2 -->

## TPipe-TQue Framework Programming Paradigm Automatic Synchronization<a id="tpipe-tque-auto-sync"></a>

### Usage Constraints<a id="tpipe-tque-auto-sync-constraints"></a>

The automatic synchronization feature of the TPipe-TQue framework programming paradigm takes effect only when the following prerequisites are met:

- Use the TPipe-TQue framework programming paradigm.
- Use the **EnQue**/**DeQue** and **AllocTensor**/**FreeTensor** APIs correctly to ensure that their automatic synchronization works properly. This constraint corresponds to [Counterexample 2](#tpipe-tque-auto-sync-counterexamples).

### Supported Synchronization Types

- Single-pipeline synchronization

    The TPipe-TQue framework programming paradigm does not support insertion of single-pipeline synchronization.
- Multi-pipeline synchronization

    Under the TPipe-TQue framework programming paradigm, the framework can automatically insert some multi-pipeline synchronization to resolve two types of data dependencies: read-after-write (RAW) and write-after-read (WAR). For details on the underlying principles, see [TPipe-TQue Framework Data Dependency and Synchronization Mechanism](../../../../../../en/guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_principles.md#data-dependency-and-synchronization-how-the-two-apis-work-together).

    <!-- npu="A3,910b" id5 -->
    Taking [NPU architecture 2201](../../../../../../en/guide/programming_guide/language_extension/simd_builtin_keywords.md) as an example, the automatic synchronization support for different pipelines in AIV and AIC under this hardware architecture is shown in [Table 1](#table-aiv-tpipe-sync) and [Table 2](#table-aic-tpipe-sync), respectively.
    <!-- end id5 -->

    **Table 1**  Synchronization between different pipelines in AIV under the TPipe-TQue framework programming paradigm <a id="table-aiv-tpipe-sync"></a>

    | Source pipeline \ destination pipeline | PIPE_S | PIPE_V | PIPE_MTE2 | PIPE_MTE3 |
    | --- | --- | --- | --- | --- |
    | PIPE_S | - | - | - | - |
    | PIPE_V | - | - | Supported | Supported |
    | PIPE_MTE2 | - | Supported | - | Supported |
    | PIPE_MTE3 | - | Supported | Supported | - |

    **Table 2**  Synchronization between different pipelines in AIC under the TPipe-TQue framework programming paradigm <a id="table-aic-tpipe-sync"></a>

    | Source pipeline \ destination pipeline | PIPE_S | PIPE_M | PIPE_MTE1 | PIPE_MTE2 | PIPE_MTE3 | PIPE_FIX |
    | --- | --- | --- | --- | --- | --- | --- |
    | PIPE_S | - | - | - | - | - | - |
    | PIPE_M | - | - | Supported | Supported | - | Supported |
    | PIPE_MTE1 | - | Supported | - | Supported | Supported | Supported |
    | PIPE_MTE2 | - | Supported | Supported | - | Supported | - |
    | PIPE_MTE3 | - | - | Supported | Supported | - | - |
    | PIPE_FIX | - | Supported | Supported | - | - | - |

### Counterexamples<a id="tpipe-tque-auto-sync-counterexamples"></a>

The following counterexamples illustrate typical scenarios where synchronization cannot be automatically inserted because the [usage constraints](#tpipe-tque-auto-sync-constraints) are not met.

- Counterexample 1: The TPipe-TQue framework programming paradigm is not used. For example, when the [Add vector addition sample based on the TPipe-TQue framework programming paradigm](../../../../../../../examples/01_simd_cpp_api/00_introduction/01_add/add_tpipe_tque) is rewritten based on the [static Tensor programming paradigm](../../../../../../en/guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md), the static Tensor programming paradigm cannot automatically insert `MTE2_V`, which ultimately causes the sample to fail.

    ```cpp
    AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
    AscendC::LocalTensor<float> xLocal = ubAllocator.Alloc<float, blockLength>();
    AscendC::LocalTensor<float> yLocal = ubAllocator.Alloc<float, blockLength>();
    AscendC::LocalTensor<float> zLocal = ubAllocator.Alloc<float, blockLength>();

    AscendC::DataCopy(xLocal, xGm, blockLength);
    AscendC::DataCopy(yLocal, yGm, blockLength);

    AscendC::Add(zLocal, xLocal, yLocal, blockLength);
    ```

- Counterexample 2: The EnQue/DeQue and AllocTensor/FreeTensor APIs (provided by the TPipe-TQue framework programming paradigm) are not used correctly. For example, in the [Add vector addition sample based on the TPipe-TQue framework programming paradigm](../../../../../../../examples/01_simd_cpp_api/00_introduction/01_add/add_tpipe_tque), if `xLocal = inQueueX.DeQue<float>()` is missing, the TPipe-TQue framework programming paradigm cannot automatically insert `MTE2_V`, which ultimately causes the sample to fail.

    ```cpp
    // Use DataCopy to move the input from GM to the Unified Buffer (UB), and enqueue the LocalTensor through EnQue for the subsequent computation stage to obtain via DeQue.
    AscendC::LocalTensor<float> xLocal = inQueueX.AllocTensor<float>();
    AscendC::LocalTensor<float> yLocal = inQueueY.AllocTensor<float>();
    AscendC::DataCopy(xLocal, xGm, blockLength);
    AscendC::DataCopy(yLocal, yGm, blockLength);
    inQueueX.EnQue(xLocal);
    inQueueY.EnQue(yLocal);

    // DeQue the input tensor, perform Add in the UB, and enqueue the result to the output queue for subsequent write-back to GM.
    // xLocal is not dequeued from inQueueX, which prevents the TPipe-TQue framework programming paradigm from automatically inserting MTE2_V.
    yLocal = inQueueY.DeQue<float>();
    AscendC::LocalTensor<float> zLocal = outQueueZ.AllocTensor<float>();
    AscendC::Add(zLocal, xLocal, yLocal, blockLength);
    ```

## Enabling the cce-auto-sync Compilation Option for Automatic Synchronization (Automatic Insertion by the BiSheng Compiler)<a id="cce-compiler-auto-sync"></a>

### Usage Constraints<a id="cce-compiler-auto-sync-constraints"></a>

The automatic synchronization feature enabled by the cce-auto-sync compilation option takes effect only when the following prerequisites are met:

- Heterogeneous compilation launch (<<<>>> invocation), kernel function (Kernel) launch operator projects, and custom operator development projects have `--cce-auto-sync=on` set by default. If developers manually set `--cce-auto-sync=off`, the BiSheng compiler cannot automatically insert synchronization.
- APIs belonging to PIPE\_V, PIPE\_MTE2, and PIPE\_MTE3 must operate on basic Tensors (including [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md) and [GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor.md)).
- APIs belonging to PIPE\_S must be within the following API set:
    - Tensor scalar read/write: [LocalTensor::GetValue](../../data_structures/LocalTensor/GetValue.md), [LocalTensor::SetValue](../../data_structures/LocalTensor/SetValue.md), [LocalTensor::operator()](../../data_structures/LocalTensor/operator_call.md), [GlobalTensor::GetValue](../../data_structures/GlobalTensor/GetValue.md), [GlobalTensor::operator()](../../data_structures/GlobalTensor/operator_call.md).
    - Vector reduction auxiliary: [GetReduceRepeatMaxMinSpr](../../memory_vector_compute/reduction_compute_aux_config/GetReduceRepeatMaxMinSpr_ISASI.md), [GetReduceRepeatSumSpr](../../memory_vector_compute/reduction_compute_aux_config/GetReduceRepeatSumSpr_ISASI.md).
    - Sort and merge: [GetMrgSortResult](../../memory_vector_compute/sort_and_merge_ISASI/GetMrgSortResult.md).

    For PIPE\_S APIs not in the above set, the BiSheng compiler cannot automatically insert synchronization:
    <!-- npu="950" id6 -->
    For example, for the Ascend 950PR&950DT products, none of the scalar atomic operation APIs are in the above API set (including [AtomicAdd](../../atomic_operations/AtomicAdd.md), [AtomicCas](../../atomic_operations/AtomicCas.md), [AtomicExch](../../atomic_operations/AtomicExch.md), [AtomicMax](../../atomic_operations/AtomicMax.md), and [AtomicMin](../../atomic_operations/AtomicMin.md)). Therefore, when these APIs have data dependencies with APIs belonging to PIPE\_MTE2 and PIPE\_MTE3 in reading/writing GM, the BiSheng compiler cannot automatically insert synchronization, and developers must manually insert synchronization based on the actual situation.
    <!-- end id6 -->

### Supported Synchronization Types

- Single-pipeline synchronization

    <!-- npu="A3,910b" id7 -->
    - For [NPU architecture 2201](../../../../../../en/guide/programming_guide/language_extension/simd_builtin_keywords.md), PIPE\_V synchronization insertion is automatically completed by the BiSheng compiler.
    <!-- end id7 -->

    <!-- npu="950" id8 -->
    - For [NPU architecture 3510](../../../../../../en/guide/programming_guide/language_extension/simd_builtin_keywords.md), the synchronization between PIPE\_V is guaranteed by hardware.
    <!-- end id8 -->

    - PIPE\_MTE2/PIPE\_MTE3 requires the developer to insert synchronization when the transfer addresses overlap (for a specific example, see [PipeBarrier Constraint Description](../intra_core_sync/PipeBarrier_ISASI.md)).

- Multi-pipeline synchronization
    - In the Vector compute unit, the BiSheng compiler supports automatic insertion of multi-pipeline synchronization (bidirectional) between PIPE\_V and PIPE\_S, PIPE\_MTE2 and PIPE\_S, and PIPE\_MTE3 and PIPE\_S.
    - In the Cube compute unit, the BiSheng compiler does not support automatic insertion of any type of synchronization.

<!-- npu="A3,910b" id9 -->
Taking [NPU architecture 2201](../../../../../../en/guide/programming_guide/language_extension/simd_builtin_keywords.md) as an example, the automatic synchronization support for different pipelines in the AIV under this hardware architecture is shown in [Table 3](#table-aiv-cce-sync).
<!-- end id9 -->

**Table 3**  Synchronization of different pipelines in the AIV when the cce-auto-sync compilation option is enabled <a id="table-aiv-cce-sync"></a>

| Source pipeline \ destination pipeline | PIPE_S | PIPE_V | PIPE_MTE2 | PIPE_MTE3 |
| --- | --- | --- | --- | --- |
| PIPE_S | - | Supported | Supported | Supported |
| PIPE_V | Supported | Supported | - | - |
| PIPE_MTE2 | Supported | - | - | - |
| PIPE_MTE3 | Supported | - | - | - |

### Counterexample<a id="cce-compiler-auto-sync-counterexamples"></a>

The following counterexamples illustrate typical scenarios where synchronization cannot be automatically inserted because the [usage constraints](#cce-compiler-auto-sync-constraints) are not met.

In the [Add sample based on pointer-based C language programming](../../../../../../../examples/02_simd_c_api/00_introduction/01_add/c_api_async_add), the 0th element of x_local is accessed directly through a pointer. In this case, the BiSheng compiler cannot automatically insert synchronization between PIPE_MTE2 (asc_copy_gm2ub) and PIPE_S.

```c
__gm__ float* x_gm = x + block_idx * block_length;
__gm__ float* y_gm = y + block_idx * block_length;
__gm__ float* z_gm = z + block_idx * block_length;

__ubuf__ float x_local[block_length];
__ubuf__ float y_local[block_length];
__ubuf__ float z_local[block_length];


asc_copy_gm2ub(x_local, x_gm, block_length * sizeof(float));
asc_copy_gm2ub(y_local, y_gm, block_length * sizeof(float));

x_local[0] = 1.0f;

asc_sync();

asc_add(z_local, x_local, y_local, block_length);
asc_sync();

asc_copy_ub2gm(z_gm, z_local, block_length * sizeof(float));
asc_sync();
```

## Automatic Synchronization Logging

When using the automatic synchronization feature, developers often cannot be certain whether the TPipe-TQue framework programming paradigm or the BiSheng compiler has inserted synchronization instructions, which types of synchronization have been inserted, and where exactly they were inserted. To help developers verify the insertion of automatic synchronization, this section introduces two ways to view automatic synchronization information: the instruction log feature in NPU simulation mode and the automatic synchronization log feature of the BiSheng compiler.

### NPU Simulation Mode Instruction Log

The synchronization inserted by the TPipe-TQue framework programming paradigm does not have a separate log feature. However, after enabling NPU simulation mode (by setting the compilation option `-DCMAKE_ASC_RUN_MODE=sim`; see [Compilation and Running](../../../../../../../examples/01_simd_cpp_api/00_introduction/01_add/add_tpipe_tque/README_en.md#compilation-and-execution)), you can **view both** the synchronization instruction information automatically inserted by the TPipe-TQue framework programming paradigm and the BiSheng compiler through the instruction log.

In the Add vector addition sample based on the TPipe-TQue framework programming paradigm, after enabling NPU simulation mode, the instruction log of vector core 0 (`core0.veccore0.instr_log.dump`) viewed in the build directory is as follows:

<!-- npu="A3,910b" id10 -->
> [!NOTE]
> The following log fractal and the corresponding analysis are valid only for [NPU Architecture 2201](../../../../../../en/guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

```text
[info] [00002009] (PC: 0x10d0d378) MTE2     : (Binary: 0x40a210bc) SET_FLAG  PIPE:MTE2, TRIGGER PIPE:VEC, FLAG ID:0, 
[info] [00002009] (PC: 0x10d0d380) MTE2     : (Binary: 0x40a210b4) SET_FLAG  PIPE:MTE2, TRIGGER PIPE:VEC, FLAG ID:1, 
[info] [00002009] (PC: 0x10d0d32c) MTE2     : (Binary: 0x711cb788) MOV_OUT_TO_UB  Src:OUT, Dst:UB, XD:X14=0x2000, XN:X11=0x11321800, XM:X15=0x1000010,  id: 4
[info] [00002010] (PC: 0x10d0d5ac) SCALAR   : (Binary: 0x04c02010) ST_XD_XN_IMM  dtype:B64, XD:X0=0x1, XN:X2=0x1c7ae8, IMM:0x10, 
[info] [00002010] (PC: 0x10d0d3ac) VEC      : (Binary: 0x40c210ac) WAIT_FLAG  PIPE:MTE2, TRIGGER PIPE:VEC, FLAG ID:0, 
[info] [00002011] (PC: 0x10d0d5b4) SCALAR   : (Binary: 0x0406b003) ST_XD_XN_IMM  dtype:B8, XD:X3=0, XN:X11=0x1c7b90, IMM:0x3, 
[info] [00002011] (PC: 0x10d0d3e4) VEC      : (Binary: 0x40c210a8) WAIT_FLAG  PIPE:MTE2, TRIGGER PIPE:VEC, FLAG ID:1, 
[info] [00002012] (PC: 0x10d0d5bc) SCALAR   : (Binary: 0x0e001500) STI_XN_XM  dtype:B8, XN:X1=0x1c7b90, XM:X10=0, #POST:0, #IMM_TYPE:ZERO, 
[info] [00002028] (PC: 0x10d0d5e0) SCALAR   : (Binary: 0x0f35e5a1) STI_XN_IMM  dtype:B8, XN:X30=0x1c8270, IMM:0xd2d, #POST:0, #IMM_TYPE:ONE, 
[info] [00002028] (PC: 0x10d0d484) VEC      : (Binary: 0x80400008) MOVEMASK  XN:X2=0x800, Pos:0, Id:295
[info] [00002028] (PC: 0x10d0d494) VEC      : (Binary: 0x40e00400) BAR  PIPE:VEC
[info] [00002045] (PC: 0x10d0d498) VEC      : (Binary: 0x804000bc) MOVEMASK  XN:X15=0, Pos:1, Id:300
[info] [00002118] (PC: 0x10d0d4b8) VEC      : (Binary: 0x85c4c6bc) VADD  XD:X2=0x4000, XN:X12=0, XM:X13=0x2000, XT:X15=0x100080808010101, Dtype:F32, Id:308
```

The log fractal above roughly corresponds to the following code fractal of the Add vector addition sample. In the log, `MOV_OUT_TO_UB` corresponds to `DataCopy` (source address GM, destination address UB), `SET_FLAG` corresponds to `SetFlag`, `WAIT_FLAG` corresponds to `WaitFlag`, and `VADD` corresponds to `Add`. From the entries numbered 00002009, 00002010, and 00002011 in the log, it can be seen that the TPipe-TQue framework programming paradigm automatically inserts the `SetFlag` and `WaitFlag` instructions between PIPE_MTE2 and PIPE_V.
<!-- end id10 -->

```cpp
AscendC::LocalTensor<float> xLocal = inQueueX.AllocTensor<float>();
AscendC::LocalTensor<float> yLocal = inQueueY.AllocTensor<float>();
AscendC::DataCopy(xLocal, xGm, blockLength);
AscendC::DataCopy(yLocal, yGm, blockLength);
inQueueX.EnQue(xLocal);
inQueueY.EnQue(yLocal);

// DeQue the input tensor, perform Add in UB, and EnQue the result to the output queue for subsequent write-back to GM.
xLocal = inQueueX.DeQue<float>();
yLocal = inQueueY.DeQue<float>();
AscendC::LocalTensor<float> zLocal = outQueueZ.AllocTensor<float>();
AscendC::Add(zLocal, xLocal, yLocal, blockLength);
```

### BiSheng Compiler Automatic Synchronization Log

The BiSheng compiler provides the `--cce-auto-sync-log=<file>` compilation option to output synchronization insertion information to the `<file>` file, helping developers explicitly identify the synchronization instruction information inserted by the BiSheng compiler in the operator file.

> [!CAUTION]Note    
> To obtain the BiSheng compiler automatic synchronization log, you also need to set the [`-g` compilation option](../../../../../../en/guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#common-compilation-options) to obtain the line numbers of the operator code file.

Depending on the development scenario, the ways to add this compilation option are as follows:

- When using the BiSheng compiler directly, you can add this compilation option directly to the compilation command.
- When using the Ascend C kernel launch operator project, you can add this compilation option through `ascendc_compile_options`.
- When using the Ascend C custom operator development project, you can add this compilation option through `add_ops_compile_options`.

The following code file is sync_log_test.h:

```cpp
AscendC::LocalTensor<T> dstLocal;
T ave_tmp = 0;
Vector_OP1(dstLocal, params); 
ave_tmp = dstLocal.GetValue(0);
Vector_OP2(dstLocal, params); 
for (int i = 0; i < ave_tmp; ++i) {
    dstLocal.SetValue(i,0);
}
```

After automatic synchronization is enabled, the insertion positions of the synchronization instructions are as follows:

```cpp
AscendC::LocalTensor<T> dstLocal;
T ave_tmp = 0;
Vector_OP1(dstLocal, params); 
AscendC::SetFlag<AscendC::HardEvent::V_S>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::V_S>(EVENT_ID0);
ave_tmp = dstLocal.GetValue(0);
AscendC::PipeBarrier<PIPE_V>();
AscendC::SetFlag<AscendC::HardEvent::S_V>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::S_V>(EVENT_ID0);
Vector_OP2(dstLocal, params); 
AscendC::SetFlag<AscendC::HardEvent::V_S>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::V_S>(EVENT_ID0);
for (int i = 0; i < ave_tmp; ++i) {
    dstLocal.SetValue(i,0);
}
```

After the automatic synchronization debug log feature is enabled, the output log is as follows:

```text
The BiSheng Auto Sync log of sync_log_test :  
Position: absolute-path/sync_log_test.h:4 : line before insert sync : SetFlag<HardEvent::V_S>(EVENT_ID0);
Position: absolute-path/sync_log_test.h:4 : line before insert sync : WaitFlag<HardEvent::V_S>(EVENT_ID0);
Position: absolute-path/sync_log_test.h:5 : line before insert sync : PipeBarrier<PIPE_V>();
Position: absolute-path/sync_log_test.h:5 : line before insert sync : SetFlag<HardEvent::S_V>(EVENT_ID0);
Position: absolute-path/sync_log_test.h:5 : line before insert sync : WaitFlag<HardEvent::S_V>(EVENT_ID0);
Position: absolute-path/sync_log_test.h:6 : line before insert sync : SetFlag<HardEvent::V_S>(EVENT_ID0);
Position: absolute-path/sync_log_test.h:6 : line before insert sync : WaitFlag<HardEvent::V_S>(EVENT_ID0);
```

Here, "line before" indicates the synchronization instruction inserted immediately before the current line.
