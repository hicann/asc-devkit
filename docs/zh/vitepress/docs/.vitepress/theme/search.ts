/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

interface PagefindResult {
  url: string
  content: string
  word_count: number
  filters: Filters
  meta: Meta
  anchors: Anchor[]
  weighted_locations: WeightedLocation[]
  locations: number[]
  raw_content: string
  raw_url: string
  excerpt: string
  sub_results: SubResult[]
}

interface SubResult {
  title: string
  url: string
  anchor: Anchor
  weighted_locations: WeightedLocation[]
  locations: number[]
  excerpt: string
}

interface WeightedLocation {
  weight: number
  balanced_score: number
  location: number
}

interface Anchor {
  element: string
  id: string
  text: string
  location: number
}

interface Meta {
  image_alt: string
  title: string
  image: string
  base64: string
  date?: string
}

interface Filters {
}

function decodeBase64AndDeserialize(base64String: string) {
  if (!base64String) {
    return {}
  }
  try {
    const serialized = decodeURIComponent(atob(base64String))
    const obj = JSON.parse(serialized)
    return obj
  }
  catch {
    return {}
  }
}

export function formatPagefindResult(result: PagefindResult, count = 1) {
  const { sub_results: subResults, anchors, weighted_locations: weightedLocations } = result
  weightedLocations.sort((a, b) => {
    if (b.weight === a.weight) {
      return a.location - b.location
    }
    return b.weight - a.weight
  })

  const subs: SubResult[] = []
  for (const { location } of weightedLocations) {
    const sub = subResults.find(candidate => (candidate.locations || []).includes(location))

    if (!sub) {
      continue
    }

    subs.push(sub)

    if (subs.length >= count) {
      break
    }
  }

  subs.sort((a, b) => {
    const [minA] = a.locations || []
    const [minB] = b.locations || []
    if (!minA || !minB) {
      return 0
    }
    return minA - minB
  })

  const filterMap = new Map<string, any>()
  return subs.map(sub => parseSubResult(sub, anchors, result))
    .filter((v) => {
      if (filterMap.has(v.meta.title)) {
        return false
      }
      filterMap.set(v.meta.title, v)
      return true
    })
}

function parseSubResult(sub: SubResult, anchors: Anchor[], result: PagefindResult) {
  const route = sub?.url || result?.url
  const description = sub?.excerpt || result?.excerpt

  const locationsAnchors = anchors?.filter((a) => {
    if (!sub)
      return false
    try {
      return a.location <= sub.anchor.location && a.element <= sub.anchor.element
    }
    catch {
      return false
    }
  }) || []
  locationsAnchors.reverse()

  const filteredAnchors = locationsAnchors.reduce((prev, curr) => {
    const isHave = prev.some(p => p.element === curr.element)
    if (isHave) {
      return prev
    }
    prev.unshift(curr)
    return prev
  }, [] as Anchor[])
  const title = filteredAnchors.length ? filteredAnchors.map(t => t.text.trim()).filter(v => !!v).join(' > ') : (result.meta.title || '').replace(/___PAGEFIND_ANCHOR___\S*/g, '').trim()

  const { base64, date, ...otherMeta } = result.meta
  return {
    route,
    meta: {
      date: date ? +date : undefined,
      ...decodeBase64AndDeserialize(base64),
      ...otherMeta,
      title,
      description,
    },
    result
  }
}
