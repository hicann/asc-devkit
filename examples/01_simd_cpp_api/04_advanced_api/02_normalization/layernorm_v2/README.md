# LayerNormV2样例

## 概述

本样例基于Kernel直调样例工程，介绍了在一个核函数中连续调用LayerNorm和Normalize高阶API，实现对输入tensor按行做归一化计算，并对比两种API的输出结果。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍
 
```
├── layernorm_v2
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── layernorm_v2.asc        // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：  
  本样例在一个核函数中依次调用LayerNorm和Normalize高阶API，两者配合使用实现完整的归一化计算。LayerNorm计算均值mean和标准差倒数rstd，然后根据rstd计算方差var，公式：
  $$
  var = 1/(rstd*rstd) - \epsilon
  $$
  最后Normalize使用LayerNorm的原始输入inputX、LayerNorm计算的均值mean和根据rstd计算得到的方差var作为输入进行归一化计算。LayerNorm的计算公式如下：
  $$
  y_i = \gamma_i \cdot \frac{x_i - \mu}{\sqrt{var + \epsilon}} + \beta_i
  $$

- 样例规格：  
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center"> layernorm_v2 </td></tr>

  <tr><td rowspan="5" align="center">样例输入</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">inputXGm</td><td align="center">[32, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">gammaGm</td><td align="center">[32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">betaGm</td><td align="center">[32]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="6" align="center">样例输出</td></tr>
  <tr><td align="center">outputGm</td><td align="center">[32, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">outputMeanGm</td><td align="center">[32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">outputRstdGm</td><td align="center">[32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">normalizeOutputGm</td><td align="center">[32, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">normalizeRstdGm</td><td align="center">[32]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">layernormv2_custom</td></tr>
  </table>

- 样例实现：  
  本样例使用 **AR格式**（shape为[A, R]），在一个核函数中依次调用LayerNorm和Normalize高阶API进行归一化计算。A为批处理轴，R为归一化轴。详细说明请参考 LayerNorm API文档和Normalize API文档。

  - Kernel实现

    本样例在一个核函数内部实现了完整的数据流计算：
    1. LayerNorm前向计算：输入inputX、gamma、beta，计算得到输出y、均值mean、标准差的倒数rstd。
    2. Normalize计算：使用同一输入计算归一化结果和rstd。
    3. 结果对比：对比LayerNorm和Normalize的输出，验证两者数学等价性。

  - Tiling实现

    样例的tiling实现流程如下：
    1. 分别获取并设置LayerNorm和Normalize接口能完成计算所需的最小临时空间大小。
    2. 根据输入shape、剩余的可供计算的空间大小等信息分别获取两个API kernel侧接口所需tiling参数。

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
  mkdir -p build && cd build;                              # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;     # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py                           # 生成测试输入数据
  ./demo                                                   # 执行编译生成的可执行程序，执行样例
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
