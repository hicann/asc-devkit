# gm_by_dcache样例

## 概述

本样例介绍经过DCache（Data Cache）从GM地址读取和写入数据。

Scalar单元访问Global Memory，首先会访问每个核内的DataCache，因此存在Data Cache与Global Memory的Cache一致性问题。需要使用DataCacheCleanAndInvalid接口刷新Cache，保证Cache的一致性。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── gm_by_dcache
│   ├── CMakeLists.txt      // 编译工程文件
│   ├── gm_by_dcache.asc    // Ascend C样例实现 & 调用样例
│   └── README.md           // 样例说明文档
```

## 样例描述

- 样例功能：
  用多核实现输入矩阵的ReduceSum操作，将所有数据之和写入输出矩阵中。通过workspace保存每个核计算的中间结果，以及每个核的计算完成状态。

- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">GmByDcache</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[4, 1024]</td><td align="center">float</td></tr>
  <tr><td align="center">workspace</td><td align="center">[1, 128]</td><td align="center">int32_t</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1, 1]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">gm_by_dcache_custom</td></tr>
  </table>

- 样例实现：
  - Kernel实现
    本样例使用4个核完成计算，每个核处理对应行1024个元素的求和。

    **第一步：每个核分别执行归约计算**

    - 将GM（Global Memory）上的输入数据（对应行1024个元素）搬运到UB（Unified Buffer）上
    - 调用`ReduceRepeat<AscendC::ReduceType::SUM>`接口，每256B数据归约为一个sum元素，结果共16个float
    - 将计算结果搬出至GM上的workspace

    **第二步：每个核写入标志位**

    - 通过Scalar单元写入标志位至GM上的workspace，表示该核的计算已经完成
    - 调用DataCacheCleanAndInvalid接口，将Data Cache中现存的数据立即写出

    **第三步：0核读取标志位，完成最终的归约计算**

    - 除了0核外的其他核return，停止运行
    - 0核调用DataCacheCleanAndInvalid接口，获取最新数据
    - 通过Scalar单元依次读取所有核的标志位，判断其他核是否完成计算
    - 若其他核的标志位都符合预期，则再次调用`ReduceRepeat<AscendC::ReduceType::SUM>`接口，完成最终的归约计算

  - 调用实现
    使用内核调用符<<<>>>调用核函数，启动4个核并行执行。

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # 编译工程，默认npu模式
  ./demo                           # 执行样例
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |

- 执行结果
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```