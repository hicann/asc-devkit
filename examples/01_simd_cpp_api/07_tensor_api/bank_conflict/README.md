# 基于Tensor API实现的ND2NZ Bank冲突调优样例

## 概述

本样例基于静态Tensor API编程范式，实现`8192 × 8192` half矩阵从ND布局到NZ布局的转换，并通过两个场景对比UB Vector写阶段的bank冲突及规避方式。

GM和UB之间的数据搬运使用Tensor API的`copy`接口，ND到NZ重排的寄存器访问使用C API的`asc_loadalign`和`asc_storealign`。`asc_storealign`一次写入8个DataBlock，通过`block_stride`控制相邻DataBlock之间的间距，从而可以直接对比bank冲突的向量写行为。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

```text
├── bank_conflict
│   ├── scripts
│   │   ├── gen_data.py                    // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py               // 真值对比文件
│   ├── figures                            // 图示
│   ├── CMakeLists.txt                     // 编译工程文件
│   ├── data_utils.h                       // 数据读入写出函数
│   ├── bank_conflict.asc                  // Ascend C样例实现
│   └── README.md                          // 样例说明文档
```

## 样例描述

- 样例功能：

  本样例将输入矩阵`x`的ND布局转换为输出矩阵`z`的NZ布局。

  NZ布局的小分形为16 × C0，小分形中的一个C0块包含16个half，即一个32B DataBlock。每个`144 × 128` tile的一行包含8个C0块，向量写阶段将这8个DataBlock写入NZ区域中对应的8个C0列。

  - `x`：输入，逻辑shape为`[M, N]`，half，ND布局
  - `z`：输出，逻辑shape为`[M, N]`，half，NZ布局

- 样例规格：

  本样例固定`M=8192`、`N=8192`，启动64个Vector block。每个block负责一个行切分和列切分区域，内部按`144 × 128` tile处理；最后一个行tile的有效高度通过`actualTileH`传入。

  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">ND2NZ</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[M, N]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[M, N]</td><td align="center">float16</td><td align="center">NZ</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">bank_conflict_kernel</td></tr>
  </table>

  编译期参数如下：

  | 参数 | 值 | 说明 |
  | :--- | :---: | :--- |
  | `TOTAL_M` | 8192 | 输入矩阵行数 |
  | `TOTAL_N` | 8192 | 输入矩阵列数 |
  | `ROW_SPLITS` | 8 | 行方向切分数 |
  | `COL_SPLITS` | 8 | 列方向切分数 |
  | `TOTAL_BLOCKS` | 64 | 启动block数 |
  | `TILE_H` | 144 | tile最大行数 |
  | `TILE_W` | 128 | tile列数 |
  | `C0_ELEMS` | 16 | 一个C0块中的half元素数 |
  | `TILE_C0_COLS` | 8 | 一个tile中的C0列数 |
  | `SCENARIO_NUM` | 1、2 | bank冲突对照场景 |
  | `dstNzC0Stride` | S1=144、S2=145 | 相邻C0列起始位置的间距，单位为DataBlock |

  > **术语说明：** DataBlock是向量指令处理的数据单元，大小为32B；half类型下1个DataBlock包含16个half。

  **图：ND与NZ数据排布示意图**

  <img src="figures/nd2nz.png" width="80%">

- 样例实现：

  - 实现流程：

    <table>
    <tr><th align="left">步骤</th><th align="left">Tensor API/C API操作</th><th align="left">功能</th><th align="left">布局或执行单元</th></tr>
    <tr><td align="left">1</td><td align="left">编译期常量</td><td align="left">定义矩阵、tile、分块和场景参数</td><td align="left">不涉及</td></tr>
    <tr><td align="left">2</td><td align="left">make_tensor + layout</td><td align="left">创建GM、UB源和UB目标Tensor视图</td><td align="left">GM为ND；NZ写入步长由Vector接口和MTE3 layout分别表达</td></tr>
    <tr><td align="left">3</td><td align="left">copy(copy_gm_to_ub)</td><td align="left">将当前有效高度的ND tile从GM搬到UB</td><td align="left">ND到ND</td></tr>
    <tr><td align="left">4</td><td align="left">slice + asc_loadalign</td><td align="left">按行切片，从UB ND tile读取128个half</td><td align="left">Vector</td></tr>
    <tr><td align="left">5</td><td align="left">asc_storealign</td><td align="left">将8个DataBlock按`dstNzC0Stride`写入UB NZ区域</td><td align="left">Vector，ND到NZ重排</td></tr>
    <tr><td align="left">6</td><td align="left">copy(copy_ub_to_gm)</td><td align="left">将NZ tile写回GM的紧凑NZ布局，跳过S2的填充空间</td><td align="left">NZ到NZ</td></tr>
    </table>

  - Tensor API核心接口：

    1. **张量创建接口**：使用`make_frame_layout`和`make_pattern_layout`描述GM、UB Tensor的形状和布局，使用`make_tensor`和`make_mem_ptr`创建GM、UB Tensor。

    2. **切片接口**：使用`slice(make_coord(...), make_shape(...))`取得当前tile的行视图，源地址和目的地址均从Tensor视图获取。

    3. **数据搬运接口**：使用`make_copy`构造`copy_gm_to_ub`和`copy_ub_to_gm`，完成GM与UB之间的数据搬运；MTE3阶段的NZ目的Tensor layout表达UB中相邻C0列的实际步长。

    4. **寄存器搬运接口**：使用`asc_loadalign`读取一行128个half，使用`asc_storealign`的`block_stride`参数控制8个DataBlock的非连续写入。`block_stride`是本样例对比bank冲突的直接控制参数。

    5. **流水接口**：使用`asc_lock`和`asc_unlock`保护MTE2、Vector和MTE3阶段，使用两组UB buffer交替处理tile。

  - ND到NZ核心实现：

    `srcTensor`和`dstTensor`先通过Tensor API创建，`slice`只负责取得当前行的起始地址；8个DataBlock的非连续落点由`asc_storealign`的`dstNzC0Stride`参数表达。

    ```cpp
    template <uint32_t dstNzC0Stride>
    __simd_vf__ inline void NdToNz(__ubuf__ half* dst, __ubuf__ half* src)
    {
        vector_bool mask = asc_create_mask_b16(PAT_ALL);
        vector_half value;
        asc_loadalign(value, src);
        asc_storealign(dst, value, dstNzC0Stride, 0, mask);
    }
    ```

    场景差异在kernel的最终调用处体现：

    ```cpp
    if constexpr (SCENARIO_NUM == 1) {
        NdToNz<TILE_H>(dstRow.data().get(), srcRow.data().get());
    } else {
        NdToNz<TILE_H + BANK_CONFLICT_OFFSET>(dstRow.data().get(), srcRow.data().get());
    }
    ```

  - 调用实现：

    Host侧使用内核调用符`<<<>>>`启动`bank_conflict_kernel`，并通过`SCENARIO_NUM`选择对照场景。

  - UB Bank结构与本样例中的冲突：

    Ascend 950PR&950DT系列产品的Unified Buffer划分为16个物理bank，并组织为8个bank group。bank `i`和bank `i+8`属于同一个bank group，关系为`bank group = bank % 8`。UB地址采用低位交织，连续地址每经过一个32B DataBlock，物理bank编号加1并按16取模。

    本样例关注`asc_storealign`一次写入8个DataBlock时的写写冲突。相邻落点之间的间距由`dstNzC0Stride`决定；当多个落点落入同一个bank group时，写入需要串行处理。

    **图：Ascend 950PR&950DT系列产品 UB Bank结构示意图**

    <img src="figures/ubBankStruct3510.png" width="80%">

    **图：Ascend 950PR&950DT系列产品 UB Bank内存排布示意图**

    <img src="figures/UB-3510.png" width="80%">

    **场景1：紧凑排放**

    编译时设置`SCENARIO_NUM=1`。此时`dstNzC0Stride=144`，即`16 × 9`个DataBlock。对同一行而言，8个写落点的物理bank编号保持不变，因此8个写访问同一个bank和bank group，形成写写冲突。

    **图：场景1紧凑排放下ND到NZ示意图**

    <img src="figures/datand2nzS1.png" width="80%">

    以tile第一行为例，8个落点为`0、144、288、...、1008`个DataBlock，对应bank 0、bank 0、...、bank 0；其他行的起始bank随行地址变化，但同一条store中的8个落点仍落在同一个bank。

    **图：场景1 UB bank冲突示意图**

    <img src="figures/s1bank3510.png" width="80%">

    **场景2：增加stride并偏移NZ buffer起点**

    编译时设置`SCENARIO_NUM=2`。此时`dstNzC0Stride=145`，即`16 × 9 + 1`个DataBlock。每次跳转后物理bank编号加1，8个写落点依次进入8个不同的bank group。

    代码同时将`nzPtr`的起始地址额外偏移8个DataBlock（`NZ_BASE_SHIFT_DATA_BLOCKS=8`）。因此以第一行为例，`asc_loadalign`从ND buffer读取bank 0~7，`asc_storealign`向NZ buffer写入bank 8~15，错开首行读写使用的物理bank，降低同拍物理bank重叠。增加的stride空间只用于改变UB落点，UB到GM搬运通过NZ Tensor的layout跳过填充位置，输出仍是紧凑NZ布局。

    **图：场景2增加stride后的ND到NZ示意图**

    <img src="figures/datand2nzS2.png" width="80%">

    **图：场景2 UB bank分散落点示意图**

    <img src="figures/s2bank3510.png" width="80%">

## Tensor API实现特点

| 特性 | 本样例实现 |
|------|------------|
| 张量表示 | 使用Tensor对象描述GM、UB ND tile和UB NZ tile |
| 张量切片 | 使用`slice`取得每个tile和每行的起始地址 |
| 数据搬运 | 使用`copy`完成GM到UB、UB到GM搬运 |
| ND到NZ重排 | 使用`asc_loadalign`和`asc_storealign`完成向量寄存器搬运 |
| Bank冲突对比 | 通过`dstNzC0Stride=144/145`对比紧凑排放和bank分散排放 |
| 地址偏移 | 场景2通过8个DataBlock的NZ起始偏移，在起始位置错开ND读取和NZ写入的物理bank |
| 双缓冲 | 使用ping-pong UB buffer交替处理tile |
| 尾块处理 | 使用`actualTileH`限制最后一个tile的有效行数 |

## 性能数据

以下数据为Ascend 950PR&950DT系列产品在`144 × 128` tile、64个block配置下的实测结果。

| 场景 | `dstNzC0Stride` | Task Duration(μs) | `aiv_time`(μs) | `aiv_total_cycles` | `aiv_vec_time`(μs) | `aiv_vec_ratio` | `aiv_scalar_time`(μs) | `aiv_scalar_ratio` | `aiv_mte2_time`(μs) | `aiv_mte2_ratio` | `aiv_mte3_time`(μs) | `aiv_mte3_ratio` | `icache_miss_rate` |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| S1 | 144 | 262.628 | 261.96 | 14112428 | **254.637** | 0.972 | 3.851 | 0.015 | 93.445 | 0.357 | 47.817 | 0.183 | 0.006 |
| S2 | 145 | 222.964 | 222.18 | 12052109 | **214.199** | 0.964 | 3.823 | 0.017 | 111.712 | 0.503 | 41.139 | 0.185 | 0.009 |

S2的`aiv_vec_time`由254.637μs降低到214.199μs，说明增加stride后写写bank冲突明显减少。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量，**当前仅支持使用[CANN master](../../../../docs/zh/quick_start.md#cann-install)**。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  以场景1为例，在本样例目录下执行如下命令。

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  ./demo                                                                    # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  编译场景2时，将`SCENARIO_NUM`设置为2即可。

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  ```bash
  cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构：dav-3510对应Ascend 950PR&950DT系列产品 |
  | `SCENARIO_NUM` | 1、2 | 1为紧凑排放，2为增加stride并偏移NZ buffer起点 |

  > **说明：** 本样例仅支持dav-3510架构（对应Ascend 950PR&950DT系列产品）。

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
