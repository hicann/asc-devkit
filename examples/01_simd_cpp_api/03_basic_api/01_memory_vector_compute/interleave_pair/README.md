# Interleave/DeInterleave样例

## 概述

本样例基于Interleave和DeInterleave接口实现元素交织和解交织功能。Interleave将两个源操作数的元素交织存入两个结果操作数，DeInterleave将两个源操作数的元素解交织存入两个结果操作数。样例支持通过编译参数切换不同场景，便于开发者理解这两个接口的使用方法和实现差异。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |

## 目录结构介绍

```
├── interleave_pair
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── interleave_pair.asc     // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 场景详细说明

本样例通过编译参数 `SCENARIO_NUM` 来切换不同的场景：

<table border="2">
<caption>表1：场景配置对照表</caption>
<tr><th>scenarioNum</th><th>接口</th><th>输入Shape</th><th>输出Shape</th><th>数据类型</th><th>说明</th></tr>
<tr><td>1</td><td>Interleave</td><td>[1, 512], [1, 512]</td><td>[1, 512], [1, 512]</td><td>half</td><td>将两个源操作数的元素交织存入两个结果操作数</td></tr>
<tr><td>2</td><td>DeInterleave</td><td>[1, 512], [1, 512]</td><td>[1, 512], [1, 512]</td><td>half</td><td>将两个源操作数的元素解交织存入两个结果操作数</td></tr>
</table>

**场景1：Interleave元素交织**
- 输入shape：src0=[1, 512], src1=[1, 512]
- 输出shape：dst0=[1, 512], dst1=[1, 512]
- 数据类型：half
- 参数：count=512
- 实现：

    ```cpp
    AscendC::Interleave(dst0Local, dst1Local, src0Local, src1Local, count);
    ```

- 说明：将src0和src1中的元素交织存入dst0和dst1。dst0存放src0前半部分和src1前半部分交织的结果，dst1存放src0后半部分和src1后半部分交织的结果
- 示例：
  - 输入src0: [1 2 3 ... 512]
  - 输入src1: [513 514 515 ... 1024]
  - 输出dst0: [1 513 2 514 ... 256 768]
  - 输出dst1: [257 769 258 770 ... 512 1024]

**场景2：DeInterleave元素解交织**
- 输入shape：src0=[1, 512], src1=[1, 512]
- 输出shape：dst0=[1, 512], dst1=[1, 512]
- 数据类型：half
- 参数：count=512
- 实现：

    ```cpp
    AscendC::DeInterleave(dst0Local, dst1Local, src0Local, src1Local, count);
    ```

- 说明：将src0和src1中的元素解交织存入dst0和dst1。dst0存放src0和src1奇数索引位置的元素，dst1存放src0和src1偶数索引位置的元素
- 示例：
  - 输入src0: [1 2 3 ... 512]
  - 输入src1: [513 514 515 ... 1024]
  - 输出dst0: [1 3 5 ... 511 513 515 ... 1023]
  - 输出dst1: [2 4 6 ... 512 514 516 ... 1024]

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
  SCENARIO_NUM=1  # 设置场景编号
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # 编译工程
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py ./output/output_dst0.bin ./output/output_dst1.bin ./output/golden_dst0.bin ./output/golden_dst1.bin  # 验证输出结果是否正确
  ```

  使用CPU调试或NPU仿真模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
  
  示例如下：

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # CPU调试模式
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`（默认） | NPU 架构：dav-3510 对应 Ascend 950PR&950DT系列产品，本样例仅支持此架构 |
  | `SCENARIO_NUM` | `1`（默认）、`2` | 场景编号：1（Interleave交织）、2（DeInterleave解交织） |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
