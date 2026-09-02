# SIMT实现Sobel边缘检测样例

## 概述

本样例以Sobel边缘检测为例，展示了如何通过Warp shuffle指令复用相邻线程加载的数据来完成 3×3 卷积计算，以及结合[asc_ballot()](../../../../../../docs/zh/api/SIMT-API/Warp_functions/Warp_vote_functions/asc_ballot.md)实现边缘像素的紧凑输出。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.1.0

## 目录结构介绍

```text
├── sobel_warp_shfl
│   ├── figures                // README中的图片资源
│   ├── CMakeLists.txt         // cmake编译文件
│   ├── sobel_warp_shfl.asc    // Sobel边缘检测实现 & 调用样例
│   └── README.md
```

## 样例描述

### 样例功能

本样例计算输入灰度图像的边缘幅值，并将满足阈值条件的边缘像素输出到紧凑列表`edge_list`。`edge_list`中每行包含边缘像素的x坐标、y坐标和边缘幅值（`strength`）。

本样例使用 **Sobel 边缘检测算法** 计算边缘幅值。Sobel 是一种常用的图像边缘检测算法，用于根据像素周围的灰度变化计算该像素的边缘幅值。对于图像中坐标为(x, y)的非边界像素，Sobel 使用该像素周围的 3×3 邻域分别计算水平梯度`Gx`和垂直梯度`Gy`：

<p align="center">
  <img src="./figures/sobel.png" width="45%">
</p>

`Gx`和`Gy`的计算方式如下：

```text
Gx = -top_left + top_right - 2 * middle_left + 2 * middle_right - bottom_left + bottom_right
Gy = -top_left - 2 * top - top_right + bottom_left + 2 * bottom + bottom_right
```

该像素的边缘幅值为：

```text
strength = |Gx| + |Gy|
```

对于图像边界像素（无法构成完整 3×3 邻域），幅值直接置为0；对于非边界像素，计算得到`strength`后，仅将`strength`超过阈值（本样例中阈值为200）的边缘像素坐标和幅值写入edge_list。

### 样例规格

<table>
<tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">sobel_edge_detect</td></tr>
<tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">input</td><td align="center">[1025,1023]</td><td align="center">uint8_t</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">edge_list</td><td align="center">[N,3]</td><td align="center">uint16_t</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">sobel_edge_detect</td></tr>
</table>

### 样例实现

**Warp内相邻像素获取**

在SIMT编程模型中，一个Warp由32个线程组成。本样例需要通过Warp shuffle获取同一行中相邻像素的值，而shuffle只能在同一Warp内的线程间传递数据，因此必须保证同一Warp内所有线程处理图像同一行的连续32个像素，每个线程负责其中一个像素。

为保证同一个Warp内的32个线程始终处理同一行像素，采用如下坐标映射方式：

```cpp
int tiles_x = (width + warpSize - 1) / warpSize;    // 图像宽度方向按Warp分块的块数（每块32列）
int x = (warp_id % tiles_x) * warpSize + lane_id;   // 像素列号：当前Warp在横向的起始位置 + Warp内偏移，保证同Warp同行
int y = warp_id / tiles_x;                          // 像素行号：第几行Warp
```

<p align="center">
  <img src="./figures/shfl.png" width="80%">
</p>


如上图所示，计算Sobel幅值需要获取当前像素周围3×3邻域共9个像素值。而相邻线程计算各自的Sobel幅值时，同样需要其周围3×3邻域的像素值，因此不同线程的3×3邻域存在重叠。利用这一特点，每个线程先从Global Memory读取同一x坐标上的上、中、下三个像素值，分别保存到寄存器变量top、mid和bot，对应当前像素3×3邻域的中间一列；然后通过[asc_shfl_up()](../../../../../../docs/zh/api/SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl_up.md)和[asc_shfl_down()](../../../../../../docs/zh/api/SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl_down.md)从相邻线程的寄存器中获取左右两列数据，避免再次从Global Memory读取。



```cpp
// 每个线程先读取当前像素所在列的上、中、下三个像素。
// in_image标识当前线程坐标是否在图像有效范围内，为false时像素值置0。
int top = in_image ? input[(y - 1) * width + x] : 0;
int mid = in_image ? input[y * width + x] : 0;
int bot = in_image ? input[(y + 1) * width + x] : 0;

// 从相邻线程获取左右两列数据。
int shfl_top_left = asc_shfl_up(top, 1, warpSize);
int shfl_top_right = asc_shfl_down(top, 1, warpSize);
int shfl_mid_left = asc_shfl_up(mid, 1, warpSize);
int shfl_mid_right = asc_shfl_down(mid, 1, warpSize);
int shfl_bot_left = asc_shfl_up(bot, 1, warpSize);
int shfl_bot_right = asc_shfl_down(bot, 1, warpSize);
```

对于需要参与Sobel计算的非边界像素，Warp边界处的线程需要特殊处理：第0号线程的左邻居和第31号线程的右邻居不在当前Warp内，因此需要从Global Memory读取数据：

```cpp
// Warp边界处无法通过shuffle获取相邻像素，需要从Global Memory读取。
int top_left = (lane_id == 0) ? input[(y - 1) * width + (x - 1)] : shfl_top_left;
int top_right = (lane_id == 31) ? input[(y - 1) * width + (x + 1)] : shfl_top_right;
int mid_left = (lane_id == 0) ? input[y * width + (x - 1)] : shfl_mid_left;
int mid_right = (lane_id == 31) ? input[y * width + (x + 1)] : shfl_mid_right;
int bot_left = (lane_id == 0) ? input[(y + 1) * width + (x - 1)] : shfl_bot_left;
int bot_right = (lane_id == 31) ? input[(y + 1) * width + (x + 1)] : shfl_bot_right;
```

获取3×3邻域的全部9个像素后，按照Sobel公式计算水平梯度`Gx`和垂直梯度`Gy`，即可得到边缘幅值。

**asc_ballot实现紧凑输出**

紧凑输出需要将满足阈值条件的边缘像素连续写入edge_list。对于同一个Warp，可能只有部分线程的边缘幅值满足阈值条件，如果每个边缘线程都单独执行[asc_atomic_add()](../../../../../../docs/zh/api/SIMT-API/atomic_operations/asc_atomic_add.md)申请输出位置，一个Warp内最多会产生32次原子操作。

本样例使用`asc_ballot()`收集Warp内各线程是否满足阈值条件，并生成edge_mask。edge_mask中第i位为1，表示Warp内的第i号线程的边缘幅值满足阈值条件：

```cpp
uint32_t edge_mask = asc_ballot(strength >= THRESHOLD);
```

edge_mask用于完成两个操作：

- 第0号线程通过[__popc(edge_mask)](../../../../../../docs/zh/api/SIMT-API/math_functions/integer_math_functions/__popc.md)统计本Warp内边缘线程数量，并通过一次`asc_atomic_add()`为整个Warp申请连续输出位置。
- 每个边缘线程根据edge_mask计算自己在Warp内的局部偏移，写入edge_list中的对应位置。

第0号线程申请输出位置后，通过[asc_shfl()](../../../../../../docs/zh/api/SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl.md)将起始位置广播给Warp内所有线程：

```cpp
uint32_t warp_base = 0;
if (lane_id == 0) {
    // 第0号线程为本Warp中的所有边缘线程一次性申请输出位置。
    warp_base = asc_atomic_add(edge_count, static_cast<uint32_t>(__popc(edge_mask)));
}
warp_base = asc_shfl(warp_base, 0, warpSize);
```

每个边缘线程再根据edge_mask计算自己在Warp内的局部偏移，将`{x, y, strength}`写入edge_list：

```cpp
if (edge_mask & lanemask_eq()) {
    // 统计当前线程之前有多少个边缘线程，得到本Warp内的紧凑写入偏移。
    uint32_t lower_mask = edge_mask & lanemask_lt();
    int local_idx = __popc(lower_mask);
    edge_list[warp_base + local_idx] = {
        static_cast<uint16_t>(x),
        static_cast<uint16_t>(y),
        strength
    };
}
```

其中，`lanemask_eq()`只生成当前线程对应的掩码，用于检测当前线程的边缘幅值是否满足阈值条件；`lanemask_lt()`生成当前线程之前所有线程的掩码，用于统计当前线程之前满足阈值条件的线程数量，即当前边缘线程在本Warp内的紧凑写入偏移。

相比每个边缘线程独立执行一次`asc_atomic_add()`，该方式将一个Warp内最多32次原子操作压缩为1次原子操作。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。
- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  mkdir -p build && cd build;                              # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # 编译工程
  ./demo                                                   # 执行样例
  ```

  编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：本样例仅支持 dav-3510（Ascend 950PR/Ascend 950DT） |

  执行结果如下，说明精度对比成功。
  ```
  [Success] Case accuracy verification passed.
  ```
