# layout and hierarchical notation

<!-- md-trans-meta sourceCommit=46a50d69bd092786e44495d519df57743eefbfc6 translatedAt=2026-10-27T02:28:10.411Z pushedAt=2026-10-27T13:03:35.304Z nl=2 -->

## layout

The shape and stride are used to represent the layout format, for example, the row-major and column-major layouts shown in the following figures.

shape is used to express the Tensor shape, while stride is used to distinguish different layout methods.

- Row-major: shape(2, 4), stride(4, 1)
- Column-major: shape(2, 4), stride(1, 2)

The number in each cell of the figure indicates the subscript of the element at that position when elements are arranged sequentially in memory. For the same cube position, the order in memory may differ under different layout methods. For example, for cube coordinate (1, 0), the corresponding elements are at positions 4 and 1 in memory under row-major and column-major layouts, respectively.

**Figure 1** Row-major layout

![Row-major layout](../../../figures/row_major_layout.png)

**Figure 2** Column-major layout

![Column-major layout](../../../figures/column_major_layout.png)

## Hierarchical Representation

Usually, an element in shape or stride is a single integer. However, for some complex memory layout scenarios, a plain two-dimensional representation may fail to express them accurately. Therefore, the [hierarchical representation](https://mgarland.org/papers/2023/graphene/) is adopted here. In this approach, an element can also be a tuple. The first element of the tuple describes the number of cube rows in different row hierarchies, and the second element describes the number of cube columns in different hierarchies.

For example, in the following figure, the shape and stride of the layout are as follows:

- shape((2, 3), (2, 4))
- stride((1, 4), (2, 12))

**Figure 3** Hierarchical representation

![Hierarchical representation](../../../figures/hierarchical_representation.png "Hierarchical representation")

The figure shows two layers of cubes: the inner layer cube is the cube enclosed by gray lines, and the outer layer cube is the cube enclosed by black lines when the inner layer cube is treated as a single element.

The first element of shape describes the shape in the row direction, where (2, 3) indicates that the inner layer cube and the outer layer cube have 2 and 3 rows, respectively. The second element of shape describes the shape in the column direction, where (2, 4) indicates that the inner layer cube and the outer layer cube have 2 and 4 columns, respectively.

Each element in stride corresponds to an element in shape, indicating the interval between the first addresses of adjacent elements in memory in that corresponding dimension. The figure uses arrows to indicate the first-address interval of adjacent elements in each dimension.

## Common layout fractals

In cube programming scenarios based on Ascend C, the following common layouts are used. These formats are all expressed using the hierarchical representation described above, which requires two levels of cubes: an inner layer and an outer layer. The specific expression is as follows, where _x is the abbreviated form of `Std::Int<x>`. For example, _16 represents the type `Std::Int<16>`, with the value 16 stored in the type:

```cpp
layout = ((shape): (stride))
shape = ((shape_row0, shape_row1), (shape_column0, shape_column1))
stride = ((stride_row0, stride_row1), (stride_column0, stride_column1))
```

- NZ layout

    In the NZ layout format, shape_row0 and shape_column0 are fixed values. The shape of the inner fractal is 16 * (32Byte / sizeof(T)), and stride_row0 and stride_column0 are also fixed values. That is, the inner fractal is organized in a Z pattern, and the outer fractal is organized in an N pattern. Therefore, stride_column1 must satisfy the requirement of alignment by whole blocks.

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_16{}, shape_row1), (_32{} / sizeof(T), shape_column1))
    stride = ((_32{} / sizeof(T), stride_row1), (_1{}, stride_column1))
    // stride_column1 must satisfy:
    // stride_column1 % (shape_row0 * shape_column0) == 0
    ```

    **Figure 4** NZ layout
    ![NZ-format](../../../figures/nz_format.png)

    The following is an example of a contiguous NZ layout, where c0_element =_32{} / sizeof(T).

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_16{}, ceil_div(row, _16{})), (c0_element, ceil_div(column, c0_element)))
    stride = ((c0_element, c0_element * _16{}), (_1{}, c0_element * ceil_align(row, _16{})))
    ```

- ZN layout

    In the ZN layout format, shape_row0 and shape_column0 are fixed values. The shape of the inner fractal is (32Byte / sizeof(T)) * 16, and stride_row0 and stride_column0 are also fixed values. That is, the inner fractal is organized in an N pattern, and the outer fractal is organized in a Z pattern. Therefore, stride_row1 must satisfy the requirement of alignment by whole blocks.

    ```cpp
    layout = ((shape) : (stride))
    shape = ((c0_element, shape_row1), (_16{}, shape_column1))
    stride = ((_1{}, stride_row1), (c0_element, stride_column1))
    // stride_row1 must satisfy:
    // stride_row1 % (shape_row0 * shape_column0) == 0
    ```

    **Figure 5** ZN layout
    ![ZN format](../../../figures/zn_fractals_of_half.png)

    The following is a contiguous ZN layout example, where c0_element =_32{} / sizeof(T).

    ```cpp
    layout = ((shape) : (stride))
    shape = ((c0_element, ceil_div(row, c0_element)), (_16{}, ceil_div(column, _16{})))
    stride = ((_1{}, c0_element * ceil_align(column, _16{})), (c0_element, c0_element * _16{}))
    ```

- DNExt layout

    In the DNExt layout format, shape_row0 and shape_column0 are fixed values, meaning the inner fractal is fixed at 1 * 1. stride_row0 and stride_column0 are also fixed at 0, indicating that the inner layer has only one element and no additional stride concept. The outer layer is stored contiguously in column-major order, so stride_column1 equals the number of rows.

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, shape_row1), (_1{}, shape_column1))
    stride = ((_0{}, _1{}), (_0{}, stride_column1))
    ```

    **Figure 6** DNExt layout
    ![DNExt layout](../../../figures/column_major_layout.png)

    The following is a contiguous DNExt layout example.

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, row), (_1{}, column))
    stride = ((_0{}, _1{}), (_0{}, row))
    ```

- NDExt layout

    In the NDExt layout format, shape_row0 and shape_column0 are fixed values, meaning the inner fractal is fixed at 1 * 1. stride_row0 and stride_column0 are also fixed at 0, indicating that the inner layer has only one element and no additional stride concept. The outer layer is stored contiguously in row-major order, so stride_row1 equals the number of columns, and stride_column1 is fixed at 1.

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, shape_row1), (_1{}, shape_column1))
    stride = ((_0{}, stride_row1), (_0{}, _1{}))
    ```

    **Figure 7** NDExt layout
    ![NDExt layout](../../../figures/row_major_layout.png)

    The following is a contiguous NDExt layout example.

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, row), (_1{}, column))
    stride = ((_0{}, column), (_0{}, _1{}))
    ```

- DN layout

    DN layout is a two-dimensional column-major layout format. From the perspective of the hierarchical representation, it can be regarded as a special case with only an outer layer cube, where the row-direction stride is fixed at 1 and the column-direction stride equals the number of rows.

    ```cpp
    layout = ((shape) : (stride))
    shape = (shape_row, shape_column)
    stride = (_1{}, shape_row)
    ```

    The following is an example of a contiguous DN layout.

    ```cpp
    layout = ((shape) : (stride))
    shape = (row, column)
    stride = (_1{}, row)
    ```

- ND layout

    ND layout is a two-dimensional row-major layout format. From the perspective of the hierarchical representation, it can be regarded as a special case with only an outer layer cube, where the column-direction stride is fixed at 1 and the row-direction stride equals the number of columns.

    ```cpp
    layout = ((shape) : (stride))
    shape = (shape_row, shape_column)
    stride = (shape_column, _1{})
    ```

    The following is an example of a contiguous ND layout.

    ```cpp
    layout = ((shape) : (stride))
    shape = (row, column)
    stride = (column, _1{})
    ```

- ZZ layout

    In the ZZ layout format, shape_row0 and shape_column0 are fixed values. The shape of the inner fractal is consistent with that of the NZ layout, and stride_row0 and stride_column0 are also fixed values, meaning that the inner fractal is organized in a Z-shape. The outer fractal is likewise organized in a Z-shape, so the column direction is contiguous, and the row-direction span is determined by the number of fractal blocks in an entire row.

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_16{}, shape_row1), (c0_element, shape_column1))
    stride = ((c0_element, stride_row1), (_1{}, c0_element * _16{}))
    ```

    The following is an example of a contiguous ZZ layout, where c0_element =_32{} / sizeof(T).

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_16{}, ceil_div(row, _16{})), (c0_element, ceil_div(column, c0_element)))
    stride = ((c0_element, _16{} * ceil_align(column, c0_element)), (_1{}, c0_element * _16{}))
    ```

- NN layout

    In the NN layout format, shape_row0 and shape_column0 are fixed values and are used only in the fp8_e8m0_t scenario. The inner fractal is fixed at 2 * 16, and stride_row0 and stride_column0 are also fixed values, meaning that the inner fractal is organized in an N-shaped glyph. The outer fractal is likewise organized in an N-shaped glyph.

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_2{}, shape_row1), (_16{}, shape_column1))
    stride = ((_1{}, _32{}), (_2{}, stride_column1))
    ```

    The following is an example of a contiguous NN layout.

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_2{}, row / _2{}), (_16{}, ceil_div(column, _16{})))
    stride = ((_1{}, _32{}), (_2{}, row * _16{}))
    ```

- ScaleAND layout

    In the ScaleAND layout format, shape_row0 and shape_column0 are fixed values. It supports only the scaleA data of fp8_e8m0_t and requires c0_element == 2. Its physical layout is consistent with a contiguous NDExt layout, that is, the inner layer is fixed at 1 * 1 and the outer layer is stored contiguously in row-major order. Semantically, it describes the scenario where scaleA is not transposed.

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, shape_row1), (_1{}, shape_column1))
    stride = ((_0{}, stride_row1), (_0{}, _1{}))
    ```

    The following is an example of a contiguous ScaleAND layout.

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, row), (_1{}, column))
    stride = ((_0{}, column), (_0{}, _1{}))
    ```

- ScaleADN layout

    In the ScaleADN layout format, shape_row0 and shape_column0 are fixed values. It supports only the scaleA data of fp8_e8m0_t and requires c0_element == 2. Its column-direction inner fractal is fixed at 2, the row-direction inner layer is fixed at 1, and the outer layer is organized in column-major order. Semantically, it describes the scenario where scaleA is transposed.

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, shape_row1), (_2{}, shape_column1))
    stride = ((_0{}, _2{}), (_1{}, stride_column1))
    ```

    The following is an example of a contiguous ScaleADN layout.

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, row), (_2{}, column / _2{}))
    stride = ((_0{}, _2{}), (_1{}, _2{} * row))
    ```

- ScaleBND layout

    In the ScaleBND layout format, shape_row0 and shape_column0 are fixed values. It supports only the scaleB data of fp8_e8m0_t and requires c0_element == 2. Its row-direction inner fractal is fixed at 2, the column-direction inner layer is fixed at 1, and the outer layer is organized in row-major order. Semantically, it describes the scenario where scaleB is transposed.

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_2{}, shape_row1), (_1{}, shape_column1))
    stride = ((_1{}, stride_row1), (_0{}, _2{}))
    ```

    The following is an example of a contiguous ScaleBND layout.

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_2{}, row / _2{}), (_1{}, column))
    stride = ((_1{}, _2{} * column), (_0{}, _2{}))
    ```

- ScaleBDN layout

    In the ScaleBDN layout format, shape_row0 and shape_column0 are fixed values. It supports only fp8_e8m0_t scaleB data and requires c0_element == 2. Its physical layout is consistent with the contiguous DNExt layout, that is, the inner layer is fixed at 1 * 1 and the outer layer is stored contiguously in column-major order. Semantically, it describes the scenario where scaleB is not transposed.

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, shape_row1), (_1{}, shape_column1))
    stride = ((_0{}, _1{}), (_0{}, stride_column1))
    ```

    The following is an example of a contiguous ScaleBDN layout.

    ```cpp
    layout = ((shape) : (stride))
    shape = ((_1{}, row), (_1{}, column))
    stride = ((_0{}, _1{}), (_0{}, row))
    ```

MX scaleA requires the cube on GM to be arranged in the ScaleAND layout or ScaleADN layout format, and on L1 it must satisfy the row-wise read requirement, that is, arranged in the ZZ layout format. As shown in the following figure, if the scaleA cube on GM is arranged in the ScaleAND or ScaleADN fractal layout, it is arranged in the ZZ layout after being moved to L1.

**Figure 8** Layout formats of the scaleA cube at different locations
![Layout formats of the scaleA cube at different locations](../../../figures/scalea_matrix_layout.png)

MX scaleB requires the cube on GM to be arranged in the ScaleBND layout or ScaleBDN layout format, and on L1 it must satisfy the column-wise read requirement, that is, arranged in the NN layout format. As shown in the following figure, if the scaleB cube on GM is arranged in the ScaleBND or ScaleBDN fractal layout, it is fractalized into the NN layout after being moved to L1.

**Figure 9** Layout formats of the scaleB cube at different locations
![Layout formats of the scaleB cube at different locations](../../../figures/scaleb_matrix_layout.png)

## Convolutional Feature Map Layout

The convolutional feature map layout is represented by a flat multi-dimensional shape and a row-major stride, without using the hierarchical shape of cube fractals. The supported LayoutPatterns are as follows.

**Table 1**  Description of convolutional feature map LayoutPatterns

| LayoutPattern | shape | stride |
| :--- | :--- | :--- |
| `nchw_layout_ptn` | `(N, C, H, W)` | `(C * H * W, H * W, W, 1)` |
| `nhwc_layout_ptn` | `(N, H, W, C)` | `(H * W * C, W * C, C, 1)` |
| `nc1hwc0_layout_ptn` | `(N, C1, H, W, C0)` | `(C1 * H * W * C0, H * W * C0, W * C0, C0, 1)` |
| `ncdhw_layout_ptn` | `(N, C, D, H, W)` | `(C * D * H * W, D * H * W, H * W, W, 1)` |
| `ndc1hwc0_layout_ptn` | `(N, D, C1, H, W, C0)` | `(D * C1 * H * W * C0, C1 * H * W * C0, H * W * C0, W * C0, C0, 1)` |

`nc1hwc0_layout_ptn` and `ndc1hwc0_layout_ptn` split the channel dimension C into C1 and C0, which must satisfy `C = C1 * C0`. The layouts above can be constructed through [make_frame_layout](make_frame_layout.md).

## layout Fractal Structure Data Table

The layout format corresponding to each fractal is shown in the following table, where T refers to the data types supported except fp8_e8m0_t, and c0_element =_32{} / sizeof(T); in special cases, when T is fp4x2_e2m1_t or fp4x2_e1m2_t, c0_element =_64{}.

**Table 2**  layout fractal structure data description

| layout_format_pattern | Type | shape_row0 | shape_row1 | shape_column0 | shape_column1 | stride_row0 | stride_row1 | stride_column0 | stride_column1 |
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
