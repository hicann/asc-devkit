# mergemode样例

## 概述
本样例基于C API编程接口验证MERGING模式的行为特性，演示未激活mask位保留矢量数据寄存器dst原值的机制，使用[asc_copy](../../../../../docs/zh/api/SIMD-API/c_api/reg/reg_load/asc_copy.md)接口进行验证。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 目录结构介绍
```
├── mergemode
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── mergemode.asc                  // AscendC样例实现 & 调用样例
│   ├── README.md                      // 样例介绍
│   └── README_en.md                   // 样例介绍（英文版）
```

## 样例描述
- 样例功能：  
  演示使用asc_copy接口在MERGING模式下进行矢量数据寄存器间的数据搬运。mask为1的位置将src数据搬入dst，mask为0的位置保持dst原值。本样例使用PAT_H掩码（高半部分激活），将输入x的高半部分搬入dst_reg，低半部分保持dst_reg（即输入y）的原值。

  - 样例规格：
    <table>
    <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
    <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">x</td><td align="center">[1, 128]</td><td align="center">float</td></tr>
    <tr><td align="center">y</td><td align="center">[1, 128]</td><td align="center">float</td></tr>
    <tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">z</td><td align="center">[1, 128]</td><td align="center">float</td></tr>
    <tr><td rowspan="1" align="center">核函数名</td><td colspan="3" align="center">mergemode_custom</td></tr>
    </table>

  - 样例实现：
    - copy_merge_vf函数内使用PAT_H掩码创建vmask，激活高半部分元素
    - 调用asc_loadalign分别加载输入x（src）和输入y（dst）到矢量数据寄存器
    - 调用asc_copy将src_reg搬入dst_reg，MERGING模式下激活位被src覆盖，未激活位保留dst原值
    - 调用asc_storealign将结果写回UB
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
