# RmsNorm样例

## 概述

本样例基于Kernel直调样例工程，介绍了调用RmsNorm高阶API实现rmsnorm单样例，实现对shape大小为[B，S，H]的输入数据的RmsNorm归一化。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── rmsnorm
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── rmsnorm.asc             // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：  
  实现对shape大小为[B，S，H]的输入数据的RmsNorm归一化，其计算公式如下：    
  $$
  y_i = RmsNorm(x_i)\\
  y_i=\frac{x_i}{\sqrt{\frac{1}{N}\sum_{i = 1}^{N}x_i^2+\varepsilon}}\times\gamma
  $$


- 样例规格：  
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center"> rmsnorm </td></tr>

  <tr><td rowspan="4" align="center">样例输入</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src</td><td align="center">[4, 8, 64]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">gamma</td><td align="center">[64]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="2" align="center">样例输出</td></tr>
  <tr><td align="center">dst</td><td align="center">[4, 8, 64]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">rmsnorm_custom</td></tr>
  </table>

- 样例实现：  

  本样例未使用基本块模式（isBasicBlock=false），详细说明请参考RmsNorm API文档。

  - Kernel实现  
    计算逻辑是：  
    输入数据需要先搬运进片上存储，然后使用RmsNorm高阶API接口完成rmsnorm计算，得到最终结果，再搬出到外部存储上。

  - Tiling实现

    样例的Tiling实现流程如下：
    1. AscendC::GetRmsNormMaxMinTmpSize 获取 RmsNorm 接口计算所需的最大和最小临时空间大小后，再根据mode计算出使用大小stackSize，然后调用 AscendC::GetRmsNormTilingInfo 根据输入 shape 和工作空间大小获取kernel侧接口所需 Tiling 参数。
    2. 将 Tiling 参数封装到 RmsNormTilingData 结构体中，传递给 Kernel 侧使用。

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
  cmake ..;make -j;             # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                        # 执行编译生成的可执行程序，执行样例
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
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
