# Layout Overview<a name="ZH-CN_TOPIC_0000002331693889"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T11:47:48.810Z -->

The **Layout<Shape, Stride\>** data structure is a fundamental template class for describing the memory layout of multidimensional tensors. Through compile-time shape (**Shape**) and stride (**Stride**) information, it maps the logical coordinate space to the one-dimensional memory address space, providing foundational support for complex tensor operations and hardware optimization. Leveraging template metaprogramming, this class performs computation and code generation at compile time, thereby reducing runtime overhead.

**Layout** consists of two core components:

-   **Shape**: Defines the logical shape of the data, such as the number of rows and columns of a two-dimensional cube or the size of each dimension of a multidimensional tensor.
-   **Stride**: Defines the stride of each dimension in memory, that is, the interval between adjacent elements of the same dimension in memory. The interval is measured in elements and corresponds one-to-one with the dimension information of **Shape**.

For example, a two-dimensional cube with **Shape** \(4, 2\) and **Stride** \(4, 1\) means:

-   The cube has 4 rows and 2 columns.
-   The stride in the column direction is 1, meaning that the interval between adjacent elements in each row is 1 element in memory; the stride in the row direction is 4, meaning that the interval between the starting addresses of adjacent rows is 4 elements.

[Table 1](#table12525201245615) shows the one-dimensional memory address space view, and [Table 2](#table17519406546) shows the logical view of this two-dimensional cube.

**Table 1** **Linear Address View**

<a name="table12525201245615"></a>
<table><thead align="left"><tr id="row20525012185616"><th class="cellrowborder" valign="top" width="8.080808080808083%" id="mcps1.2.13.1.1"><p id="p101677148592"><a name="p101677148592"></a><a name="p101677148592"></a>Address</p></th>
<th class="cellrowborder" valign="top" width="8.500850085008503%" id="mcps1.2.13.1.2"><p id="p2983172525618"><a name="p2983172525618"></a><a name="p2983172525618"></a>0</p></th>
<th class="cellrowborder" valign="top" width="8.500850085008503%" id="mcps1.2.13.1.3"><p id="p9983142535617"><a name="p9983142535617"></a><a name="p9983142535617"></a>1</p></th>
<th class="cellrowborder" valign="top" width="8.500850085008503%" id="mcps1.2.13.1.4"><p id="p29830252564"><a name="p29830252564"></a><a name="p29830252564"></a>2, 3</p></th>
<th class="cellrowborder" valign="top" width="8.500850085008503%" id="mcps1.2.13.1.5"><p id="p12983162525611"><a name="p12983162525611"></a><a name="p12983162525611"></a>4</p></th>
<th class="cellrowborder" valign="top" width="8.500850085008503%" id="mcps1.2.13.1.6"><p id="p199831425105617"><a name="p199831425105617"></a><a name="p199831425105617"></a>5</p></th>
<th class="cellrowborder" valign="top" width="8.500850085008503%" id="mcps1.2.13.1.7"><p id="p998312555616"><a name="p998312555616"></a><a name="p998312555616"></a>6, 7</p></th>
<th class="cellrowborder" valign="top" width="8.500850085008503%" id="mcps1.2.13.1.8"><p id="p159832025135614"><a name="p159832025135614"></a><a name="p159832025135614"></a>8</p></th>
<th class="cellrowborder" valign="top" width="8.500850085008503%" id="mcps1.2.13.1.9"><p id="p6983102585615"><a name="p6983102585615"></a><a name="p6983102585615"></a>9</p></th>
<th class="cellrowborder" valign="top" width="8.500850085008503%" id="mcps1.2.13.1.10"><p id="p29832025165615"><a name="p29832025165615"></a><a name="p29832025165615"></a>10, 11</p></th>
<th class="cellrowborder" valign="top" width="8.500850085008503%" id="mcps1.2.13.1.11"><p id="p169831525195612"><a name="p169831525195612"></a><a name="p169831525195612"></a>12</p></th>
<th class="cellrowborder" valign="top" width="6.910691069106911%" id="mcps1.2.13.1.12"><p id="p134212147586"><a name="p134212147586"></a><a name="p134212147586"></a>13</p></th>
</tr>
</thead>
<tbody><tr id="row12525141216566"><td class="cellrowborder" valign="top" width="8.080808080808083%" headers="mcps1.2.13.1.1 "><p id="p01671214135912"><a name="p01671214135912"></a><a name="p01671214135912"></a>Element</p></td>
<td class="cellrowborder" valign="top" width="8.500850085008503%" headers="mcps1.2.13.1.2 "><p id="p498442515619"><a name="p498442515619"></a><a name="p498442515619"></a>a00</p></td>
<td class="cellrowborder" valign="top" width="8.500850085008503%" headers="mcps1.2.13.1.3 "><p id="p69841025115617"><a name="p69841025115617"></a><a name="p69841025115617"></a>a01</p></td>
<td class="cellrowborder" valign="top" width="8.500850085008503%" headers="mcps1.2.13.1.4 "><p id="p1498472517567"><a name="p1498472517567"></a><a name="p1498472517567"></a>-</p></td>
<td class="cellrowborder" valign="top" width="8.500850085008503%" headers="mcps1.2.13.1.5 "><p id="p1098452595612"><a name="p1098452595612"></a><a name="p1098452595612"></a>a10</p></td>
<td class="cellrowborder" valign="top" width="8.500850085008503%" headers="mcps1.2.13.1.6 "><p id="p1098412510562"><a name="p1098412510562"></a><a name="p1098412510562"></a>a11</p></td>
<td class="cellrowborder" valign="top" width="8.500850085008503%" headers="mcps1.2.13.1.7 "><p id="p1984132565617"><a name="p1984132565617"></a><a name="p1984132565617"></a>-</p></td>
<td class="cellrowborder" valign="top" width="8.500850085008503%" headers="mcps1.2.13.1.8 "><p id="p49841025105613"><a name="p49841025105613"></a><a name="p49841025105613"></a>a20</p></td>
<td class="cellrowborder" valign="top" width="8.500850085008503%" headers="mcps1.2.13.1.9 "><p id="p498442512566"><a name="p498442512566"></a><a name="p498442512566"></a>a21</p></td>
<td class="cellrowborder" valign="top" width="8.500850085008503%" headers="mcps1.2.13.1.10 "><p id="p1198415250563"><a name="p1198415250563"></a><a name="p1198415250563"></a>-</p></td>
<td class="cellrowborder" valign="top" width="8.500850085008503%" headers="mcps1.2.13.1.11 "><p id="p3984725145619"><a name="p3984725145619"></a><a name="p3984725145619"></a>a30</p></td>
<td class="cellrowborder" valign="top" width="6.910691069106911%" headers="mcps1.2.13.1.12 "><p id="p1642914105818"><a name="p1642914105818"></a><a name="p1642914105818"></a>a31</p></td>
</tr>
</tbody>
</table>

**Table 2** **Cube Logical View**

<a name="table17519406546"></a>
<table><thead align="left"><tr id="row375940195412"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.1"><p id="p1337044595410"><a name="p1337044595410"></a><a name="p1337044595410"></a>Index</p></th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.2"><p id="p4370194535410"><a name="p4370194535410"></a><a name="p4370194535410"></a>Column 0</p></th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="p16370154513542"><a name="p16370154513542"></a><a name="p16370154513542"></a>Column 1</p></th>
</tr>
</thead>
<tbody><tr id="row3761740175413"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="p537084575410"><a name="p537084575410"></a><a name="p537084575410"></a>Row 0</p></td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="p1370134535418"><a name="p1370134535418"></a><a name="p1370134535418"></a>a00 (address 0)</p></td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p237024517543"><a name="p237024517543"></a><a name="p237024517543"></a>a01 (address 1)</p></td>
</tr>
<tr id="row676104005417"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="p2370645145412"><a name="p2370645145412"></a><a name="p2370645145412"></a>Row 1</p></td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="p53701345165410"><a name="p53701345165410"></a><a name="p53701345165410"></a>a10 (address 4)</p></td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p1937034518545"><a name="p1937034518545"></a><a name="p1937034518545"></a>a11 (address 5)</p></td>
</tr>
<tr id="row207654015545"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="p23701045135412"><a name="p23701045135412"></a><a name="p23701045135412"></a>Row 2</p></td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="p137019452548"><a name="p137019452548"></a><a name="p137019452548"></a>a20 (address 8)</p></td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p4370845165416"><a name="p4370845165416"></a><a name="p4370845165416"></a>a21 (address 9)</p></td>
</tr>
<tr id="row187694055418"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="p183701345125411"><a name="p183701345125411"></a><a name="p183701345125411"></a>Row 3</p></td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="p0370945105417"><a name="p0370945105417"></a><a name="p0370945105417"></a>a30 (address 12).</p></td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p9370145125416"><a name="p9370145125416"></a><a name="p9370145125416"></a>a31 (address 13).</p></td>
</tr>
</tbody>
</table>

## Header Files to Include<a name="zh-cn_topic_0000002213064918_section78885814919"></a>

```cpp
#include "kernel_operator_layout.h"
```

## Prototype<a name="section10580930144614"></a>

```cpp
template <typename ShapeType, typename StrideType>
struct Layout : private Std::tuple<ShapeType, StrideType> {
    __aicore__ inline constexpr Layout(const ShapeType& shape  = {}, const StrideType& stride = {}) : Std::tuple<ShapeType, StrideType>(shape, stride) {}

    __aicore__ inline constexpr decltype(auto) layout() {}
    __aicore__ inline constexpr decltype(auto) layout() const {}
    
    __aicore__ inline constexpr decltype(auto) GetShape() {}   
    __aicore__ inline constexpr decltype(auto) GetShape() const {}
    
    __aicore__ inline constexpr decltype(auto) GetStride() {}    
    __aicore__ inline constexpr decltype(auto) GetStride() const {}
    
    template <typename CoordType>
    __aicore__ inline constexpr auto operator()(const CoordType& coord) const {}
}
```

## Template Parameters<a name="section116801320102618"></a>

**Table 3** Template parameter description

<a name="table13588175515344"></a>
<table><thead align="left"><tr id="row1160915519346"><th class="cellrowborder" valign="top" width="21.8%" id="mcps1.2.3.1.1"><p id="p9609105553412"><a name="p9609105553412"></a><a name="p9609105553412"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="78.2%" id="mcps1.2.3.1.2"><p id="p156091955143419"><a name="p156091955143419"></a><a name="p156091955143419"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row260915573419"><td class="cellrowborder" valign="top" width="21.8%" headers="mcps1.2.3.1.1 "><p id="p2060925573411"><a name="p2060925573411"></a><a name="p2060925573411"></a>**ShapeType**</p></td>
<td class="cellrowborder" valign="top" width="78.2%" headers="mcps1.2.3.1.2 "><p id="p823866165711"><a name="p823866165711"></a><a name="p823866165711"></a><span id="ph184621011705"><a name="ph184621011705"></a><a name="ph184621011705"></a><a href="../../../../Utils-API/cpp_stdlib/container_functions/container_functions.md">Std::tuple</a> structure type used to define the logical shape of the data, such as the number of rows and columns of a two-dimensional cube or the size of each dimension of a multi-dimensional tensor.</span></p></td>
</tr>
<tr id="row1545073919457"><td class="cellrowborder" valign="top" width="21.8%" headers="mcps1.2.3.1.1 "><p id="p1745103924512"><a name="p1745103924512"></a><a name="p1745103924512"></a>**StrideType**</p></td>
<td class="cellrowborder" valign="top" width="78.2%" headers="mcps1.2.3.1.2 "><p id="p64517398452"><a name="p64517398452"></a><a name="p64517398452"></a><span id="ph292255305"><a name="ph292255305"></a><a name="ph292255305"></a><a href="../../../../Utils-API/cpp_stdlib/container_functions/container_functions.md">Std::tuple</a> structure type used to define the stride of each dimension in memory, that is, the interval between adjacent elements of the same dimension in memory, measured in elements, corresponding one-to-one to the dimension information of Shape.</span></p></td>
</tr>
</tbody>
</table>
