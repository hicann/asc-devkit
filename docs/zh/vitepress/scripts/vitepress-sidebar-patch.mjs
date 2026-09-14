/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

const LINK_END_MARKER = `      </VPLink>
      <component v-else :is="textTag" class="text" v-html="item.text" />`

const LINK_END_WITH_TOGGLE = `      </VPLink>
      <component v-else :is="textTag" class="text" v-html="item.text" />
      <button
        v-if="item.link && item.collapsed != null && item.items && item.items.length"
        type="button"
        class="sidebar-row-toggle"
        :aria-label="collapsed ? '展开栏目' : '折叠栏目'"
        :aria-expanded="!collapsed"
        :title="collapsed ? '展开' : '折叠'"
        @click.stop="toggle"
      ></button>
`

export function patchVitePressSidebarItem(code) {
  const isSidebarItemSource = code.includes('useSidebarControl') && code.includes('<template>')
  if (!isSidebarItemSource) return code

  let result = code
    .replace('depth < 5', 'true')
    .replace('props.depth + 2 === 7', 'props.depth + 2 >= 7')
    .replace(
      '<div v-if="item.items && item.items.length" class="items">',
      '<div v-if="item.items && item.items.length && !collapsed" class="items">'
    )

  if (!result.includes('class="sidebar-row-toggle"')) {
    const withToggle = result.replace(LINK_END_MARKER, LINK_END_WITH_TOGGLE)
    if (withToggle === result) {
      throw new Error('Unable to add the VitePress sidebar blank-area toggle')
    }
    result = withToggle
  }

  return result
}
