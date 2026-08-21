# 关键特性说明

## 自动同步概述

自动同步功能可减少开发者手动插入同步的工作量，自动同步特性需要了解以下要点：

- 自动插入同步的范围是**SIMD-API的核内同步（包含单流水同步和多流水同步）**。
- 部分接口间的同步由硬件保证，无需手动或者自动插入同步，具体内容请参考[硬件保证的同步](#硬件保证的同步)。
- [TPipe-TQue框架编程范式](../../../../../../zh/guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_paradigm.md)和开启[cce-auto-sync编译选项](../../../../../../zh/guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#常用的编译选项)都能自动插入同步（后者由毕昇编译器自动插入），两种方式不互斥、各自插入的同步类型不同，可同时生效。
- TPipe-TQue框架编程范式和开启cce-auto-sync编译选项自动插入同步的前提是必须满足各自的使用约束。

## 自动同步决策树

如图1所示，开发者可以按照图中流程判断一对接口之间是否需要手动插入同步，自动同步的支持情况见[表1](#table-aiv-tpipe-sync)、[表2](#table-aic-tpipe-sync)和[表3](#table-aiv-cce-sync)。

Ascend C提供了[三层梯度化SIMD编程接口](../../../../../../zh/guide/programming_guide/programming_model/ai_core_simd_programming/overview.md#ascend-c多级编程接口)，下面列出了各类接口管理同步的具体情况：

- [基于TPipe-TQue框架编程-同步机制](../../../../../../zh/guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_principles.md#数据依赖与同步机制双api的协同工作)。
- [基于指针的C语言编程-同步机制](../../../../../../zh/guide/programming_guide/programming_model/ai_core_simd_programming/c_pointer_programming/c_programming_overview.md#同步机制)。
- [基于Tensor的CPP编程-同步机制](../../../../../../zh/guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/cpp_tensor_programming_overview.md#同步机制)。

基于Tensor的CPP编程操作的Tensor可以分为基础Tensor和扩展Tensor，两种Tensor的具体含义和区别请参考[Tensor内存抽象](../../../../../../zh/guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/cpp_tensor_programming_overview.md#tensor内存抽象)。

两种方式的使用约束和支持的同步类型分别在后续章节展开：[TPipe-TQue框架范式自动同步](#tpipe-tque-auto-sync)和[开启cce-auto-sync编译选项自动同步](#cce-compiler-auto-sync)。

**图1**  自动同步决策树  
![](../../../../figures/auto_sync_decision_tree.png)

## 硬件保证的同步

<!-- npu="950" id1 -->
- 针对Ascend 950PR/Ascend 950DT，PIPE\_V与PIPE\_V间的单流水同步由硬件保证。
<!-- end id1 -->

<!-- npu="A3,910b" id2 -->
- 以下接口都属于PIPE_S，这些接口与其它流水的接口之间的多流水同步由硬件保证：

    <!-- npu="A3" id3 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id3 -->
    <!-- npu="910b" id4 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id4 -->

    具体接口如下：
    - 掩码操作接口：[SetVectorMask](../../memory_vector_compute/mask_operations/SetVectorMask.md)、[SetMaskCount](../../memory_vector_compute/mask_operations/SetMaskCount.md)、[SetMaskNorm](../../memory_vector_compute/mask_operations/SetMaskNorm.md)、[ResetMask](../../memory_vector_compute/mask_operations/ResetMask.md)。
    - 数据搬运随路原子操作接口：[SetAtomicAdd](../../atomic_operations/SetAtomicAdd.md)、[SetAtomicMax](../../atomic_operations/SetAtomicMax_ISASI.md)、[SetAtomicMin](../../atomic_operations/SetAtomicMin_ISASI.md)、[DisableDmaAtomic](../../atomic_operations/DisableDmaAtomic.md)、[SetAtomicType](../../atomic_operations/SetAtomicType.md)。
    - [调用CTRL寄存器相关的接口](../../../basic_api/special_register_access/special_register_access.md)。
    <!-- end id2 -->

## TPipe-TQue框架编程范式自动同步<a id="tpipe-tque-auto-sync"></a>

### 使用约束<a id="tpipe-tque-auto-sync-constraints"></a>

TPipe-TQue框架编程范式的自动同步功能需满足以下前提条件方可生效：

- 使用TPipe-TQue框架编程范式。
- 正确使用EnQue/DeQue、AllocTensor/FreeTensor接口才能保证其自动同步功能正常运作，本条约束对应[反例2](#tpipe-tque-auto-sync-counterexamples)。

### 支持的同步类型

- 单流水同步

    TPipe-TQue框架编程范式不支持插入单流水同步。
- 多流水同步

    在TPipe-TQue框架编程范式下，框架能够自动插入部分多流水同步，以解决先写后读（RAW，Read-After-Write）和先读后写（WAR，Write-After-Read）两类数据依赖，具体原理参考[TPipe-TQue框架数据依赖与同步机制](../../../../../../zh/guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_principles.md#数据依赖与同步机制双api的协同工作)。

    <!-- npu="A3,910b" id5 -->
    以[NPU架构2201](../../../../../../zh/guide/programming_guide/language_extension/simd_builtin_keywords.md)为例，该硬件架构下AIV和AIC中不同流水线的自动同步支持情况分别如[表1](#table-aiv-tpipe-sync)和[表2](#table-aic-tpipe-sync)所示。
    <!-- end id5 -->

    **表1**  在TPipe-TQue框架编程范式下，AIV中不同流水线的同步情况 <a id="table-aiv-tpipe-sync"></a>

    | 源流水\目的流水 | PIPE_S | PIPE_V | PIPE_MTE2 | PIPE_MTE3 |
    | --- | --- | --- | --- | --- |
    | PIPE_S | - | - | - | - |
    | PIPE_V | - | - | 支持 | 支持 |
    | PIPE_MTE2 | - | 支持 | - | 支持 |
    | PIPE_MTE3 | - | 支持 | 支持 | - |

    **表2**  在TPipe-TQue框架编程范式下，AIC中不同流水线的同步情况 <a id="table-aic-tpipe-sync"></a>

    | 源流水\目的流水 | PIPE_S | PIPE_M | PIPE_MTE1 | PIPE_MTE2 | PIPE_MTE3 | PIPE_FIX |
    | --- | --- | --- | --- | --- | --- | --- |
    | PIPE_S | - | - | - | - | - | - |
    | PIPE_M | - | - | 支持 | 支持 | - | 支持 |
    | PIPE_MTE1 | - | 支持 | - | 支持 | 支持 | 支持 |
    | PIPE_MTE2 | - | 支持 | 支持 | - | 支持 | - |
    | PIPE_MTE3 | - | - | 支持 | 支持 | - | - |
    | PIPE_FIX | - | 支持 | 支持 | - | - | - |

### 反例<a id="tpipe-tque-auto-sync-counterexamples"></a>

以下反例展示了因未满足[使用约束](#tpipe-tque-auto-sync-constraints)而无法自动插入同步的典型场景。

- 反例1：未使用TPipe-TQue框架编程范式。例如，将[基于TPipe-TQue框架编程范式的Add向量加法的样例](../../../../../../../examples/01_simd_cpp_api/00_introduction/01_add/add_tpipe_tque)改写为基于[静态Tensor编程范式](../../../../../../zh/guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md)时，静态Tensor编程范式无法自动插入`MTE2_V`，并最终导致样例执行失败。

    ```cpp
    AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
    AscendC::LocalTensor<float> xLocal = ubAllocator.Alloc<float, blockLength>();
    AscendC::LocalTensor<float> yLocal = ubAllocator.Alloc<float, blockLength>();
    AscendC::LocalTensor<float> zLocal = ubAllocator.Alloc<float, blockLength>();

    AscendC::DataCopy(xLocal, xGm, blockLength);
    AscendC::DataCopy(yLocal, yGm, blockLength);

    AscendC::Add(zLocal, xLocal, yLocal, blockLength);
    ```

- 反例2：未正确使用EnQue/DeQue、AllocTensor/FreeTensor接口（由TPipe-TQue框架编程范式提供）。例如，在[基于TPipe-TQue框架编程范式的Add向量加法的样例](../../../../../../../examples/01_simd_cpp_api/00_introduction/01_add/add_tpipe_tque)中，如果缺失了`xLocal = inQueueX.DeQue<float>()`会导致TPipe-TQue框架编程范式无法自动插入`MTE2_V`，并最终导致样例执行失败。

    ```cpp
    // 使用DataCopy将输入从GM搬运到Unified Buffer（UB），并通过EnQue将LocalTensor入队，供后续计算阶段DeQue取用。
    AscendC::LocalTensor<float> xLocal = inQueueX.AllocTensor<float>();
    AscendC::LocalTensor<float> yLocal = inQueueY.AllocTensor<float>();
    AscendC::DataCopy(xLocal, xGm, blockLength);
    AscendC::DataCopy(yLocal, yGm, blockLength);
    inQueueX.EnQue(xLocal);
    inQueueY.EnQue(yLocal);

    // DeQue取出输入张量，在UB内执行Add，并将结果EnQue到输出队列，供后续写回GM。
    // xLocal未从inQueueX中执行DeQue操作，导致TPipe-TQue框架编程范式无法自动插入MTE2_V。
    yLocal = inQueueY.DeQue<float>();
    AscendC::LocalTensor<float> zLocal = outQueueZ.AllocTensor<float>();
    AscendC::Add(zLocal, xLocal, yLocal, blockLength);
    ```

## 开启cce-auto-sync编译选项自动同步（毕昇编译器自动插入）<a id="cce-compiler-auto-sync"></a>

### 使用约束<a id="cce-compiler-auto-sync-constraints"></a>

开启cce-auto-sync编译选项的自动同步功能需满足以下前提条件方可生效：

- 异构编译直调（<<<>>>调用）、核函数（Kernel）直调算子工程以及自定义算子开发工程已默认设置`--cce-auto-sync=on`，如果开发者手动设置`--cce-auto-sync=off`，毕昇编译器无法自动插入同步。
- 属于PIPE\_V、PIPE\_MTE2、PIPE\_MTE3的接口必须操作基础Tensor（包含[LocalTensor](../../data_structures/LocalTensor/LocalTensor.md)和[GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor.md)）。
- 属于PIPE\_S的接口必须在以下接口集合内：
    - Tensor标量读写：[LocalTensor::GetValue](../../data_structures/LocalTensor/GetValue.md)、[LocalTensor::SetValue](../../data_structures/LocalTensor/SetValue.md)、[LocalTensor::operator()](../../data_structures/LocalTensor/operator_call.md)、[GlobalTensor::GetValue](../../data_structures/GlobalTensor/GetValue.md)、[GlobalTensor::operator()](../../data_structures/GlobalTensor/operator_call.md)。
    - Vector归约辅助：[GetReduceRepeatMaxMinSpr](../../memory_vector_compute/reduction_compute_aux_config/GetReduceRepeatMaxMinSpr_ISASI.md)、[GetReduceRepeatSumSpr](../../memory_vector_compute/reduction_compute_aux_config/GetReduceRepeatSumSpr_ISASI.md)。
    - 排序组合：[GetMrgSortResult](../../memory_vector_compute/sort_and_merge_ISASI/GetMrgSortResult.md)。

    不在以上集合中的PIPE\_S接口，毕昇编译器无法自动插入同步：
    <!-- npu="950" id6 -->
    例如，针对Ascend 950PR/Ascend 950DT，标量原子操作接口均不在以上接口集合内（包括[AtomicAdd](../../atomic_operations/AtomicAdd.md)、[AtomicCas](../../atomic_operations/AtomicCas.md)、[AtomicExch](../../atomic_operations/AtomicExch.md)、[AtomicMax](../../atomic_operations/AtomicMax.md)、[AtomicMin](../../atomic_operations/AtomicMin.md)），因此，当这些接口与属于PIPE\_MTE2、PIPE\_MTE3的接口在读写GM时存在数据依赖，毕昇编译器无法自动插入同步，开发者需根据实际情况手动插入同步。
    <!-- end id6 -->

### 支持的同步类型

- 单流水同步

    <!-- npu="A3,910b" id7 -->
    - 针对[NPU架构2201](../../../../../../zh/guide/programming_guide/language_extension/simd_builtin_keywords.md)，PIPE\_V之间由毕昇编译器自动完成同步插入。
    <!-- end id7 -->

    <!-- npu="950" id8 -->
    - 针对[NPU架构3510](../../../../../../zh/guide/programming_guide/language_extension/simd_builtin_keywords.md)，PIPE\_V之间的同步由硬件保证。
    <!-- end id8 -->

    - PIPE\_MTE2/PIPE\_MTE3在搬运地址有重叠的情况下需要开发者插入同步（具体示例请参考[PipeBarrier约束说明](../intra_core_sync/PipeBarrier_ISASI.md)）。

- 多流水同步
    - Vector计算单元中，毕昇编译器支持自动插入PIPE\_V与PIPE\_S、PIPE\_MTE2与PIPE\_S、PIPE\_MTE3与PIPE\_S之间的多流水同步（双向）。
    - Cube计算单元中，毕昇编译器不支持自动插入任何类型的同步。

<!-- npu="A3,910b" id9 -->
以[NPU架构2201](../../../../../../zh/guide/programming_guide/language_extension/simd_builtin_keywords.md)为例，该硬件架构下AIV中不同流水线的自动同步支持情况如[表3](#table-aiv-cce-sync)所示。
<!-- end id9 -->

**表3**  开启cce-auto-sync编译选项，AIV中不同流水线的同步情况 <a id="table-aiv-cce-sync"></a>

| 源流水\目的流水 | PIPE_S | PIPE_V | PIPE_MTE2 | PIPE_MTE3 |
| --- | --- | --- | --- | --- |
| PIPE_S | - | 支持 | 支持 | 支持 |
| PIPE_V | 支持 | 支持 | - | - |
| PIPE_MTE2 | 支持 | - | - | - |
| PIPE_MTE3 | 支持 | - | - | - |

### 反例<a id="cce-compiler-auto-sync-counterexamples"></a>

以下反例展示了因未满足[使用约束](#cce-compiler-auto-sync-constraints)而无法自动插入同步的典型场景。

[基于指针的C语言编程的Add样例](../../../../../../../examples/02_simd_c_api/00_introduction/01_add/c_api_async_add)中，操作x_local的第0个元素时直接通过指针访问。此时，毕昇编译器无法在PIPE_MTE2（asc_copy_gm2ub）与PIPE_S之间自动插入同步。

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

## 自动同步日志功能

开发者在使用自动同步功能时，往往不确定TPipe-TQue框架编程范式或毕昇编译器是否已插入同步指令、插入了哪些类型的同步以及插入的具体位置。为了帮助开发者验证自动同步的插入情况，本节介绍两种查看自动同步信息的方式：NPU仿真模式的指令日志功能和毕昇编译器的自动同步日志功能。

### NPU仿真模式指令日志

TPipe-TQue框架编程范式插入的同步并没有单独的日志功能，但是开启NPU仿真模式后（设置编译选项`-DCMAKE_ASC_RUN_MODE=sim`，参考[编译运行](../../../../../../../examples/01_simd_cpp_api/00_introduction/01_add/add_tpipe_tque/README.md#编译运行)），可以通过指令日志**同时查看**TPipe-TQue框架编程范式和毕昇编译器自动插入的同步指令信息。

基于TPipe-TQue框架编程范式的Add向量加法的样例中，开启NPU仿真模式后，在build目录下查看vector核0的指令日志（`core0.veccore0.instr_log.dump`）片段如下：

<!-- npu="A3,910b" id10 -->
> [!NOTE]说明
> 以下日志片段及对应分析仅针对[NPU架构2201](../../../../../../zh/guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)成立。

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

以上日志片段大致与Add向量加法样例的如下代码片段对应，日志中`MOV_OUT_TO_UB`对应`DataCopy`（源地址GM、目的地址UB）、`SET_FLAG`对应`SetFlag`、`WAIT_FLAG`对应`WaitFlag`、`VADD`对应`Add`，从日志中序号为00002009、00002010和00002011的条目可以看到，TPipe-TQue框架编程范式在PIPE_MTE2和PIPE_V之间自动插入了`SetFlag`和`WaitFlag`指令。
<!-- end id10 -->

```cpp
AscendC::LocalTensor<float> xLocal = inQueueX.AllocTensor<float>();
AscendC::LocalTensor<float> yLocal = inQueueY.AllocTensor<float>();
AscendC::DataCopy(xLocal, xGm, blockLength);
AscendC::DataCopy(yLocal, yGm, blockLength);
inQueueX.EnQue(xLocal);
inQueueY.EnQue(yLocal);

// DeQue取出输入张量，在UB内执行Add，并将结果EnQue到输出队列，供后续写回GM。
xLocal = inQueueX.DeQue<float>();
yLocal = inQueueY.DeQue<float>();
AscendC::LocalTensor<float> zLocal = outQueueZ.AllocTensor<float>();
AscendC::Add(zLocal, xLocal, yLocal, blockLength);
```

### 毕昇编译器自动同步日志

毕昇编译器提供`--cce-auto-sync-log=<file>`编译选项可以输出同步插入信息到`<file>`文件中，帮助开发者显式地识别毕昇编译器在算子文件中插入的同步指令信息。

> [!CAUTION]注意    
> 获取毕昇编译器自动同步日志，还需要设置[`-g`编译选项](../../../../../../zh/guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#常用的编译选项)，用于获取算子代码文件行号。

根据开发场景不同，添加该编译选项的方式如下：

- 直接使用毕昇编译器的场景，可以直接在编译命令中添加该编译选项。
- 使用Ascend C kernel直调算子工程，可以通过`ascendc_compile_options`添加该编译选项。
- 使用Ascend C自定义算子开发工程，可以通过`add_ops_compile_options`添加该编译选项。

如下的代码文件sync_log_test.h：

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

开启自动同步后，同步指令的插入位置如下：

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

开启自动同步debug日志功能后，输出日志如下：

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

其中，line before表示紧接着当前行前面插入的同步指令。
