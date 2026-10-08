# Gather类样例

## 概述

本样例基于GatherMask、Gather、Gatherb等接口完成多种场景模式下的数据选择功能，包括内置固定模式、用户自定义模式、张量偏移模式、DataBlock偏移模式，实现从源操作数中选取元素写入目的操作数。样例支持通过编译参数切换不同场景，便于开发者理解Gather类接口的使用方法和实现差异。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── gather
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── gather.asc              // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 场景详细说明
本样例通过编译参数`SCENARIO_NUM` 来切换不同的掩码生成场景：

**场景1：内置固定模式**
- 说明：通过`src1Pattern`选择对应的二进制作为掩码，来获取数据
- 输入：src0Local=[1, 256]
- 输出：[1, 256]
- 数据类型：uint32_t
- 实现：
    ```cpp
    AscendC::GatherMask(dstLocal, src0Local, src1Pattern, reduceMode, mask, gatherMaskParams, rsvdCnt);
    ```
- 参数：dstLocal和src0Local采用地址复用，并使用内置固定模式src1Pattern=2进行元素选取，reduceMode=false（Normal模式），mask=0，gatherMaskParams={1, 4, 8, 0}

**场景2：用户自定义模式**
- 说明：通过用户输入的`src1Local`对应的二进制作为掩码，来获取数据
- 输入：src0Local=[1, 256], src1Local=[1, 32]
- 输出：[1, 256]
- 数据类型：uint32_t
- 实现：
    ```cpp
    AscendC::GatherMask (dstLocal, src0Local, src1Local, reduceMode, mask, gatherMaskParams, rsvdCnt);
    ```
- 参数：使用用户提供的Tensor进行元素选取，reduceMode=true（Counter模式），mask=70，gatherMaskParams={1, 2, 4, 0}

**场景3：张量偏移模式**
- 说明：根据用户输入的地址偏移张量`srcOffset`进行地址偏移，来获取数据
- 输入：src0Local=[1, 128], srcOffset=[1, 128]
- 输出：[1, 128]
- 数据类型：输入输出uint16_t，srcOffset类型为uint32_t
- 实现：
    ```cpp
    AscendC::Gather(dstLocal, src0Local, srcOffset, srcBaseOffset, count);
    ```
- 参数：使用用户提供的srcOffset按元素地址偏移，srcBaseOffset=0表示源操作数的起始地址，count=128表示执行处理的数据个数

**场景4：DataBlock偏移模式**
- 说明：根据用户输入的地址偏移张量`srcOffset`（按照DataBlock的粒度）进行地址偏移，来获取数据
- 输入：src0Local=[1, 128], srcOffset=[1, 8]
- 输出：[1, 128]
- 数据类型：输入输出uint16_t，srcOffset类型为uint32_t
- 实现：
    ```cpp
    AscendC::Gatherb<T>(dstLocal, src0Local, srcOffset, repeatTime, params);
    ```
- 参数：用户输入的srcOffset是每个datablock在源操作数中对应的地址偏移，repeatTime=1表示重复迭代次数，params={1,8}

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
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin $SCENARIO_NUM  # 验证输出结果是否正确
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
  | `SCENARIO_NUM` | `1`（默认）、`2`、`3`、`4` | 场景编号：1（内置固定模式）、2（用户自定义模式）、3（张量偏移模式）、4（DataBlock偏移模式） |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
