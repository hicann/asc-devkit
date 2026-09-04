# Matmul FP8直调样例

## 概述

A、B矩阵为hifloat8、fp8_e4m3fn、fp8_e5m2数据类型输入的Matmul样例。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍

```
├── matmul_fp8
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── matmul_fp8.asc          // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：  
  对输入的A、B矩阵做矩阵乘和加bias偏置，实现hifloat8、fp8_e4m3fn、fp8_e5m2数据类型输入的Matmul样例。其中，输入数据类型为hifloat8时，A、B数据类型必须一致。

- 样例规格：  
  本样例中：M = 428, N = 479, K = 158。
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="5" align="center">Matmul</td></tr>
  <tr><td rowspan="4" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">hifloat8、fp8_e4m3fn、fp8_e5m2</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">hifloat8、fp8_e4m3fn、fp8_e5m2</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="5" align="center">matmul_fp8_custom</td></tr>
  </table>
- 样例实现： 
  - Kernel关键步骤
    - 创建Matmul对象，数据类型按照SCENARIO_NUM设置为hifloat8或各种fp8数据类型。
      ```cpp
      #if SCENARIO_NUM == 1
      MatmulFp8Kernel<fp8_e4m3fn_t, fp8_e4m3fn_t, float, float> MatmulFp8Kernel;
      #elif SCENARIO_NUM == 2
      MatmulFp8Kernel<fp8_e5m2_t, fp8_e5m2_t, float, float> MatmulFp8Kernel;
      #elif SCENARIO_NUM == 3
      MatmulFp8Kernel<fp8_e4m3fn_t, fp8_e5m2_t, float, float> MatmulFp8Kernel;
      #elif SCENARIO_NUM == 4
      MatmulFp8Kernel<fp8_e5m2_t, fp8_e4m3fn_t, float, float> MatmulFp8Kernel;
      #else
      MatmulFp8Kernel<hifloat8_t, hifloat8_t, float, float> MatmulFp8Kernel;
      #endif

      ...

      AscendC::Matmul<AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, aType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, bType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, cType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, biasType>, mmCfg> matmulObj;
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

  - SCENARIO_NUM：设置A、B的数据类型组合
    - 0 : 对应A hifloat8   B hifloat8场景
    - 1 : 对应A fp8_e4m3fn B fp8_e4m3fn场景
    - 2 : 对应A fp8_e5m2   B fp8_e5m2场景
    - 3 : 对应A fp8_e4m3fn B fp8_e5m2场景
    - 4 : 对应A fp8_e5m2   B fp8_e4m3fn场景

  ```bash
  mkdir -p build && cd build;        # 创建并进入build目录
  cmake -DSCENARIO_NUM=0 -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;      # 编译工程, 0可选01234， 例如cmake .. -DSCENARIO_NUM=1;make -j;
  python3 ../scripts/gen_data.py -scenarioNum=0  # 生成测试输入数据, 0可选01234， 例如python3 ../scripts/gen_data.py -scenarioNum=1
  ./demo                             # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如：
  ```bash
  cmake -DSCENARIO_NUM=0 -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # cpu调试模式
  cmake -DSCENARIO_NUM=0 -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `SCENARIO_NUM` | `0`、`1`、`2`、`3`、`4` | 场景编号，具体见样例说明 |
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　　　　　　　　　　　　　　　　　　 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功：
  ```bash
  test pass!
  ```
