# reg_load_gather样例

## 概述
本样例基于Reg编程接口实现将离散数据从UB(Unified Buffer)搬入到RegTensor(Reg矢量计算基本单元)的功能，支持两种场景，通过 CMake 编译参数 `SCENARIO_NUM` 选择场景。
    <table>
  	 	 	<tr>
        <td>SCENARIO_NUM</td>
  	 	 		<td>搬入场景</td>
  	 	 	</tr>
  	 	 	<tr>
  	 	 		<td>1</td>
  	 	 		<td>Gather（从UB按索引将单点数据收集到RegTensor）</td>
  	 	 	</tr>
  	 	 	<tr>
  	 	 		<td>2</td>
  	 	 		<td>GatherB（从UB按索引将DataBlock大小的数据收集到RegTensor）</td>
  	 	 	</tr>
  	 </table>

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍
```
├── reg_load_gather
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── gather_ld_reg.asc              // AscendC样例实现 & 调用样例
│   └── README.md                      // 样例介绍
```

## 样例描述
- 样例功能：  
  根据索引从源数据中离散收集元素到目的地址。

  **场景1：Gather模式**
  - 使用Gather接口，从UB按元素索引收集数据到RegTensor
  - 按元素收集：dst[i] = src[index[i]]
  - 源数据量为1024个元素，索引数据量为128个元素，输出数据量为128个元素。
  - 样例规格：
    <table>
    <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
    <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">src</td><td align="center">[1, 1024]</td><td align="center">half</td></tr>
    <tr><td align="center">index</td><td align="center">[1, 128]</td><td align="center">uint16_t</td></tr>
    <tr><td rowspan="1" align="center">样例输出</td><td align="center">dst</td><td align="center">[1, 128]</td><td align="center">half</td></tr>
    <tr><td rowspan="1" align="center">核函数名</td><td colspan="3" align="center">gather_ld_reg</td></tr>
    </table>
  - 样例实现：
    GatherVF函数内先通过LoadAlign加载索引数据到indexReg，再调用Gather接口从源地址按索引收集数据到dstReg，最后通过StoreAlign将结果写回UB。
    - 调用实现
      使用内核调用符`<<<>>>`调用核函数，启动1个核。

  **场景2：GatherB模式**
  - 使用GatherB接口，按32字节DataBlock收集
  - 源数据量为1024个元素，索引数据量为8个元素（对应8个DataBlock），输出数据量为128个元素。
  - 样例规格：
    <table>
    <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
    <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">src</td><td align="center">[1, 1024]</td><td align="center">half</td></tr>
    <tr><td align="center">index</td><td align="center">[1, 8]</td><td align="center">uint32_t</td></tr>
    <tr><td rowspan="1" align="center">样例输出</td><td align="center">dst</td><td align="center">[1, 128]</td><td align="center">half</td></tr>
    <tr><td rowspan="1" align="center">核函数名</td><td colspan="3" align="center">gather_ld_reg</td></tr>
    </table>
  - 样例实现：
    GatherBVF函数内先通过LoadAlign加载索引数据到indexReg，再调用GatherB接口按DataBlock从源地址收集数据到dstReg，最后通过StoreAlign将结果写回UB。
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
  SCENARIO_NUM=1                                                                 # 选择执行场景（1=Gather，2=GatherB）
  mkdir -p build && cd build;                                                    # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;  # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py -scenarioNum $SCENARIO_NUM                      # 生成测试输入数据
  ./demo                                                                         # 执行编译生成的可执行程序，执行样例
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # cpu调试模式
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式或场景前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `SCENARIO_NUM` | 1、2 | 样例执行场景：场景1=Gather模式、场景2=GatherB模式 |
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
