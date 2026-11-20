# batch copy

<!-- md-trans-meta sourceCommit=b38421a18edbe36d6f465d239cd723c961016ef1 translatedAt=2026-10-27T03:01:07.386Z pushedAt=2026-10-27T13:03:35.341Z nl=2 -->

Batch movement is the capability of adding a Batch dimension to the outermost level of a cube when moving from L0C Buffer to Global Memory or Unified Buffer (UB), and is used to move the computation results of multiple cubes at once.

## Description

The source tensor is in the `NZ` format, and the destination tensor supports the `ND`, `DN`, or `NZ` format. Users only need to pass the Batch parameter when constructing the layout, and the `copy` invocation form remains consistent with the non-batch mode.

The source tensor can be constructed through `make_frame_layout<nz_layout_ptn>(batch, m, n)`. The destination tensor can be constructed through `make_frame_layout<nd_layout_ptn>(batch, m, n)`, `make_frame_layout<dn_layout_ptn>(batch, m, n)`, `make_frame_layout<nd_ext_layout_ptn>(batch, m, n)`, `make_frame_layout<dn_ext_layout_ptn>(batch, m, n)`, or `make_frame_layout<nz_layout_ptn, DstType>(batch, m, n)`.

## Copy Behavior

When `quant` is not passed in, or when a scalar quantization parameter of the `uint64_t` type is passed in, Batch dimension matching of the quantized tensor is not involved. The API completes the movement according to the number of Batches in the source tensor. In the scalar quantization scenario, all Batches share the same quantization parameter:


```cpp
auto l0c = make_tensor(make_mem_ptr(l0c_addr), make_frame_layout<nz_layout_ptn>(batch, m, n));
auto dst = make_tensor(make_mem_ptr(dst_addr), make_frame_layout<nd_ext_layout_ptn>(batch, m, n));

auto atom = make_copy(copy_l0c_to_gm{}).with(l0c_to_gm_params{});
copy(atom, dst, l0c);

uint64_t quant = 1;
copy(atom, dst, l0c, quant);
```

When a Tensor API tensor is passed in as the tensor quantization parameter, the quantized tensor may or may not carry a Batch dimension. Not carrying a Batch dimension means that all Batches share the same quantization parameters; carrying a Batch dimension means that each Batch uses its own quantization parameters, in which case the source tensor, the destination tensor, and the quantized tensor must all carry a Batch dimension.

When the quantized tensor does not carry a Batch dimension, the API reuses the same quantization parameters to complete the batch movement:

```cpp
auto quant = make_tensor(make_mem_ptr(quant_addr), make_frame_layout<nd_ext_layout_ptn>(1, n));

auto atom = make_copy(copy_l0c_to_gm{}).with(l0c_to_gm_params{});
copy(atom, dst, l0c, quant);
```

When the quantized tensor carries a Batch dimension, the entire batch movement cannot be completed through a single Fixpipe instruction. The API executes a `for` loop per Batch and uses the corresponding quantization parameters to complete movement batch by batch:

```cpp
auto quant = make_tensor(make_mem_ptr(quant_addr), make_frame_layout<nd_ext_layout_ptn>(batch, 1, n));

auto atom = make_copy(copy_l0c_to_gm{}).with(l0c_to_gm_params{});
copy(atom, dst, l0c, quant);
```

## Constraints

Quantization parameters must not be inf/nan or denormalized numbers.
