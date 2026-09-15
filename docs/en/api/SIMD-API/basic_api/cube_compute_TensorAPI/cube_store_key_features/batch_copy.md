# Batch Copy

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T17:08:18.959Z -->

Batch copy is a capability that adds a batch dimension to the outermost layer of the copy from L0C Buffer to Global Memory or Unified Buffer, used to complete the copy of computation results of multiple matrices at once.

## Description

The source tensor is in the `NZ` format, and the destination tensor supports the `ND`, `DN`, or `NZ` format. You only need to pass the Batch parameter when constructing the Layout, and the `Copy` call form remains consistent with the non-batch mode.

The source tensor can be constructed through `MakeFrameLayout<NZLayoutPtn>(batch, m, n)`. The destination tensor can be constructed through `MakeFrameLayout<NDLayoutPtn>(batch, m, n)`, `MakeFrameLayout<DNLayoutPtn>(batch, m, n)`, `MakeFrameLayout<NDExtLayoutPtn>(batch, m, n)`, `MakeFrameLayout<DNExtLayoutPtn>(batch, m, n)`, or `MakeFrameLayout<NZLayoutPtn, DstType>(batch, m, n)`.

## Copy Behavior

When `quant` is not passed or a scalar quantization parameter of the `uint64_t` type is passed, Batch dimension matching of the quantized tensor is not involved. The API completes the copy based on the Batch count of the source tensor. In the scalar quantization scenario, all Batches share the same quantization parameter:


```cpp
auto l0c = MakeTensor(MakeMemPtr(l0cAddr), MakeFrameLayout<NZLayoutPtn>(batch, m, n));
auto dst = MakeTensor(MakeMemPtr(dstAddr), MakeFrameLayout<NDExtLayoutPtn>(batch, m, n));

auto atom = MakeCopy(CopyL0C2GM{}).with(FixpipeParams{});
Copy(atom, dst, l0c);

uint64_t quant = 1;
Copy(atom, dst, l0c, quant);
```

When a Tensor API tensor is passed as the tensor quantization parameter, the quantized tensor may or may not carry a Batch dimension. Without a Batch dimension, all Batches share the same quantization parameter. With a Batch dimension, each Batch uses its own quantization parameter, in which case the source tensor, destination tensor, and quantized tensor must all carry a Batch dimension.

When the quantized tensor does not carry a Batch dimension, the API reuses the same quantization parameter to complete the batch copy:

```cpp
auto quant = MakeTensor(MakeMemPtr(quantAddr), MakeFrameLayout<NDExtLayoutPtn>(1, n));

auto atom = MakeCopy(CopyL0C2GM{}).with(FixpipeParams{});
Copy(atom, dst, l0c, quant);
```

When the quantized tensor carries a Batch dimension, the entire batch copy cannot be completed through a single Fixpipe instruction. The API executes a `for` loop over the Batches, using the corresponding quantization parameter for each Batch to complete the copy:

```cpp
auto quant = MakeTensor(MakeMemPtr(quantAddr), MakeFrameLayout<NDExtLayoutPtn>(batch, 1, n));

auto atom = MakeCopy(CopyL0C2GM{}).with(FixpipeParams{});
Copy(atom, dst, l0c, quant);
```
