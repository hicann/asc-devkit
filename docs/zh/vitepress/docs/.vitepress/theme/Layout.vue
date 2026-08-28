<!--
  Copyright (c) 2026 Huawei Technologies Co., Ltd.
  This program is free software, you can redistribute it and/or modify it under the terms and conditions of
  CANN Open Software License Agreement Version 2.0 (the "License").
  Please refer to the License for details. You may not use this file except in compliance with the License.
  THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
  See LICENSE in the root of the software repository for the full text of the License.
-->

<script setup>
import DefaultTheme from 'vitepress/theme'
import { onContentUpdated } from 'vitepress'
import { nextTick, onBeforeUnmount, onMounted } from 'vue'
import FilterToggle from './FilterToggle.vue'
import BackToTop from './BackToTop.vue'
import CustomOutline from './CustomOutline.vue'
import ApiDownload from './ApiDownload.vue'
import EditOnGitcode from './EditOnGitcode.vue'
import PageBreadcrumb from './PageBreadcrumb.vue'
import DocumentBuildInfo from './DocumentBuildInfo.vue'
import { installAutomaticTableLayout } from './table_layout.mjs'

let sidebarScrollRequest = 0

function scrollActiveSidebarItem() {
  if (typeof window === 'undefined') return
  const request = ++sidebarScrollRequest

  nextTick(() => {
    requestAnimationFrame(() => {
      requestAnimationFrame(() => {
        if (request !== sidebarScrollRequest) return

        const sidebar = document.querySelector('.VPSidebar')
        const activeItem = sidebar?.querySelector('.VPSidebarItem.is-active > .item > .link')
        if (!sidebar || !activeItem) return

        const sidebarRect = sidebar.getBoundingClientRect()
        const activeRect = activeItem.getBoundingClientRect()
        const edgePadding = 48
        const isVisible = activeRect.top >= sidebarRect.top + edgePadding &&
          activeRect.bottom <= sidebarRect.bottom - edgePadding

        if (isVisible) return

        const top = sidebar.scrollTop + activeRect.top - sidebarRect.top -
          (sidebar.clientHeight - activeRect.height) / 2
        const behavior = window.matchMedia('(prefers-reduced-motion: reduce)').matches
          ? 'auto'
          : 'smooth'
        sidebar.scrollTo({ top: Math.max(0, top), behavior })
      })
    })
  })
}

onMounted(() => {
  installAutomaticTableLayout()
  scrollActiveSidebarItem()
})
onContentUpdated(scrollActiveSidebarItem)
onBeforeUnmount(() => { sidebarScrollRequest += 1 })
</script>

<template>
  <DefaultTheme.Layout>
    <template #nav-bar-content-after>
      <FilterToggle />
    </template>
    <template #aside-outline-before>
      <CustomOutline />
    </template>
    <template #doc-before>
      <PageBreadcrumb />
    </template>
    <template #doc-footer-before>
      <DocumentBuildInfo />
      <EditOnGitcode />
    </template>
    <template #layout-bottom>
      <ApiDownload />
      <BackToTop />
      <SidebarResizer />
    </template>
  </DefaultTheme.Layout>
</template>
