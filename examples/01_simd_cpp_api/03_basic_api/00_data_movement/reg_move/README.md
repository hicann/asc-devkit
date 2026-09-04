# move_reg样例

## 概述
本样例基于Reg编程接口演示验证从RegTensor(Reg矢量计算基本单元)中搬运数据到MaskReg（掩码寄存器），使用MaskGenWithRegTensor接口。支持多种场景，通过 CMake 编译参数 `SCENARIO_NUM` 选择场景。
  <table>
 	<tr>
        <td>SCENARIO_NUM</td>
 		<td>场景类型</td>
 	</tr>
 	<tr>
 		<td>1</td>
 		<td>下采样场景：调用MaskGenWithRegTensor从RegTensor提取数据填充到MaskReg</td>
 	</tr>
 	<tr>
 		<td>2</td>
 		<td>复合计算场景：调用MaskGenWithRegTensor实现MaskReg的搬入，再作为Select的掩码完成选择计算</td>
 	</tr>
 </table>

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍
```
├── reg_move
│   ├── scripts
│   │   └── gen_data.py                // 输入数据和真值数据生成脚本
│   ├── figures                        // 图示
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── README.md                      // 样例介绍
│   ├── move_reg_scenario_1.asc        // 场景1：下采样场景的AscendC实现
│   └── move_reg_scenario_2.asc        // 场景2：复合计算场景的AscendC实现
```

## 样例描述

### 场景1：下采样场景
- 样例功能：  
  演示验证从regTensor中搬运数据到maskReg。输入64个int32类型的cond数据，使用MaskGenWithRegTensor&lt;int32, 0&gt;从condReg的前8字节提取64bit数据，按下采样模式（每4个mask bit对应1个src bit）填充到256bit的MaskReg，输出32字节（256bit）的mask数据到UB(Unified Buffer)。

  - 样例规格：
    <table>
    <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
    <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">cond</td><td align="center">[1, 64]</td><td align="center">int32</td></tr>
    <tr><td rowspan="1" align="center">样例输出</td><td align="center">maskOut</td><td align="center">[32]</td><td align="center">uint8</td></tr>
    <tr><td rowspan="1" align="center">核函数名</td><td colspan="3" align="center">move_reg_scenario_1</td></tr>
    </table>
- 样例实现：
  1. 使用LoadAlign对齐加载cond数据到RegTensor
  2. 通过MaskGenWithRegTensor&lt;int32, 0&gt;从condReg的前8字节提取64bit数据，按下采样模式填充到256bit MaskReg：每4个mask bit对应1个src bit，float元素处理示意如图：  
  <img src="figures/move_reg_1.png">
  3. 使用StoreAlign将mask数据写回UB
- 调用实现
  使用内核调用符`<<<>>>`调用核函数，启动1个核。

### 场景2：复合计算场景
- 样例功能：  
  对根据mask中的比特位，选择对应位置的x或y中的值，当mask的比特位为1时，选取x中对应的元素；当mask的比特位为0时，选取y中对应的元素。
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="4" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td align="center">mask</td><td align="center">[1, 8]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">move_reg_scenario_2</td></tr>
  </table>

- 样例实现：  
  1. 调用LoadAlign加载选择操作数x、y到RegTensor
  2. 调用LoadUnAlignPre/LoadUnAlign加载掩码操作数mask到RegTensor，每次迭代加载2个元素，即8字节64bit数据
  3. 调用MaskGenWithRegTensor从mReg的前8字节提取64bit数据，按下采样模式填充到256bit MaskReg：每4个mask bit对应1个src bit，处理示意如图：  
  <img src="figures/move_reg_1.png">
  4. 调用Select接口，根据maskReg逐元素选择xReg或yReg中的元素，写入zReg中，如下图所示  
  <img src="figures/move_reg_2.png">
  5. 调用StoreAlign将Select数据写回UB

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
  SCENARIO_NUM=1                                                                # 设置场景编号
  mkdir -p build && cd build;                                                   # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM                     # 生成测试输入数据
  ./demo                                                                        # 执行编译生成的可执行程序，执行样例
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式或场景前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　　　　　　　| 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　|
  | ---------------------------| -----------------------------| ---------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE`　　　| `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`　　　　　　　　　| NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`、`2` | 场景编号：1=下采样场景，2=复合计算场景 |

- 执行结果  
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
