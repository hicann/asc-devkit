# 标量原子操作样例

## 概述

本样例基于 `AtomicAdd` 和 `AtomicCas` 接口，介绍在 Global Memory（GM）地址上进行标量原子加和标量原子比较交换的实现流程。原子操作可保证多个核并行访问同一内存地址时的数据一致性，避免数据竞争问题。注意，原子操作涉及标量计算单元，如果与搬运单元（MTE2/MTE3）存在数据依赖，需手动插入同步事件。

> **接口提示：** 除本样例使用的 `AtomicAdd`、`AtomicCas` 接口外，Ascend C 还提供了 `AtomicExch`、`AtomicMax`、`AtomicMin` 接口，调用方式与 `AtomicAdd` 一致，只需替换函数名即可切换。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |

## 目录结构介绍

```
├── scalar_atomic_operations
│   ├── scripts
│   │   ├── gen_data.py               // 输入数据和真值数据生成脚本
│   │   └── verify_result.py          // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt                // 编译工程文件
│   ├── data_utils.h                  // 数据读入写出函数
│   ├── scalar_atomic_operations.asc  // Ascend C样例实现 & 调用样例
│   └── README.md                     // 样例说明文档
```

## 场景详细说明

本样例通过编译参数 `SCENARIO_NUM` 来切换不同的场景：

**场景1：AtomicAdd原子加操作**  
  三个核并行执行，每个核对GM的首个元素（dst[0]）原子加1。最终dst[0]的值为初始值加3

**场景2：AtomicCas原子比较交换操作**  
  三个核并行执行，每个核检查GM首个元素是否等于expected值1，若相等则替换为newValue值2，不相等则不修改。

<table border="2">
<caption>表1：场景配置对照表</caption>
<tr><th>scenarioNum</th><th>原子操作接口</th><th>输出Shape</th><th>数据类型</th><th>说明</th></tr>
<tr><td>1</td><td>AtomicAdd</td><td>[1, 256]</td><td>int32</td><td>三个核并行对GM首个元素进行原子加1操作</td></tr>
<tr><td>2</td><td>AtomicCas</td><td>[1, 256]</td><td>uint32</td><td>三个核并行对GM首个元素进行原子比较交换（如果值为1则替换为2）</td></tr>
</table>

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
  SCENARIO_NUM=1  # 设置场景编号（取值为1、2）
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # 编译工程
  python3 ../scripts/gen_data.py -scenarioNum $SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin -scenarioNum $SCENARIO_NUM  # 验证输出结果是否正确
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
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`（默认） | NPU 架构：AtomicAdd和AtomicCas仅支持dav-3510（对应 Ascend 950PR&950DT系列产品） |
  | `SCENARIO_NUM` | `1`（默认）、`2` | 场景编号：1（AtomicAdd原子加）、2（AtomicCas原子比较交换） |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
