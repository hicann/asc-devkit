# SimdVF Dump接口功能说明


## 概述

本样例介绍simd_vf侧asc_dump系列接口使用方法，在vector函数中通过`asc_dump_ubuf`、`asc_dump_reg`、`asc_dump`等接口实现UB和寄存器数据的可视化打印。Host侧通过`ReadFile`/`WriteFile`完成二进制数据的读写，核函数从GM读取输入数据，执行Adds计算后将结果写回GM，通过比较输出与真值进行校验。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |

## 目录结构介绍

```
├── simd_vf_dump
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── simd_vf_dump.asc        // Ascend C样例实现&调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：

  使用vector编程模式，展示simd_vf侧asc_dump系列接口的基本使用方法。核函数通过`ReadFile`读取二进制输入数据，使用`asc_vf_call`调用simd_vf函数进行dump打印，执行Adds计算后通过`WriteFile`写出二进制输出数据，最终通过校验脚本比对输出与真值。

  1. `asc_dump_ubuf<T>`-打印UB数据
  2. `asc_dump_reg<T>`-打印寄存器数据
  3. `asc_dump<T>`-打印UB/Reg数据（通用接口）

- simd_vf dump接口用法：

  | 接口 | 地址类型 | 示例 |
  |------|----------|------|
  | `asc_dump_ubuf<T>` | `__ubuf__` | `asc_dump_ubuf<float>(input, desc, dump_size);` |
  | `asc_dump_reg<T>` | Reg | `asc_dump_reg<float>(input, desc, dump_size);` |
  | `asc_dump<T>` | `__ubuf__` / Reg | `asc_dump<float>(input, desc, dump_size);` |

  参数说明：
  - 模板参数`T`：指定数据类型（float/half等）
  - `input`：UB数据地址或Reg寄存器对象
  - `desc`：打印控制描述符
  - `dump_size`：单次dump的数据元素个数

- 调用实现

  在`__global__ __vector__`核函数内使用`asc_vf_call<FuncName>()`调用simd_vf函数完成dump打印，随后通过`AscendC::Adds`执行计算并将结果写回GM。Host侧通过`ReadFile`/`WriteFile`完成二进制数据的读写。

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
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # 编译工程
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  ./demo                                                                    # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`（默认） | NPU架构：dav-3510对应Ascend 950PR&950DT系列产品 |

- 执行结果
  最终执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
