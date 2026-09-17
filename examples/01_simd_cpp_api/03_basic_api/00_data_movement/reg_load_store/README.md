# Load和Store样例

## 概述

本样例演示`Load`和`Store`易用性接口对`RegTraitNumOne`和`RegTraitNumTwo`的支持。通过CMake编译参数`SCENARIO_NUM`选择场景，分别覆盖`Store`不带`count`和带`count`的两个重载。

| SCENARIO_NUM | RegTensor类型 | Load接口 | Store接口 | 单次搬运量 | 调用次数 | 总搬运量 |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | `RegTensor<uint64_t, RegTraitNumOne>` | `Load(srcReg, srcAddr)` | `Store(dstAddr, srcReg)` | 32个`uint64_t`元素（256B） | 2 | 64个`uint64_t`元素（512B） |
| 2 | `RegTensor<uint64_t, RegTraitNumOne>` | `Load(srcReg, srcAddr)` | `Store(dstAddr, srcReg, count)`，`count`为32 | 32个`uint64_t`元素（256B） | 2 | 64个`uint64_t`元素（512B） |
| 3 | `RegTensor<uint64_t, RegTraitNumTwo>` | `Load(srcReg, srcAddr)` | `Store(dstAddr, srcReg)` | 64个`uint64_t`元素（512B） | 1 | 64个`uint64_t`元素（512B） |
| 4 | `RegTensor<uint64_t, RegTraitNumTwo>` | `Load(srcReg, srcAddr)` | `Store(dstAddr, srcReg, count)`，`count`为64 | 64个`uint64_t`元素（512B） | 1 | 64个`uint64_t`元素（512B） |

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍

```text
reg_load_store/
├── scripts/
│   └── gen_data.py        // 输入数据和真值数据生成脚本
├── CMakeLists.txt         // 编译工程文件
├── data_utils.h           // 数据读入写出函数
├── load_store.asc         // Ascend C样例实现和调用入口
├── README.md              // 中文说明
└── README_en.md           // 英文说明
```

## 样例描述

输入和输出缓冲区均为68个`uint64_t`元素，其中包含64个有效搬运元素、1个用于构造非对齐地址的前置元素，以及3个用于保证缓冲区总长度32B对齐的尾部填充元素。样例按照以下流程完成数据搬运和精度验证：

1. 将输入数据从GM搬入UB，并将输出UB初始化为0。
2. 将输入和输出UB地址分别偏移一个`uint64_t`元素（8B），构造起始地址非32B对齐的场景。
3. 调用`Load`将数据从UB搬入RegTensor。
4. 根据`SCENARIO_NUM`调用对应的SIMD VF，将数据从RegTensor搬回UB。
5. 将结果从UB搬回GM，并与真值数据进行比较。

四个场景的总搬运量均为512B。`RegTraitNumOne`场景单次搬运一个VL（256B），循环调用两次；`RegTraitNumTwo`场景单次搬运两个VL（512B），调用一次。`Load`和`Store`内部完成非对齐搬运所需的预处理和后处理。

## 编译运行

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`。

- 执行样例

  在本样例目录下执行以下命令。默认使用NPU模式：

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  ```

  使用CPU调试或NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=cpu`或`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：
  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # cpu调试模式
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换运行模式或场景前，需要删除`build`目录中的`CMakeCache.txt`，再重新执行CMake和编译命令。

- 编译参数说明

  | 参数 | 可选值 | 说明 |
  | --- | --- | --- |
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 分别表示NPU执行、CPU调试和NPU仿真模式。 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | `dav-3510`对应Ascend 950PR/Ascend 950DT。 |
  | `SCENARIO_NUM` | `1`、`2`、`3`、`4` | 场景编号，具体含义参见概述中的场景表。 |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
