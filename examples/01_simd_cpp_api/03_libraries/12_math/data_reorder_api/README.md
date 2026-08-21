# Data Reorder Tensor API 样例

## 概述

## 概述
本样例基于Reg编程接口实现数据重排功能，支持多种场景，通过 CMake 编译参数 `SCENARIO_NUM` 选择场景。
    <table>
 	  	 	<tr>
        <td>SCENARIO_NUM</td>
 	  	 		<td>重排场景</td>
 	  	 	</tr>
 	  	 	<tr>
 	  	 		<td>1</td>
 	  	 		<td>Interleave（将两个uint16_t向量交织）</td>
 	  	 	</tr>
 	  	 	<tr>
 	  	 		<td>2</td>
 	  	 		<td>Pack（从uint32_t向量中提取低16位到uint16_t向量）</td>
 	  	 	</tr>
 	  	 </table>

## 目录结构

```text
data_reorder_api
├── scripts/gen_data.py
├── CMakeLists.txt
├── data_reorder_api.asc
├── data_utils.h
└── README.md
```

## 编译运行

在本目录执行：

```bash
SCENARIO_NUM=1
mkdir -p build && cd build
cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..
make -j
python3 ../scripts/gen_data.py -scenarioNum=${SCENARIO_NUM}
./demo
```

场景 2 将 `SCENARIO_NUM` 改为 `2`。CPU 调试或 NPU 仿真时，在 CMake 命令中分别增加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim`。切换编译场景或运行模式前，应清理 CMake 缓存并重新配置。

运行成功时输出：

```text
test pass!
```
