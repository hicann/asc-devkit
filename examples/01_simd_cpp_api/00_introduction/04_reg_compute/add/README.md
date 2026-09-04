# 基于RegBase编程的add样例

## 概述
本样例基于RegBase编程范式实现向量自加计算，计算逻辑为`y = x + x`。样例先将输入数据从GM（Global Memory）搬运到UB（Unified Buffer），再通过VF函数调用RegBase接口完成寄存器级别的Add计算，最后将结果从UB写回GM。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍
```
├── add
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   │   └── verify_result.py           // 真值对比文件
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── add.asc                        // AscendC样例实现 & 调用样例
│   └── README.md                      // 样例说明文档
```

## 样例描述
- 样例功能：  
  本样例基于RegBase编程范式实现向量自加操作。输入`x`为`float`类型二维数据，输出`y`与输入shape一致，每个输出元素满足`y[i] = x[i] + x[i]`。样例采用4核并行处理，输入总长度为`256 * 256`个`float`元素，每个核处理`totalLength / 4`个连续元素，展示GM/UB数据搬运、VF函数调用、RegBase寄存器计算和流水同步的基本流程。
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[256, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">y</td><td align="center">[256, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="3" align="center">vector_add</td></tr>
  </table>

- 样例实现：
  - Kernel实现
    - 通过`GetBlockIdx`获取当前核的block索引，并计算当前核处理的数据偏移`coreOffset`。
    - 使用`GlobalTensor`绑定输入、输出GM地址。
    - 使用`LocalMemAllocator<Hardware::UB>`申请当前核使用的UB缓存。
    - 调用`DataCopy`将当前核负责的数据从GM搬运到UB。
    - 使用`SetFlag<HardEvent::MTE2_V>`和`WaitFlag<HardEvent::MTE2_V>`保证GM到UB搬运完成后再开始Vector计算。
    - 通过`asc_vf_call`调用`AddVF`函数，在VF函数内完成`LoadAlign -> Add -> StoreAlign`的寄存器计算流程。
    - 使用`SetFlag<HardEvent::V_MTE3>`和`WaitFlag<HardEvent::V_MTE3>`保证Vector计算完成后再将UB结果搬运回GM。
    - 调用`DataCopy`将结果从UB搬运至GM。

  - 调用实现  
    使用内核调用符`<<<>>>`启动`vector_add`核函数。

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  ./demo                                                                    # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：Ascend 950PR/Ascend 950DT |

- 执行结果  
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
