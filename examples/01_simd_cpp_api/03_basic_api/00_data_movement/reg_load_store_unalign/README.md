# ld_st_reg_unalign样例

## 概述
本样例基于Reg编程接口实现UB(Unified Buffer)对RegTensor(Reg矢量计算基本单元)的非对齐数据搬运操作，支持多种场景，通过 CMake 编译参数 `SCENARIO_NUM` 选择场景。
    <table>
	 	<tr>
        <td>SCENARIO_NUM</td>
	 		<td>场景类型</td>
	 	</tr>
	 	<tr>
	 		<td>1</td>
	 		<td>单核非对齐数据搬运：使用LoadUnAlign和StoreUnAlign进行非对齐地址的数据搬运</td>
	 	</tr>
	 	<tr>
	 		<td>2</td>
	 		<td>多核非对齐场景计算及搬运：使用LoadUnAlign处理非对齐输入(uint16_t)，ReduceDataBlock(MAX)进行块内求最大值</td>
	 	</tr>
	 </table>


## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍
```
├── reg_load_store_unalign
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── ld_st_reg_unalign.asc          // AscendC样例实现 & 调用样例
│   └── README.md                      // 样例介绍
```

## 样例描述

### 场景1：单核地址非对齐数据搬运
- 样例功能：  
  输入shape为[1, 1024]的float类型矩阵，在源操作数和目的操作数地址均非32B对齐的情况下，搬运128个数据。
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">y</td><td align="center">[1, 128]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">ld_st_reg_unalign_kernel</td></tr>
  <tr><td rowspan="1" align="center">核数</td><td colspan="4" align="center">1</td></tr>
  </table>
- 样例实现：  
   将UB上源操作数和目的操作数地址偏移一个元素（4 Byte）长度，使得起始地址非32B对齐，调用CopyVF从该地址开始搬运128个float数，写回UB(Unified Buffer)。

### 场景2：多核地址非对齐、数据量不对齐场景搬运
- 样例功能：  
  输入shape为[14, 255]的uint16_t类型矩阵，展示LoadUnAlign处理非对齐输入的自然使用场景。使用ReduceDataBlock对每个DataBlock（16个uint16_t元素）求最大值，4核并行处理，StoreUnAlign连续存储结果。
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[14, 255]</td><td align="center">uint16_t</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">y</td><td align="center">[14, 16]</td><td align="center">uint16_t</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">ld_st_reg_unalign_kernel</td></tr>
  <tr><td rowspan="1" align="center">核数</td><td colspan="4" align="center">4</td></tr>
  </table>
- 样例实现：
  1. **数据搬入**：
     - 0、1、2核搬运4×255=1020个元素，3核搬运2×255=510个元素，连续整块搬入UB
     - UB紧密排列，每行间隔255个元素
     - 行1-3起始地址非32B对齐（510字节、1020字节、1530字节）

  2. **LoadUnAlign自然使用**：
     - 所有行统一使用LoadUnAlignPre + LoadUnAlign处理对齐地址（行0）和非对齐地址（行1-3）
     - 对齐地址也可使用StoreAlign接口，当前仅为保证代码格式统一，用作样例演示，不作为极致性能参考

  3. **ReduceDataBlock计算**：
     - 每行255元素分2个VL块处理（128 + 127）
     - 每个VL块产生8个结果（每DataBlock求最大值）

  4. **StoreUnAlign连续存储**：
     - 使用带postUpdateStride的StoreUnAlign版本
     - 每次存储偏移8个元素（resultsPerRepeat=8）
     - 循环2次后自动偏移到dstAddr+16
     - StoreUnAlignPost处理最后的边界数据

  5. **输出布局**：
     - 每行16个结果，数据连续排列
     - 核0、核1、核2输出64个数（GM[0-63]、GM[64-127]、GM[128-191]），核3输出32个数（GM[192-223]）
   
- 调用实现  
  使用内核调用符<<<>>>调用核函数，启动4个核。

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
  SCENARIO_NUM=1
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式或场景前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　|
  | ----------------| -----------------------------| ---------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`、`2` | 场景编号：1=单核地址非对齐数据搬运，2=多核地址非对齐、数据量非对齐数据搬运 |

- 执行结果  
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
