# DataCopy数据切片搬运样例

## 概述
本样例基于DataCopy实现数据切片搬运，提取多维Tensor数据的子集进行GM（Global Memory）与UB（Unified Buffer）通路之间的搬运。
## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构
```
├── data_copy_gm2ub_slice
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── slice.asc               // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述
- 样例功能：  
  实现了数据切片搬运样例，支持数据的切片搬运，提取二维的源操作数Tensor[3, 87]数据的子集（提取4个数据段：[0, 16:40], [0, 47:71], [2, 16:40]， [2, 47:71]，共96个float32数据）并连续搬运到二维的目的操作数Tensor[2, 48]。接口资料参考[切片数据搬运](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopy（GM与UB切片数据搬运）.md)。

- 样例规格：  
 
  <table>  
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">切片参数</td></tr>  
  <tr><td align="center">x</td><td align="center">[3, 87]</td><td align="center">float32</td><td align="center">ND</td><td align="center">[[0, 16:40], [0, 47:71]], [[2, 16:40]，[2, 47:71]]</td></tr>  
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">y</td><td align="center">[2, 48]</td><td align="center">float32</td><td align="center">ND</td><td align="center">连续写入Tensor[2, 48]</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="5" align="center">kernel_slice</td></tr>  
  </table>

- 样例实现：  
  - Kernel实现  
    计算逻辑是：输入数据需要先按切片参数从GM（Global Memory）搬运到UB（Unified Buffer），再搬出到外部GM（Global Memory）上。
    
    接口详细描述参考Ascend C API DataCopy切片数据搬运。

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
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  ./demo                                                                    # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
  
  示例如：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　　　　　　　　　　　　　　　　　　 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品 与 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
