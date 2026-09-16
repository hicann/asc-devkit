# Key Features<a name="ZH-CN_TOPIC_0000002555780804"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T13:52:16.588Z -->

## Automatic Synchronization Overview

Both the [TPipe-TQue framework programming paradigm](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_framework_programming_paradigm.md) and the Bisheng Compiler support automatic synchronization, which reduces the workload of manual insertion of synchronization for developers. However, in some scenarios, developers still need to complete synchronization manually. This chapter describes the types of automatic synchronization supported by the TPipe-TQue framework programming paradigm and the Bisheng Compiler (hereinafter referred to as the compiler), the constraints on using automatic synchronization, and the compiler automatic synchronization debug logging feature.

## TPipe-TQue Framework Programming Paradigm and Compiler Automatic Synchronization Support<a name="zh-cn_topic_0000002521905636_section_auto_sync"></a>

The automatic synchronization support of the TPipe-TQue framework programming paradigm and the compiler for the Vector compute unit and Cube compute unit is as follows:

- Vector compute unit
    - Single-stream synchronization: For PIPE\_V, the compiler automatically inserts synchronization. For PIPE\_MTE2/PIPE\_MTE3, the developer needs to insert synchronization when the transfer addresses overlap (for specific examples, see [Constraints](../intra_core_sync/PipeBarrier_ISASI.md)). The TPipe-TQue framework programming paradigm does not support inserting single-stream synchronization.
    - Multi-stream synchronization: The multi-stream synchronization among PIPE\_V, PIPE\_MTE2, PIPE\_MTE3, and PIPE\_S is bidirectional, as shown in the following figure. **The synchronization indicated by the yellow lines is automatically inserted by the compiler**, and the remaining synchronization indicated by the black lines is automatically inserted by the TPipe-TQue framework programming paradigm through APIs such as EnQue/DeQue and AllocTensor/FreeTensor.
        ![](../../../../figures/aiv_auto_insert_sync_diagram.png)
        > [!CAUTION]Note
        > Exception: When scalar atomic operation APIs (including [AtomicAdd](../../atomic_operations/AtomicAdd.md), [AtomicCas](../../atomic_operations/AtomicCas.md), [AtomicExch](../../atomic_operations/AtomicExch.md), [AtomicMax](../../atomic_operations/AtomicMax.md), and [AtomicMin](../../atomic_operations/AtomicMin.md)) have data dependencies with the transfer units (MTE2/MTE3) when reading and writing GM, the compiler cannot automatically insert synchronization, and the developer needs to manually insert synchronization based on the actual situation.

- Cube compute unit
    The Cube-side compiler does not support automatically inserting any type of synchronization.

<!-- npu="910b,A3" id1 -->

Taking [NPU architecture 2201](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMT-BuiltIn_keyword.md#table65291052154114) as an example, the automatic synchronization support of different streams in AIV and AIC under this hardware architecture is shown in Table 1 and Table 2, respectively. Under the TPipe-TQue framework programming paradigm, the framework can automatically insert partial synchronization to resolve the two types of data dependencies, namely write-after-read (WAR) and read-after-write (RAW). For the specific principle, see [TPipe-TQue Framework Data Dependency and Synchronization Mechanism](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_framework_programming_principles.md#data-dependency-and-synchronization-mechanism-collaborative-work-of-the-dual-apis).

**Table 1**  Synchronization of different streams in AIV under the TPipe-TQue framework programming paradigm <a name="zh-cn_topic_0000002521905636_table13753342164113"></a>

| Source Stream\Destination Stream | PIPE_S | PIPE_V | PIPE_MTE2 | PIPE_MTE3 |
| --- | --- | --- | --- | --- |
| PIPE_S | Not applicable | Compiler automatic synchronization | Compiler automatic synchronization | Compiler automatic synchronization |
| PIPE_V | Compiler automatic synchronization | Compiler automatic insertion of PipeBarrier | TPipe-TQue framework automatic synchronization | TPipe-TQue framework automatic synchronization |
| PIPE_MTE2 | Compiler automatic synchronization | TPipe-TQue framework automatic synchronization | Manual insertion of PipeBarrier | TPipe-TQue framework automatic synchronization |
| PIPE_MTE3 | Compiler automatic synchronization | TPipe-TQue framework automatic synchronization | TPipe-TQue framework automatic synchronization | Manual insertion of PipeBarrier |

**Table 2**  Synchronization between different pipelines in the AIC under the TPipe-TQue framework programming paradigm <a name="zh-cn_topic_0000002521905636_table169315822914"></a>

| Source Stream\Destination Stream | PIPE_S | PIPE_M | PIPE_MTE1 | PIPE_MTE2 | PIPE_MTE3 | PIPE_FIX |
| --- | --- | --- | --- | --- | --- | --- |
| PIPE_S | Not applicable | Not applicable | Not applicable | Not applicable | Not applicable | Not applicable |
| PIPE_M | Not applicable | Manual insertion of PipeBarrier | TPipe-TQue framework automatic synchronization | TPipe-TQue framework automatic synchronization | Not applicable | TPipe-TQue framework automatic synchronization |
| PIPE_MTE1 | Not applicable | TPipe-TQue framework automatic synchronization | Manual insertion of PipeBarrier | TPipe-TQue framework automatic synchronization | TPipe-TQue framework automatic synchronization | TPipe-TQue framework automatic synchronization |
| PIPE_MTE2 | Not applicable | TPipe-TQue framework automatic synchronization | TPipe-TQue framework automatic synchronization | Manual insertion of PipeBarrier | TPipe-TQue framework automatic synchronization | No application scenario currently |
| PIPE_MTE3 | Not applicable | Not applicable | TPipe-TQue framework automatic synchronization | TPipe-TQue framework automatic synchronization | Manual insertion of PipeBarrier | No application scenario currently |
| PIPE_FIX | Not applicable | TPipe-TQue framework automatic synchronization | TPipe-TQue framework automatic synchronization | No application scenario currently | No application scenario currently | Manual insertion of PipeBarrier |

<!-- end id1 -->

## Automatic Synchronization Constraints<a name="zh-cn_topic_0000002521905636_section56111955113614"></a>

- You can enable the compiler automatic synchronization feature through the compilation option "--cce-auto-sync" or "--cce-auto-sync=on". Kernel direct-call operator projects, custom operator development projects, and heterogeneous compilation <<<>>> direct calls have this compilation option enabled by default. To disable it, developers can manually set "--cce-auto-sync=off".
  > [!CAUTION]Note
  > Ascend C provides a [three-layer gradient programmable API](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/programming_model/ai_core_simd_programming/overview.md#ascend-c-multi-level-programming-apis). The support for automatic synchronization at each API level is as follows:
  >- When the TPipe-TQue framework programming paradigm is used, the compiler can automatically insert partial synchronization. For details, see [Table 1](#zh-cn_topic_0000002521905636_table13753342164113). In this case, the TPipe-TQue framework programming paradigm itself can also automatically insert another part of synchronization. For details, see Table 1 and [Table 2](#zh-cn_topic_0000002521905636_table169315822914).
  >- When the [static Tensor programming paradigm](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md) is used, the compiler can automatically insert partial synchronization. For details, see Table 1. Because the TPipe-TQue framework programming paradigm and the static Tensor programming paradigm cannot be used simultaneously, apart from the synchronization automatically inserted by the compiler, the remaining synchronization must be managed manually by developers.
  >- When [pointer-based C language programming](https://gitcode.com/cann/asc-devkit/tree/9.1.0/docs/en/guide/programming_guide/programming_model/ai_core_simd_programming/pointer-based-C-language-programming) is used, the TPipe-TQue framework programming paradigm cannot be used simultaneously. In addition, the compiler cannot automatically insert synchronization. Therefore, all synchronization must be managed by developers. For details, see [C API programming synchronization control](https://gitcode.com/cann/asc-devkit/tree/9.1.0/docs/en/guide/programming_guide/programming_model/ai_core_simd_programming/pointer-based-C-language-programming/C-API-programming-based-on-the-language-extension-layer.md#section6735112217720).

- All functions called in the Kernel function must be inline functions for the compiler to automatically insert synchronization.
- The TPipe-TQue framework programming paradigm must be used correctly to ensure that its automatic synchronization feature works properly. If developers use the static Tensor programming paradigm, the automatic synchronization feature provided by TPipe/TQue becomes invalid, and synchronization must be inserted manually; however, this does not affect the automatic synchronization feature supported by the compiler.
- The TPipe-TQue framework programming paradigm does not automatically insert single-stream synchronization instructions. When the copy addresses overlap, developers need to manually insert synchronization. For details, see the PipeBarrier constraints.
- Under the TPipe-TQue framework programming paradigm, if developers manually insert synchronization instructions that the compiler or the TPipe-TQue framework programming paradigm has already automatically inserted, the synchronization instructions will be inserted repeatedly, which may cause program logic exceptions or performance degradation.
- Synchronization between AIC and AIV using [SetFlag/WaitFlag](SetFlag_WaitFlag_ISASI.md) is not supported, and undefined hardware behavior may occur. Synchronization control between AIC and AIV belongs to inter-core synchronization, which requires calling [CrossCoreSetFlag](../inter_core_sync/CrossCoreSetFlag_ISASI.md) and [CrossCoreWaitFlag](../inter_core_sync/CrossCoreWaitFlag_ISASI.md).
<!-- npu="910b,A3" id2 -->

- Synchronization between the following APIs and other instructions is guaranteed by hardware, and no automatic or manual insertion of synchronization is required. This description applies to the following models:
    <!-- npu="A3" id3 -->
    - Atlas A3 training products/Atlas A3 inference products
    <!-- end id3 -->
    <!-- npu="910b" id4 -->
    - Atlas A2 training products/Atlas A2 inference products
    <!-- end id4 -->

  > [!CAUTION]Note
  > When calling the **SetVectorMask**, **SetMaskCount**, **SetMaskNorm**, and **ResetMask** APIs, you do not need to manually insert synchronization.
  > When calling the **SetAtomicAdd**, **SetAtomicMax**, **SetAtomicMin**, **DisableDmaAtomic**, and **SetAtomicType** APIs, you do not need to manually insert synchronization.
  > When calling CTRL register-related instructions, you do not need to manually insert synchronization.

<!-- end id2 -->

## Compiler Automatic Synchronization Debug Logging Feature

The BiSheng Compiler provides the `--cce-auto-sync-log=<file>` compilation option to output synchronization insertion information to the `<file>` file, helping developers explicitly identify the synchronization instructions inserted by the compiler in the operator file. The operator must be compiled in debug mode (by adding the `-g` compilation option) to obtain the line numbers in the operator code file.

- When using the BiSheng Compiler directly, you can add this compilation option directly to the compilation command.
- When using an Ascend C kernel direct-call operator project, you can add this compilation option through **ascendc_compile_options**.
- When using an Ascend C custom operator development project, you can add this compilation option through **add_ops_compile_options**.

The following code file is `sync_log_test.h`:

```cpp
LocalTensor<T> dstLocal;
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
LocalTensor<T> dstLocal;
T ave_tmp = 0;
Vector_OP1(dstLocal, params); 
SetFlag<HardEvent::V_S>(EVENT_ID0);
WaitFlag<HardEvent::V_S>(EVENT_ID0);
ave_tmp = dstLocal.GetValue(0);
PipeBarrier<PIPE_V>();
SetFlag<HardEvent::S_V>(EVENT_ID0);
WaitFlag<HardEvent::S_V>(EVENT_ID0);
Vector_OP2(dstLocal, params); 
SetFlag<HardEvent::V_S>(EVENT_ID0);
WaitFlag<HardEvent::V_S>(EVENT_ID0);
for (int i = 0; i < ave_tmp; ++i) {
    dstLocal.SetValue(i,0);
}
```

After the compiler automatic synchronization debug logging feature is enabled, the output log is as follows:

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
