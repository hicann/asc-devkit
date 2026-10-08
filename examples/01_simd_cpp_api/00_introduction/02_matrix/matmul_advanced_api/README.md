# Matmul样例

## 概述

本样例使用Matmul高阶API实现矩阵乘计算。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── matmul_advanced_api
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── matmul_advanced_api.asc // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：
  本样例使用Ascend C Matmul高阶API实现矩阵乘法。Matmul高阶API会封装矩阵计算过程中的数据搬运、Cube计算调度、基础流水同步等细节，开发者主要需要完成矩阵规格配置、tiling生成、输入输出Tensor设置和结果写回。

  矩阵乘法的计算公式如下：
  $$
  C = A * B
  $$
  其中，A矩阵形状为`[M, K]`，B矩阵形状为`[K, N]`，输出C矩阵形状为`[M, N]`。C矩阵中每个元素`C[m, n]`都是A矩阵第`m`行与B矩阵第`n`列在K轴方向逐元素相乘后累加得到的结果。

- 样例规格：
  本样例参数`M = 512, N = 512, K = 128`。代码中A矩阵K轴使用`Ka`表示，B矩阵K轴使用`Kb`表示，本样例`Ka = Kb = K = 128`。输入A、B矩阵均为`half`类型、`ND`格式，输出C矩阵为`float`类型、`ND`格式。输入输出规格如下表所示：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Matmul</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">A</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">B</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">C</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">matmul_custom</td></tr>
  </table>

  样例为纯Cube矩阵计算场景，固定按2个Cube核生成tiling。在本样例规格下，tiling结果会把`M = 512`按2个核均分，每个核处理`singleCoreM = 256`、`singleCoreN = 512`、`singleCoreK = 128`。原始K维对应`tiling.Ka = tiling.Kb = 128`。

- 样例实现：
  - Tiling生成流程
    - 创建`matmul_tiling::MultiCoreMatmulTiling`对象`tilingApi`，用于生成多核Matmul所需的tiling参数。
    - `SetDim(2)`表示多核Matmul tiling计算时可使用2个Cube核。纯Cube矩阵计算场景下，`GetTiling`会在该核数约束内生成实际使用核数`usedCoreNum`。
    - `SetAType`、`SetBType`、`SetCType`分别设置A、B、C矩阵的数据来源位置、数据格式和数据类型，需要和Kernel侧`MatmulType`模板参数保持一致。
    - `SetOrgShape(M, N, K)`设置原始完整矩阵形状，`SetShape(M, N, K)`设置本次实际参与Matmul计算的`M, N, K方向的大小（单位为元素）`。
    - `EnableBias(false)`表示本样例不带bias。
    - `SetBufferSpace(-1, -1, -1)`设置Matmul可使用的L1/L0C/UB空间大小。传入`-1`表示使用当前AI处理器对应buffer的默认大小，由tiling接口据此选择base块和搬运策略。
    - `GetTiling(tilingData)`生成最终tiling结果。如果返回`-1`表示tiling计算失败，tiling结果不可继续使用。Host侧生成`TCubeTiling`后作为Kernel参数直接传入。

  - Kernel侧整体思路
    - `ASCENDC_CUBE_ONLY`表示当前是纯cube模式（只有矩阵计算）。
    - `matmul_custom`是一个`__global__ __cube__`核函数，运行在Cube计算单元上。
    - Kernel入参中的`tiling`类型为`AscendC::tiling::TCubeTiling`，由Host侧生成后作为Kernel参数直接传入。Kernel侧通过该参数控制分核、base块大小和Matmul内部buffer使用。
    - 创建`GlobalTensor`对象`aGlobal`、`bGlobal`、`cGlobal`，分别表示GM中的A、B、C矩阵。`GlobalTensor`只描述GM上的地址和元素个数，真正的数据搬运、L1/L0切分由Matmul高阶API结合tiling完成。
    - 创建高阶API对象`mm`。`MatmulType`模板参数描述A/B/C矩阵在Kernel侧的位置、格式和数据类型，本样例均为GM、ND格式，A/B为`half`，C为`float`。
    - Host侧通过`GetLibApiWorkSpaceSize()`获取系统workspace大小，申请`workspaceDevice`后作为Kernel参数`workspace`传入。
    - 通过`REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling)`注册Matmul对象。
    - Host侧以`numBlocks = tiling.usedCoreNum`启动Kernel，启动的Block均参与实际计算。
    - 调用`mm.SetOrgShape(tiling.M, tiling.N, tiling.Ka, tiling.Kb)`设置原始完整矩阵形状，单位为元素。该接口需要在`SetTensorA`、`SetTensorB`之前调用。
    - 调用`mm.SetTensorA(aGlobal[GetBlockIdx() * tiling.Ka * tiling.singleCoreM], false)`设置当前核要读取的A矩阵起始地址。第0个核从A矩阵第0行开始读取，第1个核从A矩阵第256行开始读取。第二个参数`false`表示不转置。
    - 调用`mm.SetTensorB(bGlobal[0], false)`设置B矩阵起始地址。两个核都需要完整的B矩阵参与计算，所以B矩阵地址都从首地址开始，第二个参数`false`表示不转置。
    - 调用`mm.IterateAll(cGlobal[GetBlockIdx() * tiling.singleCoreM * tiling.N])`执行当前核负责的全部Matmul计算，并将结果写回C矩阵对应偏移位置。
    - 调用`mm.End()`结束当前Matmul对象的使用并释放内部计算资源。后续如果还有其他Matmul对象，调用`End`可避免多个Matmul对象之间的资源冲突。

  - 分核和地址偏移说明
    - `GetBlockIdx()`表示当前核号。第0个核处理C矩阵第0到255行，第1个核处理C矩阵第256到511行。
    - A矩阵形状为`[512, 128]`，每一行长度为`tiling.Ka = 128`，所以A矩阵偏移为`GetBlockIdx() * tiling.Ka * tiling.singleCoreM`。
    - B矩阵形状为`[128, 512]`，对应代码中的`[Kb, N]`。两个核分别计算C矩阵的不同行，但每个输出元素都需要完整K轴累加，因此两个核都从`bGlobal[0]`读取完整B矩阵。
    - C矩阵形状为`[512, 512]`，每一行长度为`tiling.N = 512`，所以C矩阵写回偏移为`GetBlockIdx() * tiling.singleCoreM * tiling.N`。

  - 调用实现
    使用内核调用符`<<<>>>`调用核函数。纯Cube场景下调用时`numBlocks`来自`tiling.usedCoreNum`，运行时参数依次传入Device侧A矩阵地址、B矩阵地址、C矩阵地址、workspace地址和Host侧生成的tiling数据。

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

  使用 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU 运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |

- 执行结果
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
