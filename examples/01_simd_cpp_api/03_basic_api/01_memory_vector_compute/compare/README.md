# Compare类样例


## 概述

本样例基于Compare、Compares接口完成多场景下的数据比较功能，实现逐元素大小比较。如果比较结果为真，则输出结果的对应比特位为1，否则为0。并且比较结果以8位压缩方式存储，每8个比较结果打包成一个字节（uint8_t/int8_t）。

样例支持通过编译参数切换不同场景，便于开发者理解Compare类接口的使用方法和实现差异。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── compare
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── compare.asc             // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 场景详细说明
本样例通过编译参数`SCENARIO_NUM` 来切换不同的场景：

**场景1：Compare**
- 说明：逐元素比较`src0Local`和`src1Local`两个tensor大小
- 输入：src0Local=[1, 256], src1Local=[1, 256]
- 输入数据类型：float
- 输出：dstLocal=[1, 32]
- 输出数据类型：uint8_t
- 实现：
  ```cpp
  AscendC::Compare(dstLocal, src0Local, src1Local, cmpMode, srcDataSize);
  ```
- 参数：cmpMode=AscendC::CMPMODE::LT，srcDataSize=256

**场景2：Compare（结果存入寄存器）**
- 说明：逐元素比较`src0Local`和`src1Local`两个tensor大小，并将计算结果存入cmpMask寄存器中
- 输入：src0Local=[1, 64], src1Local=[1, 64] 
- 输入数据类型：float
- 输出：[1, 32] 
- 输出数据类型：uint8_t
- 实现：
  ```cpp
    AscendC::Compare(src0Local, src1Local, cmpMode, mask, repeatParams);  // Compare接口无repeat输入，repeat默认为1，即支持一条指令计算256字节的数据
    AscendC::PipeBarrier<PIPE_V>();
    AscendC::GetCmpMask(dstLocal);  // 通过GetCmpMask接口获取寄存器中保存的数据，dstLocal要求不少于128字节，但实际结果数据只占了8字节
  ```
- 参数：cmpMode=AscendC::CMPMODE::LT；repeatParams为默认值，控制操作数地址步长

**场景3：Compares**
- 说明：逐元素比较`src0Local`（tensor）中的元素和`src1Scalar`（标量）的大小
- 输入：src0Local=[1, 256], src1Local=[1, 16]  其中`src1Scalar`通过GetValue(idx)方法从中获取一个元素作为标量进行比较
- 输入数据类型：float
- 输出：dstLocal=[1, 32]
- 输出数据类型：uint8_t
- 数据类型：float
- 实现：
    ```cpp
    AscendC::Compares(dstLocal, src0Local, src1Scalar, cmpMode, srcDataSize);
    ```
- 参数：src1Scalar=src1Local.GetValue(0), cmpMode=AscendC::CMPMODE::LT，srcDataSize=256

**场景4：Compares（灵活标量位置）----此场景仅在 Ascend 950PR&950DT系列产品支持**
- 说明：逐元素比较`src0Local`（tensor）中的元素和`src1Scalar`（标量）的大小，其中标量可以支持在前和在后两种场景
- 输入：src0Local=[1, 256], src1Local=[1, 16]  其中`src1Scalar`通过src1Local[idx]方法从中获取一个元素作为标量进行比较
- 输入数据类型：float
- 输出：dstLocal=[1, 32]
- 输出数据类型：uint8_t
- 实现：
    ```cpp
    AscendC::Compares(dstLocal, src0Local, src1Scalar, cmpMode, srcDataSize);  // 标量在后
    AscendC::Compares(dstLocal, src1Scalar, src0Local, cmpMode, srcDataSize);  // 标量在前
    ```
- 参数：src1Scalar=src1Local[0], cmpMode=AscendC::CMPMODE::LT，srcDataSize=256

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
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin -scenario_num=$SCENARIO_NUM  # 验证输出结果是否正确
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
  | `SCENARIO_NUM` | `1`（默认）、`2`、`3`、`4` | 场景编号<br>1：Compare<br>2：Compare（结果存入寄存器）<br>3：Compares<br>4：Compares（灵活标量位置） |

- 执行结果  
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
