# squeeze样例

## 概述
本样例基于Reg编程接口实现Squeeze运算，主要调用Squeeze接口和StoreUnAlign/StoreUnAlignPost接口。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍
```
├── squeeze
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── squeeze.asc                    // AscendC样例实现 & 调用样例
│   └── README.md                      // 样例介绍
```

## 样例描述
- 样例功能：
  本样例以MaskReg设置为MaskPattern::M4为例，每轮迭代选择输入向量xReg中索引为4的倍数元素依次复制到输出向量yReg中连续排列，且输出向量中剩余位置元素置为0。
  - Squeeze接口的模板参数store配置为STORE_REG时，可以记录有效元素个数并存入AR特殊寄存器中，配合StoreUnAlign接口使用
  - StoreUnAlign接口可以使用AR特殊寄存器中记录的值作为搬运的元素个数，实现连续非对齐搬运，将Squeeze的结果连续搬出
- 约束说明：
  - 当Squeeze接口的模板参数store配置为STORE_REG时，必须保证Squeeze接口和StoreUnAlign接口交替使用，以保证AR特殊寄存器的正常使能
  - 在计算前需要调用ClearSpr接口，将AR寄存器进行清零，否则可能会有数据残留导致精度问题
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">y</td><td align="center">[1, 64]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">squeeze</td></tr>
  </table>
- 样例实现：  
  - 输入向量的shape为[1, 256]，数据类型为float，进行4次迭代，每轮迭代处理64个元素
  - Squeeze：每次调用选择xReg中索引为4的倍数的元素，即xReg[i * 4]依次连续写入yReg[i]中，结果y[0:16]为16个有效元素，剩余的y[16:64]置0，AR特殊寄存器的值置为16
  - StoreUnAlign：根据AR寄存器中的值作为搬运元素个数，搬出yReg中前16个元素至非对齐寄存器ureg或者输出UB地址yAddr
  - StoreUnAlignPost：4次迭代结束后，搬出ureg中剩余的数据
  - 调用实现：使用内核调用符<<<>>>调用核函数。

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
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
