# IsFinite样例

## 概述

本样例基于IsFinite高阶API实现按元素判断输入的浮点数是否非NAN、非INF的功能，输出结果为浮点数或者布尔值。

> **接口提示：** 除本样例使用的 `IsFinite` 接口外，Ascend C 还提供了以下判断相关高阶API接口，如需调用替换接口名即可：
>
> - **IsInf**：判断输入是否为无穷大。
> - **IsNaN**：判断输入是否为非数值。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |

## 目录结构介绍

```text
├── isfinite
│   ├── scripts
│   │   └── gen_data.py         // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── isfinite.asc            // Ascend C算子实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：  
  按元素判断输入的浮点数是否非NAN、非INF，输出结果为浮点数或者布尔值。对于非NAN或者非INF的输入数据，当输出为浮点类型时，对应位置的结果为该浮点类型的1，反之为0；当输出为bool类型时，对应位置的结果为true，反之false。  
  计算公式如下：  
  $$dst_i = IsFinite(src_i)$$

  当输入为浮点类型时：  
  $$
  IsFinite(x) = 
  \begin{cases}
  0.0, & x = \pm\inf \text{ or } x = \text{nan} \\
  1.0, & x \ne \pm\inf \text{ and } x \ne \text{nan}
  \end{cases}
  $$

  当输出为bool类型时：  
  $$
  IsFinite(x) =
  \begin{cases}
  false, & x = \pm\inf \text{ or } x = \text{nan} \\
  true, & x \ne \pm\inf \text{ and } x \ne \text{nan}
  \end{cases}
  $$
- 样例规格：  
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center"> isfinite </td></tr>

  <tr><td rowspan="3" align="center">样例输入</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 1024]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">isfinite_custom</td></tr>
  </table>

- 样例实现：  
  本样例中实现的是固定shape为输入x[1, 1024]，输出y[1, 1024]的isfinite_custom样例。

  - Kernel实现  
    计算逻辑是：Ascend C提供的矢量计算接口的操作元素都为LocalTensor，输入数据需要先搬运进片上存储，然后使用IsFinite高阶API接口完成isfinite计算，得到最终结果，再搬出到外部存储上。

  - 调用实现  
    使用内核调用符<<<>>>调用核函数。

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如：

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
