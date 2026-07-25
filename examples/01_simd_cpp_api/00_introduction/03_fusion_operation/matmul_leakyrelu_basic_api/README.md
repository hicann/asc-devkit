# Matmul和LeakyRelu融合计算样例

## 算子概述

本样例基于静态Tensor编程模式（编译期确定张量形状和内存布局的编程方式）实现Matmul和LeakyRelu融合计算，展示Cube单元（矩阵计算单元，执行Matmul等矩阵运算）和Vector单元（向量计算单元，执行逐元素运算）协同计算的编程模式。Cube核完成矩阵乘计算，Vector核完成LeakyRelu激活计算。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── matmul_leakyrelu_basic_api
│   ├── scripts
│   │   ├── gen_data.py                 // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py            // 真值对比文件
│   ├── CMakeLists.txt                  // 编译工程文件
│   ├── data_utils.h                    // 数据读入写出函数
│   ├── matmul_leakyrelu_basic_api.asc  // Ascend C样例实现 & 调用样例
│   └── README.md                       // 样例说明文档
```

## 算子描述

- 样例功能：
  实现Matmul和LeakyRelu融合计算，计算公式如下：

  Matmul计算：
  $$
  C_{ij} = \sum_{k} A_{ik} \times B_{kj}
  $$

  LeakyRelu计算：
  $$
  C_{ij} = \begin{cases}
  C_{ij} & \text{if } C_{ij} \geq 0 \\
  C_{ij} \times 0.001 & \text{if } C_{ij} < 0
  \end{cases}
  $$

  其中，A为左矩阵，形状为[M, K]；B为右矩阵，形状为[K, N]；C为输出矩阵，形状为[M, N]。

- 样例规格：
  本样例参数M = 512, K = 128, N = 128，调用2个Cube核和4个Vector核完成计算，输入规格如下表所示：

  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Matmul+LeakyRelu融合</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">A（左矩阵）</td><td align="center">[512, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">B（右矩阵）</td><td align="center">[128, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">C</td><td align="center">[512, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">mmad_vec_custom</td></tr>
  </table>

  **分核逻辑**：

  输出矩阵C按M方向（shape第一维，行方向）分为2个分块，N方向（shape第二维，列方向）不分块，使用2个Cube核完成Matmul计算；每个Cube核对应2个Vector核，因此使用4个Vector核完成LeakyRelu计算。每个Cube核产生一个baseM×baseN结果块，对应2个Vector核各处理baseM/2×baseN的数据。

  **M/N/K参数图解**：

  在矩阵乘 C = A × B 中，M/N/K分别对应矩阵的不同维度，分核偏移也围绕这些维度展开：

  ```
           K                N
       ┌─────────┐      ┌──────────┐
       │         │      │          │
     M │    A    │  × K │    B     │  =  C (M × N)
       │         │      │          │
       └─────────┘      └──────────┘
  ```

  - M方向分块数：M / singleCoreM = 512 / 256 = 2
  - N方向分块数：N / singleCoreN = 128 / 128 = 1
  - 总Cube核数：2 × 1 = 2
  - 总Vector核数：2 × 2 = 4

- 算子实现：
  - **整体计算流程**：

    样例整体流程如下（展示Cube核和Vector核协同计算）：

      > **前置说明：Ascend AI Core内存层级与数据通路**
      >
      > Ascend AI Core内部有多级片上缓存，数据从外部DDR（Global Memory, GM）逐级搬运至计算单元：
      > - **GM（Global Memory）**：芯片外部DDR，存放输入/输出数据，容量最大但访问延迟最高
      > - **L1**：片上缓存（Cube侧主要存放A/B矩阵数据），由MTE2引擎从GM搬入
      > - **L0A/L0B**：Cube矩阵计算单元的输入缓存，由MTE1引擎从L1搬入
      > - **L0C**：Cube矩阵计算单元的输出缓存，存放Mmad计算结果
      > - **UB（Unified Buffer，AscendC中对应TPosition::VECCALC）**：Vector侧片上缓存，存放Vector计算所需的输入/输出数据
      >
      > 数据需在搬运过程中进行格式转换（ND→Nz/Zn），因为Cube计算单元要求特定的分形（Fractal）排布格式，而非ND排布。
      >
      > 同步机制说明：
      > - **核内同步**：[SetFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md)/[WaitFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md)用于同一核内不同流水线引擎之间的依赖同步，如MTE2搬运完成后通知MTE1可以开始搬运
      > - **核间同步**：[CrossCoreSetFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核间同步/CrossCoreSetFlag(ISASI).md)/[CrossCoreWaitFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核间同步/CrossCoreWaitFlag(ISASI).md)用于不同核之间的依赖同步，如Cube核完成计算后通知Vector核可以开始读取数据

      **1）数据流路径**

      Cube侧数据流：
      ```
      GM(A:ND,half) -> L1(A:Nz,half) -> L0A(A:Nz,half) -
                  │                  │                 │
                DataCopy            LoadData            │
                ND->Nz              Nz->Zz/Nz           │
                                                        │--->L0C(Nz,float) -> GM(C:ND,half)
                                                        │  │                  │
                                                        │ Mmad             Fixpipe
                                                        │ C=A×B       Nz->ND, float32->half
                                                        │
      GM(B:ND,half) -> L1(B:Nz,half) -> L0B(B:Zn,half) -
                  │                  │
                DataCopy            LoadData
                ND->Nz              Nz->Zn(转置)
      ```

      > **注意**：L0A分形在不同产品上有差异：
      > - Ascend 950PR/Ascend 950DT产品：L0A的分形为Nz
      > - Atlas A2/A3系列产品：L0A的分形为Zz

      核间同步与Vector侧数据流（1个Cube核的结果由2个Vector核处理）：
      ```
      Cube核:  Fixpipe -> GM(C:ND,half)
                          │
                          │ CrossCoreSetFlag / CrossCoreWaitFlag
                          ▼
      Vector核: GM(C:ND,half) -> UB(VECCALC,half) -> UB(VECCALC,half) -> GM(C:ND,half)
                            │                   │                   │
                        DataCopyPad          LeakyRelu          DataCopyPad
                  MTE2搬运（baseM/2×baseN）    VEC计算            MTE3写出
      ```

      **2）流程详解**：

      1. **Cube核计算阶段**：

          **GlobalTensor定义与分核偏移**：使用[GlobalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/GlobalTensor/GlobalTensor简介.md)（全局内存张量）访问芯片外部DDR中的输入/输出数据，并通过[GetBlockIdx](../../../../../docs/zh/api/SIMD-API/基础API/工具接口/系统资源与变量/GetBlockIdx.md)（获取当前核的逻辑编号）计算分核偏移：

          ```cpp
          class KernelMatmul {
          public:
              __aicore__ inline void Init(__gm__ uint8_t* xMatrix, __gm__ uint8_t* yMatrix, __gm__ uint8_t* zMatrix)
              {
                  // 设置GlobalTensor的起始地址，指向GM中对应的输入/输出矩阵
                  aGM.SetGlobalBuffer((__gm__ half*)xMatrix);
                  bGM.SetGlobalBuffer((__gm__ half*)yMatrix);
                  cGM.SetGlobalBuffer((__gm__ half*)zMatrix);

                  // Cube核按M方向分核，每个Cube核负责singleCoreM * N大小输出块
                  if ASCEND_IS_AIC {
                      aGM = aGM[AscendC::GetBlockIdx() * singleCoreM * K];  // 偏移到当前核负责的A矩阵行
                      cGM = cGM[AscendC::GetBlockIdx() * singleCoreM * N];  // 偏移到当前核负责的C矩阵行
                  }
                  // Vector核数是Cube核数的2倍，1个Cube核的结果由2个Vector核处理
                  // GetBlockIdx() / 2 将Vector核编号映射回对应的Cube核输出块起始地址
                  if ASCEND_IS_AIV {
                      cGM = cGM[AscendC::GetBlockIdx() / 2 * singleCoreM * N];
                  }
              }
          // ...
          private:
              AscendC::GlobalTensor<half> aGM;  // 左矩阵A，[M, K]
              AscendC::GlobalTensor<half> bGM;  // 右矩阵B，[K, N]
              AscendC::GlobalTensor<half> cGM;  // 输出矩阵C，[M, N]
          };
          ```

          **内存搬运与计算流程**：

          - **LocalTensor创建**：使用[LocalMemAllocator](../../../../../docs/zh/api/SIMD-API/基础API/资源管理/LocalMemAllocator/LocalMemAllocator简介.md)（片上内存分配器，按申请顺序自动分配，避免手动维护地址偏移）为各片上缓存创建[LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md)（片上内存张量）。其中A、B矩阵在L1中的临时空间由同一个L1 allocator按申请顺序分配，避免手动维护L1地址偏移
          - **GM → L1**：使用[DataCopy](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（GMToL1随路转换-ND2NZ搬运）.md)将A、B矩阵从GM搬运到L1，完成ND到Nz格式转换（Cube计算单元要求Nz分形排布，因此需在搬运时将ND格式转为Nz格式）
          - **L1 → L0A/L0B**：使用[LoadData](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md)将数据搬运到L0A和L0B，B矩阵需要转置（Nz→Zn）
          - **L0A/L0B → L0C**：使用[Mmad](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md)（矩阵乘累加指令）执行矩阵乘加，累加K轴方向的所有数据块
          - **L0C → GM**：使用[Fixpipe](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)将结果搬出到GM，完成Nz到ND格式转换和float32到half类型转换

          代码实例如下：

          ```cpp
          __aicore__ inline void Process()
          {
              if ASCEND_IS_AIC {
                  // ==================== Local Tensor 分配 ====================
                  // 使用LocalMemAllocator按申请顺序自动分配片上内存，避免手动维护地址偏移
                  AscendC::LocalMemAllocator<AscendC::Hardware::L1> l1Allocator;
                  AscendC::LocalMemAllocator<AscendC::Hardware::L0A> l0aAllocator;
                  AscendC::LocalMemAllocator<AscendC::Hardware::L0B> l0bAllocator;
                  AscendC::LocalMemAllocator<AscendC::Hardware::L0C> l0cAllocator;
                  // L1: A/B矩阵的临时存储; L0A/L0B: Cube计算单元输入; L0C: Cube计算单元输出
                  // Alloc的TPosition模板参数可缺省，Alloc函数会根据allocator对应的物理位置给出默认值
                  AscendC::LocalTensor<half> a1Local = l1Allocator.Alloc<half>(baseM * baseK);
                  AscendC::LocalTensor<half> b1Local = l1Allocator.Alloc<half>(baseK * baseN);
                  AscendC::LocalTensor<half> a2Local = l0aAllocator.Alloc<half>(baseM * baseK);
                  AscendC::LocalTensor<half> b2Local = l0bAllocator.Alloc<half>(baseK * baseN);
                  AscendC::LocalTensor<float> cLocal = l0cAllocator.Alloc<float>(baseM * baseN);

                  // ==================== GM → L1: DataCopy + ND→Nz格式转换 ====================
                  // DataCopy搬运时同时完成ND到Nz的格式转换（Cube计算单元要求Nz分形排布）
                  AscendC::DataCopy(a1Local, aGM, AscendC::Nd2NzParams{1, baseM, baseK, 0, K, baseM, 1, 0});
                  AscendC::DataCopy(b1Local, bGM, AscendC::Nd2NzParams{1, baseK, baseN, 0, N, baseK, 1, 0});
                  // 核内同步：MTE2搬运完成后通知MTE1可以开始搬运
                  AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE1>(EVENT_ID0);
                  AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE1>(EVENT_ID0);

                  // ==================== L1 → L0A/L0B: LoadData ====================
                  // ⚠️ 架构约束：不同产品的L0A排布格式不同，LoadData参数需对应调整
                  // A2/A3(2201)为Zz排布需逐块搬运，950PR(3510)为Nz排布可一次完成
                  // A2/A3架构 (dav-2201): L0A排布为Zz格式，需逐块搬运
          #if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
                  for (int i = 0; i < baseM / CUBE_BLOCK; ++i) {
                      AscendC::LoadData(a2Local[i * baseK * CUBE_BLOCK], a1Local[i * 512 / sizeof(half)],
                          AscendC::LoadData2DParams{0, baseK / CUBE_BLOCK, baseM / CUBE_BLOCK, 0, 0, false, 0});
                  }
                  // B矩阵Nz→Zn转置搬运
                  for (int i = 0; i < baseK / CUBE_BLOCK; ++i) {
                      AscendC::LoadData(b2Local[i * baseN * CUBE_BLOCK], b1Local[i * 512 / sizeof(half)],
                          AscendC::LoadData2DParams{0, baseN / CUBE_BLOCK, baseK / CUBE_BLOCK, 0, 0, true, 0});
                  }
                  // Ascend 950PR架构 (dav-3510): L0A排布为Nz格式，一次搬运完成
          #elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
                  AscendC::LoadData(a2Local, a1Local,
                      AscendC::LoadData2DParamsV2{0, 0, baseM / CUBE_BLOCK, baseK / CUBE_BLOCK,
                          baseM / CUBE_BLOCK, baseM / CUBE_BLOCK, false, 0});
                  AscendC::LoadData(b2Local, b1Local,
                      AscendC::LoadData2DParamsV2{0, 0, baseK / CUBE_BLOCK, baseN / CUBE_BLOCK,
                          baseK / CUBE_BLOCK, baseN / CUBE_BLOCK, true, 0});
          #endif
                  // 核内同步：MTE1搬运完成后通知M流水可以开始Mmad计算
                  AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID0);
                  AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID0);

                  // ==================== L0A/L0B → L0C: Mmad矩阵乘累加 ====================
                  // cmatrixInitVal=true: 首次计算时初始化L0C累加器为0
                  AscendC::Mmad(cLocal, a2Local, b2Local,
                      AscendC::MmadParams{baseM, baseN, baseK, 0, false, true});

                  // ==================== L0C → GM: Fixpipe搬出 + Nz→ND + fp32→fp16 ====================
                  // 核内同步：M流水完成后通知FIX流水可以开始Fixpipe
                  AscendC::SetFlag<AscendC::HardEvent::M_FIX>(EVENT_ID0);
                  AscendC::WaitFlag<AscendC::HardEvent::M_FIX>(EVENT_ID0);
                  // Fixpipe将L0C结果搬出到GM，同时完成NZ→ND格式转换和float32→half精度转换
                  AscendC::Fixpipe(cGM, cLocal,
                      AscendC::FixpipeParamsV220{baseN, baseM, baseM, N, false, QuantMode_t::F322F16, 0, 1, 0, 0, 0});

                  // ==================== 核间同步：通知Vector核数据就绪 ====================
                  AscendC::CrossCoreSetFlag<0x2, PIPE_FIX>(0);
              }
              // ... Vector核部分见下方
          }
          ```

      2. **Vector核计算阶段**：
          - **核间同步**：Vector核通过[CrossCoreWaitFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核间同步/CrossCoreWaitFlag(ISASI).md)（核间同步标志等待，阻塞直到标志被设置）等待Cube核完成Fixpipe写回，确保Matmul计算完成后才开始LeakyRelu
          - **LocalTensor创建**：使用UB allocator创建`VECCALC`位置的[LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md)，用于承载当前Vector核处理的半块结果
          - **GM → UB**：使用[DataCopyPad](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopyPad（GMToUB非对齐数据搬运）.md)（带Padding的GM与UB之间数据搬运）将Matmul结果搬运到UB，每个Vector核处理baseM/2×baseN的数据
          - **UB计算**：使用[LeakyRelu](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/基础算术/LeakyRelu.md)（LeakyReLU激活函数，负值部分乘以negativeSlope）执行激活计算，负值部分乘以0.001
          - **UB → GM**：使用[DataCopyPad](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopyPad（UBToGM非对齐数据搬运）.md)将结果写回GM，完成融合计算

          代码实例如下：

          ```cpp
              if ASCEND_IS_AIV {
                  // ==================== UB Local Tensor 分配 ====================
                  AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
                  // 每个Vector核处理baseM/2 × baseN个元素（1个Cube核的结果由2个Vector核平分）
                  AscendC::LocalTensor<half> vecLocal = ubAllocator.Alloc<half>(baseM / 2 * baseN);

                  // ==================== 核间同步：等待Cube核Fixpipe完成 ====================
                  AscendC::CrossCoreWaitFlag(0);

                  // ==================== GM → UB: DataCopyPad读取Matmul结果 ====================
                  // 计算当前Vector核在GM中的起始偏移
                  // GetBlockIdx() % 2 区分同一逻辑核内的2个AIV，处理不同半块数据
                  uint32_t gmOffset = AscendC::GetBlockIdx() % 2 * (baseM / 2 * N);
                  uint32_t blockLen = baseN * sizeof(half);
                  uint32_t srcStride = (N - baseN) * sizeof(half);  // GM中相邻行间的跳转步长
                  AscendC::DataCopyPad<half>(
                      vecLocal, cGM[gmOffset],
                      AscendC::DataCopyExtParams{static_cast<uint16_t>(baseM / 2), blockLen, srcStride, 0, 0},
                      AscendC::DataCopyPadExtParams<half>{true, 0, 0, 0});
                  // 核内同步：MTE2搬运完成后通知V流水可以开始计算
                  AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
                  AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

                  // ==================== UB计算: LeakyRelu激活 ====================
                  // LeakyRelu(x) = x >= 0 ? x : x * 0.001
                  AscendC::LeakyRelu(vecLocal, vecLocal, (half)0.001, baseM / 2 * baseN);
                  // 核内同步：V流水完成后通知MTE3可以开始写回
                  AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
                  AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);

                  // ==================== UB → GM: DataCopyPad写回结果 ====================
                  AscendC::DataCopyPad<half>(cGM[gmOffset], vecLocal,
                      AscendC::DataCopyExtParams{static_cast<uint16_t>(baseM / 2), blockLen, 0, srcStride, 0});
              }
          }
          ```

  - **约束条件**：
    1. baseM/baseK/baseN满足16对齐
    2. M/N应能被singleCoreM/singleCoreN整除
    3. singleCoreM/singleCoreN应能被baseM/baseN整除，K应能被baseK整除，不支持非整切场景
    4. Vector核数是Cube核数的2倍

  - **调用实现**：
    核调用符`__mix__(1, 2)`实现Cube和Vector核的协同调用，其中参数`(1, 2)`表示每个逻辑核由1个AIC（Cube核）和2个AIV（Vector核）组成，Cube:Vector核数比例为1:2。

    ```cpp
    // __global__: 核函数声明修饰符
    // __mix__(1,2): 混合核函数，1个AIC(Cube) + 2个AIV(Vector)组成一个逻辑核
    __global__ __mix__(1, 2) void mmad_vec_custom(__gm__ uint8_t* xMatrix, __gm__ uint8_t* yMatrix, __gm__ uint8_t* zMatrix)
    {
        AscendC::InitSocState();  // 初始化硬件状态
        KernelMatmul<M, K, N, singleCoreM, singleCoreK, singleCoreN,
                     baseM, baseK, baseN> op;
        op.Init(xMatrix, yMatrix, zMatrix);   // GlobalTensor初始化与分核偏移
        op.Process();        // AIC侧执行Matmul，AIV侧执行LeakyRelu
        AscendC::PipeBarrier<PIPE_ALL>();  // 等待所有流水阶段完成
    }
    ```

- **接口参数说明**

  以下结构体均以花括号`{}`方式传参，各字段含义如下（字段顺序与API文档保持一致，实际struct声明中部分字段顺序可能不同）：

  **[AscendC::Nd2NzParams](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（GMToL1随路转换-ND2NZ搬运）.md)** — `DataCopy`接口使用，描述ND→Nz格式转换参数：
  ```cpp
  struct Nd2NzParams {
      int32_t  ndNum;              // 传输ND矩阵的数目，[0, 4095]
      uint16_t nValue;             // ND矩阵的行数，[0, 16384]
      int32_t  dValue;             // ND矩阵的列数，[0, 65535]
      int32_t  srcNdMatrixStride;  // 相邻ND矩阵起始地址偏移，单位：元素，[0, 65535]
      int32_t  srcDValue;          // 同一ND矩阵相邻行偏移，单位：元素，[1, 65535]
      uint16_t dstNzC0Stride;      // 目的Nz中同源行转换后多行相邻偏移，单位：C0_SIZE(32B)，[1, 16384]
      uint16_t dstNzNStride;       // 目的Nz中Z型矩阵相邻行偏移，单位：C0_SIZE(32B)，[1, 16384]
      int32_t  dstNzMatrixStride;  // 目的Nz中相邻Nz矩阵起始地址偏移，单位：元素，[1, 65535]
  };
  ```
  例如搬运A矩阵时`{1, baseM, baseK, 0, K, baseM, 1, 0}`，将baseM×baseK的ND数据转为Nz格式。

  **[AscendC::LoadData2DParams](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md)** — `LoadData`接口使用，描述Atlas A2 训练系列产品/Atlas A2 推理系列产品、Atlas A3 训练系列产品/Atlas A3 推理系列产品中A矩阵L1到L0A和B矩阵L1到L0B的数据搬运参数：
  ```cpp
  struct LoadData2DParams {
      int32_t startIndex;   // 分形矩阵ID（0为第1个），单位：512B，[0, 65535]
      int32_t repeatTimes;  // 迭代次数，每个迭代处理512B，[1, 255]
      int32_t srcStride;    // 相邻迭代源分形起始地址间隔，单位：512B，[0, 65535]
      int32_t sid;          // 预留，配置为0
      int32_t dstGap;       // 目的端相邻迭代分形间隔，单位：512B，[0, 65535]
      bool    ifTranspose;  // 是否转置每个分形，默认false
      bool    addrMode;     // 地址更新方式，false=递增，true=递减，默认false
  };
  ```
  例如：Atlas A2 训练系列产品/Atlas A2 推理系列产品、Atlas A3 训练系列产品/Atlas A3 推理系列产品中，L0A上的排布格式为Zz，搬运A矩阵时`{0, baseK / CUBE_BLOCK, baseM / CUBE_BLOCK, 0, 0, false, 0}`；<br>
  搬运B矩阵时`ifTranspose=true`，完成Nz到Zn的转置搬运。

  **[AscendC::LoadData2DParamsV2](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D_V2.md)** — `LoadData`接口使用，描述Ascend 950PR/Ascend 950DT产品中A矩阵L1到L0A和B矩阵L1到L0B的数据搬运参数：
  ```cpp
  struct LoadData2DParamsV2 {
      uint32_t mStartPosition;  // M方向起始位置，单位：512B
      uint32_t kStartPosition;  // K方向起始位置，单位：512B
      uint16_t mStep;           // M方向搬运分形数
      uint16_t kStep;           // K方向搬运分形数
      int32_t  srcStride;       // 源端相邻K方向分形间隔，单位：512B
      uint16_t dstStride;       // 目的端相邻K方向分形间隔，单位：512B
      bool     ifTranspose;     // 是否转置每个分形，默认false
      uint8_t  sid;             // 预留，配置为0
  };
  ```
  Ascend 950PR/Ascend 950DT产品中，L0A上的排布格式为Nz，搬运A矩阵时使用`{0, 0, baseM / CUBE_BLOCK, baseK / CUBE_BLOCK, baseM / CUBE_BLOCK, baseM / CUBE_BLOCK, false, 0}`，一次完成A矩阵Nz到Nz搬运；搬运B矩阵时使用`{0, 0, baseK / CUBE_BLOCK, baseN / CUBE_BLOCK, baseK / CUBE_BLOCK, baseN / CUBE_BLOCK, true, 0}`，一次完成B矩阵Nz到Zn搬运。

  **[AscendC::MmadParams](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md)** — `Mmad`接口使用，描述矩阵乘参数：
  ```cpp
  struct MmadParams {
      uint16_t m;               // 左矩阵Height（M维），[0, 4095]
      uint16_t n;               // 右矩阵Width（N维），[0, 4095]
      uint16_t k;               // 左矩阵Width/右矩阵Height（K维），[0, 4095]
      uint16_t unitFlag;        // Mmad与Fixpipe细粒度并行控制，默认0
      bool     cmatrixSource;   // C矩阵初始值来源，false=CO1，true=C2，默认false
      bool     cmatrixInitVal;  // C矩阵初始值是否为0，默认true
  };
  ```
  例如`{baseM, baseN, baseK, 0, false, true}`，计算baseM×baseN输出块并在K方向累加baseK长度。

  **[AscendC::FixpipeParamsV220](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)** — `Fixpipe`接口使用，描述L0C到GM的数据搬运和精度转换参数：
  ```cpp
  struct FixpipeParamsV220 {
      int32_t     nSize;        // 源Nz矩阵N方向大小，[1, 4095]
      uint16_t    mSize;        // 源Nz矩阵M方向大小（Nz2ND时[1, 8192]）
      uint16_t    srcStride;    // 源Nz相邻Z排布起始偏移，单位：C0_SIZE，[0, 65535]
      int32_t     dstStride;    // Nz2ND时目的ND矩阵每行元素数，单位：element
      bool        reluEn;       // 是否使能ReLU
      QuantMode_t quantPre;     // 量化模式，F322F16表示float→half
      uint64_t    deqScalar;    // scalar量化参数，单个scale值
      int32_t     ndNum;        // 源Nz矩阵数目，[1, 65535]
      int32_t     srcNdStride;  // 不同Nz矩阵起始地址间隔，单位：16×C0_SIZE，[1, 512]
      int32_t     dstNdStride;  // 目的相邻ND矩阵偏移，单位：element，[1, 65535]
      int32_t     unitFlag;     // Mmad与Fixpipe并行控制
  };
  ```
  例如`{baseN, baseM, baseM, N, false, F322F16, 0, 1, 0, 0, 0}`，将L0C中的baseM×baseN float32结果转为half并写回GM。

  **[AscendC::DataCopyExtParams](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopyPad（GMToUB非对齐数据搬运）.md)** — `DataCopyPad`接口使用，描述GM与UB之间按块搬运的参数：
  ```cpp
  struct DataCopyExtParams {
      uint16_t blockCount;  // 连续传输数据块个数
      uint32_t blockLen;    // 每个数据块长度，单位：Byte
      uint32_t srcStride;   // 源端相邻数据块间隔，单位：Byte
      uint32_t dstStride;   // 目的端相邻数据块间隔，单位：Byte
      uint32_t rsv;         // 预留字段，配置为0
  };
  ```
  例如GM搬运到UB时`{static_cast<uint16_t>(baseM / 2), blockLen, srcStride, 0, 0}`，每个Vector核读取baseM/2行结果。

  **[AscendC::DataCopyPadExtParams\<half\>](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopyPad（GMToUB非对齐数据搬运）.md)** — `DataCopyPad`接口使用，描述尾块补齐参数：
  ```cpp
  template <typename T>
  struct DataCopyPadExtParams {
      bool isPad;       // 是否开启补齐
      uint8_t leftPad;  // 左侧补齐长度
      uint8_t rightPad; // 右侧补齐长度
      T paddingValue;   // 补齐值
  };
  ```
  例如`{true, 0, 0, 0}`表示开启补齐能力，但左右两侧均不额外补齐。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  ./demo                                                                    # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　　　　　　　　　　　　　　　　　　 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```

## 实现流程解析

下表按时间顺序详细拆解Cube核和Vector核的每一步操作：

### Cube核流程

| 阶段 | 数据流动/行为 | 实现目的/原因 |
|:---|:---|:---|
| 初始化 | 使用[LocalMemAllocator](../../../../../docs/zh/api/SIMD-API/基础API/资源管理/LocalMemAllocator/LocalMemAllocator简介.md)分配L1、L0A/L0B、L0C等片上缓存的[LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md) | 按申请顺序自动分配片上内存，避免手动维护地址偏移 |
| GM → L1 | [DataCopy](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（GMToL1随路转换-ND2NZ搬运）.md)将A/B矩阵从GM搬入L1，同时完成ND→Nz格式转换 | Cube计算单元要求Nz分形排布格式，因此在搬运时必须将ND格式转为Nz格式，避免额外转换开销 |
| L1 → L0A/L0B | [LoadData](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md)将A矩阵从L1搬入L0A（Nz→Zz/Nz），B矩阵从L1搬入L0B（Nz→Zn转置） | B矩阵需要转置是因为Mmad指令要求B矩阵以Zn（转置Nz）格式输入 |
| L0A/L0B → L0C | [Mmad](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md)执行矩阵乘加，在K方向累加所有数据块 | 完成A×B的矩阵乘计算，K方向分块累加确保正确性 |
| 核内同步 | [SetFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md)/[WaitFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md)确保数据搬运完成后再开始下一步操作 | 避免LoadData读取尚未搬运完成的数据，避免Mmad读取尚未加载完成的数据 |
| L0C → GM | [Fixpipe](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)将L0C结果搬出到GM，同时完成Nz→ND格式转换和fp32→fp16精度转换 | 输出结果需回到GM供Vector核读取，格式需转为ND排布，精度需从fp32降为fp16以匹配输出要求 |
| 核间同步 | [CrossCoreSetFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核间同步/CrossCoreSetFlag(ISASI).md)通知Vector核数据就绪 | 确保Vector核不会在Fixpipe完成前开始读取GM数据 |

### Vector核流程

| 阶段 | 数据流动/行为 | 实现目的/原因 |
|:---|:---|:---|
| 核间同步 | [CrossCoreWaitFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核间同步/CrossCoreWaitFlag(ISASI).md)等待Cube核Fixpipe完成 | 阻塞直到Cube核通知数据就绪，确保读取到完整的Matmul结果 |
| 初始化 | 使用UB allocator分配VECCALC位置的[LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md) | 为GM→UB搬运和Vector计算分配UB缓冲区 |
| GM → UB | [DataCopyPad](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopyPad（GMToUB非对齐数据搬运）.md)将Matmul结果从GM搬入UB，每个Vector核读取baseM/2行、每行baseN个元素 | Vector核从GM读取Cube核写回的Matmul结果，每个Vector核处理半块数据 |
| UB计算 | [LeakyRelu](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/基础算术/LeakyRelu.md)执行激活计算，负值部分乘以0.001 | 对Matmul结果施加LeakyRelu激活函数，完成融合计算 |
| UB → GM | [DataCopyPad](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopyPad（UBToGM非对齐数据搬运）.md)将LeakyRelu结果写回GM | 将最终计算结果输出到GM供后续使用 |

## 可优化方向分析

| 可优化方向 | 当前实现的问题 | 预期优化收益 |
|:---|:---|:---|
| L1/L0双缓冲流水线 | Cube核内GM→L1→L0A/L0B→L0C→GM各阶段串行执行，搬运和计算无法重叠 | Ping-Pong双缓冲使MTE2搬运和MTE1/Mmad计算并行，大幅提升吞吐量 |
| Fixpipe与Mmad细粒度并行 | 当前Mmad和Fixpipe整块串行，Mmad计算时Fixpipe空闲，Fixpipe搬出时Mmad空闲 | 拆分为更小粒度的块，Mmad和Fixpipe交替执行，减少流水线气泡 |
| UB双缓冲 | Vector核内GM→UB→计算→GM串行执行 | UB Ping-Pong双缓冲使MTE2搬运和VEC计算并行 |
| Vector核VF融合 | 当前使用MemBase API，中间结果需写回UB | 使用RegBase + [asc_vf_call](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/VF调用/asc_vf_call.md)进行VF融合，中间计算在寄存器完成，减少UB读写次数 |
| 大包搬运 | 当前仅搬运单块baseM×baseN数据 | 增大singleCoreM/singleCoreN，减少搬运次数，提高带宽利用率 |
| GM中转优化 | Cube核结果需经GM中转给Vector核（L0C→GM→UB），两次搬运开销大 | Ascend 950PR支持Fixpipe直接写入UB（L0C→UB），省去GM中转开销 |

## 功能调试

### printf

该接口提供CPU域/NPU域调试场景下的格式化输出功能。

在算子kernel侧实现代码中需要输出日志信息的地方调用printf接口打印相关内容。

示例如下：

```cpp
AscendC::printf("matmul blockIdx=%d\n", AscendC::GetBlockIdx());
```

> **注意：** printf（PRINTF）接口打印功能会对算子实际运行的性能带来一定影响，通常在调测阶段使用。开发者可以按需通过设置ASCENDC_DUMP=0的方式关闭打印功能。

### DumpTensor

基于算子工程开发的算子，可以使用该接口Dump指定[LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md)的内容。同时支持打印自定义的附加信息（仅支持uint32\_t数据类型的信息），比如打印当前行号等。

在算子kernel侧实现代码中需要打印Tensor数据的地方调用DumpTensor接口打印相关内容。样例如下：

```cpp
// Vector核LeakyRelu结果
AscendC::LeakyRelu(zLocal, xLocal, ..., len);
AscendC::DumpTensor(zLocal, 1, 16);
```

> **注意：** [DumpTensor](../../../../../docs/zh/api/SIMD-API/基础API/调试接口/上板打印/DumpTensor.md)接口打印功能会对算子实际运行的性能带来一定影响，通常在调测阶段使用。开发者可以按需通过设置ASCENDC_DUMP=0来关闭打印功能。

## 性能调试

### msOpProf工具介绍
msOpProf工具是单算子性能分析工具。包含msopprof和msopprof simulator两种使用方式。该工具协助用户定位算子内存、算子代码以及算子指令的异常，实现全方位的算子调优。当前支持基于不同运行模式（上板或仿真）和不同文件形式（可执行文件或算子二进制.o文件）进行性能数据的采集和自动解析。

- 上板性能采集

    通过上板性能采集，可以直接测定算子昇腾AI处理器上的运行时间。该方式适合在板环境中快速定位算子性能问题。

    基于可执行文件demo通过msopprof执行算子调优：
    ```
    msopprof ./demo
    ```

    - 性能数据说明  
      命令完成后，会在默认目录下生成以“OPPROF_{timestamp}_XXX”命名的文件夹,性能数据文件夹结构示例如下：

      ```bash
      ├──dump                       # 原始的性能数据，用户无需关注
      ├──ArithmeticUtilization.csv  # cube/vector指令cycle占比
      ├──L2Cache.csv                # L2 Cache命中率，影响MTE2，建议合理规划数据搬运逻辑，增加命中率
      ├──Memory.csv                 # UB，L1和主存储器读写带宽速率
      ├──MemoryL0.csv               # L0A，L0B，和L0C读写带宽速率
      ├──MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
      ├──OpBasicInfo.csv            # 算子基础信息
      ├──PipeUtilization.csv        # 采集计算单元和搬运单元耗时和占比
      ├──ResourceConflictRatio.csv  # UB上的bank group、bank conflict和资源冲突率在所有指令中的占比
      └──visualize_data.bin         # MindStudio Insight呈现文件
      ```

查看具体的性能分析结果：

```bash
# 查看Task Duration 以及各项数据
cat ./OPPROF_*/PipeUtilization.csv
```
