# cast样例

## 概述
本样例基于C API编程接口实现Cast运算，主要调用[asc_half2int32_rd](../../../../../docs/zh/api/SIMD-API/c_api/reg/data_type_convert/asc_half2int32.md)/asc_float2int16_rna_sat接口实现数据类型转换。  
本样例支持两种数据类型转换场景，通过 CMake 编译参数 `SCENARIO_NUM` 选择场景。
  | SCENARIO_NUM | 数据类型转换 |
  | --- | --- |
  | 1 | 数据类型位宽小转大，以half转int32_t为例 |
  | 2 | 数据类型位宽大转小，以float转int16_t为例 |

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 目录结构介绍
```
├── cast
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── figures                        // 图示
│   ├── cast.asc                       // AscendC样例实现 & 调用样例
│   ├── README.md                      // 样例介绍
│   └── README_en.md                   // 样例介绍（英文版）
```

## 样例描述
本样例对输入向量做数据类型转换。当输入输出的数据类型的位宽不同时，asc_half2int32_rd/asc_float2int16_rna_sat接口会间隔地读取或写入，所以本样例在数据搬入或搬出时会使用相应的压缩或解压缩模式，asc_loadalign/asc_storealign接口的使用仅供参考，具体说明如下：

**场景1：数据类型位宽小转大**
- 样例功能：将half类型的数据转换成int32_t类型的数据。
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">half</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 256]</td><td align="center">int32_t</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="3" align="center">cast_custom_s1</td></tr>
  </table>
- 样例实现：  
  cast_vf函数内从half到int32_t的asc_half2int32_rd接口每次处理64个数据，具体流程和示意图如下：
  - 搬入：调用asc_loadalign_unpack接口，使用解压缩模式，搬入数据至2\*N位置，同时2\*N+1位置置0
  - 计算：调用asc_half2int32_rd接口，输入输出位宽比为1:2，所以从矢量数据寄存器x的2\*N位置读取数据，类型转换后依次写入至矢量数据寄存器y
  - 搬出：调用asc_storealign接口，常规搬出
  - 调用实现：使用内核调用符<<<>>>调用核函数。  
  <img src="figures/cast1.png">

**场景2：数据类型位宽大转小**
- 样例功能：将float类型的数据转换成int16_t类型的数据。
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 256]</td><td align="center">int16_t</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="3" align="center">cast_custom_s2</td></tr>
  </table>
- 样例实现：  
  cast_vf函数内从float到int16_t的asc_float2int16_rna_sat接口每次处理64个数据，具体流程和示意图如下：
  - 搬入：调用asc_loadalign接口，常规搬入
  - 计算：调用asc_float2int16_rna_sat接口，输入输出位宽比为2:1，所以从矢量数据寄存器x依次读取数据，类型转换后依次写入至矢量数据寄存器y的2\*N位置，同时2\*N+1位置置0
  - 搬出：调用asc_storealign_pack接口，采用压缩模式，仅搬出2\*N位置的数据
  - 调用实现：使用内核调用符<<<>>>调用核函数。  
  <img src="figures/cast2.png">

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
  SCENARIO_NUM=1                                                                 # 执行场景1
  mkdir -p build && cd build;                                                    # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;  # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM                      # 生成测试输入数据
  ./demo                                                                         # 执行编译生成的可执行程序，执行样例
  ```
 
  使用 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  | --- | --- | --- |
  | `SCENARIO_NUM` | 1、2 | 样例执行场景：场景1：数据类型位宽小转大、场景2：数据类型位宽大转小 |
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU 运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果  
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
