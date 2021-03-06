// Copyright 2014 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "syzygy/agent/asan/heap_checker.h"

#include "syzygy/agent/asan/asan_runtime.h"
#include "syzygy/agent/asan/page_protection_helpers.h"
#include "syzygy/agent/asan/shadow.h"

namespace agent {
namespace asan {

bool HeapChecker::IsHeapCorrupt(CorruptRangesVector* corrupt_ranges) {
  DCHECK_NE(reinterpret_cast<CorruptRangesVector*>(NULL), corrupt_ranges);

  corrupt_ranges->clear();

  // Grab the page protection lock. This prevents multiple heap checkers from
  // running simultaneously, and also prevents page protections from being
  // modified from underneath us.
  ::common::AutoRecursiveLock scoped_lock(block_protect_lock);

  // Walk over all of the addressable memory to find the corrupt blocks.
  // TODO(sebmarchand): Iterates over the heap slabs once we have switched to
  //     a new memory allocator.
  GetCorruptRangesInSlab(
      reinterpret_cast<const uint8*>(Shadow::kAddressLowerBound),
      Shadow::kAddressUpperBound - Shadow::kAddressLowerBound - 1,
      corrupt_ranges);

  return !corrupt_ranges->empty();
}

void HeapChecker::GetCorruptRangesInSlab(const uint8* lower_bound,
                                         size_t length,
                                         CorruptRangesVector* corrupt_ranges) {
  DCHECK_NE(reinterpret_cast<const uint8*>(NULL), lower_bound);
  DCHECK_NE(0U, length);
  DCHECK_NE(reinterpret_cast<CorruptRangesVector*>(NULL), corrupt_ranges);

  ShadowWalker shadow_walker(false, lower_bound, lower_bound + length);

  AsanCorruptBlockRange* current_corrupt_range = NULL;

  // Iterates over the blocks.
  BlockInfo block_info = {};
  while (shadow_walker.Next(&block_info)) {
    // Remove the protections on this block so its checksum can be safely
    // validated. We leave the protections permanently removed so that the
    // minidump generation has free access to block contents.
    BlockProtectNone(block_info);

    bool current_block_is_corrupt = IsBlockCorrupt(block_info.block, NULL);
    // If the current block is corrupt and |current_corrupt_range| is NULL
    // then this means that the current block is at the beginning of a corrupt
    // range.
    if (current_block_is_corrupt && current_corrupt_range == NULL) {
      AsanCorruptBlockRange corrupt_range;
      corrupt_range.address = block_info.block;
      corrupt_range.length = 0;
      corrupt_range.block_count = 0;
      corrupt_range.block_info = NULL;
      corrupt_range.block_info_count = 0;
      corrupt_ranges->push_back(corrupt_range);
      current_corrupt_range = &corrupt_ranges->back();
    } else if (!current_block_is_corrupt && current_corrupt_range != NULL) {
      current_corrupt_range = NULL;
    }

    if (current_block_is_corrupt) {
      // If the current block is corrupt then we need to update the size of the
      // current range.
      DCHECK_NE(reinterpret_cast<AsanCorruptBlockRange*>(NULL),
                current_corrupt_range);
      current_corrupt_range->block_count++;
      const uint8* current_block_end = block_info.block + block_info.block_size;
      current_corrupt_range->length = current_block_end -
          reinterpret_cast<const uint8*>(current_corrupt_range->address);
    }
  }
}

}  // namespace asan
}  // namespace agent
