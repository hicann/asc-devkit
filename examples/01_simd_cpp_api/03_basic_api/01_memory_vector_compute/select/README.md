# Select类样例

## 概述

本样例基于Select接口完成多种场景下的数据选择功能，根据`selMask`掩码在两个向量或向量与标量之间选取元素写入目的向量。选择的规则为：当selMask的比特位是1时，从src0中选取，比特位是0时从src1选取。

样例支持通过编译参数切换不同场景，便于开发者理解Select接口的使用方法和实现差异。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── select
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── select.asc              // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 场景详细说明

  **场景1：Select**

  - 说明：根据selMask在两个tensor中选取元素。每轮迭代中，根据selMask的有效位数据（限制为256/sizeof(T)个有效位数，T为输入数据类型，本样例中T为float）进行选择操作。
  - 实现：
    ```cpp
    // cmpMode = AscendC::SELMODE::VSEL_CMPMASK_SPR
    // src0和src1都是tensor。当selMask的比特位是1时，从src0中选取；比特位是0时,从src1选取
    AscendC::Select(dstl, selMask, src0, src1, cmpMode, count);
    ```
  
  **场景2：Select（标量）**

  - 说明：根据selMask在1个tensor和1个scalar标量中选取元素，selMask无有效数据限制。多轮迭代时，每轮迭代连续使用selMask的不同部分。
  - 实现：
    ```cpp
    // cmpMode = AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE
    // src0是tensor，src1是标量。当selMask的比特位是1时，从src0中选取；比特位是0时，等于src1
    AscendC::Select(dst, selMask, src0, src1, cmpMode, count);
    ```

  **场景3：Select（selMask无有效位数据限制）**

  - 说明：根据selMask在两个tensor中选取元素，selMask无有效数据限制。多轮迭代时，每轮迭代连续使用selMask的不同部分。
  - 实现：
    ```cpp
    // cmpMode = AscendC::SELMODE::VSEL_TENSOR_TENSOR_MODE
    // src0和src1都是tensor。当selMask的比特位是1时，从src0中选取；比特位是0时,从src1选取
    AscendC::Select(dst, selMask, src0, src1, cmpMode, count);
    ```

**场景4：Select（灵活标量位置）----此场景仅在 Ascend 950PR&950DT系列产品支持**

  - 说明：与场景2功能类似，只是标量位置更灵活。
  - 实现：
    ```cpp
    // cmpMode = AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE
    // 标量在后。src0是tensor，src1是标量。当selMask的比特位是1时，从src0中选取；比特位是0时，等于src1
    static constexpr AscendC::BinaryConfig config = { 1 };
    AscendC::Select<AscendC::BinaryDefaultType, uint8_t, config>(dst, selMask, src0, src1, cmpMode, count);

    // 标量在前。src0是标量，src1是tensor。当selMask的比特位是1时，等于src0；比特位是0时，从src1中选取
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Select<AscendC::BinaryDefaultType, uint8_t, config>(dst, selMask, src0, src1, cmpMode, count); 
    ```

- 场景规格

  样例可通过编译参数 `SCENARIO_NUM` 来切换不同的场景，参数详见下表：

  | 场景编号 | 场景名称 | src0 shape |src0数据类型 | src1 shape | src1数据类型 | selMask shape |selMask数据类型 | dst shape | dst数据类型 |
  |------|------|------|------|------|------|------|------|------|------|
  | 1 | Select | [1, 256] | float | [1, 256] | float | [1, 8]有效位数限制 | uint8_t |[1, 256] | float |
  | 2 | Select（标量） | [1, 256] | float | 标量 | float | [1, 32] | uint8_t |[1, 256] | float |
  | 3 | Select（selMask无有效位数据限制） | [1, 256] | float | [1, 256] | float | [1, 32] | uint8_t |[1, 256] | float |
  | 4 | Select（灵活标量位置） | [1, 256] | float | 标量 | float | [1, 32] | uint8_t |[1, 256] | float |

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
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # 编译工程
  python3 ../scripts/gen_data.py -scenario_num=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin  # 验证输出结果是否正确
  ```

  使用CPU调试或NPU仿真模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
  示例如下：
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # CPU调试模式
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU仿真模式
  ```
  
  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |
  | `SCENARIO_NUM` | `1`（默认）、`2`、`3`、`4` | 场景编号：1 对应 Select、2 对应 Select（标量）、3 对应 Select（selMask无有效位数据限制）、4 对应 Select（灵活标量位置） |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
