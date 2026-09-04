# GmByPassDCache类样例

## 概述

本样例组合调用 `ReadGmBypassDCache` 和 `WriteGmBypassDCache` 接口，实现不经过DCache从GM地址读取和写入数据。本样例会将读取的值加100后写入到输出地址。

当多核操作GM地址时，如果数据无法对齐到Cache Line，经过DCache的方式下，由于按照Cache Line大小进行读写，会导致多核数据随机覆盖的问题。此时，可以采用不经过DCache直接读写GM地址的方式，从而避免上述随机覆盖的问题。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── gm_by_pass_dcache
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── gm_by_pass_dcache.asc   // Ascend C算子实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：  
  组合调用 `ReadGmBypassDCache` 和 `WriteGmBypassDCache`，实现：
  1. 使用 `ReadGmBypassDCache` 从 GM 地址逐个读取int32_t类型元素
  2. 使用 `WriteGmBypassDCache` 将读出的值加100后，写入到输出GM地址

- 样例规格：
  <table>
  <caption>表1：样例规格说明</caption>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1, 1024]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">kernel_gm_by_pass_dcache</td></tr>  
  </table>

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
  mkdir -p build && cd build;                                        # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;               # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py                                     # 生成测试输入数据
  ./demo                                                             # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin  # 验证输出结果是否正确
  ```

  使用 CPU调试 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 参数即可。

  示例如：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;  # cpu调试模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu` | 运行模式：NPU 运行、CPU调试 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品/Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |
  
- 执行结果

  执行结果如下，说明精度对比成功：

  ```bash
  test pass!
  ```
