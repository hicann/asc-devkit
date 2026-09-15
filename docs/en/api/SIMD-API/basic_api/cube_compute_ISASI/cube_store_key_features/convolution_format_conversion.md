# Convolution Format Conversion

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:12:21.600Z -->

In convolution operations, the data stored in the L0C Buffer is in the NC1HWC0 format. Through the [NZ2ND](NZ2ND.md) capability, the Fixpipe instruction supports converting the data in the L0C Buffer to the NHWC format for output. The conversion diagram is as follows:

**Figure 1** NC1HWC0 to NHWC format conversion<a name="zh-cn_topic_0000002515830464_fig63620361414"></a>  

![](../../../../figures/Fixpipe_NC1HWC0_NHWC.png)

<!-- npu="950" id1 -->
In particular, for the Ascend 950PR/Ascend 950DT products, the Fixpipe instruction also supports converting the data in the L0C Buffer to the NCHW format for output through the [NZ2DN](NZ2DN.md) capability. The conversion diagram is as follows:

**Figure 2**  NC1HWC0 to NCHW format conversion<a name="fig188671630134414"></a>  

![](../../../../figures/Fixpipe_NC1HWC0_NCHW.png)
<!-- end id1 -->
