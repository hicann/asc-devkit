# Coordinate Introduction<a name="ZH-CN_TOPIC_0000002400346593"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T11:41:46.181Z -->

**Coordinate** is essentially a tuple that represents the position information of a tensor in different dimensions, that is, the coordinate value. **Coordinate** and [Layout](../Layout/Layout.md), as well as **Index** (memory position index), are closely related:

-   Conversion from **Coordinate** to **Index**: **Layout** defines the shape of the tensor and the stride of each dimension. Based on this information and a given **Coordinate**, the position index of the coordinate in memory can be calculated.
-   Conversion from **Index** to **Coordinate**: Based on the shape and stride information defined in **Layout**, for a known memory position index, the **Coordinate** corresponding to the index can be obtained through corresponding calculation.

## Prototype<a name="section10580930144614"></a>

```cpp
template <typename... Coords>
using Coord = Std::tuple<Coords...>
```

## Template Parameters<a name="section116801320102618"></a>

**Table 1** Template parameter description

<a name="table13588175515344"></a>
<table><thead align="left"><tr id="row1160915519346"><th class="cellrowborder" valign="top" width="21.8%" id="mcps1.2.3.1.1"><p id="p9609105553412"><a name="p9609105553412"></a><a name="p9609105553412"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="78.2%" id="mcps1.2.3.1.2"><p id="p156091955143419"><a name="p156091955143419"></a><a name="p156091955143419"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row260915573419"><td class="cellrowborder" valign="top" width="21.8%" headers="mcps1.2.3.1.1 "><p id="p2060925573411"><a name="p2060925573411"></a><a name="p2060925573411"></a>Coords</p></td>
<td class="cellrowborder" valign="top" width="78.2%" headers="mcps1.2.3.1.2 "><p id="p18109358203112"><a name="p18109358203112"></a><a name="p18109358203112"></a>Parameter pack representing the input data types, with the number of parameters ranging from [0, 64].</p>
<p id="p1329915004219"><a name="p1329915004219"></a><a name="p1329915004219"></a>The supported input data types include size_t and Std::<a href="../../../../Utils-API/cpp_stdlib/type_traits/integral_constant.md">Int</a>.</p></td>
</tr>
</tbody>
</table>

## Related APIs<a name="section104554349817"></a>

```cpp
// Constructor of the Coord structure.
template <typename... Ts>
__aicore__ inline constexpr Coord<Ts...> MakeCoord(Ts const&... t)

// Layout input. Converts a Coordinate to a memory location index Index.
template <typename CoordType, typename ShapeType, typename StrideType>
__aicore__ inline constexpr auto Crd2Idx(const CoordType& coord, const Layout<ShapeType, StrideType>& layout)

// Shape and Stride input. Converts a Coordinate to a memory location index Index.
template <typename CoordType, typename ShapeType, typename StrideType>
__aicore__ inline constexpr auto Crd2Idx(const CoordType& coord, const ShapeType& shape, const StrideType& stride)
```
