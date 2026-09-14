# Matmul Advanced API Enabling L2 Cache Tiling<a name="ZH-CN_TOPIC_0000002341067578"></a>

<!-- md-trans-meta sourceCommit=e4a5905334db2bef02e668063a0b99d64a2e7709 translatedAt=2026-08-26T13:16:52.572Z -->

## Case Introduction<a name="section5590915836"></a>

This case demonstrates the performance improvement of the operator achieved through L2 Cache data splitting when the total amount of input and output data exceeds the L2 Cache size during Matmul computation. For the complete sample that enables L2 Cache tiling, see the [L2 Cache tiling operator sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_l2cache).

The AI processor used in this case has an L2 Cache size of 192 MB, and the pure read bandwidth of the L2 Cache is about three to four times that of the GM, resulting in a significant gap between the two. When moving in or moving out the same amount of data, accessing data in the L2 Cache is faster than accessing the GM. If the data cannot hit the L2 Cache, that is, the data to be accessed is not in the L2 Cache, the GM must be accessed for reads and writes, resulting in low bandwidth utilization. Eventually, moving data in or out of the operator becomes the performance bottleneck of the entire operator execution process.

-   Scenarios where enabling L2 Cache tiling is applicable

    The amount of input and output data exceeds the L2 Cache size.

The operator specifications in this case are as follows:

**Table 1**  Operator specifications

<a name="table101751125175213"></a>
<table><thead align="left"><tr id="row8175525185219"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p1417582516529"><a name="p1417582516529"></a><a name="p1417582516529"></a>Input</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p417532575212"><a name="p417532575212"></a><a name="p417532575212"></a>Shape</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p1017582585214"><a name="p1017582585214"></a><a name="p1017582585214"></a>Data Type</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p317502512522"><a name="p317502512522"></a><a name="p317502512522"></a>Format</p></th>
</tr>
</thead>
<tbody><tr id="row217562525215"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p517515253529"><a name="p517515253529"></a><a name="p517515253529"></a>a</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p1345112459266"><a name="p1345112459266"></a><a name="p1345112459266"></a>30720, 1024</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p181099364265"><a name="p181099364265"></a><a name="p181099364265"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p19110203618264"><a name="p19110203618264"></a><a name="p19110203618264"></a>ND</p></td>
</tr>
<tr id="row10176102512525"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p161761325185216"><a name="p161761325185216"></a><a name="p161761325185216"></a>b</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p4176112555211"><a name="p4176112555211"></a><a name="p4176112555211"></a>4096, 1024</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1176132515212"><a name="p1176132515212"></a><a name="p1176132515212"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p817692575212"><a name="p817692575212"></a><a name="p817692575212"></a>ND</p></td>
</tr>
</tbody>
</table>

## Obtaining Performance Data<a name="section443613301436"></a>

Use the msOpProf tool to obtain the [operator simulation pipeline diagram](../../performance_analysis/obtain_performance_data.md#section17259539153513) and [on-board profiling](../../performance_analysis/obtain_performance_data.md#section17953123893415) data. Because the L2 Cache tiling feature mainly leverages the higher-bandwidth L2 Cache to reduce the MTE2 data transfer overhead, focus on analyzing the MTE2 pipeline.

## Analyzing the Main Bottleneck<a name="section10759361232"></a>

This case is further optimized based on Tiling full constant. For details about Tiling full constant, see the [example of enabling tiling full constant for the Matmul advanced API](matmul_high_level_api_enabling_tiling_full_constant.md). The profiling data before optimization is as follows: the aic_time in column C is 867 us, the aic_mte2_time in column K is 861.9 us, and the MTE2 ratio is 99%. MTE2 data transfer is the bottleneck of the current operator performance.

![](../../../figures/zh-cn_image_0000002410305297.png)

## Designing an Optimization Solution<a name="section6251134218311"></a>

-   Optimization point 1: Adjust the tile size and the number of computation iterations

    -   Before optimization, the input data is not split, and all cores compute all the data in a single pass. As shown in the following figure, the numbers in the figure indicate core IDs. The 24 cores compute all the data of matrices A and B in one pass.
    -   After optimization, the input data is split into multiple parts, and all cores compute in multiple passes. Each core depends only on the split data volume in a single computation. The L2 Cache tiling solution ensures that the data of a single computation is all cached in the L2 Cache, making the transfer of input data more efficient.

    **Figure 1**  Schematic diagram of optimization point 1<a name="fig37871937335"></a>  
    ![](../../../figures/optimization_point_1_diagram.png "Schematic diagram of optimization point 1")

-   Optimization point 2: Select an L2 Cache tiling solution with a smaller tail

    Based on the principle of [inter-core load balancing](../../simd_operator_optimization/tiling_strategy/inter_core_load_balancing.md), the number of physical cores of an AI processor is fixed. After the data is split by the L2 Cache, some cores may have a computation tail. That is, the total computation volume of all cores in each pass divided by the data volume processed by each core in a single computation cannot be evenly divided by the number of cores, so the remaining data must be computed by some tail cores at the end of each computation. During tail-core computation, some cores remain idle, degrading the overall performance of the operator. In the following figure, the data blocks highlighted in yellow are the tail blocks. In the solution on the left, due to the tail, cores 0, 1, 2, and 3 perform one extra pass to process the remaining data in each computation. To achieve globally optimal load, the positions of the tail cores are adjusted, as shown in the solution on the right. When all computations are complete, cores 0 to 7 each perform one extra computation of a data block.

    In actual scenarios, on the premise that the split data volume is smaller than the L2 Cache size, the smaller the tail, the better. Based on this principle, the number of L2 Cache tiling blocks can be determined.

    **Figure 2**  Schematic diagram of optimization point 2<a name="fig187267372095"></a>  
    ![](../../../figures/optimization_point_2_diagram.png "Schematic diagram of optimization point 2")

-   Optimization point 3: Stagger core assignment to reduce same-address conflicts between the left and right matrices

    Same-address conflict: When multiple cores concurrently execute Matmul computation, if they access the same address of the input matrix at the same time, an address conflict occurs, which degrades performance.

    In the M and N directions, split the matrix data into large data blocks in the L2 Cache, and then assign cores in a staggered manner across the data blocks, that is, assign each data block to a different core along the diagonal for processing, thereby effectively reducing same-address conflicts. For example, when processing the tail blocks 0, 1, 2, and 3 in the same row, if the cores are assigned sequentially, multiple cores read the left matrix data of the same row at the same time, causing read-read conflicts. If the cores are assigned along the diagonal, the tail blocks on the diagonal are assigned to cores 0, 1, 2, and 3 for computation, and multiple cores access the left matrix data of different rows, reducing the number of same-address conflicts.

    **Figure 3** Schematic diagram of optimization point 3<a name="fig12727153717911"></a>  
    ![](../../../figures/optimization_point_3_diagram.png "Schematic diagram of optimization point 3")

For the complete sample of enabling L2 Cache tiling for the Matmul API, see the [operator sample for L2 cache tiling](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_l2cache). The key steps for implementing L2 Cache tiling are as follows:

1.  Determine whether L2 Cache tiling is required. If the total amount of data exceeds the configured L2 Cache size, calculate the number of L2 Cache tiles.

    ```
    bool smallDim = mTileNum_ < L1_MIN_UST_DIM && nTileNum_ < L1_MIN_UST_DIM; 
    if (smallDim || (!EnableL2Tile())) { // Determine whether the total amount of computation data is less than the L2Cache threshold.
        mL2TileNum_ = mTileNum_;
        nL2TileNum_ = nTileNum_;
        mL2BlockNum_ = 1;
        nL2BlockNum_ = 1;
        return; // No tiling is required. Return in advance.
    } 
    InitL2TileTail(); // Calculate the L2 tiling.
    ```

2.  Based on the load balancing principle, calculate the number of L2 Cache tilings: the number of L2 Cache tilings in the M direction is mL2TileNum\_, and the number of L2 Cache tilings in the N direction is nL2TileNum\_.

    ```
    int64_t mConflict = INT64_MAX; 
    int64_t nConflict = INT64_MAX; 
    constexpr bool isNMajor = l1N > l1M; // Determine the major dimension based on the shape size.
    for (int64_t i = maxMajor; i >= L1_MIN_UST_DIM; i--) {     
        for (int64_t j = maxMinor; j >= minMinor; j--) {         
            if (GetTotalSize(j * l1M, i * l1N, k_) <= L2_TILE_THRESHOLD) { // Ensure that the tile is smaller than the L2 Cache threshold.
                uint64_t mConflictTmp = AscendC::Ceil(blockNum_, mL2TileNumTailTmp); // Calculate the load conflict value.
                uint64_t nConflictTmp = AscendC::Ceil(blockNum_, nL2TileNumTailTmp);            
                if (mConflict >= mConflictTmp && nConflict >= nConflictTmp) { // If the conflict value is smaller, update the number of tiles.
                    mConflict = mConflictTmp;              
                    nConflict = nConflictTmp;         
                    mL2TileNum_ = curMajorDim;                 
                    nL2TileNum_ = curMinorDim;     
                }       
             }   
         }
     }
    ```

3.  Assign cores in a staggered manner. Input the index of the current data block to obtain the index of the core assigned along the diagonal.

    ```
    __aicore__ inline BlockCoord GetBlockCoord(int64_t tileIdx)    {  
        GetCommonTileIndex(tileIdx); 
        int64_t mTileIdx = newBlockIdx_ % mL2TileNumTmp_;
        mTileIdx = mTileIdx + mL2Idx_ * mL2TileNum_;
        int64_t nTileIdx = 0;     
        if (mL2TileNumTmp_ != 0 && nL2TileNumTmp_ != 0) {  
            int64_t tmp = newBlockIdx_ /CalcLcm(mL2TileNumTmp_, nL2TileNumTmp_);
            nTileIdx = (newBlockIdx_ + tmp) % nL2TileNumTmp_;
        }      
        nTileIdx = nTileIdx + nL2Idx_ * nL2TileNum_;     
        return {mTileIdx * l1M, nTileIdx * l1N, 0};
    }
    ```

4.  Set the left and right matrices, and compute Matmul repeatedly in a loop based on the number of L2 Cache tilings and the index of the executing core calculated in the preceding steps.

    ```
    L2CacheOpt l2Opt(shapes, blockNum); 
    matmulObj.SetOrgShape(shapes.m, shapes.n, shapes.k);
    for (int64_t tileIdx = curBlockIdx; tileIdx < l2Opt.GetTileNum(); tileIdx += blockNum) { 
        auto blockShape = l2Opt.GetBlockShape(tileIdx);  // Obtain the L2 split block size for a single computation.
        if (Get<0>(blockShape) <= 0 ||
            Get<1>(blockShape) <= 0){
            return;
        }
        auto blockCoord = l2Opt.GetBlockCoord(tileIdx); 
        // Obtain the index blockCoord of the core currently executing the computation.
        matmulObj.SetTail(Get<0>(blockShape), Get<1>(blockShape), Get<2>(blockShape)); 
        const auto& offsetCoord = CalcOffset(shapes, blockCoord); // Calculate the matrix offset based on the index.
        int64_t offsetA = Get<0>(offsetCoord);
        int64_t offsetB = Get<1>(offsetCoord);   
        int64_t offsetC = Get<2>(offsetCoord);
        matmulObj.SetTensorA(aGlobal[offsetA], false);  
        matmulObj.SetTensorB(bGlobal[offsetB], false);  
        if (shapes.isBias) {          
            matmulObj.SetBias(biasGlobal);    
        }  
        matmulObj.IterateAll(cGlobal[offsetC]);  // Compute the L2 split block.
    } 
    matmulObj.End();
    ```

## Verifying the Performance Gains of the Optimization Solution<a name="section36229589310"></a>

The optimized profiling data is as follows. The aic_time of column C is 805.6 us. Compared with the pre-optimization value, the total execution time is reduced by about 7.1%, and the MTE2 transfer time is reduced by about 10.7%.

![](../../../figures/zh-cn_image_0000002376979814.png)

## Summary<a name="section1593812115413"></a>

When the Matmul computation data exceeds the L2 Cache size, consider enabling L2 Cache tiling to improve the L2 Cache hit rate and leverage the high bandwidth of L2 Cache to boost operator performance.
