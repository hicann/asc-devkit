# Transferring Large Data Blocks in a Single Operation<a name="ZH-CN_TOPIC_0000001893771813"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:43:59.722Z pushedAt=2026-11-03T07:05:13.338Z nl=2 -->

[Priority] High

[Description] When transferring data blocks of different sizes, the bandwidth utilization (effective bandwidth/theoretical bandwidth) differs. Based on actual test experience, when the data length of a single transfer reaches 16 KB or above, optimal bandwidth performance can usually be achieved. Therefore, for a single transfer, you should transfer data blocks as large as possible. The following figures show the actual bandwidth variation under different single transfer data volumes measured on a certain AI processor.

>[!NOTE] Note
>The test data is related to the processor model, and slight fluctuation may occur during actual testing. The specific bandwidth values may not be strictly consistent with the test data below.

**Figure 1**  UB-\>GM actual bandwidth occupancy variation under different single transfer data volumes in different directions<a name="fig925755124312"></a><br>
![](../../../figures/ub_gm_bw.png "Actual bandwidth occupancy variation under different single transfer data volumes in the UB->GM direction")

**Figure 2**  GM-\>UB actual bandwidth occupancy variation under different single transfer data volumes in different directions<a name="fig859712315453"></a>  
![](../../../figures/gm_ub_bw.png "Actual bandwidth occupancy variation under different single transfer data volumes in the GM->UB direction")
