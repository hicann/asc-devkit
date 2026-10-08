# CreateVecIndex样例

## 概述

本样例在数据填充场景下，基于CreateVecIndex API实现创建指定起始值的向量索引功能，用于生成连续的整数序列。CreateVecIndex API支持从指定起始值开始，创建连续递增的索引向量，常用于需要生成序列索引的场景，例如为数组元素生成下标、创建步长序列等。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── create_vec_index
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── create_vec_index.asc    // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：  
  本样例展示了使用CreateVecIndex API创建指定起始值的向量索引，用于生成连续的整数序列。CreateVecIndex API适用于需要生成序列索引的场景，例如为数组元素生成下标、创建步长序列、创建循环索引等。通过startIndex参数指定起始值，通过count参数指定生成的元素个数。所用API详细介绍请参考[CreateVecIndex API文档](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta1/API/ascendcopapi/atlasascendc_api_07_0090.html)。

- 样例规格：  
  <table border="2" align="center">
  <caption>表1：样例输入输出规格</caption>
  <tr><td rowspan="1" align="center">样例类型</td><td colspan="4" align="center">CreateVecIndex</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">-</td><td align="center">-</td><td align="center">-</td><td align="center">-</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td></tr>
  <tr><td align="center">y</td><td align="center">[1,128]</td><td align="center">uint16_t</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">testCreateVecIndex</td></tr>
  </table>

- 样例实现：  
  本样例中实现的是固定shape为输出y[1,128]的CreateVecIndex样例，生成从0开始的128个连续整数索引（0, 1, 2, ..., 127）。
  
  CreateVecIndex API参数说明：
  - dst：目的操作数，用于存储生成的索引向量
  - startIndex：索引的起始值，本样例中为0
  - count：参与计算的元素个数，本样例中为128

  - Kernel实现  
    - 调用Duplicate接口，初始化UB（Unified Buffer）缓冲区为0
    - 调用CreateVecIndex接口，生成从0开始的连续索引向量
    - 调用DataCopy基础API，将结果从UB（Unified Buffer）搬运到GM（Global Memory）

  - 调用实现  
    使用内核调用符<<<>>>调用核函数。

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
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```

  使用CPU调试或NPU仿真模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
  
  示例如下：
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # CPU调试模式
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # NPU仿真模式
  ```
  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |
