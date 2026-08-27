# fill样例

## 概述
本样例基于Tensor API实现fill运算（标量填充模式），主要调用实验性接口 `asc::te::experimental::fill`。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 目录结构介绍
```
├── fill_reg_api
│   ├── scripts
│   │   │   ├── gen_data.py            // 真值数据生成脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── fill_reg_api.asc               // AscendC样例实现 & 调用样例
│   └── README.md                      // 样例介绍
```

## 样例描述
- 样例功能：
  将标量值1填充到输出向量的每个位置，向量元素个数为256，数据类型为float。  
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">fill</td></tr>
  </table>
- 样例实现：  
  fill_vf函数内调用fill接口进行标量填充：
  - 使用fill接口将标量值填充到dstReg的每个元素
  - 使用store将结果写回UB
  - 调用实现  
    使用内核调用符<<<>>>调用核函数。

## 编译运行
在本样例根目录下执行如下步骤，编译并执行样例。
- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](https://gitcode.com/cann/asc-devkit/blob/master/docs/zh/quick_start.md)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..;make -j; # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py                                            # 生成测试真值数据
  ./demo                                                                    # 执行编译生成的可执行程序，执行样例
  ```

  使用 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

| 选项　　　　　　　　　　　| 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　|
| ---------------------------| -----------------------------| ---------------------------------------------------|
| `CMAKE_ASC_RUN_MODE`　　　| `npu`（默认）、`sim` | 运行模式：NPU 运行、NPU仿真　　　　　　　|
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510`　　　　　　　　　| NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |
| `CANN_ASC_USE_EXPERIMENTAL` | `ON`（本样例必选）、`OFF`（默认） | 开启实验性ASC接口 |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
