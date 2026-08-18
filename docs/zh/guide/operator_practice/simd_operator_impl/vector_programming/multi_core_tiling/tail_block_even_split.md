# 尾块均分

该场景中，完成主块分配后仍有剩余数据，但剩余数据无法继续按主块均匀分配到每个核。为了均衡利用多核计算能力，继续以DataBlock为最小单位将主块均分后剩余数据`mainTileRemainder`均衡分配到每个核，每个核分配到`tailBlockNumEachCore`个DataBlock的数据，且按DataBlock均分后无剩余。最终各核处理的数据量相同，核内除主块外还存在等长尾块。

本场景样例的完整代码请参考[场景1：尾块均分](../../../../../../../examples/01_simd_cpp_api/02_features/02_tiling_selector/vector_tiling_strategy/README.md#scenario1-main-tile-tail-block)。样例中固定使用8个核，主块长度`mainTileLength`为3200个`half`元素，数据整体长度为260096个`half`元素。数据平均分配到8个核上处理，每个核计算32512个元素；每个核内包含10个主块和512个元素的尾块。数据切分示意如下图所示。

**图 1**  数据切分示意图

![数据切分示意图](../../../../figures/tailblock_tiling.png "数据切分示意图")

与主块均分相比，本场景每个核内除主块外还存在尾块，因此Tiling参数在`blockLength`和`tileNum`基础上新增`lastTileLength`，用于记录尾块的元素数量。

## Tiling实现

算子的Tiling结构体定义如下：

```cpp
struct MainTileWithTailBlockTiling {
    uint32_t blockLength;    // 每个核处理的元素数量
    uint32_t tileNum;        // 主块数量
    uint32_t lastTileLength; // 尾块元素数量
};
```

Host侧Tiling沿用[主块均分](main_block_even_split.md)中的DataBlock对齐和主块分配计算，得到：主块均分后的剩余元素数量`mainTileRemainder`、每个核已分配的基础元素数量`baseLength`、以及对应的主块数量`baseTileNum`。当`mainTileRemainder`不为0时，主块均分分支不成立，继续执行以下步骤：

1. 以DataBlock为最小单位，将主块均分后的剩余数据`mainTileRemainder`均匀分配到每个核上。一个DataBlock包含`alignNum`个`half`元素，因此代码中以`alignNum`作为均分的单位。用`mainTileRemainder`除以`numBlocks * alignNum`得到每个核分配到的DataBlock数量`tailBlockNumEachCore`，取余得到按DataBlock均分后仍无法均分到每个核的剩余元素数量`remainingTailLength`；再将`tailBlockNumEachCore`乘以`alignNum`得到每个核基础尾块的元素数量`baseLastTileLength`，并累加到`baseLength`上得到每个核的基础元素数量。

    ```cpp
    uint32_t tailBlockNumEachCore = mainTileRemainder / (numBlocks * alignNum);
    uint32_t remainingTailLength = mainTileRemainder % (numBlocks * alignNum);
    uint32_t baseLastTileLength = tailBlockNumEachCore * alignNum;
    baseLength += baseLastTileLength;
    ```

2. 判断`remainingTailLength`是否为0。等于0表示`mainTileRemainder`能以DataBlock为最小单位均匀分配到每个核，此时进入尾块均分场景，使用`MainTileWithTailBlockTiling`结构体传递Tiling切分信息。

    ```cpp
    if (remainingTailLength == 0U) {
        MainTileWithTailBlockTiling tiling{baseLength, baseTileNum, baseLastTileLength};
        add_custom<MainTileWithTailBlockTiling><<<numBlocks, 0, stream>>>(xDevice, yDevice, zDevice, tiling);
        ...
    }
    ```

对形状为`(1, 260096)`的输入数据计算后，Tiling结构体内各个变量的值如下：

```cpp
MainTileWithTailBlockTiling tiling{
    32512, // blockLength：每个核计算32512个half，8个核共计算260096个half
    10,    // tileNum：每个核包含10个主块
    512    // lastTileLength：每个核包含512个half的尾块
};
```

## 算子类实现

核函数（Kernel）侧算子仍采用[静态Tensor编程](../../../../programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md)方式实现。与[主块均分](main_block_even_split.md)相比，本场景中每个核处理的数据长度仍相同，因此Global Memory偏移仍由`tiling.blockLength`和`GetBlockIdx()`计算；差异在于`Init`函数需要额外保存`tiling.lastTileLength`，供`Process`函数判断最后一次循环是否处理尾块。

```cpp
__aicore__ inline void Init(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z,
                            MainTileWithTailBlockTiling tiling)
{
    uint32_t offset = AscendC::GetBlockIdx() * tiling.blockLength;
    InitGm(x, y, z, offset, tiling.blockLength);
    this->tileNum = tiling.tileNum;
    this->lastTileLength = tiling.lastTileLength;
}
```

由于尾块长度`lastTileLength`与主块长度`mainTileLength`不同，`Process`函数需要根据当前循环位置确定本次待处理的数据长度。`totalLoop`为当前核需要处理的循环次数：前`tileNum`轮处理主块，最后一轮处理尾块；`curLen`为本轮处理的元素数量，`startElement`为本轮在核内的元素偏移。

```cpp
__aicore__ inline void Process()
{
    AscendC::LocalTensor<half> xLocal(AscendC::TPosition::VECCALC, xAddr, mainTileLength);
    AscendC::LocalTensor<half> yLocal(AscendC::TPosition::VECCALC, yAddr, mainTileLength);
    AscendC::LocalTensor<half> zLocal(AscendC::TPosition::VECCALC, zAddr, mainTileLength);

    uint32_t totalLoop = this->tileNum + (this->lastTileLength > 0U ? 1U : 0U);
    for (uint32_t loopIdx = 0; loopIdx < totalLoop; loopIdx++) {
        uint32_t curLen = (loopIdx < this->tileNum) ? mainTileLength : this->lastTileLength;
        uint32_t startElement = loopIdx * mainTileLength;
        // ... 同步管理代码省略，请参考完整样例代码 ...
        AscendC::DataCopy(xLocal, xGm[startElement], curLen);
        AscendC::DataCopy(yLocal, yGm[startElement], curLen);
        // ... 同步管理代码省略 ...
        AscendC::Add(zLocal, xLocal, yLocal, curLen);
        // ... 同步管理代码省略 ...
        AscendC::DataCopy(zGm[startElement], zLocal, curLen);
        // ... 同步管理代码省略 ...
    }
}
```
