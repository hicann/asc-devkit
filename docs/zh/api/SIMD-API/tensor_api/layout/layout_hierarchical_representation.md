# layout和层次化表述法

## layout

我们使用shape和stride来表示layout排布格式，例如下图中的行优先和列优先排布。

shape用于表达Tensor形状，stride则用于区分不同的排布方式。

- 行优先：shape(2, 4)，stride(4, 1)
- 列优先：shape(2, 4)，stride(1, 2)

图中每个方格中的数字表示该位置元素在内存中按顺序排列时的下标。对于相同的矩阵位置，排布方式不同时，其在内存中的顺序可能不同。例如，矩阵坐标(1, 0)在行优先和列优先的情况下，对应元素在内存中的顺序分别是4和1。

**图1** 行优先排布

![行优先排布](../../../figures/row_major_layout.png)

**图2** 列优先排布

![列优先排布](../../../figures/column_major_layout.png)

## 层次化表述法

通常shape或stride中的元素是一个单独的整数，但遇到一些复杂的内存排布情况时，普通二维表述可能无法准确表达，因此这里采用[层次化表述法](https://mgarland.org/papers/2023/graphene/)。在这种方式中，元素也可以是一个元组。元组的第一个元素用于描述不同行层次中的矩阵行数，第二个元素用于描述不同层次中的矩阵列数。

例如在下图中，layout的shape和stride分别为：

- shape((2, 3), (2, 4))
- stride((1, 4), (2, 12))

**图3** 层次化表述法

![层次化表述法](../../../figures/hierarchical_representation.png "层次化表述法")

图中展示了两层矩阵：内层矩阵为内部用灰色线包裹的矩阵，外层矩阵为将内层矩阵视为一个元素时，用黑色线包裹的矩阵。

shape的第一个元素描述行方向的形状，（2，3）表示内层矩阵和外层矩阵的行数分别为2和3；shape的第二个元素描述列方向的形状，（2，4）表示内层矩阵和外层矩阵的列数分别为2和4。

stride中的每个元素与shape中的元素对应，表示该对应维度下，相邻元素首地址在内存地址上的间隔。图片中用箭头表示了每个维度相邻元素的首地址间隔。

## 常见的layout分形

在基于Ascend C进行矩阵编程的场景中，会用到以下几种常用的layout。这些格式都采用如上文介绍的层次化表述法来表达，要求有内外层两层矩阵，具体表达方式如下，其中_x是`Std::Int<x>`的简写形式，比如_16表示类型`Std::Int<16>`，值16保存在类型中：

```cpp
layout = ((shape): (stride))
shape = ((shape_row0, shape_row1), (shape_column0, shape_column1))
stride = ((stride_row0, stride_row1), (stride_column0, stride_column1))
```

- NZ layout

    NZ layout格式的shape_row0、shape_column0为固定值。内层分形的shape为16 * (32Byte / sizeof(T))，stride_row0、stride_column0也为固定值，即内层分形按Z字形组织，外层分形按N字形组织，因此stride_column1需要满足按整块对齐的要求。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_16{}, shape_row1), (_32{} / sizeof(T), shape_column1))
    stride = ((_32{} / sizeof(T), stride_row1), (_1{}, stride_column1))
    // stride_column1应满足：
    // stride_column1 % (shape_row0 * shape_column0) == 0
    ```

    **图4** NZ layout
    ![NZ-格式](../../../figures/nz_format.png)

    下面是一个连续的NZ layout示例，其中c0_element =_32{} / sizeof(T)。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_16{}, ceil_div(row, _16{})), (c0_element, ceil_div(column, c0_element)))
    stride = ((c0_element, c0_element * _16{}), (_1{}, c0_element * ceil_align(row, _16{})))
    ```

- ZN layout

    ZN layout格式的shape_row0、shape_column0为固定值。内层分形的shape为（32Byte / sizeof(T)）* 16，stride_row0、stride_column0也为固定值，即内层分形按N字形组织，外层分形按Z字形组织，因此stride_row1需要满足按整块对齐的要求。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((c0_element, shape_row1), (_16{}, shape_column1))
    stride = ((_1{}, stride_row1), (c0_element, stride_column1))
    // stride_row1应满足：
    // stride_row1 % (shape_row0 * shape_column0) == 0
    ```

    **图5** ZN layout
    ![ZN-格式](../../../figures/zn_fractals_of_half.png)

    下面是一个连续的ZN layout示例，其中c0_element =_32{} / sizeof(T)。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((c0_element, ceil_div(row, c0_element)), (_16{}, ceil_div(column, _16{})))
    stride = ((_1{}, c0_element * ceil_align(column, _16{})), (c0_element, c0_element * _16{}))
    ```

- DNExt layout

    DNExt layout格式的shape_row0、shape_column0为固定值，即内层分形固定为1 * 1。stride_row0、stride_column0也固定为0，表示内层只有一个元素，没有额外的步长概念。外层按列优先方式连续存储，因此stride_column1等于行数。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, shape_row1), (_1{}, shape_column1))
    stride = ((_0{}, _1{}), (_0{}, stride_column1))
    ```

    **图6** DNExt layout
    ![DNExt layout](../../../figures/column_major_layout.png)

    下面是一个连续的DNExt layout示例。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, row), (_1{}, column))
    stride = ((_0{}, _1{}), (_0{}, row))
    ```

- NDExt layout

    NDExt layout格式的shape_row0、shape_column0为固定值，即内层分形固定为1 * 1。stride_row0、stride_column0也固定为0，表示内层只有一个元素，没有额外的步长概念。外层按行优先方式连续存储，因此stride_row1等于列数，stride_column1固定为1。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, shape_row1), (_1{}, shape_column1))
    stride = ((_0{}, stride_row1), (_0{}, _1{}))
    ```

    **图7** NDExt layout
    ![NDExt layout](../../../figures/row_major_layout.png)

    下面是一个连续的NDExt layout示例。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, row), (_1{}, column))
    stride = ((_0{}, column), (_0{}, _1{}))
    ```

- DN layout

    DN layout是一种二维列优先排布格式。从层次化表述法角度看，它可以视为只有外层矩阵的特例，其中行方向步长固定为1，列方向步长等于行数。

    ```cpp
    layout = ((shape) : (stride))
    shape = (shape_row, shape_column)
    stride = (_1{}, shape_row)
    ```

    下面是一个连续的DN layout示例。

    ```cpp
    layout = ((shape) : (stride))
    shape = (row, column)
    stride = (_1{}, row)
    ```

- ND layout

    ND layout是一种二维行优先排布格式。从层次化表述法角度看，它可以视为只有外层矩阵的特例，其中列方向步长固定为1，行方向步长等于列数。

    ```cpp
    layout = ((shape) : (stride))
    shape = (shape_row, shape_column)
    stride = (shape_column, _1{})
    ```

    下面是一个连续的ND layout示例。

    ```cpp
    layout = ((shape) : (stride))
    shape = (row, column)
    stride = (column, _1{})
    ```

- ZZ layout

    ZZ layout格式的shape_row0、shape_column0为固定值。内层分形的shape与NZ layout一致，stride_row0、stride_column0也为固定值，即内层分形按Z字形组织，外层分形同样按Z字形组织，因此列方向连续，行方向跨度由整行分形块数决定。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_16{}, shape_row1), (c0_element, shape_column1))
    stride = ((c0_element, stride_row1), (_1{}, c0_element * _16{}))
    ```

    下面是一个连续的ZZ layout示例，其中c0_element =_32{} / sizeof(T)。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_16{}, ceil_div(row, _16{})), (c0_element, ceil_div(column, c0_element)))
    stride = ((c0_element, _16{} * ceil_align(column, c0_element)), (_1{}, c0_element * _16{}))
    ```

- NN layout

    NN layout格式的shape_row0、shape_column0为固定值，仅用于fp8_e8m0_t场景。内层分形固定为2 * 16，stride_row0、stride_column0也为固定值，即内层分形按N字形组织。外层分形同样按N字形组织。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_2{}, shape_row1), (_16{}, shape_column1))
    stride = ((_1{}, _32{}), (_2{}, stride_column1))
    ```

    下面是一个连续的NN layout示例。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_2{}, row / _2{}), (_16{}, ceil_div(column, _16{})))
    stride = ((_1{}, _32{}), (_2{}, row * _16{}))
    ```

- ScaleAND layout

    ScaleAND layout格式的shape_row0、shape_column0为固定值，仅支持fp8_e8m0_t的scaleA数据，且要求c0_element == 2。其物理布局与连续的NDExt layout一致，即内层固定为1 * 1，外层按行优先方式连续存储，语义上用于描述scaleA不转置的场景。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, shape_row1), (_1{}, shape_column1))
    stride = ((_0{}, stride_row1), (_0{}, _1{}))
    ```

    下面是一个连续的ScaleAND layout示例。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, row), (_1{}, column))
    stride = ((_0{}, column), (_0{}, _1{}))
    ```

- ScaleADN layout

    ScaleADN layout格式的shape_row0、shape_column0为固定值，仅支持fp8_e8m0_t的scaleA数据，且要求c0_element == 2。其列方向内层分形固定为2，行方向内层固定为1，外层按列优先方式组织。语义上用于描述scaleA转置的场景。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, shape_row1), (_2{}, shape_column1))
    stride = ((_0{}, _2{}), (_1{}, stride_column1))
    ```

    下面是一个连续的ScaleADN layout示例。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, row), (_2{}, column / _2{}))
    stride = ((_0{}, _2{}), (_1{}, _2{} * row))
    ```

- ScaleBND layout

    ScaleBND layout格式的shape_row0、shape_column0为固定值，仅支持fp8_e8m0_t的scaleB数据，且要求c0_element == 2。其行方向内层分形固定为2，列方向内层固定为1，外层按行优先方式组织。语义上用于描述scaleB转置的场景。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_2{}, shape_row1), (_1{}, shape_column1))
    stride = ((_1{}, stride_row1), (_0{}, _2{}))
    ```

    下面是一个连续的ScaleBND layout示例。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_2{}, row / _2{}), (_1{}, column))
    stride = ((_1{}, _2{} * column), (_0{}, _2{}))
    ```

- ScaleBDN layout

    ScaleBDN layout格式的shape_row0、shape_column0为固定值，仅支持fp8_e8m0_t的scaleB数据，且要求c0_element == 2。其物理布局与连续的DNExt layout一致，即内层固定为1 * 1，外层按列优先方式连续存储，语义上用于描述scaleB不转置的场景。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, shape_row1), (_1{}, shape_column1))
    stride = ((_0{}, _1{}), (_0{}, stride_column1))
    ```

    下面是一个连续的ScaleBDN layout示例。

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, row), (_1{}, column))
    stride = ((_0{}, _1{}), (_0{}, row))
    ```

MX scaleA要求在矩阵GM上按照ScaleAND layout或ScaleADN layout格式排布，在L1上需满足按行读取需求，即按照ZZ layout格式排布。如下图所示，若GM上scaleA矩阵为ScaleAND或者ScaleADN分形排布，搬运到L1后分形为ZZ排布。

**图8** scaleA矩阵在不同位置上的排布格式
![scaleA矩阵在不同位置上的排布格式](../../../figures/scalea_matrix_layout.png)

MX scaleB要求在矩阵GM上按照ScaleBND layout或ScaleBDN layout格式排布，在L1上需满足按列读取需求，即按照NN layout格式排布。如下图所示，若GM上scaleB矩阵为ScaleBND或者ScaleBDN分形排布，搬运到L1后分形为NN排布。

**图9** scaleB矩阵在不同位置上的排布格式
![scaleB矩阵在不同位置上的排布格式](../../../figures/scaleb_matrix_layout.png)

## 卷积特征图layout

卷积特征图layout使用扁平的多维shape和行主序stride表示，不使用矩阵分形的层次化shape。支持的LayoutPattern如下。

**表1**  卷积特征图LayoutPattern说明

| LayoutPattern | shape | stride |
| :--- | :--- | :--- |
| `nchw_layout_ptn` | `(N, C, H, W)` | `(C * H * W, H * W, W, 1)` |
| `nhwc_layout_ptn` | `(N, H, W, C)` | `(H * W * C, W * C, C, 1)` |
| `nc1hwc0_layout_ptn` | `(N, C1, H, W, C0)` | `(C1 * H * W * C0, H * W * C0, W * C0, C0, 1)` |
| `ncdhw_layout_ptn` | `(N, C, D, H, W)` | `(C * D * H * W, D * H * W, H * W, W, 1)` |
| `ndc1hwc0_layout_ptn` | `(N, D, C1, H, W, C0)` | `(D * C1 * H * W * C0, C1 * H * W * C0, H * W * C0, W * C0, C0, 1)` |

`nc1hwc0_layout_ptn`和`ndc1hwc0_layout_ptn`将通道维C拆分为C1和C0，需要满足`C = C1 * C0`。上述layout可通过[make_frame_layout](make_frame_layout.md)构造。

## layout分形结构数据表

每种分形对应的layout格式如下表所示，其中T指的是支持的数据类型中除fp8_e8m0_t之外的数据类型，c0_element =_32{} / sizeof(T)；特殊情况下当T为fp4x2_e2m1_t或fp4x2_e1m2_t时，c0_element =_64{}。

**表2**  layout分形结构数据说明

| layout_format_pattern | 类型 | shape_row0 | shape_row1 | shape_column0 | shape_column1 | stride_row0 | stride_row1 | stride_column0 | stride_column1 |
|-------------|------|-----------|-----------|--------------|--------------|------------|------------|---------------|---------------|
| nz_layout_ptn | T | _16{} | ceil_div(row,_16{}) | c0_element | ceil_div(column, c0_element) | c0_element | c0_element *_16{} | _1{} | c0_element * ceil_align(row, _16{}) |
| zn_layout_ptn | T | c0_element | ceil_div(row, c0_element) | _16{} | ceil_div(column,_16{}) | _1{} | c0_element * ceil_align(column, _16{}) | c0_element | c0_element *_16{} |
| dn_ext_layout_ptn | T | _1{} | row | _1{} | column | _0{} | _1{} | _0{} | row |
| nd_ext_layout_ptn | T | _1{} | row | _1{} | column | _0{} | column | _0{} | _1{} |
| dn_layout_ptn | T | row | - | column | - | _1{} | - | row | - |
| nd_layout_ptn | T | row | - | column | - | column | - | _1{} | - |
| zz_layout_ptn | T | _16{} | ceil_div(row,_16{}) | c0_element | ceil_div(column, c0_element) | c0_element | _16{} * ceil_align(column, c0_element) | _1{} | c0_element *_16{} |
| zz_layout_ptn | fp8_e8m0_t | _16{} | ceil_div(row,_16{}) | _2{} | column / _2{} | _2{} | column * _16{} | _1{} | _32{} |
| nn_layout_ptn | fp8_e8m0_t | _2{} | row / _2{} | _16{} | ceil_div(column,_16{}) | _1{} | _32{} | _2{} | row * _16{} |
| scalea_nd_layout_ptn | fp8_e8m0_t | _1{} | row | _1{} | column | _0{} | column | _0{} | _1{} |
| scalea_dn_layout_ptn | fp8_e8m0_t | _1{} | row | _2{} | column / _2{} | _0{} | _2{} | _1{} | _2{} * row |
| scaleb_nd_layout_ptn | fp8_e8m0_t | _2{} | row / _2{} | _1{} | column | _1{} | _2{} * column | _0{} | _2{} |
| scaleb_dn_layout_ptn | fp8_e8m0_t | _1{} | row | _1{} | column | _0{} | _1{} | _0{} | row |
