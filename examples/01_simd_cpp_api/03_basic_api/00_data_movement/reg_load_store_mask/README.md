# ld_st_reg_mask样例

## 概述
本样例基于Reg编程接口实现UB(Unified Buffer)对MaskReg(掩码寄存器)的搬入搬出，以及使用mask进行掩码搬出的操作。支持多种场景，通过 CMake 编译参数 `SCENARIO_NUM` 选择场景。
    <table>
	 	<tr>
        <td>SCENARIO_NUM</td>
	 		<td>场景类型</td>
	 	</tr>
	 	<tr>
	 		<td>1</td>
	 		<td>基础搬运场景：使用LoadAlign、StoreAlign等接口，实现MaskReg的搬入搬出</td>
	 	</tr>
	 	<tr>
	 		<td>2</td>
	 		<td>复合计算场景：使用LoadAlign实现MaskReg的搬入，再作为Select的掩码完成选择计算</td>
	 	</tr>
	 </table>

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍
```
├── reg_load_store_mask
│   ├── scripts
│   │   └── gen_data.py                // 输入数据和真值数据生成脚本
│   ├── figures                        // 图示
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── README.md                      // 样例介绍
│   ├── ld_st_reg_mask_scenario_1.asc  // 场景1：基础搬运场景的AscendC实现
│   └── ld_st_reg_mask_scenario_2.asc  // 场景2：复合计算场景的AscendC实现
```

## 样例描述

### 场景1：基础搬运场景
- 样例功能：  
  - 数据搬入：取输入矩阵的前256bit数据，调用LoadAlign接口将数据从UB搬入MaskReg。
  - 数据搬出：将MaskReg的比特位全部置为1，调用StoreAlign接口将数据从MaskReg搬出至UB。
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">uint8_t</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">y</td><td align="center">[1, 1024]</td><td align="center">uint8_t</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">ld_st_reg_mask_scenario_1</td></tr>
  </table>
- 样例实现：  
<p align="center">
  <img src="figures/ld_st_reg_mask.png" width="1000">
</p>

  1. 在CopyVF函数里，调用LoadAlign接口从UB搬运256bit(32*uint8_t)数据到MaskReg，以实现mask的动态设置。本样例中，32个uint8_t数据设置为1,0,...,1，即第一个数和最后一个数为1(b'00000001)，因为芯片每次从低位读取数字，所以这32个数字最终填充到MaskReg中为b'1000...1...000。
  2. 调用Duplicate接口进行数据填充，MaskReg可以指示计算过程中参与计算的元素，由step1可知MaskReg中第1个bit和第249个bit为1，使用该掩码将只填充RegTensor第1个数和第249个数为2。
  3. 使用StoreAlign接口将RegTensor内的结果保存到UB。
  4. 将MaskReg中的bit位全部置为1，通过StoreAlign接口将MaskReg中的数据保存到UB(地址=step3中的保存地址+256B)，实现MaskReg数据存储在UB上的功能。对应32个uint8_t数，每个数的每位比特位都为1，因此每个数的值都是255(0xFF)。

### 场景2：复合计算场景
- 样例功能：  
  对根据maskReg中的比特位，选择对应位置的xReg或yReg向量中的值，当mask的比特位为1时，选取src0中对应的元素；当mask的比特位为0时，选取src1中对应的元素。
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="4" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td align="center">mask</td><td align="center">[1, 32]</td><td align="center">uint8_t</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">ld_st_reg_mask_scenario_2</td></tr>
  </table>
- 样例实现：  
   SelectVF函数内，调用LoadAlign接口将掩码数据从UB搬入MaskReg，再传入Select接口进行计算，结果写回UB。

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

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　　　　　　　| 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　|
  | ---------------------------| -----------------------------| ---------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE`　　　| `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`　　　　　　　　　| NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`、`2` | 场景编号：1=基础搬运场景，2=复合计算场景 |

- 执行结果  
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
