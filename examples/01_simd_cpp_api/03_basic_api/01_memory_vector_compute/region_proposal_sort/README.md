# Region Proposal排序流水线示例

## 概述

本样例基于ProposalConcat、RpSort16、MrgSort4和ProposalExtract四个接口实现完整排序流水线。首先通过ProposalConcat将连续score值合入Region Proposal格式，再经RpSort16对每组16个Region Proposal按score域降序排列，然后通过MrgSort4将4组已排序的Region Proposal合并为1组，最后通过ProposalExtract从合并后的Region Proposal中提取score域，得到全局降序排列的连续score值。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Atlas推理系列产品AI Core | >= CANN 9.0.0 |

## 目录结构介绍

```
├── region_proposal_sort
│   ├── scripts
│   │   ├── gen_data.py                    // 输入数据和真值数据生成脚本
│   │   └── verify_result.py               // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt                     // 编译工程文件
│   ├── data_utils.h                       // 数据读入写出函数
│   ├── region_proposal_sort.asc           // Ascend C样例实现 & 调用样例
│   └── README.md                          // 样例说明文档
```

## 样例描述

- 样例功能：  
  将64个score值通过完整流水线排序后输出。一个Region Proposal由8个half元素组成，其中第5个元素（offset=4）为score域。

- 样例规格：

<table border="2">
<caption>表1：Region Proposal排序样例规格</caption>
<tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 64]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">y</td><td align="center">[1, 64]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">region_proposal_sort_custom</td></tr>
</table>

- 样例实现：  
  本样例实现的是Region Proposal排序完整流水线：
  1. **ProposalConcat**：将连续的16个score值合入16个Region Proposal的score域位置（offset=4），共4组，每组1次repeat
  2. **RpSort16**：对每组16个Region Proposal按score域从大到小排序，共4组
  3. **MrgSort4**：将4组已排序的Region Proposal合并为1组，结果按score降序排列
  4. **ProposalExtract**：从合并后的64个Region Proposal中提取score域，得到64个降序排列的连续score值

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
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2002 ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin  # 验证输出结果是否正确
  ```

  使用CPU调试或NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=cpu`或`-DCMAKE_ASC_RUN_MODE=sim`参数即可。
  
  示例如：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2002 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2002 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2002`（默认） | NPU 架构：dav-2002 对应 Atlas推理系列产品AI Core |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
