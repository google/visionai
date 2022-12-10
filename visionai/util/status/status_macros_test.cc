// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/status/status_macros.h"

#include <grpcpp/support/status.h>

#include <memory>
#include <tuple>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "include/grpcpp/grpcpp.h"

namespace visionai {

namespace {
using ::testing::AllOf;
using ::testing::Eq;
using ::testing::HasSubstr;

absl::Status ReturnOk() { return absl::OkStatus(); }

StatusBuilder ReturnOkBuilder() { return StatusBuilder(absl::OkStatus()); }

absl::Status ReturnError(absl::string_view msg) {
  return absl::Status(absl::StatusCode::kUnknown, msg);
}

grpc::Status ReturnGrpcError(absl::string_view msg) {
  return grpc::Status(grpc::StatusCode::UNKNOWN, std::string(msg));
}

StatusBuilder ReturnErrorBuilder(absl::string_view msg) {
  return StatusBuilder(absl::Status(absl::StatusCode::kUnknown, msg));
}

absl::StatusOr<int> ReturnStatusOrValue(int v) { return v; }

absl::StatusOr<int> ReturnStatusOrError(absl::string_view msg) {
  return absl::Status(absl::StatusCode::kUnknown, msg);
}

template <class... Args>
absl::StatusOr<std::tuple<Args...>> ReturnStatusOrTupleValue(Args&&... v) {
  return std::tuple<Args...>(std::forward<Args>(v)...);
}

template <class... Args>
absl::StatusOr<std::tuple<Args...>> ReturnStatusOrTupleError(
    absl::string_view msg) {
  return absl::Status(absl::StatusCode::kUnknown, msg);
}

absl::StatusOr<std::unique_ptr<int>> ReturnStatusOrPtrValue(int v) {
  return std::make_unique<int>(v);
}

TEST(AssignOrReturn, Works) {
  auto func = []() -> absl::Status {
    VAI_ASSIGN_OR_RETURN(int value1, ReturnStatusOrValue(1));
    EXPECT_EQ(1, value1);
    VAI_ASSIGN_OR_RETURN(const int value2, ReturnStatusOrValue(2));
    EXPECT_EQ(2, value2);
    VAI_ASSIGN_OR_RETURN(const int& value3, ReturnStatusOrValue(3));
    EXPECT_EQ(3, value3);
    VAI_ASSIGN_OR_RETURN(int value4, ReturnStatusOrError("EXPECTED"));
    value4 = 0;  // fix unused errro
    return ReturnError("ERROR");
  };

  EXPECT_THAT(func().message(), Eq("EXPECTED"));
}

TEST(AssignOrReturn, WorksForGrpc) {
  auto func = []() -> grpc::Status {
    return ReturnGrpcError("ERROR");
  };

  EXPECT_THAT(func().error_message(), Eq("ERROR"));
}

TEST(AssignOrReturn, WorksWithCommasInType) {
  auto func = []() -> absl::Status {
    VAI_ASSIGN_OR_RETURN((std::tuple<int, int> t1),
                         ReturnStatusOrTupleValue(1, 1));
    EXPECT_EQ((std::tuple<int, int>{1, 1}), t1);
    VAI_ASSIGN_OR_RETURN(
        (const std::tuple<int, std::tuple<int, int>, int> t2),
        ReturnStatusOrTupleValue(1, std::tuple<int, int>{1, 1}, 1));
    EXPECT_EQ((std::tuple<int, std::tuple<int, int>, int>{
                  1, std::tuple<int, int>{1, 1}, 1}),
              t2);
    VAI_ASSIGN_OR_RETURN(
        (std::tuple<int, std::tuple<int, int>, int> t3),
        (ReturnStatusOrTupleError<int, std::tuple<int, int>, int>("EXPECTED")));
    t3 = {};  // fix unused error
    return ReturnError("ERROR");
  };

  EXPECT_THAT(func().message(), Eq("EXPECTED"));
}

TEST(AssignOrReturn, WorksWithStructureBindings) {
  auto func = []() -> absl::Status {
    VAI_ASSIGN_OR_RETURN(
        (const auto& [t1, t2, t3, t4, t5]),
        ReturnStatusOrTupleValue(std::tuple<int, int>{1, 1}, 1, 2, 3, 4));
    EXPECT_EQ((std::tuple<int, int>{1, 1}), t1);
    EXPECT_EQ(1, t2);
    EXPECT_EQ(2, t3);
    EXPECT_EQ(3, t4);
    EXPECT_EQ(4, t5);
    VAI_ASSIGN_OR_RETURN(int t6, ReturnStatusOrError("EXPECTED"));
    t6 = 0;
    return ReturnError("ERROR");
  };

  EXPECT_THAT(func().message(), Eq("EXPECTED"));
}

TEST(AssignOrReturn, WorksWithParenthesesAndDereference) {
  auto func = []() -> absl::Status {
    int integer;
    int* pointer_to_integer = &integer;
    VAI_ASSIGN_OR_RETURN((*pointer_to_integer), ReturnStatusOrValue(1));
    EXPECT_EQ(1, integer);
    VAI_ASSIGN_OR_RETURN(*pointer_to_integer, ReturnStatusOrValue(2));
    EXPECT_EQ(2, integer);
    // Make the test where the order of dereference matters and treat the
    // parentheses.
    pointer_to_integer--;
    int** pointer_to_pointer_to_integer = &pointer_to_integer;
    VAI_ASSIGN_OR_RETURN((*pointer_to_pointer_to_integer)[1],
                         ReturnStatusOrValue(3));
    EXPECT_EQ(3, integer);
    VAI_ASSIGN_OR_RETURN(int t1, ReturnStatusOrError("EXPECTED"));
    t1 = 0;
    return ReturnError("ERROR");
  };

  EXPECT_THAT(func().message(), Eq("EXPECTED"));
}

TEST(AssignOrReturn, WorksWithAppend) {
  auto fail_test_if_called = []() -> std::string {
    ADD_FAILURE();
    return "FAILURE";
  };
  auto func = [&]() -> absl::Status {
    int value;
    VAI_ASSIGN_OR_RETURN(value, ReturnStatusOrValue(1),
                         _ << fail_test_if_called());
    VAI_ASSIGN_OR_RETURN(value, ReturnStatusOrError("EXPECTED A"),
                         _ << "EXPECTED B");
    return ReturnOk();
  };

  EXPECT_THAT(func().message().data(),
              AllOf(HasSubstr("EXPECTED A"), HasSubstr("EXPECTED B")));
}

TEST(AssignOrReturn, WorksWithAdaptorFunc) {
  auto fail_test_if_called = [](StatusBuilder builder) {
    ADD_FAILURE();
    return builder;
  };
  auto adaptor = [](StatusBuilder builder) { return builder << "EXPECTED B"; };
  auto func = [&]() -> absl::Status {
    int value;
    VAI_ASSIGN_OR_RETURN(value, ReturnStatusOrValue(1), fail_test_if_called(_));
    VAI_ASSIGN_OR_RETURN(value, ReturnStatusOrError("EXPECTED A"), adaptor(_));
    return ReturnOk();
  };

  EXPECT_THAT(func().message().data(),
              AllOf(HasSubstr("EXPECTED A"), HasSubstr("EXPECTED B")));
}

TEST(AssignOrReturn, WorksWithThirdArgumentAndCommas) {
  auto fail_test_if_called = [](StatusBuilder builder) {
    ADD_FAILURE();
    return builder;
  };
  auto adaptor = [](StatusBuilder builder) { return builder << "EXPECTED B"; };
  auto func = [&]() -> absl::Status {
    VAI_ASSIGN_OR_RETURN((const auto& [t1, t2, t3]),
                         ReturnStatusOrTupleValue(1, 2, 3),
                         fail_test_if_called(_));
    EXPECT_EQ(t1, 1);
    EXPECT_EQ(t2, 2);
    EXPECT_EQ(t3, 3);
    VAI_ASSIGN_OR_RETURN(
        (const auto& [t4, t5, t6]),
        (ReturnStatusOrTupleError<int, int, int>("EXPECTED A")), adaptor(_));
    // Silence errors about the unused values.
    static_cast<void>(t4);
    static_cast<void>(t5);
    static_cast<void>(t6);
    return ReturnOk();
  };

  EXPECT_THAT(func().message().data(),
              AllOf(HasSubstr("EXPECTED A"), HasSubstr("EXPECTED B")));
}

TEST(AssignOrReturn, WorksWithAppendIncludingLocals) {
  auto func = [&](const std::string& str) -> absl::Status {
    int value;
    VAI_ASSIGN_OR_RETURN(value, ReturnStatusOrError("EXPECTED A"), _ << str);
    return ReturnOk();
  };

  EXPECT_THAT(func("EXPECTED B").message().data(),
              AllOf(HasSubstr("EXPECTED A"), HasSubstr("EXPECTED B")));
}

TEST(AssignOrReturn, WorksForExistingVariable) {
  auto func = []() -> absl::Status {
    int value = 1;
    VAI_ASSIGN_OR_RETURN(value, ReturnStatusOrValue(2));
    EXPECT_EQ(2, value);
    VAI_ASSIGN_OR_RETURN(value, ReturnStatusOrValue(3));
    EXPECT_EQ(3, value);
    VAI_ASSIGN_OR_RETURN(value, ReturnStatusOrError("EXPECTED"));
    return ReturnError("ERROR");
  };

  EXPECT_THAT(func().message(), Eq("EXPECTED"));
}

TEST(AssignOrReturn, UniquePtrWorks) {
  auto func = []() -> absl::Status {
    VAI_ASSIGN_OR_RETURN(std::unique_ptr<int> ptr, ReturnStatusOrPtrValue(1));
    EXPECT_EQ(*ptr, 1);
    return ReturnError("EXPECTED");
  };

  EXPECT_THAT(func().message(), Eq("EXPECTED"));
}

TEST(AssignOrReturn, UniquePtrWorksForExistingVariable) {
  auto func = []() -> absl::Status {
    std::unique_ptr<int> ptr;
    VAI_ASSIGN_OR_RETURN(ptr, ReturnStatusOrPtrValue(1));
    EXPECT_EQ(*ptr, 1);

    VAI_ASSIGN_OR_RETURN(ptr, ReturnStatusOrPtrValue(2));
    EXPECT_EQ(*ptr, 2);
    return ReturnError("EXPECTED");
  };

  EXPECT_THAT(func().message(), Eq("EXPECTED"));
}

TEST(ReturnIfError, Works) {
  auto func = []() -> absl::Status {
    VAI_RETURN_IF_ERROR(ReturnOk());
    VAI_RETURN_IF_ERROR(ReturnOk());
    VAI_RETURN_IF_ERROR(ReturnError("EXPECTED"));
    return ReturnError("ERROR");
  };

  EXPECT_THAT(func().message(), Eq("EXPECTED"));
}

TEST(ReturnIfError, WorksWithBuilder) {
  auto func = []() -> absl::Status {
    VAI_RETURN_IF_ERROR(ReturnOkBuilder());
    VAI_RETURN_IF_ERROR(ReturnOkBuilder());
    VAI_RETURN_IF_ERROR(ReturnErrorBuilder("EXPECTED"));
    return ReturnErrorBuilder("ERROR");
  };

  EXPECT_THAT(func().message(), Eq("EXPECTED"));
}

TEST(ReturnIfError, WorksWithLambda) {
  auto func = []() -> absl::Status {
    VAI_RETURN_IF_ERROR([] { return ReturnOk(); }());
    VAI_RETURN_IF_ERROR([] { return ReturnError("EXPECTED"); }());
    return ReturnError("ERROR");
  };

  EXPECT_THAT(func().message(), Eq("EXPECTED"));
}

TEST(ReturnIfError, WorksWithAppend) {
  auto fail_test_if_called = []() -> std::string {
    ADD_FAILURE();
    return "FAILURE";
  };
  auto func = [&]() -> absl::Status {
    VAI_RETURN_IF_ERROR(ReturnOk()) << fail_test_if_called();
    VAI_RETURN_IF_ERROR(ReturnError("EXPECTED A")) << "EXPECTED B";
    return absl::OkStatus();
  };

  EXPECT_THAT(func().message().data(),
              AllOf(HasSubstr("EXPECTED A"), HasSubstr("EXPECTED B")));
}

TEST(ReturnIfError, WorksWithVoidReturnAdaptor) {
  int code = 0;
  int phase = 0;
  auto adaptor = [&](absl::Status status) -> void { code = phase; };
  auto func = [&]() -> void {
    phase = 1;
    VAI_RETURN_IF_ERROR(ReturnOk()).With(adaptor);
    phase = 2;
    VAI_RETURN_IF_ERROR(ReturnError("EXPECTED A")).With(adaptor);
    phase = 3;
  };

  func();
  EXPECT_EQ(phase, 2);
  EXPECT_EQ(code, 2);
}

}  // namespace

}  // namespace visionai
