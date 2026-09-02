# SIMT编程模式实现Gather算子样例

## 概述

本样例基于Ascend C SIMT编程方式实现支持泛化shape的Gather算子，包括基础版gather和增强版gather_v2。gather算子从二维输入张量中采集指定索引的行数据，gather_v2算子支持从多维输入张量中按指定维度收集数据，并支持batch_dims批量处理模式。样例展示了泛化场景下离散内存访问类算子的开发方法。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.0.0

## 目录结构介绍

```text
├── general_gather
│   ├── CMakeLists.txt         // cmake编译文件
│   ├── gather_v2.asc          // SIMT实现gather_v2调用样例
│   ├── gather.asc             // SIMT实现gather调用样例
│   └── README.md
```

## 算子描述

### 1. gather算子

- 算子功能：   
  gather算子实现了从shape为[M,N]的二维输入张量input中获取指定索引的m行数据的功能，这m行的行索引由输入index指定。算子输出output第i行数据的计算公式为：

  ```text
  output[i] = input[index[i]]
  ```

- 算子规格：
  <table>
  <tr><td rowspan="1" align="center">算子类型(OpType)</td><td colspan="4" align="center">gather</td></tr>
  <tr><td rowspan="3" align="center">算子输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[M,N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">index</td><td align="center">[m] (m < M, m <= 65535 * 2048)</td><td align="center">uint32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">算子输出</td><td align="center">output</td><td align="center">[m,N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">gather_custom</td></tr>
  </table>

- 数据切分：
  * gridDim：根据具体输入shape动态分配，最大不超过65535
  * blockDim：根据具体输入shape动态分配，最大不超过2048
  * 单线程处理：1行
  * 最大处理能力：65535 * 2048 = 134215680行

- 算子实现：   
  gather算子的实现流程为从输入input（Global Memory）中获取指定索引的数据。基于上述数据切分，首先计算线程应处理数据的索引，然后通过赋值操作将一行数据存储到Global Memory上。由于计算过程相对简单，设置核函数的最大线程数限制为2048。

- 调用实现：   
  使用内核调用符`<<<>>>`调用核函数。

### 2. gather_v2算子

- 算子功能：   
  gather_v2算子实现了从多维输入张量input中按照指定维度axis收集数据的功能，indices张量指定了要收集的索引位置。支持batch_dims批量处理模式，让不同的batch使用不同的索引集合。
- 处理流程：   
  例如输入张量input的shape为[2,2,3,2]，索引张量indices的shape为[2,2]：

  ```text
  input:
   [[[[ 1,  2],
      [ 3,  4],
      [ 5,  6]],

     [[ 7,  8],
      [ 9, 10],
      [11, 12]]],


    [[[13, 14],
      [15, 16],
      [17, 18]],

     [[19, 20],
      [21, 22],
      [23, 24]]]]

  indices:
   [[1, 2],
    [0, 1]]
  ```

  axis=2, batch_dims=1表明收集维度为2，并且每个batch下使用不同的索引：
  - batch=0: output[0, :, :, :] = input[0, :, [1, 2], :]，即从input[0]的维度2上收集indices[0]对应的切片
  - batch=1: output[1, :, :, :] = input[1, :, [0, 1], :]，即从input[1]的维度2上收集indices[1]对应的切片

  ```text
  output:
   [[[[ 3,  4],
      [ 5,  6]],

     [[ 9, 10],
      [11, 12]]],

    [[[13, 14],
      [15, 16]],

     [[19, 20],
      [21, 22]]]]
  ```

- 算子规格：
  <table>
  <tr><td rowspan="1" align="center">算子类型(OpType)</td><td colspan="5" align="center">gather_v2</td></tr>
  <tr><td rowspan="5" align="center">算子输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">description</td></tr>
  <tr><td align="center">input</td><td align="center">[128,20000,512]</td><td align="center">float</td><td align="center">ND</td><td align="center">多维输入张量</td></tr>
  <tr><td align="center">indices</td><td align="center">[128,2048]</td><td align="center">uint32_t / int32_t</td><td align="center">ND</td><td align="center">索引张量，指定收集位置</td></tr>
  <tr><td align="center">axis</td><td align="center">-</td><td align="center">int32_t</td><td align="center">-</td><td align="center">标量，用于指定收集维度</td></tr>
  <tr><td align="center">batch_dims</td><td align="center">-</td><td align="center">int32_t</td><td align="center">-</td><td align="center">标量，用于指定批处理维度</td></tr>
  <tr><td rowspan="1" align="center">算子输出</td><td align="center">output</td><td align="center">[128,2048,512]</td><td align="center">float</td><td align="center">ND</td><td align="center">收集后的输出张量</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="5" align="center">gather_custom_v2</td></tr>
  </table>

- 约束说明：
  * indices：indices的前batch_dims维必须与input的前batch_dims维相同，即indices.shape[0:batch_dims] = input.shape[0:batch_dims]
  * axis：收集维度axis不能小于batch_dims，且必须小于input的维度数，即batch_dims <= axis < input.rank
  * batch_dims：批量维度数不能超过input和indices中较小的维度数，即0 <= batch_dims <= min(input.rank, indices.rank)
  * output.shape：input.shape[:axis] + indices.shape[batch_dims:] + input.shape[axis+1:]
  * 样例实现中，axis和batch_dims也支持传入负值，并会在计算前转换为对应的非负维度索引
- 数据切分：
  * gridDim：根据收集总数据量动态分配。优先根据blockDim按需计算block数；当所需block数超过设备AIV核数时，取设备AIV核数作为block数
  * blockDim：根据具体总数据量动态分配，最大不超过2048
  * 单线程处理：根据具体收集数据量均衡分配，每个线程处理的元素数量最多相差1个。在核函数中以线程总数作为循环步幅，每个线程从起始位置begin = blockIdx.x * blockDim.x + threadIdx.x开始，以步长stride = gridDim.x * blockDim.x遍历并处理元素

  这种切分方式的优势在于：
  * 负载均衡：所有线程的工作量差异最多为1个元素，避免线程空闲
  * 内存访问友好：相邻线程访问连续的内存地址，有利于合并访存
- 算子实现：   
  gather_v2算子的实现流程为从多维输入张量input中按照指定维度axis收集数据。基于上述数据切分策略，每个线程会动态处理一部分数据，对每个输出索引进行分解，将输出的一维索引转换为逻辑坐标，从而根据indices找到对应的收集位置，最后计算input的线性索引并完成数据搬运。
- 调用实现：   
  使用内核调用符`<<<>>>`调用核函数。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。

- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  mkdir -p build && cd build;   # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # 编译工程
  ./gather                      # 执行样例
  ./gather_v2                   # 执行样例
  ```

  编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：本样例仅支持 dav-3510（Ascend 950PR/Ascend 950DT） |

  执行结果如下，说明精度对比成功：

  ```text
  [Success] Case accuracy is verification passed.
  ```
