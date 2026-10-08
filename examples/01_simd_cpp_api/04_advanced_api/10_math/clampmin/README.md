# 单边Clamp样例

## 概述

本样例基于ClampMin/ClampMax高阶API实现将Tensor单边截断到scalar的功能。  
ClampMin将输入Tensor中小于scalar的元素替换为scalar，ClampMax将输入Tensor中大于scalar的元素替换为scalar，通过参数配置选择使用ClampMin或ClampMax功能。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```text
├── clampmin
│   ├── scripts
│   │   └── gen_data.py   // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt    // 编译工程文件
│   ├── data_utils.h      // 数据读入写出函数
│   ├── clampmin.asc      // Ascend C样例实现 & 调用样例
│   └── README.md         // 样例说明文档
```

## 样例描述

- 样例功能：  
  通过参数配置选择使用ClampMin或ClampMax功能：
  - ClampMin：将srcTensor中小于scalar的数替换为scalar，大于等于scalar的数保持不变，作为dstTensor输出
  - ClampMax：将srcTensor中大于scalar的数替换为scalar，小于等于scalar的数保持不变，作为dstTensor输出

  计算公式如下：  

  $$
  ClampMin(srcTensor_i, scalar) =
  \begin{cases}
  srcTensor_i, & srcTensor_i \geq scalar \\
  scalar, & srcTensor_i < scalar
  \end{cases}
  $$

  $$
  ClampMax(srcTensor_i, scalar) =
  \begin{cases}
  srcTensor_i, & srcTensor_i \leq scalar \\
  scalar, & srcTensor_i > scalar
  \end{cases}
  $$

- 样例规格：  
  <table>
  <caption>表1：样例规格</caption>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center"> clampmin/clampmax </td></tr>

  <tr><td rowspan="3" align="center">样例输入</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src</td><td align="center">[1, 256]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td></tr>
  <tr><td align="center">dst</td><td align="center">[1, 256]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">clampmin_clampmax_custom</td></tr>
  </table>

- 样例实现：  

  - Kernel实现

    使用ClampMin/ClampMax高阶API接口完成单边截断计算，通过参数配置选择使用ClampMin或ClampMax功能。

  - Tiling实现

    Host侧通过GetClampMaxMinTmpSize接口获取ClampMin/ClampMax接口计算所需的最大和最小临时空间。

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
  SCENARIO=0  # 0: ClampMin, 1: ClampMax
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DSCENARIO=$SCENARIO -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py --scenario $SCENARIO  # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
  
  示例如：

  ```bash
  cmake -DSCENARIO=$SCENARIO -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DSCENARIO=$SCENARIO -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |
  | `SCENARIO` | `0`（默认）、`1` | 场景：0 对应 ClampMin，1 对应 ClampMax |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
