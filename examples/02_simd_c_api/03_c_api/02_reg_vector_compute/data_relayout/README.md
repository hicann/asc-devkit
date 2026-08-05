# data_relayout样例

## 概述
本样例基于C API编程接口实现数据重排功能，主要调用[asc_intlv](../../../../../docs/zh/api/SIMD-API/c_api/reg/data_rearrange/asc_intlv.md)/[asc_pack_to_low](../../../../../docs/zh/api/SIMD-API/c_api/reg/data_compress/asc_pack_to_low.md)接口，支持多种场景，通过 CMake 编译参数 `SCENARIO_NUM` 选择场景。
    | SCENARIO_NUM | 重排场景 |
    | --- | --- |
    | 1 | Interleave（将两个uint16_t向量交织） |
    | 2 | Pack（从uint32_t向量中提取低16位到uint16_t向量） |

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 目录结构介绍
```
├── data_relayout
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── data_relayout.asc              // AscendC样例实现 & 调用样例
│   ├── README.md                      // 样例介绍
│   └── README_en.md                   // 样例介绍（英文版）
```

## 样例描述
- 样例功能：
  演示数据重排类接口（Interleave/Pack）的用法，支持Interleave和Pack两种场景。

  **场景1：Interleave模式**
  - 两个uint16_t向量（各128个元素）交织，输出两个uint16_t向量
  - Interleave将VL(128)个元素分为高低两半(各64)分别交织：
    - dst0 = [src0[0], src1[0], src0[1], src1[1], ..., src0[63], src1[63]]
    - dst1 = [src0[64], src1[64], src0[65], src1[65], ..., src0[127], src1[127]]
  - 样例规格：
    <table>
    <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
    <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">src0</td><td align="center">[1, 128]</td><td align="center">uint16_t</td></tr>
    <tr><td align="center">src1</td><td align="center">[1, 128]</td><td align="center">uint16_t</td></tr>
    <tr><td rowspan="3" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">dst0</td><td align="center">[1, 128]</td><td align="center">uint16_t</td></tr>
    <tr><td align="center">dst1</td><td align="center">[1, 128]</td><td align="center">uint16_t</td></tr>
    <tr><td rowspan="1" align="center">核函数名</td><td colspan="3" align="center">interleave_custom</td></tr>
    </table>
  - 样例实现：
    interleave_vf函数内调用asc_intlv接口进行数据交织。
    - 调用实现
      使用内核调用符`<<<>>>`调用核函数。

  **场景2：Pack模式**
  - 从1个uint32_t向量（128个元素）中提取低16位到1个uint16_t向量
  - Pack<uint16_t, uint32_t, LOWEST>：提取每个uint32_t的低16位
  - 样例规格：
    <table>
    <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
    <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">src</td><td align="center">[1, 128]</td><td align="center">uint32_t</td></tr>
    <tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">dst</td><td align="center">[1, 128]</td><td align="center">uint16_t</td></tr>
    <tr><td rowspan="1" align="center">核函数名</td><td colspan="3" align="center">pack_custom</td></tr>
    </table>
  - 样例实现：
    pack_vf函数内调用asc_pack_to_low接口提取低16位。
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
  SCENARIO_NUM=1                                                                 # 选择执行场景（1=Interleave，2=Pack）
  mkdir -p build && cd build;                                                    # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;  # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py -scenarioNum $SCENARIO_NUM                      # 生成测试输入数据
  ./demo                                                                         # 执行编译生成的可执行程序，执行样例
  ```

  使用 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  | --- | --- | --- |
  | `SCENARIO_NUM` | 1、2 | 样例执行场景：场景1=Interleave，场景2=Pack |
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU 运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
