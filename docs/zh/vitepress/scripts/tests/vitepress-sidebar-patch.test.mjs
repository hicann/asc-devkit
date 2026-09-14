/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

import assert from 'node:assert/strict'
import test from 'node:test'
import { patchVitePressSidebarItem } from '../vitepress-sidebar-patch.mjs'

const sidebarItemSource = `<script setup>
const { collapsed, toggle } = useSidebarControl(item)
const textTag = props.depth + 2 === 7
</script>
<template>
  <VPLink class="link" :href="item.link">
    <component :is="textTag" class="text" v-html="item.text" />
      </VPLink>
      <component v-else :is="textTag" class="text" v-html="item.text" />
  <div v-if="item.items && item.items.length" class="items">
    <template v-if="depth < 5"></template>
  </div>
  <div class="caret" @click="onCaretClick"></div>
</template>`

test('adds a dedicated toggle only beside linked sidebar groups', () => {
  const result = patchVitePressSidebarItem(sidebarItemSource)

  assert.match(result, /class="sidebar-row-toggle"/)
  assert.match(result, /v-if="item\.link && item\.collapsed != null && item\.items && item\.items\.length"/)
  assert.match(result, /@click\.stop="toggle"/)
  assert.match(result, /:aria-expanded="!collapsed"/)
  assert.match(result, /<VPLink class="link" :href="item\.link">/)
  assert.match(result, /<\/VPLink>\s*<component v-else[^>]+\/>\s*<button/)
  assert.match(result, /class="caret" @click="onCaretClick"/)
})

test('preserves the existing deep-sidebar changes and is idempotent', () => {
  const once = patchVitePressSidebarItem(sidebarItemSource)
  const twice = patchVitePressSidebarItem(once)

  assert.equal(twice, once)
  assert.match(once, /<template v-if="true">/)
  assert.match(once, /props\.depth \+ 2 >= 7/)
  assert.match(once, /item\.items && item\.items\.length && !collapsed/)
  assert.equal((once.match(/class="sidebar-row-toggle"/g) || []).length, 1)
})

test('ignores unrelated VitePress modules', () => {
  const source = 'export function useSidebarControl() {}'
  assert.equal(patchVitePressSidebarItem(source), source)
})
