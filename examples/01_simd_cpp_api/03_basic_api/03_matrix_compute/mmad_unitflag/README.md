# Mmad unitFlag特性样例

## 概述

本样例介绍了如何在调用Mmad指令时，使用unitFlag功能。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.2.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

```
├── mmad_unitflag
│   ├── scripts
│   │   ├── gen_data.py             // 输入数据和真值数据生成脚本
│   │   └── verify_result.py        // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── data_utils.h                // 数据读入写出函数
│   ├── mmad_unitflag.asc           // Ascend C样例实现 & 调用样例
│   └── README.md                   // 样例说明文档
```

## 样例描述

unitFlag用于控制Mmad指令和Fixpipe指令细粒度并行。使能该功能后，硬件每计算完一个分形，计算结果就会被搬出，该功能不适用于在L0C Buffer累加的场景。

本样例中，A矩阵shape为[128, 512]，B矩阵shape为[512, 256]，执行Mmad指令时，沿着K轴进行8次迭代循环，每次迭代K长度为64。输入文件 `x1_gm.bin` 按 `[kRound, M, K/kRound]` 的连续K分块布局存储，便于每次迭代直接读取当前K分块。前7次Mmad仍需继续向L0C累加，unitFlag设置为2；最后1次Mmad累加完成后需允许Fixpipe读取L0C，Mmad和Fixpipe的unitFlag设置为3。每次迭代中Mmad计算时，unitFlag的取值说明如表1所示：

<a name="表1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表1：unitFlag取值说明</span></caption>
  <tr>
    <td >unitFlag值</td>
    <td>取值说明</td>
    <td>样例实现</td>
  </tr>
  <tr>
    <td>0</td>
    <td>不使能unitFlag</td>
    <td>-</td>
  </tr>
  <tr>
    <td>1</td>
    <td>保留值</td>
    <td>-</td>
  </tr>
  <tr>
    <td>2</td>
    <td>使能unitFlag，硬件执行完指令之后，不会关闭unitFlag功能</td>
    <td>前7次Mmad的unitFlag设为2，保证后续Mmad可以写入L0C</td>
  </tr>
  <tr>
    <td>3</td>
    <td>使能unitFlag，硬件执行完指令之后，会将unitFlag功能关闭</td>
    <td>最后1次Mmad和Fixpipe需设为3，保证Fixpipe可以读取L0C</td>
  </tr>
</table>

## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。
- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。
  
- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DENABLE_UNITFLAG=1 ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```
  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DENABLE_UNITFLAG=1 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DENABLE_UNITFLAG=1 ..;make -j; # NPU仿真模式
  ```
  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品/Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |
  | `ENABLE_UNITFLAG` | `0`、`1`（默认） | 是否开启unitFlag |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
