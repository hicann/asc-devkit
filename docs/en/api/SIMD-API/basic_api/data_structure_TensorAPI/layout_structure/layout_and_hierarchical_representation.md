# Layout and Hierarchical Representation

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-27T17:51:48.500Z -->

## Layout

We use **Shape** and **Stride** to represent the layout format, such as the row-major layout and column-major layout shown in the following figures.

**Shape** is used to express the tensor shape, while **Stride** is used to distinguish different layout methods.

- Row-major: Shape(2, 4), Stride(4, 1)
- Column-major: Shape(2, 4), Stride(1, 2)

The number in each cell in the figure indicates the subscript of the element at that position when elements are arranged sequentially in memory. For the same cube position, the order in memory may differ under different layout methods. For example, for cube coordinate (1, 0), the corresponding elements are at positions 4 and 1 in memory under the row-major and column-major layouts, respectively.

**Figure 1** Row-major layout

![Row-major layout](../../../../figures/row_major_layout.png)

**Figure 2** Column-major layout

![Column-major layout](../../../../figures/column_major_layout.png)

## Hierarchical Representation

Normally, an element in Shape or Stride is a single integer. However, for some complex memory layout scenarios, the ordinary two-dimensional representation may fail to express them accurately. Therefore, a [hierarchical representation](https://dl.acm.org/doi/abs/10.1145/3582016.3582018) is adopted here. In this approach, an element can also be a tuple. The first element of the tuple describes the number of cube rows in different row hierarchies, and the second element describes the number of cube columns in different hierarchies.

For example, in the following figure, the Shape and Stride of the Layout are as follows:

- Shape((2, 3), (2, 4))
- Stride((1, 4), (2, 12))

**Figure 3** Hierarchical representation

![Hierarchical representation](../../../../figures/hierarchical_representation.png "Hierarchical representation")

The figure shows two layers of matrices: the inner cube is the cube enclosed by gray lines, and the outer cube is the cube enclosed by black lines when the inner cube is treated as a single element.

The first element of Shape describes the shape in the row direction. (2, 3) indicates that the numbers of rows of the inner cube and the outer cube are 2 and 3, respectively. The second element of Shape describes the shape in the column direction. (2, 4) indicates that the numbers of columns of the inner cube and the outer cube are 2 and 4, respectively.

Each element in Stride corresponds to an element in Shape, indicating the interval in memory addresses between the first addresses of adjacent elements in the corresponding dimension. In the figure, arrows indicate the first-address interval between adjacent elements in each dimension.

## Common Layout Fractals

In cube programming scenarios based on Ascend C, the following common layouts are used. These formats are all expressed using the hierarchical representation introduced above, which requires two layers of matrices: an inner layer and an outer layer. The specific expression is as follows, where _x is the abbreviated form of `Std::Int<x>`. For example, _16 represents the type Std::Int<16>, with the value 16 stored in the type:

```cpp
Layout = ((Shape): (Stride))
Shape = ((ShapeRow0, ShapeRow1), (ShapeColumn0, ShapeColumn1))
Stride = ((StrideRow0, StrideRow1), (StrideColumn0, StrideColumn1))
```

- NZ Layout

    In the NZ layout format, ShapeRow0 and ShapeColumn0 are fixed values. The shape of the inner fractal is 16 * (32Byte / sizeof(T)), and StrideRow0 and StrideColumn0 are also fixed values. That is, the inner fractal is organized in a Z-shaped pattern, and the outer fractal is organized in an N-shaped pattern. Therefore, StrideColumn1 must satisfy the requirement of being aligned by whole blocks.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_16{}, ShapeRow1), (_32{} / sizeof(T), ShapeColumn1))
    Stride = ((_32{} / sizeof(T), StrideRow1), (_1{}, StrideColumn1))
    // StrideColumn1 must satisfy:
    // StrideColumn1 % (ShapeRow0 * ShapeColumn0) == 0
    ```

    **Figure 4** NZ Layout  
    ![NZ-format](../../../../figures/nz_format.png)

    Below is a continuous NZ layout example, where C0_ELEMENT =_32{} / sizeof(T).

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_16{}, ceil_div(row, _16{})), (C0_ELEMENT, ceil_div(column, C0_ELEMENT)))
    Stride = ((C0_ELEMENT, C0_ELEMENT * _16{}), (_1{}, C0_ELEMENT * ceil_align(row, _16{})))
    ```

- ZN Layout

    In the ZN layout format, ShapeRow0 and ShapeColumn0 are fixed values. The shape of the inner fractal is (32Byte / sizeof(T)) * 16, and StrideRow0 and StrideColumn0 are also fixed values. That is, the inner fractal is organized in an N-shaped pattern, and the outer fractal is organized in a Z-shaped pattern. Therefore, StrideRow1 must satisfy the requirement of being aligned by whole blocks.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((C0_ELEMENT, ShapeRow1), (_16{}, ShapeColumn1))
    Stride = ((_1{}, StrideRow1), (C0_ELEMENT, StrideColumn1))
    // StrideRow1 must satisfy:
    // StrideRow1 % (ShapeRow0 * ShapeColumn0) == 0
    ```

    **Figure 5** ZN Layout
    ![ZN format](../../../../figures/Zn_format_half.png)

    Below is a continuous ZN Layout example, where C0_ELEMENT =_32{} / sizeof(T).

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((C0_ELEMENT, ceil_div(row, C0_ELEMENT)), (_16{}, ceil_div(column, _16{})))
    Stride = ((_1{}, C0_ELEMENT * ceil_align(column, _16{})), (C0_ELEMENT, C0_ELEMENT * _16{}))
    ```

- DNExt Layout

    In the DNExt Layout format, ShapeRow0 and ShapeColumn0 are fixed values, i.e., the inner fractal is fixed as 1 * 1. StrideRow0 and StrideColumn0 are also fixed as 0, indicating that the inner layer has only one element and has no additional stride concept. The outer layer is stored contiguously by column priority method, therefore StrideColumn1 equals the number of rows.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, ShapeRow1), (_1{}, ShapeColumn1))
    Stride = ((_0{}, _1{}), (_0{}, StrideColumn1))
    ```

    **Figure 6** DNExt Layout
    ![DNExt Layout](../../../../figures/column-major layout.png)

    Below is a continuous DNExt Layout example.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, row), (_1{}, column))
    Stride = ((_0{}, _1{}), (_0{}, row))
    ```

- NDExt Layout

    In the NDExt Layout format, ShapeRow0 and ShapeColumn0 are fixed values, i.e., the inner fractal is fixed as 1 * 1. StrideRow0 and StrideColumn0 are also fixed as 0, indicating that the inner layer has only one element and has no additional stride concept. The outer layer is stored contiguously by row priority method, therefore StrideRow1 equals the number of columns, and StrideColumn1 is fixed as 1.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, ShapeRow1), (_1{}, ShapeColumn1))
    Stride = ((_0{}, StrideRow1), (_0{}, _1{}))
    ```

    **Figure 7** NDExt Layout
    ![NDExt Layout](../../../../figures/row-major layout.png)

    Below is a continuous NDExt Layout example.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, row), (_1{}, column))
    Stride = ((_0{}, column), (_0{}, _1{}))
    ```

- DN Layout

    DN Layout is a two-dimensional column-major layout format. From the perspective of the hierarchical representation, it can be regarded as a special case with only the outer cube, where the row stride is fixed as 1 and the column stride equals the number of rows.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = (ShapeRow, ShapeColumn)
    Stride = (_1{}, ShapeRow)
    ```

    Below is a continuous DN Layout example.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = (row, column)
    Stride = (_1{}, row)
    ```

- ND Layout

    ND Layout is a two-dimensional row-major layout format. From the perspective of the hierarchical representation, it can be regarded as a special case with only the outer cube, where the column stride is fixed as 1 and the row stride equals the number of columns.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = (ShapeRow, ShapeColumn)
    Stride = (ShapeColumn, _1{})
    ```

    Below is a continuous ND Layout example.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = (row, column)
    Stride = (column, _1{})
    ```

- ZZ Layout

    In the ZZ Layout format, ShapeRow0 and ShapeColumn0 are fixed values. The shape of the inner fractal is consistent with that of NZ Layout, and StrideRow0 and StrideColumn0 are also fixed values, i.e., the inner fractal is organized in a Z-glyph pattern, and the outer fractal is likewise organized in a Z-glyph pattern. Therefore, the column direction is contiguous, and the row span is determined by the number of fractal blocks in an entire row.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_16{}, ShapeRow1), (C0_ELEMENT, ShapeColumn1))
    Stride = ((C0_ELEMENT, StrideRow1), (_1{}, C0_ELEMENT * _16{}))
    ```

    Below is a continuous ZZ Layout example, where C0_ELEMENT =_32{} / sizeof(T).

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_16{}, ceil_div(row, _16{})), (C0_ELEMENT, ceil_div(column, C0_ELEMENT)))
    Stride = ((C0_ELEMENT, _16{} * ceil_align(column, C0_ELEMENT)), (_1{}, C0_ELEMENT * _16{}))
    ```

- NN Layout

    In the NN layout format, ShapeRow0 and ShapeColumn0 are fixed values and are used only in the fp8_e8m0_t scenario. The inner fractal is fixed as 2 * 16, and StrideRow0 and StrideColumn0 are also fixed values, i.e., the inner fractal is organized in an N-shaped pattern. The outer fractal is likewise organized in an N-shaped pattern.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_2{}, ShapeRow1), (_16{}, ShapeColumn1))
    Stride = ((_1{}, _32{}), (_2{}, StrideColumn1))
    ```

    Below is a contiguous NN layout example.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_2{}, row / _2{}), (_16{}, ceil_div(column, _16{})))
    Stride = ((_1{}, _32{}), (_2{}, row * _16{}))
    ```

- ScaleAND Layout

    In the ScaleAND layout format, ShapeRow0 and ShapeColumn0 are fixed values. It only supports the scaleA data of fp8_e8m0_t and requires C0_ELEMENT == 2. Its physical layout is consistent with a contiguous NDExt layout, i.e., the inner layer is fixed as 1 * 1, and the outer layer is stored contiguously in a row-major manner. Semantically, it is used for describing the scenario where scaleA is not transposed.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, ShapeRow1), (_1{}, ShapeColumn1))
    Stride = ((_0{}, StrideRow1), (_0{}, _1{}))
    ```

    Below is a contiguous ScaleAND layout example.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, row), (_1{}, column))
    Stride = ((_0{}, column), (_0{}, _1{}))
    ```

- ScaleADN Layout

    In the ScaleADN layout format, ShapeRow0 and ShapeColumn0 are fixed values. It only supports the scaleA data of fp8_e8m0_t and requires C0_ELEMENT == 2. Its inner fractal in the column direction is fixed as 2, and the inner layer in the row direction is fixed as 1, with the outer layer organized in a column-major manner. Semantically, it is used for describing the scenario where scaleA is transposed.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, ShapeRow1), (_2{}, ShapeColumn1))
    Stride = ((_0{}, _2{}), (_1{}, StrideColumn1))
    ```

    Below is a contiguous ScaleADN layout example.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, row), (_2{}, column / _2{}))
    Stride = ((_0{}, _2{}), (_1{}, _2{} * row))
    ```

- ScaleBND Layout

    In the ScaleBND layout format, ShapeRow0 and ShapeColumn0 are fixed values. It only supports the scaleB data of fp8_e8m0_t and requires C0_ELEMENT == 2. Its inner fractal in the row direction is fixed as 2, and the inner layer in the column direction is fixed as 1, with the outer layer organized in a row-major manner. Semantically, it is used for describing the scenario where scaleB is transposed.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_2{}, ShapeRow1), (_1{}, ShapeColumn1))
    Stride = ((_1{}, StrideRow1), (_0{}, _2{}))
    ```

    Below is a continuous ScaleBND Layout example.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_2{}, row / _2{}), (_1{}, column))
    Stride = ((_1{}, _2{} * column), (_0{}, _2{}))
    ```

- ScaleBDN Layout

    In the ScaleBDN Layout format, ShapeRow0 and ShapeColumn0 are fixed values. It only supports the scaleB data of fp8_e8m0_t and requires C0_ELEMENT == 2. Its physical layout is consistent with the continuous DNExt Layout, i.e., the inner layer is fixed as 1 * 1, and the outer layer is stored contiguously by column priority method. Semantically, it is used for describing the scenario where scaleB is not transposed.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, ShapeRow1), (_1{}, ShapeColumn1))
    Stride = ((_0{}, _1{}), (_0{}, StrideColumn1))
    ```

    Below is a continuous ScaleBDN Layout example.

    ```cpp
    Layout = ((Shape) : (Stride))
    Shape = ((_1{}, row), (_1{}, column))
    Stride = ((_0{}, _1{}), (_0{}, row))
    ```

MX scaleA requires that the cube on GM be arranged in the ScaleAND Layout or ScaleADN Layout format, and on L1 it must satisfy the row-wise read requirement, i.e., arranged in the ZZ Layout format. As shown in the following figure, if the scaleA cube on GM is arranged in the ScaleAND or ScaleADN fractal layout, it becomes the ZZ layout after being moved to L1.

**Figure 8** Layout format of the scaleA cube at different positions
![Layout format of the scaleA cube at different positions](../../../../figures/scale_a_cube_layout.png)

MX scaleB requires that the cube on GM be arranged in the ScaleBND Layout or ScaleBDN Layout format, and on L1 it must satisfy the column-wise read requirement, i.e., arranged in the NN Layout format. As shown in the following figure, if the scaleB cube on GM is arranged in the ScaleBND or ScaleBDN fractal layout, it becomes the NN layout after being moved to L1.

**Figure 9** Layout format of the scaleB cube at different positions
![Layout format of the scaleB cube at different positions](../../../../figures/scale_b_cube_layout.png)

## Layout Fractal Structure Data Table

The Layout format corresponding to each fractal is shown in the following table, where T refers to the supported data types except fp8_e8m0_t, and C0_ELEMENT =_32{} / sizeof(T); in special cases, when T is fp4x2_e2m1_t or fp4x2_e1m2_t, C0_ELEMENT =_64{}.

| LayoutFormatPattern | Type | ShapeRow0 | ShapeRow1 | ShapeColumn0 | ShapeColumn1 | StrideRow0 | StrideRow1 | StrideColumn0 | StrideColumn1 |
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
