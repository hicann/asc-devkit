# IBSet与IBWait核间同步样例

## 概述

本样例基于IBSet和IBWait实现核间同步，适用于以下场景：当两个核协同操作同一块全局内存且存在数据依赖时，通过IBSet和IBWait实现核间同步，避免数据读写错误。

> **注意：** 本样例仅适用于基于TPipe和TQue的编程模式。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── ib_set_wait
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── ib_set_wait.asc         // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例功能描述

本样例通过两个核协同工作，每个核处理256个half数据，共512个元素。核0负责读取输入x[0:256]和y[0:256]，计算结果写入z[0:256]；核1负责读取核0的输出z[0:256]和输入y[256:512]，计算结果写入z[256:512]。需确保核1在核0完成写操作后才能读取z[0:256]，避免写后读数据依赖问题。

本样例中通过IBSet和IBWait实现核间同步：

- 核0完成z[0:256]的写操作后，通过IBSet设置同步标志位
- 核1在读取z[0:256]之前，通过IBWait等待核0完成写操作

### 样例规格

<table>
<caption>表1：样例输入输出规格</caption>
<tr><td rowspan="1" align="center">样例类型</td><td colspan="5" align="center">核间同步</td></tr>
<tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center"></td></tr>
<tr><td align="center">x</td><td align="center">[512]</td><td align="center">half</td><td align="center">ND</td><td align="center"></td></tr>
<tr><td align="center">y</td><td align="center">[512]</td><td align="center">half</td><td align="center">ND</td><td align="center"></td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[512]</td><td align="center">half</td><td align="center">ND</td><td align="center"></td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="5" align="center">kernel_ib_set_wait</td></tr>
</table>

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
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j;    # 编译工程
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin  # 验证输出结果是否正确
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

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
