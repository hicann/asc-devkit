# 融合编程matmul_fused_manual样例

## 概述

分核AIC、AIV融合编程实现，主要介绍了Matmul高阶API的纯Cube模式，需要调用相关接口实现手动控制AIC、AIV的核间同步。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── matmul_fused_manual
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── matmul_fused_manual.asc // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：
  本样例在AIC核创建纯Cube模式的Matmul对象，实现Matmul计算，中间结果输出至GM。AIC核计算完成后，通过调用CrossCoreSetFlag和CrossCoreWaitFlag接口手动控制核间同步，继续在AIV核对GM上的中间结果进行LeakyRelu的计算。

- 样例规格：
  本样例中：M = 128, N = 128, K = 256
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="5" align="center">Matmul</td></tr>
  <tr><td rowspan="4" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="5" align="center">matmul_fused_manual_custom</td></tr>
  </table>

- 样例实现：

  - Kernel关键步骤
    - AIC侧具体步骤：
      - 创建Matmul对象并初始化。
        在#include "lib/matmul_intf.h"前配置ASCENDC_CUBE_ONLY宏，创建纯Cube模式的Matmul对象
          ```cpp
          #define ASCENDC_CUBE_ONLY
          #include "lib/matmul_intf.h"
          ```
      - 完成Matmul计算。
      - 设置核间同步。
        ```cpp
        if ASCEND_IS_AIC {
          AscendC::CrossCoreSetFlag<0x2, PIPE_FIX>(3);
        }
        ```
    - AIV侧具体步骤：
      - 创建LeakyRelu对象并初始化。
      - 等待核间同步。
        ```cpp
        if ASCEND_IS_AIV {
          AscendC::CrossCoreWaitFlag<0x2>(3);
        }
        ```
      - 完成LeakyRelu计算。

  - Tiling关键步骤
    - 设置自定义MatmulConfig参数，将Kernel侧配置的参数如scheduleType等，同步到Tiling侧。
      ```cpp
      tilingApi.SetOrgShape(M, N, K);
      tilingApi.SetShape(M, N, K);
      tilingApi.SetTraverse(matmul_tiling::MatrixTraverse::FIRSTM);
      tilingApi.SetFixSplit(baseM, baseN, -1);
      ```

  - 调用实现
    使用内核调用符<<<>>>调用核函数。

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
  mkdir -p build && cd build;   # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;             # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                        # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　|
  | ----------------| -----------------------------| ---------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功：
  ```bash
  test pass!
  ```
