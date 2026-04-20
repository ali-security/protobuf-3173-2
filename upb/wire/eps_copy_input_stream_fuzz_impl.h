// Protocol Buffers - Google's data interchange format
// Copyright 2023 Google LLC.  All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef GOOGLE_UPB_UPB_WIRE_EPS_COPY_INPUT_STREAM_FUZZ_IMPL_H__
#define GOOGLE_UPB_UPB_WIRE_EPS_COPY_INPUT_STREAM_FUZZ_IMPL_H__

#include <variant>
#include <vector>

struct ReadOp {
  int bytes;
};

struct ReadStringOp {
  int bytes;
};

struct ReadStringAliasOp {
  int bytes;
};

struct PushLimitOp {
  int bytes;
};

typedef std::variant<ReadOp, ReadStringOp, ReadStringAliasOp, PushLimitOp> Op;

struct EpsCopyTestScript {
  int data_size;
  std::vector<Op> ops;
};

void TestAgainstFakeStream(const EpsCopyTestScript& script);

#endif  // GOOGLE_UPB_UPB_WIRE_EPS_COPY_INPUT_STREAM_FUZZ_IMPL_H__
