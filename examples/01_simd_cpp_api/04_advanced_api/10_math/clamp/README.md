# Clamp样例

## 概述

本样例基于Clamp高阶API实现将输入中除nan值以外的数截断到区间[min, max]的功能。
当min大于max时，将除nan值外所有值替换为max。min和max均可以为标量或张量。  

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |

## 目录结构介绍

```text
├── clamp
│   ├── scripts
│   │   └── gen_data.py   // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt    // 编译工程文件
│   ├── data_utils.h      // 数据读入写出函数
│   ├── clamp.asc         // Ascend C样例实现 & 调用样例
│   └── README.md         // 样例说明文档
```

## 样例描述

- 样例功能：  
  将输入中大于max的非NaN值替换为max，小于min的非NaN值替换为min，小于等于max和大于等于min的数保持不变，作为输出。当min大于max时，将所有非NaN值替换为max。min和max可以为标量或张量。

  计算公式如下：  

  $$
  dst_i = Clamp(src_i, min_i, max_i)
  $$

  $$
  Clamp(src_i, min_i, max_i) =
  \begin{cases}
  min_i, & src_i < min_i \\
  src_i, & min_i \le src_i \le max_i \\
  max_i, & src_i > max_i \\
  \end{cases}
  $$

- 样例规格：  
  <table>
  <caption>表1：样例规格</caption>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center"> clamp </td></tr>

  <tr><td rowspan="5" align="center">样例输入</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src</td><td align="center">[1, 128]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">src_min</td><td align="center">[1, 128]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">src_max</td><td align="center">[1, 128]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td></tr>
  <tr><td align="center">dst</td><td align="center">[1, 128]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">clamp_custom</td></tr>
  </table>

- 场景说明：
  <table>
  <caption>表2：scalarType参数说明</caption>
  <tr><td align="center">scalarType</td><td align="center">min类型</td><td align="center">max类型</td><td align="center">说明</td></tr>
  <tr><td align="center">1</td><td align="center">张量</td><td align="center">张量</td><td align="center">min和max都是张量</td></tr>
  <tr><td align="center">2</td><td align="center">张量</td><td align="center">标量</td><td align="center">min是张量，max是标量</td></tr>
  <tr><td align="center">3</td><td align="center">标量</td><td align="center">张量</td><td align="center">min是标量，max是张量</td></tr>
  <tr><td align="center">4</td><td align="center">标量</td><td align="center">标量</td><td align="center">min和max都是标量</td></tr>
  </table>

- 样例实现：

  本样例中实现的是shape为输入src[128]、src_min[128]、src_max[128]，输出dst[128]的clamp_custom样例，支持min和max为张量或标量的4种场景组合。

  - Kernel实现

    使用Clamp高阶API接口完成Clamp计算，得到最终结果，再搬出到外部存储上。

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
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
  
  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`（默认） | NPU 架构：dav-3510 对应 Ascend 950PR&950DT系列产品 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
