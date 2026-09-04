# DataCopy量化激活搬运样例

## 概述

本样例基于DataCopy实现数据随路量化激活搬运，将矩阵乘的结果从L0C Buffer搬运到GM（Global Memory），并支持随路NZ2ND、随路量化、relu激活能力组合。本样例给出了6种不同的测试场景（scenario）。
    <table>
 	  	 	<tr>
 	  	 		<td>scenarioNum</td>
          <td>输出格式</td>
 	  	 		<td>量化模式</td>
          <td>类型转换</td>
          <td>激活模式</td>
 	  	 	</tr>
 	  	 	<tr>
 	  	 		<td>1</td>
          <td>ND</td>
 	  	 		<td>Scalar量化模式</td>
          <td>s322f16</td>
          <td>relu激活</td>
 	  	 	</tr>
 	  	 	<tr>
 	  	 		<td>2</td>
          <td>NZ</td>
 	  	 		<td>Vector量化模式</td>
          <td>s322f16</td>
          <td>无</td>
 	  	 	</tr>
        <tr>
 	  	 		<td>3</td>
          <td>NZ</td>
 	  	 		<td>Scalar量化模式</td>
          <td>f322s8</td>
          <td>无</td>
 	  	 	</tr>
        <tr>
 	  	 		<td>4</td>
          <td>ND</td>
 	  	 		<td>Vector量化模式</td>
          <td>f322s8</td>
          <td>relu激活</td>
 	  	 	</tr>
        <tr>
 	  	 		<td>5</td>
          <td>ND</td>
 	  	 		<td>Scalar量化模式</td>
          <td>s322s8</td>
          <td>无</td>
 	  	 	</tr>
        <tr>
 	  	 		<td>6</td>
          <td>NZ</td>
 	  	 		<td>Vector量化模式</td>
          <td>s322s8</td>
          <td>relu激活</td>
 	  	 	</tr>
 	  	 </table>

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.2.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

```
├── data_copy_l0c2gm
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── data_copy_l0c2gm.asc    // Ascend C样例实现 & 调用样例
│   ├── README.md               // 中文样例说明文档
│   └── README_en.md            // 英文样例说明文档
```

## 样例描述

- 样例功能  
  将矩阵乘的结果从L0C Buffer搬运到GM（Global Memory），支持以下两种随路量化模式：
  - Scalar量化：使用SetFixpipePreQuantFlag接口设置Scalar量化参数。
  - Vector量化：使用SetFixPipeConfig接口设置Vector量化参数。
  并支持与随路NZ2ND、relu激活能力组合。接口资料参考随路量化激活搬运。

- 样例规格

  <table>
  <caption>场景1：Scalar量化+relu激活</caption>
  <tr><td rowspan="4" align="center">样例输入</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[128, 128]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[128, 256]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td></tr>
  <tr><td align="center">z</td><td align="center">[128, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">data_copy_l0c2gm</td></tr>
  </table>

  <table>
  <caption>场景2：Vector量化</caption>
  <tr><td rowspan="4" align="center">样例输入</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[128, 128]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[128, 256]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td></tr>
  <tr><td align="center">z</td><td align="center">[128, 256]</td><td align="center">half</td><td align="center">NZ</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">data_copy_l0c2gm</td></tr>
  </table>

  <table>
  <caption>场景3：Scalar量化</caption>
  <tr><td rowspan="4" align="center">样例输入</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[128, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[128, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td></tr>
  <tr><td align="center">z</td><td align="center">[128, 256]</td><td align="center">int8_t</td><td align="center">NZ</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">data_copy_l0c2gm</td></tr>
  </table>

  <table>
  <caption>场景4：Vector量化+relu激活</caption>
  <tr><td rowspan="4" align="center">样例输入</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[128, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[128, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td></tr>
  <tr><td align="center">z</td><td align="center">[128, 256]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">data_copy_l0c2gm</td></tr>
  </table>

  <table>
  <caption>场景5：Scalar量化</caption>
  <tr><td rowspan="4" align="center">样例输入</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[128, 128]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[128, 256]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td></tr>
  <tr><td align="center">z</td><td align="center">[128, 256]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">data_copy_l0c2gm</td></tr>
  </table>

  <table>
  <caption>场景6：Vector量化+relu激活</caption>
  <tr><td rowspan="4" align="center">样例输入</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[128, 128]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[128, 256]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td></tr>
  <tr><td align="center">z</td><td align="center">[128, 256]</td><td align="center">int8_t</td><td align="center">NZ</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">data_copy_l0c2gm</td></tr>
  </table>

- 样例实现

  - Kernel实现

    - 调用DataCopy基础API，将数据从GM（Global Memory）搬运到A1（L1 Buffer）和B1（L1 Buffer），并做ND到NZ的格式转换。

    - 调用LoadData接口，将数据从A1（L1 Buffer）和B1（L1 Buffer）搬运到A2（L0A Buffer）和B2(L0B Buffer)。

    - 调用Mmad接口，将输入shape[128, 128]的矩阵A和输入shape[128, 256]的矩阵B做矩阵乘法，得到输出shape[128, 256]的结果矩阵。

    - 配置DataCopyCO12DstParams参数用于DataCopy随路量化激活搬运，将Mmad计算出的结果从L0C Buffer搬出到GM（Global Memory）。

  - 调用实现  
    使用内核调用符`<<<>>>`调用核函数。

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
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM ./output/output.bin ./output/golden.bin  # 验证输出结果是否正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
  
  示例如下：
  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　　　　　　　　　　　　　　　　　　 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品 与 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`、`2`、`3`、`4`、`5`、`6`　　　　　| 场景编号：1=Scalar量化+relu激活+ND输出，2=Vector量化+NZ输出，3=Scalar量化+NZ输出，4=Vector量化+relu激活+ND输出，5=Scalar量化+ND输出，6=Vector量化+relu激活+NZ输出 |

- 执行结果  
  执行结果如下，说明精度对比成功：
  ```bash
  test pass!
  ```
