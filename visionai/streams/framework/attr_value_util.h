/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_FRAMEWORK_ATTR_VALUE_UTIL_H_
#define THIRD_PARTY_VISIONAI_STREAMS_FRAMEWORK_ATTR_VALUE_UTIL_H_

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "visionai/streams/framework/attr_value.pb.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

// Generates an error if attr_value doesn't have the indicated attr type.
absl::Status AttrValueHasType(const AttrValue& attr_value,
                              absl::string_view type);

// Parses the string `text` as a value of type `type`, and then store it into an
// AttrValue with the appriate fields set.
//
// For example, if type is "int" and text is "-3", then AttrValue will have the
// oneof of `i` set to the value -3. If type were "string", then `s` will be set
// to the value "-3".
//
// An error will be returned if the text could not be parsed as the given type.
absl::StatusOr<AttrValue> ParseAttrValue(absl::string_view type,
                                         absl::string_view text);

// Sets *out based on the type of value.
void SetAttrValue(const std::string& value, AttrValue* out);
void SetAttrValue(const char* value, AttrValue* out);
void SetAttrValue(absl::string_view value, AttrValue* out);
void SetAttrValue(int64_t value, AttrValue* out);
void SetAttrValue(int32_t value, AttrValue* out);
void SetAttrValue(float value, AttrValue* out);
void SetAttrValue(double value, AttrValue* out);
void SetAttrValue(bool value, AttrValue* out);
void SetAttrValue(const AttrValue& value, AttrValue* out);

// Gets the value of the specific type from the AttrValue.
//
// Returns an error if the AttrValue does not contain the proper type.
absl::Status GetAttrValue(const AttrValue& value, int32_t* out);
absl::Status GetAttrValue(const AttrValue& value, int64_t* out);
absl::Status GetAttrValue(const AttrValue& value, float* out);
absl::Status GetAttrValue(const AttrValue& value, double* out);
absl::Status GetAttrValue(const AttrValue& value, bool* out);
absl::Status GetAttrValue(const AttrValue& value, std::string* out);

template <typename T>
absl::StatusOr<T> GetAttrValue(const AttrValue& value) {
  T out;
  VAI_RETURN_IF_ERROR(GetAttrValue(value, &out));
  return out;
}

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_FRAMEWORK_ATTR_VALUE_UTIL_H_
