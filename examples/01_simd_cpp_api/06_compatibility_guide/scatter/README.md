# Scatter兼容性样例


## 概述

本样例演示数据离散功能，可根据输入张量和目的地址偏移张量，将输入张量分散到结果张量中。

通过编译时宏隔离不同硬件实现：
- Atlas A2/A3：不支持Scatter指令，使用标量搬出方式（GetValue/SetValue循环）实现。
- Ascend 950PR&950DT系列产品：直接调用Scatter指令实现。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── scatter
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── scatter_custom.asc      // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例规格

<table>
<caption>样例规格表</caption>
<tr><td rowspan="1" align="center">类别</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td rowspan="2" align="center">样例输入</td>
<td align="center">src</td><td align="center">[1, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td align="center">dst_offset</td><td align="center">[1, 128]</td><td align="center">uint32_t</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">dst</td><td align="center">[1, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">scatter_custom</td></tr>
</table>

## 样例实现

- Atlas A2/A3：不支持Scatter指令，使用标量GetValue/SetValue循环逐元素读取偏移地址和源数据，将源数据写入目标位置，实现数据离散。
- Ascend 950PR&950DT系列产品：调用Scatter指令将源数据按偏移地址离散写入目标张量。

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin
  ```

  使用CPU调试或NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=cpu`或`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |

- 执行结果

  执行结果如下，说明精度对比成功：

  ```bash
  test pass!
  ```
