# mmad_with_sparse样例

## 概述

本样例介绍基础API MmadWithSparse调用样例。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── mmad_with_sparse
│   ├── scripts
│   │   ├── gen_data.py             // 输入数据和真值数据生成脚本
│   │   └── verify_result.py        // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── data_utils.h                // 数据读入写出函数
│   ├── mmad_with_sparse.asc        // Ascend C样例实现 & 调用样例
│   └── README.md                   // 样例说明文档
```

## 样例描述

- 样例功能：
  本样例中实现的是[m, n, k]固定为[128, 128, 64]的稀疏矩阵乘，使用Ascend C基础API MmadWithSparse接口实现。4选2 sparseMatmul是一种特殊的矩阵乘，要求一个连续的4个权重或激活值的组中，最多只有2个值为非零，其余2个强制为零，数学表达式为：

  ```
  C = A * B
  ```
  其中原始B矩阵shape为[N, K]，每4个元素中至少包含2个零元素，通过4选2稠密化策略压缩为[N, K/2]后输入。B矩阵必须转置存储，即按[N, K]构造并压缩输入。A、B矩阵数据类型只支持int8_t，index矩阵数据类型为uint8_t。
- 样例规格：

<a name="表1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表1：样例规格表</span></caption>
  <tr><td rowspan="4" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">a</td><td align="center">[128, 64]</td><td align="center">int8</td><td align="center">NZ</td></tr>
  <tr><td align="center">b</td><td align="center">[128, 32]</td><td align="center">int8</td><td align="center">ZN</td></tr>
  <tr><td align="center">idx</td><td align="center">[128, 8]</td><td align="center">uint8</td><td align="center">ZN</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">c</td><td align="center">[128, 128]</td><td align="center">int32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">mmad_with_sparse_custom</td></tr>
</table>

- 样例实现：
  样例实现分为以下阶段：
  - **CopyIn**：将Global Memory上的输入数据搬运到Local Memory L1中，并将索引矩阵加载到L1中，其中Index分形与B矩阵分形必须为Zn分形，即GM上的B矩阵必须为转置，Index离线生成必须为Zn排布
  - **SplitB**：使用LoadDataWithSparse将B矩阵和索引矩阵从L1搬运到L0B与内置索引buffer中
  - **SplitA**：将A矩阵从L1搬运到L0A中
  - **Compute**：使用MmadWithSparse完成稀疏矩阵乘运算，计算结果存储在Local Memory L0C中
  - **CopyOut**：将输出数据从L0C搬运至Global Memory上的输出GM中

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
  cmake ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```
  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim ..;make -j; # NPU仿真模式
  ```
  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认） | NPU架构：dav-2201 对应 Atlas A2系列产品/Atlas A3系列产品 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```

## 数据生成与验证说明

### gen_data.py 脚本功能

1. **构造稀疏矩阵B**：生成指定形状的稀疏矩阵，每行的每4个元素块中至少包含2个零元素
2. **稠密化处理**：将稀疏矩阵B通过4选2策略稠密化，生成稠密矩阵dense_B
3. **索引矩阵生成**:
   - 生成index_matrix：记录每个块中被选中元素的相对位置（用于NPU计算）
   - 生成index_mask_matrix：记录被选中元素的绝对索引（用于golden计算）
4. **生成golden数据**：使用稠密化后的矩阵和索引矩阵计算稀疏矩阵乘的真值
5. **数据格式转换**:
   - 将索引矩阵从uint8转换为uint2格式
   - 对输入矩阵进行ND到NZ的分格转置
