# reg_sync样例

## 概述
本样例基于Reg编程接口实现UB(Unified Buffer)读或写操作的同步机制，支持多种场景，通过 CMake 编译参数 `SCENARIO_NUM` 选择场景。
    <table>
  	 	<tr>
        <td>SCENARIO_NUM</td>
 	 		<td>同步场景</td>
 	 	</tr>
 	 	<tr>
 	 		<td>1</td>
 	 		<td>寄存器保序</td>
 	 	</tr>
 	 	<tr>
 	 		<td>2</td>
 	 		<td>LocalMemBar（写读依赖）</td>
 	 	</tr>
 	 </table>


## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍

```
├── reg_sync
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── reg_sync.asc                   // AscendC算子实现 & 调用样例
│   └── README.md                      // 样例介绍
```

## 样例描述

### 场景1：寄存器保序

**样例功能**：对输入向量 x 进行原地 exp 计算，结果写回同一地址。

**样例规格**：
<table>
<tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
<tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
<tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">float</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1, 1024]</td><td align="center">float</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="3" align="center">reg_sync</td></tr>
</table>

- 样例实现  
1. 调用LoadAlign接口，从UB地址 dataAddr 读取数据，写入 reg0 中
2. 对 reg0 进行 Exp 计算，结果也写入 reg0 中
3. 调用StoreAlign接口，将计算后的 reg0 写回相同的 dataAddr

- 同步说明
1. 上述样例实现的步骤1、步骤3中，LoadAlign和StoreAlign操作同一地址，需要保证先读UB、后写UB的时序
2. 因为读UB、写UB使用的是同一寄存器 reg0，所以触发了硬件的寄存器保序，指令会按代码顺序执行，不需要插入同步指令

- 调用实现  
  使用内核调用符`<<<>>>`调用核函数，启动1个核。

### 场景2：LocalMemBar（写读依赖）

**样例功能**：对输入向量 x 进行 softmax 计算

**样例规格**：
<table>
<tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
<tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
<tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">float</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1, 1024]</td><td align="center">float</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="3" align="center">reg_sync</td></tr>
</table>

- 样例实现 
 
1. 对全部输入数据 x 求最大值，将最大值填充满 maxReg
2. 依次计算输入数据和最大值的差值，并对差值取自然指数，即 expReg = exp(srcReg - maxReg)
3. 调用StoreAlign接口，将 exp 结果搬出至UB的临时缓冲区（tmpLocal）中
4. 对全部的 exp 结果求和，将和填充满 sumReg
5. 调用LoadAlign接口，从UB临时缓冲区（tmpLocal）读取 exp 结果
6. 依次计算 exp 除 sumReg，结果搬出

- 同步说明

1. 上述样例实现的步骤3、步骤5中，StoreAlign和LoadAlign操作同一地址，存在写后读依赖（RAW），需要保证先写UB、后读UB的时序
2. 手动调用LocalMemBar接口，插入 VEC_STORE 和 VEC_LOAD 之间的同步，以保证指令按代码顺序执行
3. 若缺少LocalMemBar，步骤5可能先执行，从 tmpLocal 读到未经步骤3更新的旧数据

- 调用实现  
  使用内核调用符`<<<>>>`调用核函数，启动1个核。

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
  SCENARIO_NUM=1
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
  | `SCENARIO_NUM` | `1`、`2`　　　　　　　　　　| 场景编号：1=寄存器保序，2=LocalMemBar　　　　　　　|

- 执行结果  
  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
