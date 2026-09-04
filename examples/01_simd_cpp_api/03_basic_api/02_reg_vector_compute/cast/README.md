# cast样例

## 概述
本样例基于Reg编程接口实现Cast运算，主要调用Cast接口实现数据类型转换。  
本样例支持两种数据类型转换场景，通过 CMake 编译参数 `SCENARIO_NUM` 选择场景。
  <table>
    <tr>
      <td>SCENARIO_NUM</td>
        <td>数据类型转换</td>
    </tr>
    <tr>
      <td>1</td>
      <td>数据类型位宽小转大，以half转int32_t为例</td>
    </tr>
    <tr>
      <td>2</td>
      <td>数据类型位宽大转小，以float转int16_t为例</td>
    </tr>
    </table>

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍
```
├── cast
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── figures                        // 图示
│   ├── cast.asc                       // AscendC样例实现 & 调用样例
│   └── README.md                      // 样例介绍
```

## 样例描述
本样例对输入向量做数据类型转换。当输入输出的数据类型的位宽不同时，Cast接口会间隔地读取或写入，所以本样例在数据搬入或搬出时会使用相应的压缩或解压缩模式，LoadAlign/StoreAlign接口的使用仅供参考，具体说明如下：

**场景1：数据类型位宽小转大**
- 样例功能：将half类型的数据转换成int32_t类型的数据。
- 参数说明：
  - layoutMode = RegLayout::ZERO: Cast接口从xReg索引2\*N+0的位置读取数据，用于配合LoadAlign接口将输入数据依次搬入xReg的2\*N位置
  - satMode = SatMode::NO_SAT：本场景介绍浮点数转整数的非饱和模式，输入数据超过输出数据类型最值时，结果被截断为输出数据类型的宽度。例如输入half值为4294967297.0，对应整数4294967297（0x100000001），则取低32位，输出int32_t值为1
  - roundMode = RoundMode::CAST_FLOOR：本场景介绍floor（向下取整）舍入模式。例如输入half值为2.5，则输出int32值为2
  - mask：Cast接口中，mask会按照输入和输出中，位宽较大的数据类型来筛选，所以本场景按照int32_t数据类型生成MaskReg
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">half</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">y</td><td align="center">[1, 256]</td><td align="center">int32_t</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">cast</td></tr>
  </table>
- 样例实现：  
  从half到int32_t的Cast接口每次处理64个数据，具体流程和示意图如下：
  - 搬入：调用LoadAlign接口，使用解压缩模式，搬入数据至2\*N位置，同时2\*N+1位置置0
  - 计算：调用Cast接口，输入输出位宽比为1:2，所以从xReg的2\*N位置读取数据，类型转换后依次写入至yReg
  - 搬出：调用StoreAlign接口，常规搬出
  - 调用实现：使用内核调用符<<<>>>调用核函数。  
  <img src="figures/reg_cast_1.png">

**场景2：数据类型位宽大转小**
- 样例功能：将float类型的数据转换成int16_t类型的数据。
- 参数说明：
  - layoutMode = RegLayout::ZERO: Cast接口将数据写入yReg索引2\*N+0的位置，用于配合StoreAlign接口搬出yReg的2\*N位置的数据
  - satMode = SatMode::SAT：本场景介绍浮点数转整数的饱和模式，输入数据超过输出数据类型最值时，结果为输出类型的对应最值。例如输入float值为32768.0，对应整数32768，则取int16_t的最大值，输出int16_t值为32767
  - roundMode = RoundMode::CAST_ROUND：本场景介绍round（四舍五入取整）舍入模式。例如输入half值为2.5，则输出int32值为3
  - mask：Cast接口中，mask会按照入输入和输出中，位宽较大的数据类型来筛选，所以本场景按照float数据类型生成MaskReg
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">y</td><td align="center">[1, 256]</td><td align="center">int16_t</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">cast</td></tr>
  </table>
- 样例实现：  
  从float到int16_t的Cast接口每次处理64个数据，具体流程和示意图如下：
  - 搬入：调用LoadAlign接口，常规搬入
  - 计算：调用Cast接口，输入输出位宽比为2:1，所以从xReg依次读取数据，类型转换后依次写入至yReg的2\*N位置，同时2\*N+1位置置0
  - 搬出：调用StoreAlign接口，采用压缩模式，仅搬出2\*N位置的数据
  - 调用实现：使用内核调用符<<<>>>调用核函数。  
  <img src="figures/reg_cast_2.png">

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
 
  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # cpu调试模式
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `SCENARIO_NUM` | 1、2 | 样例执行场景：场景1：数据类型位宽小转大、场景2：数据类型位宽大转小 |
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果  
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
