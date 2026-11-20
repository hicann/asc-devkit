# Layout

<!-- md-trans-meta sourceCommit=46a50d69bd092786e44495d519df57743eefbfc6 translatedAt=2026-10-27T02:28:41.749Z pushedAt=2026-10-27T13:03:35.303Z nl=2 -->

Layout uses Shape and Stride to describe the logical shape of data and the linear address stride corresponding to each dimension coordinate. The Tensor API supports Shape, Stride, and Coord in the form of plain integers and nested tuples, and expresses data layouts such as ND, DN, NZ, and ZN through Layout Pattern.

## layout Data Structure

- **[layout and Hierarchical Representation](layout_hierarchical_representation.md)**
- **[layout](layout.md)**
- **[shape](shape.md)**
- **[stride](stride.md)**
- **[coord](coord.md)**

## layout-related APIs

- **[make_layout](make_layout.md)**
- **[make_frame_layout](make_frame_layout.md)**
- **[make_shape](make_shape.md)**
- **[make_stride](make_stride.md)**
- **[make_coord](make_coord.md)**
- **[crd2idx](crd2idx.md)**
- **[get_shape](get_shape.md)**
- **[get_stride](get_stride.md)**
- **[get](get.md)**
- **[select](select.md)**
- **[squeeze](squeeze.md)**
- **[capacity](capacity.md)**
- **[size](size.md)**
- **[coshape](coshape.md)**
- **[cosize](cosize.md)**
- **[rank](rank.md)**

The APIs above constitute the currently public Layout algebra capabilities, which can be used to construct a Layout, extract substructures, compute capacity and coordinate indexes, and remove dimensions of size 1. Internal Layout derivation functions are not public APIs.
