# histogram样例

## 概述
本样例基于C API编程接口实现直方图统计功能，使用了[asc_frequency_histogram_bin0](../../../../../docs/zh/api/SIMD-API/c_api/reg/histogram_compute/asc_frequency_histogram.md)/asc_frequency_histogram_bin1接口。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 目录结构介绍
```
├── histogram
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── histogram.asc                  // AscendC样例实现 & 调用样例
│   ├── README.md                      // 样例介绍
│   └── README_en.md                   // 样例介绍（英文版）
```

## 样例描述
- 样例功能：  
  对uint8_t输入数据做直方图统计，输入向量shape为[1, 256]，数据类型为uint8_t，输出向量shape为[1, 256]，即256个数值的直方图统计结果，数据类型为uint16_t。

  - 一个核处理全部256个输入数据，统计结果为256个bin的频率统计
  - 样例规格：
    <table>
    <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
    <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">uint8_t</td></tr>
    <tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">y</td><td align="center">[1, 256]</td><td align="center">uint16_t</td></tr>
    <tr><td rowspan="1" align="center">核函数名</td><td colspan="3" align="center">histogram_custom</td></tr>
    </table>
  - 样例实现：
    histogram_vf函数内：
    1. 使用asc_duplicate_scalar初始化矢量数据寄存器dst为0
    2. 使用asc_loadalign加载输入数据
    3. 调用asc_frequency_histogram_bin0/asc_frequency_histogram_bin1接口进行统计计算，dst0统计[0-127]区间、dst1统计[128-255]区间
    4. 循环累加所有输入数据块的统计结果
    5. 使用asc_storealign输出统计结果到UB
    - 调用实现
      使用内核调用符`<<<>>>`调用核函数。

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
  mkdir -p build && cd build;                                                    # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                           # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py                                                 # 生成测试输入数据
  ./demo                                                                         # 执行编译生成的可执行程序，执行样例
  ```

  使用 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  | --- | --- | --- |
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU 运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
