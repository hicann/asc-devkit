/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

import DefaultTheme from 'vitepress/theme'
import Layout from './Layout.vue'
import BackToTop from './BackToTop.vue'
import SidebarResizer from './SidebarResizer.vue'
import './code-theme.css'
import './search-overrides.css'

import './filter.css'
import './code-copy.js'

export default {
  extends: DefaultTheme,
  Layout,
  enhanceApp({ app }) {
    app.component('BackToTop', BackToTop)
    app.component('SidebarResizer', SidebarResizer)
  }
}
