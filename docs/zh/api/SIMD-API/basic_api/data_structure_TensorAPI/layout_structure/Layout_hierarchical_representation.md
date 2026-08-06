# Layout和层次化表述法

## Layout

我们使用Shape和Stride来表示Layout排布格式，例如下图中的行优先和列优先排布。

Shape用于表达Tensor形状，Stride则用于区分不同的排布方式。

- 行优先：Shape(2, 4)，Stride(4, 1)
- 列优先：Shape(2, 4)，Stride(1, 2)

图中每个方格中的数字表示该位置元素在内存中按顺序排列时的下标。对于相同的矩阵位置，排布方式不同时，其在内存中的顺序可能不同。例如，矩阵坐标(1, 0)在行优先和列优先的情况下，对应元素在内存中的顺序分别是4和1。

**图1** 行优先排布

![行优先排布](../../../../figures/row_major_layout.png)

**图2** 列优先排布

![列优先排布](../../../../figures/column_major_layout.png)

## 层次化表述法

通常Shape或Stride中的元素是一个单独的整数，但遇到一些复杂的内存排布情况时，普通二维表述可能无法准确表达，因此这里采用[层次化表述法](https://mgarland.org/papers/2023/graphene/)。在这种方式中，元素也可以是一个元组。元组的第一个元素用于描述不同行层次中的矩阵行数，第二个元素用于描述不同层次中的矩阵列数。

例如在下图中，Layout的Shape和Stride分别为：

- Shape((2, 3), (2, 4))
- Stride((1, 4), (2, 12))

**图3** 层次化表述法

![层次化表述法](../../../../figures/hierarchical_representation.png "层次化表述法")

图中展示了两层矩阵：内层矩阵为内部用灰色线包裹的矩阵，外层矩阵为将内层矩阵视为一个元素时，用黑色线包裹的矩阵。

Shape的第一个元素描述行方向的形状，（2，3）表示内层矩阵和外层矩阵的行数分别为2和3；Shape的第二个元素描述列方向的形状，（2，4）表示内层矩阵和外层矩阵的列数分别为2和4。

Stride中的每个元素与Shape中的元素对应，表示该对应维度下，相邻元素首地址在内存地址上的间隔。图片中用箭头表示了每个维度相邻元素的首地址间隔。

## 常见的Layout分形

在基于Ascend C进行矩阵编程的场景中，会用到以下几种常用的Layout。这些格式都采用如上文介绍的层次化表述法来表达，要求有内外层两层矩阵，具体表达方式如下，其中_x是`Std::Int<x>`的简写形式，比如_16表示类型`Std::Int<16>`，值16保存在类型中：

```cpp
Layout = ((Shape): (Stride))
Shape = ((ShapeRow0, ShapeRow1), (ShapeColumn0, ShapeColumn1))
Stride = ((StrideRow0, StrideRow1), (StrideColumn0, StrideColumn1))
```

- NZ Layout

    NZ Layout格式的ShapeRow0、ShapeColumn0为固定值。内层分形的Shape为16 * (32Byte / sizeof(T))，StrideRow0、StrideColumn0也为固定值，即内层分形按Z字形组织，外层分形按N字形组织，因此StrideColumn1需要满足按整块对齐的要求。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_16{}, ShapeRow1), (_32{} / sizeof(T), ShapeColumn1))
    Stride = ((_32{} / sizeof(T), StrideRow1), (_1{}, StrideColumn1))
    // StrideColumn1应满足：
    // StrideColumn1 % (ShapeRow0 * ShapeColumn0) == 0
    ```

    **图4** NZ Layout  
    ![NZ-格式](../../../../figures/nz_format.png)
    
    下面是一个连续的NZ Layout示例，其中C0_ELEMENT =_32{} / sizeof(T)。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_16{}, ceil_div(row, _16{})), (C0_ELEMENT, ceil_div(column, C0_ELEMENT)))
    Stride = ((C0_ELEMENT, C0_ELEMENT * _16{}), (_1{}, C0_ELEMENT * ceil_align(row, _16{})))
    ```

- ZN Layout

    ZN Layout格式的ShapeRow0、ShapeColumn0为固定值。内层分形的Shape为（32Byte / sizeof(T)）* 16，StrideRow0、StrideColumn0也为固定值，即内层分形按N字形组织，外层分形按Z字形组织，因此StrideRow1需要满足按整块对齐的要求。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((C0_ELEMENT, ShapeRow1), (_16{}, ShapeColumn1))
    Stride = ((_1{}, StrideRow1), (C0_ELEMENT, StrideColumn1))
    // StrideRow1应满足：
    // StrideRow1 % (ShapeRow0 * ShapeColumn0) == 0
    ```

    **图5** ZN Layout  
    ![ZN-格式](../../../../figures/zn_fractals_of_half.png)

    下面是一个连续的ZN Layout示例，其中C0_ELEMENT =_32{} / sizeof(T)。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((C0_ELEMENT, ceil_div(row, C0_ELEMENT)), (_16{}, ceil_div(column, _16{})))
    Stride = ((_1{}, C0_ELEMENT * ceil_align(column, _16{})), (C0_ELEMENT, C0_ELEMENT * _16{}))
    ```

- DNExt Layout

    DNExt Layout格式的ShapeRow0、ShapeColumn0为固定值，即内层分形固定为1 * 1。StrideRow0、StrideColumn0也固定为0，表示内层只有一个元素，没有额外的步长概念。外层按列优先方式连续存储，因此StrideColumn1等于行数。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, ShapeRow1), (_1{}, ShapeColumn1))
    Stride = ((_0{}, _1{}), (_0{}, StrideColumn1))
    ```

    **图6** DNExt Layout
    ![DNExt Layout](../../../../figures/column_major_layout.png)

    下面是一个连续的DNExt Layout示例。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, row), (_1{}, column))
    Stride = ((_0{}, _1{}), (_0{}, row))
    ```

- NDExt Layout

    NDExt Layout格式的ShapeRow0、ShapeColumn0为固定值，即内层分形固定为1 * 1。StrideRow0、StrideColumn0也固定为0，表示内层只有一个元素，没有额外的步长概念。外层按行优先方式连续存储，因此StrideRow1等于列数，StrideColumn1固定为1。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, ShapeRow1), (_1{}, ShapeColumn1))
    Stride = ((_0{}, StrideRow1), (_0{}, _1{}))
    ```

    **图7** NDExt Layout
    ![NDExt Layout](../../../../figures/row_major_layout.png)

    下面是一个连续的NDExt Layout示例。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, row), (_1{}, column))
    Stride = ((_0{}, column), (_0{}, _1{}))
    ```

- DN Layout

    DN Layout是一种二维列优先排布格式。从层次化表述法角度看，它可以视为只有外层矩阵的特例，其中行方向步长固定为1，列方向步长等于行数。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = (ShapeRow, ShapeColumn)
    Stride = (_1{}, ShapeRow)
    ```

    下面是一个连续的DN Layout示例。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = (row, column)
    Stride = (_1{}, row)
    ```

- ND Layout

    ND Layout是一种二维行优先排布格式。从层次化表述法角度看，它可以视为只有外层矩阵的特例，其中列方向步长固定为1，行方向步长等于列数。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = (ShapeRow, ShapeColumn)
    Stride = (ShapeColumn, _1{})
    ```

    下面是一个连续的ND Layout示例。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = (row, column)
    Stride = (column, _1{})
    ```

- ZZ Layout

    ZZ Layout格式的ShapeRow0、ShapeColumn0为固定值。内层分形的Shape与NZ Layout一致，StrideRow0、StrideColumn0也为固定值，即内层分形按Z字形组织，外层分形同样按Z字形组织，因此列方向连续，行方向跨度由整行分形块数决定。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_16{}, ShapeRow1), (C0_ELEMENT, ShapeColumn1))
    Stride = ((C0_ELEMENT, StrideRow1), (_1{}, C0_ELEMENT * _16{}))
    ```

    下面是一个连续的ZZ Layout示例，其中C0_ELEMENT =_32{} / sizeof(T)。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_16{}, ceil_div(row, _16{})), (C0_ELEMENT, ceil_div(column, C0_ELEMENT)))
    Stride = ((C0_ELEMENT, _16{} * ceil_align(column, C0_ELEMENT)), (_1{}, C0_ELEMENT * _16{}))
    ```

- NN Layout

    NN Layout格式的ShapeRow0、ShapeColumn0为固定值，仅用于fp8_e8m0_t场景。内层分形固定为2 * 16，StrideRow0、StrideColumn0也为固定值，即内层分形按N字形组织。外层分形同样按N字形组织。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_2{}, ShapeRow1), (_16{}, ShapeColumn1))
    Stride = ((_1{}, _32{}), (_2{}, StrideColumn1))
    ```

    下面是一个连续的NN Layout示例。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_2{}, row / _2{}), (_16{}, ceil_div(column, _16{})))
    Stride = ((_1{}, _32{}), (_2{}, row * _16{}))
    ```

- ScaleAND Layout

    ScaleAND Layout格式的ShapeRow0、ShapeColumn0为固定值，仅支持fp8_e8m0_t的scaleA数据，且要求C0_ELEMENT == 2。其物理布局与连续的NDExt Layout一致，即内层固定为1 * 1，外层按行优先方式连续存储，语义上用于描述scaleA不转置的场景。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, ShapeRow1), (_1{}, ShapeColumn1))
    Stride = ((_0{}, StrideRow1), (_0{}, _1{}))
    ```

    下面是一个连续的ScaleAND Layout示例。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, row), (_1{}, column))
    Stride = ((_0{}, column), (_0{}, _1{}))
    ```

- ScaleADN Layout

    ScaleADN Layout格式的ShapeRow0、ShapeColumn0为固定值，仅支持fp8_e8m0_t的scaleA数据，且要求C0_ELEMENT == 2。其列方向内层分形固定为2，行方向内层固定为1，外层按列优先方式组织。语义上用于描述scaleA转置的场景。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, ShapeRow1), (_2{}, ShapeColumn1))
    Stride = ((_0{}, _2{}), (_1{}, StrideColumn1))
    ```

    下面是一个连续的ScaleADN Layout示例。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, row), (_2{}, column / _2{}))
    Stride = ((_0{}, _2{}), (_1{}, _2{} * row))
    ```

- ScaleBND Layout

    ScaleBND Layout格式的ShapeRow0、ShapeColumn0为固定值，仅支持fp8_e8m0_t的scaleB数据，且要求C0_ELEMENT == 2。其行方向内层分形固定为2，列方向内层固定为1，外层按行优先方式组织。语义上用于描述scaleB转置的场景。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_2{}, ShapeRow1), (_1{}, ShapeColumn1))
    Stride = ((_1{}, StrideRow1), (_0{}, _2{}))
    ```

    下面是一个连续的ScaleBND Layout示例。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_2{}, row / _2{}), (_1{}, column))
    Stride = ((_1{}, _2{} * column), (_0{}, _2{}))
    ```

- ScaleBDN Layout

    ScaleBDN Layout格式的ShapeRow0、ShapeColumn0为固定值，仅支持fp8_e8m0_t的scaleB数据，且要求C0_ELEMENT == 2。其物理布局与连续的DNExt Layout一致，即内层固定为1 * 1，外层按列优先方式连续存储，语义上用于描述scaleB不转置的场景。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, ShapeRow1), (_1{}, ShapeColumn1))
    Stride = ((_0{}, _1{}), (_0{}, StrideColumn1))
    ```

    下面是一个连续的ScaleBDN Layout示例。

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, row), (_1{}, column))
    Stride = ((_0{}, _1{}), (_0{}, row))
    ```

MX scaleA要求在矩阵GM上按照ScaleAND Layout或ScaleADN Layout格式排布，在L1上需满足按行读取需求，即按照ZZ Layout格式排布。如下图所示，若GM上scaleA矩阵为ScaleAND或者ScaleADN分形排布，搬运到L1后分形为ZZ排布。

**图8** scaleA矩阵在不同位置上的排布格式
![scaleA矩阵在不同位置上的排布格式](../../../../figures/scalea_matrix_layout.png)

MX scaleB要求在矩阵GM上按照ScaleBND Layout或ScaleBDN Layout格式排布，在L1上需满足按列读取需求，即按照NN Layout格式排布。如下图所示，若GM上scaleB矩阵为ScaleBND或者ScaleBDN分形排布，搬运到L1后分形为NN排布。

**图9** scaleB矩阵在不同位置上的排布格式
![scaleB矩阵在不同位置上的排布格式](../../../../figures/scaleb_matrix_layout.png)

## 卷积特征图Layout

卷积特征图Layout使用扁平的多维Shape和行主序Stride表示，不使用矩阵分形的层次化Shape。支持的LayoutPattern如下。

| LayoutPattern | Shape | Stride |
| :--- | :--- | :--- |
| `NCHWLayoutPtn` | `(N, C, H, W)` | `(C * H * W, H * W, W, 1)` |
| `NHWCLayoutPtn` | `(N, H, W, C)` | `(H * W * C, W * C, C, 1)` |
| `NC1HWC0LayoutPtn` | `(N, C1, H, W, C0)` | `(C1 * H * W * C0, H * W * C0, W * C0, C0, 1)` |

`NC1HWC0LayoutPtn`将通道维C拆分为C1和C0，需要满足`C = C1 * C0`。上述Layout可通过[MakeFrameLayout](MakeFrameLayout.md)构造。

## Layout分形结构数据表

每种分形对应的Layout格式如下表所示，其中T指的是支持的数据类型中除fp8_e8m0_t之外的数据类型，C0_ELEMENT =_32{} / sizeof(T)；特殊情况下当T为fp4x2_e2m1_t或fp4x2_e1m2_t时，C0_ELEMENT =_64{}。

| LayoutFormatPattern | 类型 | ShapeRow0 | ShapeRow1 | ShapeColumn0 | ShapeColumn1 | StrideRow0 | StrideRow1 | StrideColumn0 | StrideColumn1 |
|-------------|------|-----------|-----------|--------------|--------------|------------|------------|---------------|---------------|
| NZLayoutPtn | T | _16{} | ceil_div(row,_16{}) | C0_ELEMENT | ceil_div(column, C0_ELEMENT) | C0_ELEMENT | C0_ELEMENT *_16{} | _1{} | C0_ELEMENT * ceil_align(row, _16{}) |
| ZNLayoutPtn | T | C0_ELEMENT | ceil_div(row, C0_ELEMENT) | _16{} | ceil_div(column,_16{}) | _1{} | C0_ELEMENT * ceil_align(column, _16{}) | C0_ELEMENT | C0_ELEMENT *_16{} |
| DNExtLayoutPtn | T | _1{} | row | _1{} | column | _0{} | _1{} | _0{} | row |
| NDExtLayoutPtn | T | _1{} | row | _1{} | column | _0{} | column | _0{} | _1{} |
| DNLayoutPtn | T | row | - | column | - | _1{} | - | row | - |
| NDLayoutPtn | T | row | - | column | - | column | - | _1{} | - |
| ZZLayoutPtn | T | _16{} | ceil_div(row,_16{}) | C0_ELEMENT | ceil_div(column, C0_ELEMENT) | C0_ELEMENT | _16{} * ceil_align(column, C0_ELEMENT) | _1{} | C0_ELEMENT *_16{} |
| ZZLayoutPtn | fp8_e8m0_t | _16{} | ceil_div(row,_16{}) | _2{} | column / _2{} | _2{} | column * _16{} | _1{} | _32{} |
| NNLayoutPtn | fp8_e8m0_t | _2{} | row / _2{} | _16{} | ceil_div(column,_16{}) | _1{} | _32{} | _2{} | row * _16{} |
| ScaleANDLayoutPtn | fp8_e8m0_t | _1{} | row | _1{} | column | _0{} | column | _0{} | _1{} |
| ScaleADNLayoutPtn | fp8_e8m0_t | _1{} | row | _2{} | column / _2{} | _0{} | _2{} | _1{} | _2{} * row |
| ScaleBNDLayoutPtn | fp8_e8m0_t | _2{} | row / _2{} | _1{} | column | _1{} | _2{} * column | _0{} | _2{} |
| ScaleBDNLayoutPtn | fp8_e8m0_t | _1{} | row | _1{} | column | _0{} | _1{} | _0{} | row |
