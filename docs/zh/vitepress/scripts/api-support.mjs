/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

const SUPPORT_HEADING_RE = /^##[ \t]+产品支持情况(?:[ \t]*<|[ \t]*$)/
const NEXT_H2_RE = /^##[ \t]+/
const SUPPORT_ROW_RE = /^[-*+][ \t]+(.+?)[ \t]*[：:][ \t]*(不支持|支持)[ \t]*$/

const PRODUCT_MATCHERS = [
  ['950', /^Ascend 950PR\/Ascend 950DT$/],
  ['A3', /^Atlas A3 训练系列产品\/Atlas A3 推理系列产品$/],
  ['910b', /^Atlas A2 训练系列产品\/Atlas A2 推理系列产品$/],
  ['310b', /^Atlas 200I\/500 A2 推理产品$/],
  ['310p', /^Atlas 推理系列产品 ?(?:AI Core|Vector Core)$/],
  ['910', /^Atlas 训练系列产品$/],
  ['x90', /^Kirin X90$/],
  ['9030', /^Kirin 9030$/],
]

function normalizeProductName(value) {
  return value
    .replace(/<!--[^]*?-->/g, '')
    .replace(/<[^>]+>/g, '')
    .replace(/[ \t]+/g, ' ')
    .trim()
}

export function getProductFilterValue(productName) {
  const normalized = normalizeProductName(productName)
  return PRODUCT_MATCHERS.find(([, pattern]) => pattern.test(normalized))?.[0] || null
}

export function extractUnsupportedProducts(markdown) {
  const lines = markdown.replace(/\r\n?/g, '\n').split('\n')
  const headingIndex = lines.findIndex(line => SUPPORT_HEADING_RE.test(line))
  if (headingIndex < 0) return []

  const states = new Map()
  for (let i = headingIndex + 1; i < lines.length; i++) {
    if (NEXT_H2_RE.test(lines[i])) break
    const match = lines[i].trim().match(SUPPORT_ROW_RE)
    if (!match) continue

    const product = getProductFilterValue(match[1])
    if (!product) continue
    const productStates = states.get(product) || []
    productStates.push(match[2] === '支持')
    states.set(product, productStates)
  }

  return [...states.entries()]
    .filter(([, productStates]) => productStates.length > 0 && !productStates.some(Boolean))
    .map(([product]) => product)
}

export function normalizeApiRoute(value) {
  if (!value) return ''
  let route = String(value).replace(/\\/g, '/').replace(/[?#].*$/, '')
  route = route.replace(/^https?:\/\/[^/]+/i, '')
  try {
    route = decodeURI(route)
  } catch {}
  if (!route.startsWith('/')) route = '/' + route
  route = route.replace(/\.(?:md|html)$/i, '').replace(/\/+$/, '')
  return route || '/'
}

export function isApiRouteUnsupported(index, route, selectedFilter) {
  if (!selectedFilter || selectedFilter === 'all') return false
  const unsupported = index?.[normalizeApiRoute(route)]
  return Array.isArray(unsupported) && unsupported.includes(selectedFilter)
}

function getRouteSegments(link) {
  return normalizeApiRoute(link)
    .split('/')
    .filter(Boolean)
}

function isStructuralSidebarItem(item, parent) {
  if (!item?.link) return false

  const segments = getRouteSegments(item.link)
  const basename = segments.at(-1)?.toLowerCase()
  if (!basename) return false
  if (['overview', 'introduction', 'readme', '\u6982\u8ff0'].includes(basename)) {
    return true
  }

  const parentRoute = normalizeApiRoute(parent?.link)
  if (parentRoute && normalizeApiRoute(item.link) === parentRoute) return true

  const directoryName = segments.at(-2)?.toLowerCase()
  return Boolean(directoryName && basename === directoryName)
}

function hasSubstantiveDescendant(items, parent) {
  return (items || []).some(item => {
    if (Array.isArray(item.items) && item.items.length > 0) {
      return true
    }
    return !isStructuralSidebarItem(item, parent)
  })
}

export function filterSidebarItems(items, selectedFilter, unsupportedIndex) {
  return (items || []).reduce((result, item) => {
    const hadChildren = Array.isArray(item.items) && item.items.length > 0
    const originallyHadContent = hadChildren
      ? hasSubstantiveDescendant(item.items, item)
      : false
    const children = hadChildren
      ? filterSidebarItems(item.items, selectedFilter, unsupportedIndex)
      : []
    const unsupported = item.link
      ? isApiRouteUnsupported(unsupportedIndex, item.link, selectedFilter)
      : false

    if (
      selectedFilter !== 'all' &&
      originallyHadContent &&
      !hasSubstantiveDescendant(children, item)
    ) {
      return result
    }

    if (children.length > 0) {
      const entry = { ...item, items: children }
      if (unsupported) delete entry.link
      result.push(entry)
      return result
    }

    if (hadChildren && !item.link) return result
    if (unsupported) return result

    const entry = { ...item }
    if (hadChildren) delete entry.items
    result.push(entry)
    return result
  }, [])
}

export function renumberSidebarItems(items, prefix = '') {
  return (items || []).map((item, index) => {
    const number = prefix ? `${prefix}${index + 1}.` : `${index + 1}.`
    const unnumberedText = item.unnumberedText ||
      String(item.text || '').replace(/^\d+(?:\.\d+)*\.?[ \t]+/, '')
    return {
      ...item,
      unnumberedText,
      text: `${number} ${unnumberedText}`,
      ...(item.items
        ? { items: renumberSidebarItems(item.items, number) }
        : {}),
    }
  })
}

export function getFilteredSidebarConfig(theme, selectedFilter) {
  const apiSidebarSource = Array.isArray(theme?.apiSidebarSource)
    ? theme.apiSidebarSource
    : theme?.sidebar?.['/api/']
  if (!theme?.sidebar || !Array.isArray(apiSidebarSource)) {
    return theme?.sidebar
  }
  if (!selectedFilter || selectedFilter === 'all') return theme.sidebar

  return {
    ...theme.sidebar,
    '/api/': renumberSidebarItems(
      filterSidebarItems(
        apiSidebarSource,
        selectedFilter,
        theme.apiUnsupportedIndex
      )
    ),
  }
}

export function filterNavigationCandidates(
  candidates,
  currentRelativePath,
  selectedFilter,
  unsupportedIndex,
  filteredApiSidebar
) {
  if (!selectedFilter || selectedFilter === 'all') return candidates

  const currentRoute = normalizeApiRoute(currentRelativePath)
  const visibleApiItems = Array.isArray(filteredApiSidebar)
    ? collectSidebarItemsByRoute(filteredApiSidebar)
    : null
  return (candidates || []).reduce((result, candidate) => {
    const candidateRoute = normalizeApiRoute(candidate.link)
    const visibleItem = visibleApiItems?.get(candidateRoute)
    if (candidateRoute === currentRoute) {
      result.push(visibleItem ? { ...candidate, text: visibleItem.text } : candidate)
      return result
    }
    if (!candidateRoute.startsWith('/api/')) {
      result.push(candidate)
      return result
    }
    if (visibleApiItems) {
      if (visibleItem) result.push({ ...candidate, text: visibleItem.text })
      return result
    }
    if (!isApiRouteUnsupported(unsupportedIndex, candidate.link, selectedFilter)) {
      result.push(candidate)
    }
    return result
  }, [])
}

function collectSidebarItemsByRoute(items, itemMap = new Map()) {
  for (const item of items || []) {
    if (item.link) itemMap.set(normalizeApiRoute(item.link), item)
    if (item.items) collectSidebarItemsByRoute(item.items, itemMap)
  }
  return itemMap
}
