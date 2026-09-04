# auxscalar_reg样例

## 概述
本样例基于Reg编程接口实现AuxScalar方式从UB(Unified Buffer)读取多个标量数据，结合Adds进行向量与标量加法计算。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍
```
├── reg_auxscalar
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── auxscalar_reg.asc              // AscendC样例实现 & 调用样例
│   └── README.md                      // 样例介绍
```

## 样例描述
- 样例功能：  
  从UB上读取4个标量数据，和向量x进行Adds计算。向量x shape为[1, 512]，通过for循环控制每个标量和128个连续的向量元素进行计算。  
  - AuxScalar读取标量在VF函数内可以直接使用，在MainScalar（VF函数外）使用时需要加同步指令
  
  **AuxScalar + Adds模式**
  - 使用AuxScalar方式（`__ubuf__`指针下标访问，如`scalarAddr[0]`）从UB读取标量，结合Adds进行向量与标量加法
  - 样例规格：
    <table>
    <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
    <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">x</td><td align="center">[1, 512]</td><td align="center">half</td></tr>
    <tr><td align="center">scalar</td><td align="center">[1, 4]（input_y.bin实际包含16个half数，用于32B对齐，后12个为padding）</td><td align="center">half</td></tr>
    <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1, 512]</td><td align="center">half</td></tr>
    <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">auxscalar_reg</td></tr>
    </table>
  - 样例实现：
    AuxScalarAddsVF函数内，通过`scalarAddr[i]`从UB读取第i个标量值，调用`Adds`进行向量加标量计算。
    - 调用实现
      使用内核调用符`<<<>>>`调用核函数，启动1个核。

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
  mkdir -p build && cd build;                                                         # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                                # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py;                                                     # 生成测试输入数据
  ./demo                                                                              # 执行编译生成的可执行程序，执行样例
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
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
