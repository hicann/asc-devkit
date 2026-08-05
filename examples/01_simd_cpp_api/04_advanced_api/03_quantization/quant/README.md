# Quant样例

## 概述

本样例基于[AscendQuant](../../../../../docs/zh/api/SIMD-API/adv_api/quantization/AscendQuant.md)高阶API实现量化计算，用于将高精度数据转换为低精度数据以减少存储和计算开销。样例展示了将float类型输入数据通过scale缩放和offset偏移，量化为int8_t类型输出的过程。在950上兼容AscendQuant接口的前提下，推荐优先使用[Quantize](../../../../../docs/zh/api/SIMD-API/adv_api/quantization/Quantize.md)接口，该接口可通过统一的结构体配置，适配各类量化场景。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── quant
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── quant.asc               // Ascend C算子实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能:  
  QuantCustom单样例，对输入tensor按元素做量化计算，将half/float数据类型量化为int8_t数据类型。

- 样例规格：
  <table border="2" align="left">
  <caption>表1：样例输入输出规格</caption>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center"> quant </td></tr>

  <tr><td rowspan="3" align="center">样例输入</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">inputGm</td><td align="center">[1，1024]</td><td align="center">float</td><td align="center">ND</td></tr>


  <tr><td rowspan="2" align="center">样例输出</td></tr>
  <tr><td align="center">outputGm</td><td align="center">[1，1024]</td><td align="center">int8_t</td><td align="center">ND</td></tr>


  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">quant_custom</td></tr>
  </table>
  <br clear="left" />
<br />

- 样例实现:  
  本样例实现了固定shape输入inputGm[1，1024]，量化参数scale=2.0、offset=0.9。本样例为PER_TENSOR场景（按张量量化），将float数据类型量化为int8_t数据类型。

  - Kernel实现  
    计算逻辑是：Ascend C提供的矢量计算接口的操作元素都为LocalTensor，输入数据需要先搬运进片上存储，然后使用AscendQuant（A2A3）或Quantize（950系列）高阶API接口完成量化计算，得到最终结果，再搬出到外部存储上。

  - Tiling实现  
    QuantCustom样例的tiling实现流程如下：首先获取AscendQuant或Quantize接口能完成计算所需最大/最小临时空间大小，使用最小临时空间，然后根据输入长度dataLength确定所需tiling参数。

  - 调用实现  
    使用内核调用符<<<>>>调用核函数。

## 编译运行  

在本样例根目录下执行如下步骤，编译并执行算子。
- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。
    
- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # 默认npu模式
  python3 ../scripts/gen_data.py -DCMAKE_ASC_ARCHITECTURES=dav-2201   # 生成测试输入数据
  ./demo
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |
  
- 执行结果  
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
