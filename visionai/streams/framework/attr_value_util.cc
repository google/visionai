// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/framework/attr_value_util.h"

#include <string>

#include "absl/status/status.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"

namespace visionai {

absl::Status AttrValueHasType(const AttrValue& attr_value,
                              absl::string_view type) {
#define VALIDATE_FIELD(type_string, oneof_case)                          \
  do {                                                                   \
    if (attr_value.value_case() == AttrValue::oneof_case) {              \
      if (type != type_string) {                                         \
        return absl::InvalidArgumentError(absl::StrFormat(               \
            "AttrValue had value with type '%s' when '%s' is expected.", \
            type_string, type));                                         \
      }                                                                  \
    }                                                                    \
  } while (false)

  VALIDATE_FIELD("string", kS);
  VALIDATE_FIELD("int", kI);
  VALIDATE_FIELD("float", kF);
  VALIDATE_FIELD("bool", kB);
#undef VALIDATE_FIELD

  return absl::OkStatus();
}

absl::StatusOr<AttrValue> ParseAttrValue(absl::string_view type,
                                         absl::string_view text) {
  AttrValue out;
  if (type == "string") {
    *out.mutable_s() = std::string(text);
  } else if (type == "int") {
    int64_t value;
    if (!absl::SimpleAtoi(text, &value)) {
      return absl::InvalidArgumentError(
          absl::StrFormat("Text '%s' could not be parsed as an int64.", text));
    }
    out.set_i(value);
  } else if (type == "float") {
    float value;
    if (!absl::SimpleAtof(text, &value)) {
      return absl::InvalidArgumentError(
          absl::StrFormat("Text '%s' could not be parsed as a float32.", text));
    }
    out.set_f(value);
  } else if (type == "bool") {
    bool value;
    if (!absl::SimpleAtob(text, &value)) {
      return absl::InvalidArgumentError(
          absl::StrFormat("Text '%s' could not be parsed as a bool.", text));
    }
    out.set_b(value);
  } else {
    return absl::InvalidArgumentError(
        absl::StrFormat("Unsupported type '%s' to parse into.", type));
  }
  return out;
}

void SetAttrValue(const AttrValue& value, AttrValue* out) { *out = value; }
void SetAttrValue(const std::string& value, AttrValue* out) {
  out->set_s(value);
}
void SetAttrValue(const char* value, AttrValue* out) { out->set_s(value); }
void SetAttrValue(absl::string_view value, AttrValue* out) {
  out->set_s(value.data());
}
void SetAttrValue(int64_t value, AttrValue* out) { out->set_i(value); }
void SetAttrValue(int32_t value, AttrValue* out) { out->set_i(value); }
void SetAttrValue(float value, AttrValue* out) { out->set_f(value); }
void SetAttrValue(double value, AttrValue* out) { out->set_f(value); }
void SetAttrValue(bool value, AttrValue* out) { out->set_b(value); }

#define DEFINE_GET_ATTR_VALUE(TYPE, FIELD, ATTR_TYPE, CAST)      \
  absl::Status GetAttrValue(const AttrValue& value, TYPE* out) { \
    VAI_RETURN_IF_ERROR(AttrValueHasType(value, ATTR_TYPE));         \
    const auto& v = value.FIELD();                               \
    *out = CAST;                                                 \
    return absl::OkStatus();                                     \
  }

DEFINE_GET_ATTR_VALUE(std::string, s, "string", v)
DEFINE_GET_ATTR_VALUE(bool, b, "bool", v)
DEFINE_GET_ATTR_VALUE(int64_t, i, "int", v)
DEFINE_GET_ATTR_VALUE(int32_t, i, "int", v)
DEFINE_GET_ATTR_VALUE(float, f, "float", v)
DEFINE_GET_ATTR_VALUE(double, f, "float", v)
#undef DEFINE_GET_ATTR_VALUE

}  // namespace visionai
