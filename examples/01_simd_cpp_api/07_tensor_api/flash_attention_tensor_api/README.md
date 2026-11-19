# FlashAttention Tensor API样例

## 概述

本样例实现FlashAttention前向计算，展示如何使用Tensor API完成单行Query的注意力计算。输入和输出使用`bfloat16_t`数据，softmax中间状态使用`float`数据。

注意力计算要为每个Query head得到一个上下文表示。在一个head内，Query是一行查询特征，Key是一组索引特征，Value是与Key逐行对应的内容特征。计算时，Query先与所有Key行比较相似度；softmax把这些相似度转换为总和为1的注意力权重；每个权重决定对应Value行在结果中的占比。最终输出是所有Value行的加权组合，也就是该Query在当前上下文中的表示。

本样例包含32个Query head和4个KV head，并通过GQA（Grouped Query Attention，分组查询注意力）复用Key/Value：每个KV head的Key和Value由8个Query head共享。每个Query head只有1行Query，每个KV head有4160行Key和Value，因此样例面向单行Query的前向计算场景。样例不包含反向计算、causal mask、attention bias、dropout和变长序列。

直接实现需要先保存完整的`Query × Key`相似度矩阵，再对每行做softmax。FlashAttention把4160行KV划分为多个KV split和tile，每个tile只更新当前最大值、softmax分母和未归一化输出，最后合并这些状态。这样不需要在GM中保存完整的相似度矩阵或权重矩阵，同时保持softmax的数学结果不变。

## 计算原理

### 单个Query head的注意力计算

先只看第`h`个Query head，`h`的取值范围是0~31。本样例中GQA映射为`g(h) = h / 8`：第0~7个Query head使用第0个KV head，第8~15个Query head使用第1个KV head，依此类推。因此每个Query head只会与自己映射到的KV head中的Key和Value计算。

公式中的`T`表示矩阵转置。Q表示第`h`个Query head的`1 × 128` Query行；K表示该head映射到的KV head的`4160 × 128` Key矩阵；V表示同一个KV head的`4160 × 128` Value矩阵。`softmax_scale`是缩放系数，本样例取`1 / sqrt(128)`，用于把点积结果控制在与`float`指数运算相匹配的数值范围内。

第`h`个Query head与第`t`行Key的相似度记为score：

$$
S_h = \mathit{softmax\_scale} \times Q_h K_{g(h)}^T
$$

矩阵乘法得到`1 × 4160`的结果，其中第`t`个元素表示Query与第`t`行Key的相似度。相似度越大，这一行Value在后续加权中的占比越高。

softmax先找到所有score的最大值，避免较大的score直接进入指数运算导致溢出：

$$
m_h = \max_t S_{h,t}
$$

`t`遍历当前KV head的全部4160行。接着用每个score减去该最大值，再计算指数：

$$
w_{h,t} = \exp(S_{h,t} - m_h)
$$

得到的`w`是非负权重，但它还不是最终的softmax权重；只有除以全部`w`的和之后，4160个权重的总和才为1。因此最终输出为：

$$
O_h = \frac{\sum_{t=0}^{L-1} w_{h,t} V_{g(h),t}}{\sum_{t=0}^{L-1} w_{h,t}}
$$

`L`是KV序列长度，本样例中为4160。分子的第`t`项用第`t`个未归一化权重乘第`t`行Value，分母把全部未归一化权重相加。除法完成后，O就是第`h`个Query head经注意力加权得到的`1 × 128`输出。

### 在线softmax

标准softmax必须先得到当前Query head与全部4160行Key的score，才能知道全局最大值并计算归一化分母。在线softmax改变执行顺序：它把KV序列切分成小块，每处理一块就更新一次状态，因此不需要保存完整的score矩阵。

本样例把4160行KV切分为13个KV split，每个split再切分为5个`64 × 128`tile。对每个Query head和KV split，decode阶段维护三个`float`状态：当前最大值`m`、softmax分母`l`和未归一化输出`o`。进入新tile时，先用新tile的score更新最大值：

$$
m_{\mathrm{new}} = \max(m_{\mathrm{old}}, \max_t S_t)
$$

这里`t`只遍历当前tile内的64行。如果新最大值发生变化，历史`l`和`o`是在旧最大值基准下计算的，必须先换算到新基准。换算系数为：

$$
\alpha = \exp(m_{\mathrm{old}} - m_{\mathrm{new}})
$$

用`α`缩放历史状态，并把当前tile的权重累加进去：

$$
l_{\mathrm{new}} = \alpha l_{\mathrm{old}} + \sum_t \exp(S_t - m_{\mathrm{new}})
$$

$$
o_{\mathrm{new}} = \alpha o_{\mathrm{old}} + \sum_t \exp(S_t - m_{\mathrm{new}}) V_t
$$

处理完该KV split的全部5个tile后，得到该split的局部`m/l/o`。merge阶段再用同样的关系合并13个split的局部状态，最终输出为：

$$
O = \frac{o_{\mathrm{total}}}{l_{\mathrm{total}}}
$$

因此，在线softmax与标准softmax的数学结果相同；区别是标准softmax在全部score就绪后一次性归一化，而在线softmax每处理一个tile就更新状态，并且只保存固定大小的`m/l/o`。

## 本样例支持的产品及CANN软件版本

<table border="2">
  <caption>支持产品及CANN软件版本表</caption>
  <tr><td>产品</td><td>CANN软件版本</td></tr>
  <tr><td>Ascend 950PR&950DT系列产品</td><td>>= CANN 9.3.0</td></tr>
</table>

## 目录结构介绍

```text
├── CMakeLists.txt                  // 编译工程
├── README.md                       // 中文样例说明
├── README_en.md                    // 英文样例说明
├── data_utils.h                    // Host侧读写二进制文件工具
├── flash_attention_tensor_api.asc  // Kernel与Host调用代码
└── scripts
    ├── gen_data.py                 // 输入和真值数据生成脚本
    └── verify_result.py            // 输出校验脚本
```

## 样例描述

### 输入输出

<table border="2">
  <caption>输入输出表</caption>
  <tr><td align="left">Tensor</td><td align="left">Shape</td><td align="left">数据类型</td><td align="left">说明</td></tr>
  <tr><td align="left">query</td><td align="left">[32, 128]</td><td align="left">bfloat16_t</td><td align="left">32个Query head，每个head 1行</td></tr>
  <tr><td align="left">key</td><td align="left">[4, 4160, 128]</td><td align="left">bfloat16_t</td><td align="left">4个KV head的Key</td></tr>
  <tr><td align="left">value</td><td align="left">[4, 4160, 128]</td><td align="left">bfloat16_t</td><td align="left">4个KV head的Value</td></tr>
  <tr><td align="left">output</td><td align="left">[32, 128]</td><td align="left">bfloat16_t</td><td align="left">每个Query head的注意力输出</td></tr>
</table>

样例运行时还会在Device GM上申请两个内部工作区：

- `partial` workspace：大小为`32 × 4 × 256 × 4 = 131072 bytes`，保存decode阶段每个`(head, range)`的局部`m/l/o`状态。
- `completed`计数器：大小为`4 bytes`，记录已经写完局部状态的block数量。

这两个工作区由Host侧申请并在kernel内更新，属于中间状态，不是样例的输入或输出接口。

### 样例规格

<table border="2">
  <caption>样例规格表</caption>
  <tr><td align="left">项目</td><td align="right">值</td><td align="left">说明</td></tr>
  <tr><td align="left">`query_heads`</td><td align="right">32</td><td align="left">Query head数量</td></tr>
  <tr><td align="left">`kv_heads`</td><td align="right">4</td><td align="left">KV head数量</td></tr>
  <tr><td align="left">`kv_length`</td><td align="right">4160</td><td align="left">每个KV head的序列长度</td></tr>
  <tr><td align="left">`head_dim`</td><td align="right">128</td><td align="left">每个head的特征维度</td></tr>
  <tr><td align="left">`tile_rows`</td><td align="right">64</td><td align="left">每次处理的KV行数</td></tr>
  <tr><td align="left">`splits`</td><td align="right">13</td><td align="left">KV序列切分数；每个切分称为一个KV split</td></tr>
  <tr><td align="left">`tiles_per_split`</td><td align="right">5</td><td align="left">每个KV split内的64行tile数量，13 × 5 × 64 = 4160</td></tr>
  <tr><td align="left">`head_group`</td><td align="right">2</td><td align="left">一个block同时处理的相邻Query head数量，两个head共享同一份K/V tile</td></tr>
  <tr><td align="left">`range_count`</td><td align="right">4</td><td align="left">每个Query head组划分出的KV range数量，13个split被划分为4/3/3/3个split</td></tr>
  <tr><td align="left">`partial_size`</td><td align="right">256</td><td align="left">每个head每个KV range的float状态区大小，单位为float</td></tr>
  <tr><td align="left">Vector block数量</td><td align="right">64</td><td align="left">16个Query head组 × 4个KV range；block可分波执行，最后完成的block负责merge</td></tr>
  <tr><td align="left">`softmax_scale`</td><td align="right">1/sqrt(128)</td><td align="left">QK点积后的缩放系数</td></tr>
</table>

Vector block是kernel启动时的一个并行执行实例；本样例的每个Vector block独立处理一个Query head组和一个KV range。

### 计算流程

样例在一个kernel内完成两阶段计算：decode阶段按KV range和tile计算局部softmax状态，merge阶段合并同一个Query head的4个局部状态并生成最终输出。每个Vector block负责2个相邻Query head和1个KV range。13个KV split被划分为4个range，各range包含4/3/3/3个split。

1. Host侧程序从`input/query.bin`、`input/key.bin`和`input/value.bin`读取三个输入Tensor。
2. Host侧程序调用`aclrtMemcpy`，把query、key和value从Host内存拷贝到Device GM。
3. Host侧程序向stream提交`flash_attention`，共启动64个Vector block。kernel按`head_pair = block_idx / range_count`得到Query head组，按`range = block_idx % range_count`得到KV range，再按`head = head_pair * head_group`得到组内第一个Query head，最后通过`g(h) = h / 8`确定该组对应的KV head。
4. Kernel为GM输入、UB输入缓冲、UB状态缓冲、GM workspace和UB Output Tensor创建Tensor视图。
5. Kernel用`copy(make_copy(copy_gm_to_ub{}), ...)`把该block负责的2行Query从Device GM拷贝到UB中的Query Tensor。
6. 对当前KV range内的全部tile，kernel依次执行decode阶段。每个`64 × 128`tile的处理步骤为：
   1. kernel根据range编号、range内tile编号和GQA映射计算当前Key/Value tile在Device GM中的地址。
   2. kernel用`copy(make_copy(copy_gm_to_ub{}), ...)`把当前Key tile和Value tile从Device GM拷贝到UB中的Key Tensor和Value Tensor。
   3. `compute_decode_pair_tile`函数用`load<unpack>`从UB Query Tensor和Key Tensor读取`bfloat16_t`元素，并用`cast`把它们转换为`float`。
   4. 对Key tile中的每一行，kernel用乘法和`fma`同时计算2个Query head的QK点积，再用`reduce_sum`把每个点积归约成1个score。因此每个K/V tile只加载一次，即可同时服务2个Query head。
   5. kernel把128行score写入UB中的Score Tensor，并维护2个Query head在当前tile内的最大score。
   6. 如果当前tile不是该KV range的第一个tile，kernel从UB State Tensor读取上一tile保存的`m/l/o`，用新旧最大值差把历史`l/o`缩放到新基准。
   7. kernel从UB Value Tensor读取`bfloat16_t`的Value行，用`cast`转换为`float`，用`exp_diff`计算当前tile权重，用`fma`累加`weight × Value`，同时更新2个head的softmax分母`l`和未归一化输出`o`。
7. 当前KV range处理完成后，kernel把2个head的range局部`m/l/o`从UB写入Device GM workspace。每个`(head, range)`占用256个float：偏移0开始是128个未归一化输出，偏移128开始是max，偏移192开始是sum；workspace总大小为`32 × 4 × 256 × 4 = 131072 bytes`。
8. 每个block写完range局部状态后，先用`sync_data_barrier`保证GM写入完成，再对GM中的完成计数执行一次`atomic_add`；前63个block随后退出，释放AIV给后续block调度。
9. 第64个完成的block是唯一merge block。前序block已经用`sync_data_barrier`保证partial写入GM后才开始merge，该block从GM workspace依次读回每个Query head的4个range局部状态。`compute_merge_decode`函数先得到全局max。
10. 对每个KV range，merge阶段读取局部sum和128个未归一化输出，用`exp_diff`计算`exp(local_max - global_max)`，把局部sum和局部输出缩放到全局max基准，并累加得到全局sum和全局未归一化输出。
11. merge阶段用全局未归一化输出除以全局sum，得到`float`注意力输出，用`cast`转换为`bfloat16_t`，用`deinterleave`提取有效结果，用`store`写入UB Output Tensor，再用`copy(copy_out, ...)`把128个输出元素写入Device GM的output Tensor。
12. Host侧程序同步stream，调用`aclrtMemcpy`把输出从Device GM拷贝回Host内存，并写入`output/output.bin`。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** ${install_path} 为CANN包安装目录，未指定安装目录时默认安装至 /usr/local/Ascend 下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  ./demo                                                                    # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行`rm CMakeCache.txt`后重新cmake。

- 编译选项说明

  <table border="2">
    <caption>编译选项表</caption>
    <tr><td align="left">选项</td><td align="left">可选值</td><td align="left">说明</td></tr>
    <tr><td align="left">`CMAKE_ASC_RUN_MODE`</td><td align="left">`npu`（默认）、`sim`</td><td align="left">运行模式：NPU 运行、NPU仿真</td></tr>
    <tr><td align="left">`CMAKE_ASC_ARCHITECTURES`</td><td align="left">`dav-3510`</td><td align="left">NPU 架构：dav-3510 对应 Ascend 950PR&950DT系列产品</td></tr>
  </table>

  > **说明：** 本样例仅支持dav-3510架构（对应 Ascend 950PR&950DT系列产品）。

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
