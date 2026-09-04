# int4数据类型下Matmul兼容性样例


## 概述

本样例演示 int4 矩阵乘计算的端到端实现，通过编译时宏隔离不同硬件实现。
- Atlas A2/A3 训练/推理系列产品支持int4数据类型，可直接进行Matmul计算。
- Ascend 950PR/950DT的Cube计算单元不支持int4数据类型，需将int4x2解压为int8后再进行Matmul计算。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.2.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

```
├── matmul_s4
│   ├── scripts                   // 数据生成脚本目录
│   │   ├── gen_data.py           // 输入数据和真值数据生成脚本
│   │   └── verify_result.py      // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt            // 编译工程文件
│   ├── data_utils.h              // 数据读入写出函数
│   ├── matmul_s4.asc             // AscendC样例实现 & 调用样例
│   └── README.md                 // 样例说明文档
```

## 样例规格：

  <table>
  <caption>表1 样例规格说明</caption>
  <tr><td rowspan="1" align="center">类别</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td rowspan="4" align="center">样例输入</td></tr>
  <tr><td align="center">x4</td><td align="center">[256, 256]</td><td align="center">int4b_t</td><td align="center">ND</td></tr>
  <tr><td align="center">y4</td><td align="center">[256, 256]</td><td align="center">int4b_t</td><td align="center">ND</td></tr>
  <tr><td align="center">tiling</td><td align="center">[1, 32]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[256, 256]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">matmul_s4</td></tr>
  </table>

## 样例实现：

  - **Atlas A2/A3 训练/推理系列产品**：Cube计算单元支持int4数据类型，可直接执行Matmul计算。
  - **Ascend 950PR/950DT**：Cube计算单元不支持int4数据类型，需通过mix模式在Vector Core执行Unzip操作，将int4x2解压为int8后再进行Matmul计算。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # 默认npu模式
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  ./demo                                                                    # 执行编译生成的可执行程序
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果
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

  执行结果如下，说明精度对比成功：

  ```bash
  test pass!
  ```
