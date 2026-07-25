# TPipe和TQue编程范式<a name="ZH-CN_TOPIC_0000002465655445"></a>

上一章阐述了TPipe‑TQue编程模型的底层设计原理：基于经典C/C++队列管道（Queue Pipeline）思想，将算子任务拆分为多级流水Stage，依靠TQue队列解决写后读数据依赖、依靠AllocTensor/FreeTensor解决读后写内存复用冲突，通过底层同步指令实现多单元异步并行的时序安全。

本章将上述原理固化为可直接编码的标准化编程范式。上一章回答**为什么这样设计**，本章则聚焦**如何按此设计编写代码**。TPipe‑TQue编程范式的本质，是队列管道思想在AI Core上的工程化落地——它将流水线拆分、队列同步、内存生命周期管理、异步并行调度等复杂底层逻辑，封装为一套统一、固定的开发流程，帮助开发者快速编写规范、可流水且高性能的算子核函数。

该范式定义了算子核函数实现的标准化固定流程。开发者严格遵循此范式，无需手动编写底层同步逻辑，也不必独自处理数据依赖与时序阻塞，即可快速搭建稳定、高效的算子代码框架。

从硬件执行形态看，AI Core内部的向量单元、矩阵单元和数据搬运单元天然支持异步并行与流水线执行。TPipe‑TQue编程范式完全贴合这一硬件特性，将硬件的多级流水行为抽象为软件层面统一的**数据搬入—计算—数据搬出**三段式流水线开发模型。

流水并行的核心逻辑可通过示意图直观理解：完整算子任务被拆分为多个有序子阶段，不同硬件单元负责不同工序；同一数据分片需在各阶段间串行流动，以保证数据依赖合法；不同数据分片则可在不同阶段同时处理，实现流水线吞吐叠加，与经典C/C++管道程序的执行逻辑完全一致。

**图1**  流水线并行示意图<a name="fig3986112422"></a>  
![](../../../../figures/流水线并行示意图.png "流水线并行示意图")

结合上一章的阐述可总结出：Ascend C流水线体系由两大核心模块共同支撑：
- **[TPipe](../../../../../api/SIMD-API/基础API/资源管理/TPipe/TPipe.md)** 资源管理器：统一管理系统内存资源和用于同步的事件等；
- **[TQue](../../../../../api/SIMD-API/基础API/资源管理/TQue/TQue.md)** 队列：完成Stage任务之间的同步和通信。

基于这套机制，Ascend C固化了矢量、矩阵、融合三类算子的标准化流水线编程范式。下文将结合硬件特性与工程实例逐一展开。

-   **[矢量编程范式](./TPipe-TQue框架编程范式.md#section116515238815)**
-   **[矩阵编程范式](./TPipe-TQue框架编程范式.md#section8213173433312)**
-   **[融合算子编程范式](./TPipe-TQue框架编程范式.md#section57815481855)**

## 矢量编程范式<a name="section116515238815"></a>

![](../../../../figures/矩阵编程逻辑位置示意图.png)

如上图所示，矢量编程范式将算子实现流程划分为三个基本任务：CopyIn、Compute和CopyOut。

- **CopyIn**负责数据搬入：将输入数据从Global Memory搬运到Local Memory（VECIN表示矢量计算搬入数据的存放位置），搬运完成后将数据入队；
- **Compute**负责矢量指令计算：从队列取出数据后，在Local Memory上执行计算，计算完成后将结果入队；
- **CopyOut**负责数据搬出：从队列取出计算结果，将其从Local Memory（VECOUT表示矢量计算搬出数据的存放位置）搬运回Global Memory。

上述VECIN、VECOUT均属于TPosition（逻辑位置）概念。Ascend C通过抽象的TPosition表达不同层级的物理存储，以替代直接的片上物理存储概念，实现硬件架构的隐藏。除VECIN和VECOUT外，矢量编程中还会用到VECCALC，通常在定义临时变量时使用。

从编程角度看，具体流程（如下方伪代码所示）及流程图如下：

![](../../../../figures/矢量编程Queue队列.png)

```
AscendC::TPipe pipe;                                // 创建全局资源管理器
AscendC::TQue<AscendC::TPosition::VecIn, 1> queIn;  // 创建CopyIn阶段队列
AscendC::TQue<AscendC::TPosition::VecOut, 1> queOut;// 创建CopyOut阶段队列
// Init阶段
pipe.InitBuffer(queIn, 2, 1024);                    // 开启DoubleBuffer，将待处理数据一分为二，实现流水并行
pipe.InitBuffer(queOut, 2, 1024);
for-loop {
    // CopyIn阶段
    {
        auto tensor = queIn.AllocTensor<half>();   // 从队列申请资源，长度1024
        AscendC::DataCopy(tensor, gm, 1024);       // 将数据从GM搬运至UB（VECIN）
        queIn.EnQue(tensor);
    }
    // Compute阶段
    {
        auto tensor = queIn.DeQue<half>();
        auto tensorOut = queOut.AllocTensor<half>();
        AscendC::Abs(tensorOut, tensor, 1024);    // 执行计算
        queIn.FreeTensor(tensor);
        queOut.EnQue(tensorOut);
    }
    // CopyOut阶段
    {
        auto tensor = queOut.DeQue<half>();
        AscendC::DataCopy(gmOut, tensor, 1024);   // 将数据从UB（VECOUT）搬运至GM
        queOut.FreeTensor(tensor);                // 释放资源
    }
}
```

任务间数据传递所使用的内存、事件等资源，统一由TPipe管理。如下内存管理示意图所示，TPipe通过[InitBuffer](../../../../../api/SIMD-API/基础API/资源管理/TPipe/InitBuffer.md)接口对外提供队列内存初始化功能，开发者可通过该接口为指定队列分配内存。

队列内存初始化完成后，当需要内存时通过[AllocTensor](../../../../../api/SIMD-API/基础API/资源管理/TQue/AllocTensor.md)为LocalTensor分配内存；当该LocalTensor完成相关计算不再使用时，再调用[FreeTensor](../../../../../api/SIMD-API/基础API/资源管理/TQue/FreeTensor.md)回收其内存。

**图2**  内存管理示意图<a name="fig375042942717"></a>  
![](../../../../figures/内存管理示意图.png "内存管理示意图")

编程过程中使用的临时变量内存同样由TPipe管理。临时变量可使用TBuf数据结构申请指定TPosition上的存储空间。通过TBuf申请的内存仅能参与计算，不可执行队列的入队、出队操作。具体接口使用说明请参见[TBuf](../../../../../api/SIMD-API/基础API/资源管理/TBuf/TBuf.md)。

遵循上述范式编程，即可实现单核数据的并行处理。待处理数据被切分为多个分片，每个并行任务依次完成对所有分片的处理。任务间的箭头表示数据依赖，例如CopyIn完成第一个分片的处理后，Compute才能开始处理该分片。

**图3**  流水任务示意图<a name="fig1556061818199"></a>  
![](../../../../figures/流水任务示意图.png "流水任务示意图")

上图对应的流水任务运行时序如下图所示。可以看出：对于同一数据分片，CopyIn、Compute、CopyOut之间存在依赖，必须串行执行；对于不同数据分片，同一时刻可有多个任务并行处理，从而以任务级并行提升整体性能。

**图4**  流水任务运行示意图<a name="fig123244111202"></a>  
![](../../../../figures/流水任务运行示意图.png "流水任务运行示意图")

## 矩阵编程范式<a name="section8213173433312"></a>

Cube计算的典型数据流图如下所示：

![](../../../../figures/矩阵编程逻辑位置示意图-2.png)

与矢量编程一致，矩阵编程同样使用逻辑位置（TPosition）表达数据流，主要使用的TPosition定义如下：

- A1：用于存放左矩阵的逻辑内存，物理对应AI Core的L1 Buffer。
- B1：用于存放右矩阵的逻辑内存，物理对应AI Core的L1 Buffer。
- C1：用于存放Bias（偏置）数据的逻辑内存，物理对应AI Core的L1 Buffer或Unified Buffer。
- A2：用于存放小块左矩阵（经分割适配L0A Buffer容量）的逻辑内存，物理对应AI Core的L0A Buffer。
- B2：用于存放小块右矩阵（经分割适配L0B Buffer容量）的逻辑内存，物理对应AI Core的L0B Buffer。
- C2：用于存放小块Bias数据（经分割适配BT Buffer容量）的逻辑内存，物理对应AI Core的BT Buffer或L0C Buffer。
- CO1：用于存放小块矩阵计算结果（如切分后的结果分块）的逻辑内存，物理对应AI Core的L0C Buffer。
- CO2：用于存放最终矩阵计算结果的逻辑内存，物理对应Global Memory或AI Core的Unified Buffer。
- VECIN：用于存放矢量计算输入数据的逻辑内存，物理对应AI Core的Unified Buffer。
- VECCALC：用于存放矢量计算临时变量的逻辑内存，物理对应AI Core的Unified Buffer。
- VECOUT：用于存放矢量计算输出数据的逻辑内存，物理对应AI Core的Unified Buffer。

Cube计算流程本质上也可归纳为CopyIn、Compute、CopyOut三个阶段。因其流程相对复杂，Matmul高阶API提供了高阶封装，有效简化了编程范式。

![](../../../../figures/矩阵编程逻辑位置示意图-3.png)

如图所示：CopyIn阶段对应[SetTensorA](../../../../../api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetTensorA.md)、[SetTensorB](../../../../../api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetTensorB.md)、[SetBias](../../../../../api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/SetBias.md)接口；Compute阶段对应[Iterate](../../../../../api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/Iterate.md)接口；CopyOut阶段对应[GetTensorC](../../../../../api/SIMD-API/高阶API/矩阵计算/Matmul-Kernel侧接口/GetTensorC.md)接口。示例如下：

```
// 创建Matmul对象，需传入A、B、C、Bias的参数类型信息
// 类型信息通过MatmulType定义，包括：内存逻辑位置、数据格式、数据类型
typedef MatmulType<TPosition::GM, CubeFormat::ND, half> aType;
typedef MatmulType<TPosition::GM, CubeFormat::ND, half> bType;
typedef MatmulType<TPosition::GM, CubeFormat::ND, float> cType;
typedef MatmulType<TPosition::GM, CubeFormat::ND, float> biasType;
Matmul<aType, bType, cType, biasType> mm;

REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling); // 初始化
// CopyIn阶段：完成从GM到Local Memory的搬运
mm.SetTensorA(gm_a);    // 设置左矩阵A
mm.SetTensorB(gm_b);    // 设置右矩阵B
mm.SetBias(gm_bias);    // 设置Bias
// Compute阶段：执行矩阵乘计算
while (mm.Iterate()) {
    // CopyOut阶段：完成从Local Memory到GM的搬运
    mm.GetTensorC(gm_c);
}
// 结束矩阵乘操作
mm.End();
```

## 融合算子编程范式<a name="section57815481855"></a>

支持Vector与Cube混合计算的算子称为融合算子。Ascend C提供**融合算子的编程范式**，方便开发者基于该范式表达融合算子的数据流，快速实现自定义融合算子。

**融合算子数据流**指其输入输出在各存储位置间的流向。以一个典型的Cube与Vector融合算子为例，逻辑位置间的数据流向如下图所示（为简化描述，未列出Bias）：

- Cube的输出可作为Vector的输入：CO2→VECIN
- Vector的输出也可作为Cube的输入：VECOUT→A1→A2、VECOUT→B1→B2

![](../../../../figures/矩阵编程逻辑位置示意图-4.png)

基于Matmul高阶API的融合算子编程范式，将上述数据流简化为以下步骤：

**图5**  融合算子编程范式<a name="fig321783243811"></a>  
![](../../../../figures/融合算子编程范式.png "融合算子编程范式")

1. 初始化MatMul对象，将输入数据从Global Memory搬运到Cube核上。
2. 执行MatMul内部计算。
3. 将MatMul计算结果搬运到Vector核上。
4. 执行Vector矢量计算。
5. 将输出结果搬运回Global Memory。

示例代码如下（伪代码）：

```
template<typename aType, typename bType, typename cType, typename biasType>
__aicore__ inline void MatmulLeakyKernel<aType, bType, cType, biasType>::Process()
{
    // 步骤1：初始化MatMul对象，将输入数据从Global Memory搬运到Cube核上
    uint32_t computeRound = 0;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), matmulObj);
    matmulObj.Init(&tiling);
    matmulObj.SetTensorA(aGlobal);
    matmulObj.SetTensorB(bGlobal);
    matmulObj.SetBias(biasGlobal);

    while (matmulObj.template Iterate<true>()) { // 步骤2：执行MatMul内部计算
        // 步骤3：将MatMul计算结果搬运到Vector核上
        reluOutLocal = reluOutQueue_.AllocTensor<cType>();
        matmulObj.template GetTensorC<true>(reluOutLocal, false, true);
        // 步骤4：执行Vector矢量计算
        AscendC::LeakyRelu(reluOutLocal, reluOutLocal, (cType)alpha, tiling.baseM * tiling.baseN);
        reluOutQueue_.EnQue(reluOutLocal);
        // 步骤5：将输出结果搬运回Global Memory
        reluOutQueue_.DeQue<cType>();
        ...
        AscendC::DataCopy(cGlobal[startOffset], reluOutLocal, copyParam);
        reluOutQueue_.FreeTensor(reluOutLocal);

        computeRound++;
    }
    matmulObj.End();
}
```

## 小结
本章内容完全承接上一章的队列管道底层原理，将理论中的Stage流水线拆分、数据依赖与同步等，落地为Ascend C统一的工程编程范式。无论是简单矢量算子、高密度矩阵算子，还是复杂融合算子，其本质都是经典C/C++队列管道模型在AI Core异构硬件上的不同形态实现：通过标准化三段式流水、队列数据同步、统一资源管控，屏蔽底层复杂的并行时序与内存冲突问题，让开发者以统一、简洁、可复用的流水线思维，高效开发高性能算子。
