// Protocol Buffers - Google's data interchange format
// Copyright 2025 Google LLC.  All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include <stdint.h>

#include "upb/message/message.h"
#include "upb/mini_table/extension_registry.h"
#include "upb/mini_table/internal/message.h"
#include "upb/mini_table/message.h"
#include "upb/wire/decode_fast/cardinality.h"
#include "upb/wire/decode_fast/field_parsers.h"
#include "upb/wire/internal/decoder.h"

// Must be last.
#include "upb/port/def.inc"

UPB_FORCEINLINE void _upb_FastDecoder_PickHandlerForExtensionOrUnknown(
    struct upb_Decoder* d, intptr_t table, uint64_t data,
    upb_DecodeFastNext* next) {
  uint32_t field_num;
  if (UPB_LIKELY((data & 0x80) == 0)) {
    field_num = (uint8_t)data >> 3;
  } else if ((data & 0x8000) == 0) {
    field_num = ((data & 0x7f) | ((data & 0x7f00) >> 1)) >> 3;
  } else {
    // Tag >=2048.
    UPB_DECODEFAST_EXIT(kUpb_DecodeFastNext_FallbackToMiniTable, next);
    return;
  }

  const upb_MiniTable* mt = decode_totablep(table);
  if (d->extreg && upb_ExtensionRegistry_Lookup(d->extreg, mt, field_num)) {
    _upb_Decoder_Trace(d, 'e');
    UPB_DECODEFAST_EXIT(kUpb_DecodeFastNext_FallbackToMiniTable, next);
    return;
  }

  *next = kUpb_DecodeFastNext_DecodeUnknown;
}

UPB_PRESERVE_NONE const char* _upb_FastDecoder_DecodeExtensionOrUnknown(
    struct upb_Decoder* d, const char* ptr, upb_Message* msg, intptr_t table,
    uint64_t hasbits, uint64_t data) {
  upb_DecodeFastNext next;
  _upb_FastDecoder_PickHandlerForExtensionOrUnknown(d, table, data, &next);
  UPB_DECODEFAST_NEXT(next);
}
