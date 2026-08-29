# Layout

Layout使用Shape和Stride描述数据的逻辑形状及各维坐标对应的线性地址步长。Tensor API支持普通整数和嵌套元组形式的Shape、Stride和Coord，并通过Layout Pattern表达ND、DN、NZ、ZN等数据排布。

## layout数据结构

- **[layout和层次化表述法](layout_hierarchical_representation.md)**
- **[layout](layout.md)**
- **[shape](shape.md)**
- **[stride](stride.md)**
- **[coord](coord.md)**

## layout相关接口

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

上述接口构成当前公开的Layout代数能力，可用于构造Layout、提取子结构、计算容量和坐标索引以及删除大小为1的维度。内部Layout推导函数不属于对外接口。
