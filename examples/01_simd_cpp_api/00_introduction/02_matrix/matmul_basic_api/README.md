# 基于静态Tensor编程实现Matmul基础API计算

## 概述

本样例基于静态Tensor编程范式实现多核矩阵乘计算。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── matmul_basic_api
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── matmul_basic_api.asc    // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：  
  本样例使用Ascend C基础API实现一个最基础的矩阵乘法（Matmul）[核函数](../../../../../docs/zh/guide/编程指南/编程模型/AI-Core-SIMD编程/核函数.md)。矩阵乘法的计算公式如下：
  $$
  C = A * B
  $$
  其中，A矩阵的形状为`[M, K]`，B矩阵的形状为`[K, N]`，输出C矩阵的形状为`[M, N]`。对输出矩阵C中的每一个元素`C[m, n]`，都会累加A矩阵第`m`行和B矩阵第`n`列在K轴上的乘积。在矩阵乘法中，**M方向**指矩阵C的行方向，**N方向**指矩阵C的列方向，**K方向**指矩阵C乘法的内维（累加维度）。

- 样例规格：  
  本样例参数`M = 256, N = 256, K = 64`，输入输出均为`half`类型、[`ND`](../../../../../docs/zh/guide/技术附录/概念原理和术语/神经网络和算子/数据排布格式.md)格式。样例启动2个核完成计算，每个核负责输出矩阵C在M轴方向的128行、N轴方向的全部256列：
  - 第0个核计算C矩阵的第`0~127`行。
  - 第1个核计算C矩阵的第`128~255`行。

  输入输出规格如下表所示：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Matmul</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">A</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">B</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">C</td><td align="center">[M, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">mmad_custom</td></tr>
  </table>

- 样例实现：
  - Kernel侧整体思路
    - `mmad_custom`是一个[`__global__`](../../../../../docs/zh/guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md) [`__cube__`](../../../../../docs/zh/guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)核函数，表示该函数运行在[AI Core](../../../../../docs/zh/guide/技术附录/概念原理和术语/术语表.md)的[Cube](../../../../../docs/zh/guide/技术附录/概念原理和术语/术语表.md)计算单元上，主要用于矩阵计算。
    - 样例使用[静态Tensor编程方式](../../../../../docs/zh/guide/编程指南/编程模型/AI-Core-SIMD编程/基于Tensor的CPP编程/静态Tensor编程.md)，通过[`LocalMemAllocator`](../../../../../docs/zh/api/SIMD-API/基础API/资源管理/LocalMemAllocator/LocalMemAllocator简介.md)创建[`LocalTensor`](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md)。
    - `CUBE_BLOCK = 16`表示half数据类型分形为`16 x 16`，代码中按`16 x 16`的分形为单位进行[`LoadData`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md)搬运。

  - Kernel侧详细流程
    - 创建[`GlobalTensor`](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/GlobalTensor/GlobalTensor简介.md)`<half>`对象`aGM`、`bGM`、`cGM`，分别表示[GM（Global Memory，全局内存）](../../../../../docs/zh/guide/编程指南/高级编程/硬件实现/基本架构.md)中的A、B、C矩阵。
    - 通过[AscendC::GetBlockIdx()](../../../../../docs/zh/api/SIMD-API/基础API/工具接口/系统资源与变量/GetBlockIdx.md)获取当前核号，并计算`mIterIdx`。本样例只沿M轴切分任务，因此每个核只需要处理A矩阵和C矩阵中属于自己的M轴分片。
    - 设置GM地址偏移：
      - `aGM`偏移`mIterIdx * singleCoreM * K`，使当前核读取自己负责的A矩阵行块。
      - `bGM`不偏移，因为每个核都需要读取完整B矩阵。
      - `cGM`偏移`mIterIdx * singleCoreM * N`，使当前核把结果写回C矩阵中自己负责的行块。
    - 通过`LocalMemAllocator`创建静态`LocalTensor`：
      - `a1Local`：A矩阵在L1中的临时存储。
      - `a2Local`：A矩阵在L0A中的临时存储，供[`Mmad`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md)读取。
      - `b1Local`：B矩阵在L1中的临时存储。`a1Local`和`b1Local`使用同一个L1 allocator按申请顺序分配，避免手动维护L1地址偏移。
      - `b2Local`：B矩阵在L0B中的临时存储，供`Mmad`读取。
      - `cLocal`：矩阵乘结果在L0C中的临时存储。
    - 调用[`DataCopy`](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopy（GMToUB随路转换ND2NZ搬运）.md)将A、B矩阵从GM搬运到L1。这里使用[`Nd2NzParams`](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopy（GMToUB随路转换ND2NZ搬运）.md)参数，在搬运过程中将输入的[ND](../../../../../docs/zh/guide/技术附录/概念原理和术语/神经网络和算子/数据排布格式.md)格式数据转换为Cube计算需要的[Nz](../../../../../docs/zh/guide/技术附录/概念原理和术语/神经网络和算子/数据排布格式.md)格式。
    - 调用[`SetFlag<HardEvent::MTE2_MTE1>`](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md)和[`WaitFlag<HardEvent::MTE2_MTE1>`](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md)进行同步。`DataCopy`属于MTE2流水，后续`LoadData`属于[MTE1](../../../../../docs/zh/guide/技术附录/概念原理和术语/术语表.md)流水，[MTE1](../../../../../docs/zh/guide/技术附录/概念原理和术语/术语表.md)必须等待[MTE2](../../../../../docs/zh/guide/技术附录/概念原理和术语/术语表.md)完成，避免读取到尚未搬运完成的L1数据。
    - 调用`LoadData`将A矩阵从L1搬运到L0A，将B矩阵从L1搬运到L0B。L0A和L0B是Cube矩阵计算单元直接读取的输入缓存。
    - 调用[`SetFlag<HardEvent::MTE1_M>`](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md)和[`WaitFlag<HardEvent::MTE1_M>`](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md)进行同步。`LoadData`属于MTE1流水，后续`Mmad`属于M流水，M流水必须等待MTE1完成，避免读取到尚未搬运完成的L0A/L0B数据。
    - 调用[`Mmad`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md)`(cLocal, a2Local, b2Local, {baseM, baseN, baseK, 0, false, true})`执行矩阵乘。这里`baseM = 128`、`baseN = 256`、`baseK = 64`，对应单个核一次计算的矩阵块大小。
    - 调用[`SetFlag<HardEvent::M_FIX>`](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md)和[`WaitFlag<HardEvent::M_FIX>`](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md)进行同步。[`Mmad`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md)属于M流水，后续[`Fixpipe`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)属于FIX流水，FIX流水必须等待M流水完成，避免读取到尚未计算完成的L0C结果。
    - 调用[`Fixpipe`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)将L0C中的`float`累加结果转换为`half`并搬运回GM中的C矩阵输出位置。
    - 最后调用[`PipeBarrier`](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/PipeBarrier(ISASI).md)`<PIPE_ALL>()`，确保当前核内相关流水任务完成。

  - 调用实现  
    使用[内核调用符](../../../../../docs/zh/guide/编程指南/编程模型/AI-Core-SIMD编程/核函数.md)`<<<>>>`调用[核函数](../../../../../docs/zh/guide/编程指南/编程模型/AI-Core-SIMD编程/核函数.md)。调用时模板参数传入矩阵规格、单核计算量和基础Tile大小，运行时参数传入Device侧A、B、C矩阵地址。

- 接口参数说明：

  以下结构体均以花括号`{}`方式传参，各字段含义如下（字段顺序与API文档保持一致，实际struct声明中部分字段顺序可能不同）：

  **[`AscendC::Nd2NzParams`](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopy（GMToUB随路转换ND2NZ搬运）.md)** — [`DataCopy`](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopy（GMToUB随路转换ND2NZ搬运）.md)接口使用，描述ND→Nz格式转换参数：
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

  **[`AscendC::LoadData2DParams`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md)** — [`LoadData`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md)接口使用，描述Atlas A2 训练系列产品/Atlas A2 推理系列产品、Atlas A3 训练系列产品/Atlas A3 推理系列产品中A矩阵L1到L0A和B矩阵L1到L0B的数据搬运参数：

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

  **[`AscendC::LoadData2DParamsV2`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md)** — [`LoadData`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md)接口使用，描述Ascend 950PR/Ascend 950DT产品中A矩阵L1到L0A和B矩阵L1到L0B的数据搬运参数：
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

  **[`AscendC::MmadParams`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md)** — [`Mmad`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md)接口使用，描述矩阵乘参数：

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

  **[`AscendC::FixpipeParamsV220`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)** — [`Fixpipe`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)接口使用，描述L0C到GM的数据搬运和精度转换参数：
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

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果  
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```

## 功能调试

### printf

该接口提供CPU域或NPU域调试场景下的格式化输出功能。

在算子kernel侧实现代码中需要输出日志信息的地方调用 [printf](../../../../../docs/zh/api/SIMD-API/基础API/调试接口/上板打印/printf.md)接口打印相关内容。

示例如下：

```cpp
AscendC::printf("matmul blockIdx=%d\n", AscendC::GetBlockIdx());
```

> [!CAUTION]注意 
>printf（PRINTF）接口打印功能会对算子实际运行的性能带来一定影响，通常在调测阶段使用。开发者可以按需通过设置ASCENDC\_DUMP=0的方式关闭打印功能。

### DumpTensor

基于算子工程开发的算子，可以使用[DumpTensor](../../../../../docs/zh/api/SIMD-API/基础API/调试接口/上板打印/DumpTensor.md)接口Dump指定Tensor的内容。同时支持打印自定义的附加信息（仅支持uint32\_t数据类型的信息），比如打印当前行号等。

在算子kernel侧实现代码中需要打印Tensor数据的地方调用DumpTensor接口打印相关内容。样例如下：

```cpp
AscendC::DumpTensor(cLocal, baseM * baseN);
```

> [!CAUTION]注意 
>DumpTensor接口打印功能会对算子实际运行的性能带来一定影响，通常在调测阶段使用。开发者可以按需通过设置ASCENDC\_DUMP=0来关闭打印功能。

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
