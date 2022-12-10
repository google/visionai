// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/status/status_builder.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/base/log_severity.h"
#include "absl/log/scoped_mock_log.h"

namespace visionai {
namespace util {
namespace {
using ::testing::_;
using ::testing::AnyOf;
using ::testing::Eq;
using ::testing::HasSubstr;
using ::testing::Pointee;
using ::testing::Test;

// We use `#line` to produce some `source_location` values pointing at various
// different (fake) files to test e.g. `VLog`, but we use it at the end of this
// file so as not to mess up the source location data for the whole file.
// Making them static data members lets us forward-declare them and define them
// at the end.
struct Locs {
  static const SourceLocation kSecret;
  static const SourceLocation kLevel0;
  static const SourceLocation kLevel1;
  static const SourceLocation kLevel2;
  static const SourceLocation kFoo;
  static const SourceLocation kBar;
};

class StatusBuilderTest : public Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}

  absl::ScopedMockLog scoped_mock_log_;
};

// Converts a StatusBuilder to a Status.
absl::Status ToStatus(const StatusBuilder& s) { return s; }

// Converts a StatusBuilder to a Status and then ignores it.
void ConvertToStatusAndIgnore(const StatusBuilder& s) {
  absl::Status status = s;
  (void)status;
}

TEST_F(StatusBuilderTest, Size) {
  EXPECT_LE(sizeof(StatusBuilder), 40)
      << "Relax this test with caution and thorough testing. If StatusBuilder "
         "is too large it can potentially blow stacks, especially in debug "
         "builds. See the comments for StatusBuilder::Rep.";
}

TEST_F(StatusBuilderTest, ExplicitSourceLocation) {
  const SourceLocation kLocation = VISIONAI_STREAMS_LOC;

  {
    const StatusBuilder builder(absl::OkStatus(), kLocation);
    EXPECT_THAT(builder.source_location().file_name(),
                Eq(kLocation.file_name()));
    EXPECT_THAT(builder.source_location().line(), Eq(kLocation.line()));
  }
}

TEST_F(StatusBuilderTest, ImplicitSourceLocation) {
  const StatusBuilder builder(absl::OkStatus());
  auto loc = VISIONAI_STREAMS_LOC;
  EXPECT_THAT(builder.source_location().file_name(),
              AnyOf(Eq(loc.file_name()), Eq("<source_location>")));
  EXPECT_THAT(builder.source_location().line(),
              AnyOf(Eq(1), Eq(loc.line() - 1)));
}

TEST_F(StatusBuilderTest, StatusCode) {
  // OK
  {
    const StatusBuilder builder(absl::StatusCode::kOk);
    EXPECT_TRUE(builder.ok());
    EXPECT_THAT(builder.code(), Eq(absl::StatusCode::kOk));
  }
  // Non-OK code
  {
    const StatusBuilder builder(absl::StatusCode::kInvalidArgument);
    EXPECT_FALSE(builder.ok());
    EXPECT_THAT(builder.code(), Eq(absl::StatusCode::kInvalidArgument));
  }
}

TEST_F(StatusBuilderTest, Streaming) {
  EXPECT_THAT(
      ToStatus(StatusBuilder(absl::CancelledError(), Locs::kFoo) << "booyah"),
      Eq(absl::CancelledError("booyah")));

  EXPECT_THAT(ToStatus(StatusBuilder(absl::AbortedError("hello"), Locs::kFoo)
                       << "world"),
              Eq(absl::AbortedError("hello; world")));
}

TEST_F(StatusBuilderTest, PrependLvalue) {
  {
    StatusBuilder builder(absl::CancelledError(), SourceLocation());
    EXPECT_THAT(ToStatus(builder.SetPrepend() << "booyah"),
                Eq(absl::CancelledError("booyah")));
  }
  {
    StatusBuilder builder(absl::AbortedError(" hello"), SourceLocation());
    EXPECT_THAT(ToStatus(builder.SetPrepend() << "world"),
                Eq(absl::AbortedError("world hello")));
  }
}

TEST_F(StatusBuilderTest, PrependRvalue) {
  EXPECT_THAT(
      ToStatus(
          StatusBuilder(absl::CancelledError(), SourceLocation()).SetPrepend()
          << "booyah"),
      Eq(absl::CancelledError("booyah")));
  EXPECT_THAT(
      ToStatus(StatusBuilder(absl::AbortedError(" hello"), SourceLocation())
                   .SetPrepend()
               << "world"),
      Eq(absl::AbortedError("world hello")));
}

TEST_F(StatusBuilderTest, AppendLvalue) {
  {
    StatusBuilder builder(absl::CancelledError(), SourceLocation());
    EXPECT_THAT(ToStatus(builder.SetAppend() << "booyah"),
                Eq(absl::CancelledError("booyah")));
  }
  {
    StatusBuilder builder(absl::AbortedError("hello"), SourceLocation());
    EXPECT_THAT(ToStatus(builder.SetAppend() << " world"),
                Eq(absl::AbortedError("hello world")));
  }
}

TEST_F(StatusBuilderTest, AppendRvalue) {
  EXPECT_THAT(
      ToStatus(
          StatusBuilder(absl::CancelledError(), SourceLocation()).SetAppend()
          << "booyah"),
      Eq(absl::CancelledError("booyah")));
  EXPECT_THAT(
      ToStatus(StatusBuilder(absl::AbortedError("hello"), SourceLocation())
                   .SetAppend()
               << " world"),
      Eq(absl::AbortedError("hello world")));
}

TEST_F(StatusBuilderTest, LogToMultipleErrorLevelsLvalue) {
  EXPECT_CALL(scoped_mock_log_,
              Log(absl::LogSeverity::kWarning,
                  HasSubstr(Locs::kSecret.file_name()), HasSubstr("no!")))
      .Times(1);
  EXPECT_CALL(scoped_mock_log_,
              Log(absl::LogSeverity::kError,
                  HasSubstr(Locs::kSecret.file_name()), HasSubstr("yes!")))
      .Times(1);
  EXPECT_CALL(scoped_mock_log_,
              Log(absl::LogSeverity::kInfo,
                  HasSubstr(Locs::kSecret.file_name()), HasSubstr("Oui!")))
      .Times(1);

  scoped_mock_log_.StartCapturingLogs();
  {
    StatusBuilder builder(absl::CancelledError(), Locs::kSecret);
    ConvertToStatusAndIgnore(builder.Log(absl::LogSeverity::kWarning) << "no!");
  }
  {
    StatusBuilder builder(absl::AbortedError(""), Locs::kSecret);
    ConvertToStatusAndIgnore(builder.Log(absl::LogSeverity::kError) << "yes!");

    // This one shouldn't log because vlogging is disabled.
    absl::SetFlag(&FLAGS_v, 0);
    ConvertToStatusAndIgnore(builder.VLog(2) << "Non!");

    absl::SetFlag(&FLAGS_v, 2);
    ConvertToStatusAndIgnore(builder.VLog(2) << "Oui!");
  }
}

TEST_F(StatusBuilderTest, LogToMultipleErrorLevelsRvalue) {
  EXPECT_CALL(scoped_mock_log_,
              Log(absl::LogSeverity::kWarning,
                  HasSubstr(Locs::kSecret.file_name()), HasSubstr("no!")))
      .Times(1);
  EXPECT_CALL(scoped_mock_log_,
              Log(absl::LogSeverity::kError,
                  HasSubstr(Locs::kSecret.file_name()), HasSubstr("yes!")))
      .Times(1);
  EXPECT_CALL(scoped_mock_log_,
              Log(absl::LogSeverity::kInfo,
                  HasSubstr(Locs::kSecret.file_name()), HasSubstr("Oui!")))
      .Times(1);

  scoped_mock_log_.StartCapturingLogs();
  ConvertToStatusAndIgnore(StatusBuilder(absl::CancelledError(), Locs::kSecret)
                               .Log(absl::LogSeverity::kWarning)
                           << "no!");
  ConvertToStatusAndIgnore(StatusBuilder(absl::AbortedError(""), Locs::kSecret)
                               .Log(absl::LogSeverity::kError)
                           << "yes!");
  // This one shouldn't log because vlogging is disabled.
  absl::SetFlag(&FLAGS_v, 0);
  ConvertToStatusAndIgnore(
      StatusBuilder(absl::AbortedError(""), Locs::kSecret).VLog(2) << "Non!");
  absl::SetFlag(&FLAGS_v, 2);
  ConvertToStatusAndIgnore(
      StatusBuilder(absl::AbortedError(""), Locs::kSecret).VLog(2) << "Oui!");
}

TEST_F(StatusBuilderTest, LogEveryNFirstLogs) {
  EXPECT_CALL(scoped_mock_log_,
              Log(absl::LogSeverity::kWarning,
                  HasSubstr(Locs::kSecret.file_name()), HasSubstr("no!")))
      .Times(1);

  scoped_mock_log_.StartCapturingLogs();

  StatusBuilder builder(absl::CancelledError(), Locs::kSecret);
  ConvertToStatusAndIgnore(builder.LogEveryN(absl::LogSeverity::kWarning, 3)
                           << "no!");
}

TEST_F(StatusBuilderTest, LogEveryN2Lvalue) {
  EXPECT_CALL(scoped_mock_log_,
              Log(absl::LogSeverity::kWarning,
                  HasSubstr(Locs::kSecret.file_name()), HasSubstr("no!")))
      .Times(3);

  scoped_mock_log_.StartCapturingLogs();

  StatusBuilder builder(absl::CancelledError(), Locs::kSecret);
  // Only 3 of the 6 should log.
  for (int i = 0; i < 6; ++i) {
    ConvertToStatusAndIgnore(builder.LogEveryN(absl::LogSeverity::kWarning, 2)
                             << "no!");
  }
}

TEST_F(StatusBuilderTest, LogEveryN3Lvalue) {
  EXPECT_CALL(scoped_mock_log_,
              Log(absl::LogSeverity::kWarning,
                  HasSubstr(Locs::kSecret.file_name()), HasSubstr("no!")))
      .Times(2);

  scoped_mock_log_.StartCapturingLogs();

  StatusBuilder builder(absl::CancelledError(), Locs::kSecret);
  // Only 2 of the 6 should log.
  for (int i = 0; i < 6; ++i) {
    ConvertToStatusAndIgnore(builder.LogEveryN(absl::LogSeverity::kWarning, 3)
                             << "no!");
  }
}

TEST_F(StatusBuilderTest, LogEveryN7Lvalue) {
  EXPECT_CALL(scoped_mock_log_,
              Log(absl::LogSeverity::kWarning,
                  HasSubstr(Locs::kSecret.file_name()), HasSubstr("no!")))
      .Times(3);

  scoped_mock_log_.StartCapturingLogs();

  StatusBuilder builder(absl::CancelledError(), Locs::kSecret);
  // Only 3 of the 20 should log.
  for (int i = 0; i < 20; ++i) {
    ConvertToStatusAndIgnore(builder.LogEveryN(absl::LogSeverity::kWarning, 7)
                             << "no!");
  }
}

TEST_F(StatusBuilderTest, LogEveryNRvalue) {
  EXPECT_CALL(scoped_mock_log_,
              Log(absl::LogSeverity::kWarning,
                  HasSubstr(Locs::kSecret.file_name()), HasSubstr("no!")))
      .Times(2);

  scoped_mock_log_.StartCapturingLogs();

  // Only 2 of the 4 should log.
  for (int i = 0; i < 4; ++i) {
    ConvertToStatusAndIgnore(
        StatusBuilder(absl::CancelledError(), Locs::kSecret)
            .LogEveryN(absl::LogSeverity::kWarning, 2)
        << "no!");
  }
}

TEST_F(StatusBuilderTest, LogIncludesFileAndLine) {
  EXPECT_CALL(scoped_mock_log_,
              Log(absl::LogSeverity::kWarning, HasSubstr("/foo/secret.cc"),
                  HasSubstr("maybe?")))
      .Times(1);

  scoped_mock_log_.StartCapturingLogs();

  ConvertToStatusAndIgnore(StatusBuilder(absl::AbortedError(""), Locs::kSecret)
                               .Log(absl::LogSeverity::kWarning)
                           << "maybe?");
}

TEST_F(StatusBuilderTest, NoLoggingLvalue) {
  EXPECT_CALL(scoped_mock_log_, Log(_, _, _)).Times(0);

  scoped_mock_log_.StartCapturingLogs();

  {
    StatusBuilder builder(absl::AbortedError(""), Locs::kSecret);
    EXPECT_THAT(ToStatus(builder << "nope"), Eq(absl::AbortedError("nope")));
  }
  {
    StatusBuilder builder(absl::AbortedError(""), Locs::kSecret);
    // Enable and then disable logging.
    EXPECT_THAT(ToStatus(builder.Log(absl::LogSeverity::kWarning).SetNoLogging()
                         << "not at all"),
                Eq(absl::AbortedError("not at all")));
  }
}

TEST_F(StatusBuilderTest, NoLoggingRvalue) {
  EXPECT_CALL(scoped_mock_log_, Log(_, _, _)).Times(0);

  scoped_mock_log_.StartCapturingLogs();

  EXPECT_THAT(
      ToStatus(StatusBuilder(absl::AbortedError(""), Locs::kSecret) << "nope"),
      Eq(absl::AbortedError("nope")));
  // Enable and then disable logging.
  EXPECT_THAT(ToStatus(StatusBuilder(absl::AbortedError(""), Locs::kSecret)
                           .Log(absl::LogSeverity::kWarning)
                           .SetNoLogging()
                       << "not at all"),
              Eq(absl::AbortedError("not at all")));
}

TEST_F(StatusBuilderTest, EmitStackTracePlusSomethingLikelyUniqueLvalue) {
  EXPECT_CALL(scoped_mock_log_,
              Log(absl::LogSeverity::kError, HasSubstr("/bar/baz.cc"),
                  HasSubstr("EmitStackTracePlusSomethingLikelyUniqueLvalue")))
      .Times(1);

  scoped_mock_log_.StartCapturingLogs();

  StatusBuilder builder(absl::AbortedError(""), Locs::kBar);
  ConvertToStatusAndIgnore(builder.LogError().EmitStackTrace() << "maybe?");
}

TEST_F(StatusBuilderTest, EmitStackTracePlusSomethingLikelyUniqueRvalue) {
  EXPECT_CALL(scoped_mock_log_,
              Log(absl::LogSeverity::kError, HasSubstr("/bar/baz.cc"),
                  HasSubstr("EmitStackTracePlusSomethingLikelyUniqueRvalue")))
      .Times(1);

  scoped_mock_log_.StartCapturingLogs();

  ConvertToStatusAndIgnore(StatusBuilder(absl::AbortedError(""), Locs::kBar)
                               .LogError()
                               .EmitStackTrace()
                           << "maybe?");
}

TEST_F(StatusBuilderTest, WithRvalueRef) {
  auto policy = [](StatusBuilder sb) { return sb << "policy"; };
  EXPECT_THAT(ToStatus(StatusBuilder(absl::AbortedError("hello"), Locs::kLevel0)
                           .With(policy)),
              Eq(absl::AbortedError("hello; policy")));
}

TEST_F(StatusBuilderTest, WithRef) {
  auto policy = [](StatusBuilder sb) { return sb << "policy"; };
  StatusBuilder sb(absl::AbortedError("zomg"), Locs::kLevel1);
  EXPECT_THAT(ToStatus(sb.With(policy)),
              Eq(absl::AbortedError("zomg; policy")));
}

TEST_F(StatusBuilderTest, WithTypeChange) {
  auto policy = [](StatusBuilder sb) -> std::string {
    return sb.ok() ? "true" : "false";
  };
  EXPECT_EQ(
      StatusBuilder(absl::CancelledError(), SourceLocation()).With(policy),
      "false");
  EXPECT_EQ(StatusBuilder(absl::OkStatus(), SourceLocation()).With(policy),
            "true");
}

TEST_F(StatusBuilderTest, WithVoidTypeAndSideEffects) {
  absl::StatusCode code;
  auto policy = [&code](absl::Status status) { code = status.code(); };
  StatusBuilder(absl::CancelledError(), SourceLocation()).With(policy);
  EXPECT_EQ(absl::StatusCode::kCancelled, code);
  StatusBuilder(absl::OkStatus(), SourceLocation()).With(policy);
  EXPECT_EQ(absl::StatusCode::kOk, code);
}

struct MoveOnlyAdaptor {
  std::unique_ptr<int> value;
  std::unique_ptr<int> operator()(const absl::Status&) && {
    return std::move(value);
  }
};

TEST_F(StatusBuilderTest, WithMoveOnlyAdaptor) {
  StatusBuilder sb(absl::AbortedError("zomg"), SourceLocation());
  EXPECT_THAT(sb.With(MoveOnlyAdaptor{std::make_unique<int>(100)}),
              Pointee(100));
  EXPECT_THAT(StatusBuilder(absl::AbortedError("zomg"), SourceLocation())
                  .With(MoveOnlyAdaptor{std::make_unique<int>(100)}),
              Pointee(100));
}

template <typename T>
std::string ToStringViaStream(const T& x) {
  std::ostringstream os;
  os << x;
  return os.str();
}

TEST_F(StatusBuilderTest, StreamInsertionOperator) {
  absl::Status status = absl::AbortedError("zomg");
  StatusBuilder builder(status, SourceLocation());
  EXPECT_EQ(ToStringViaStream(status), ToStringViaStream(builder));
  EXPECT_EQ(ToStringViaStream(status),
            ToStringViaStream(StatusBuilder(status, SourceLocation())));
}

TEST(WithExtraMessagePolicyTest, AppendsToExtraMessage) {
  // The policy simply calls operator<< on the builder; the following examples
  // demonstrate that, without duplicating all of the above tests.
  EXPECT_THAT(ToStatus(StatusBuilder(absl::AbortedError("hello"), Locs::kLevel2)
                           .With(ExtraMessage("world"))),
              Eq(absl::AbortedError("hello; world")));
  EXPECT_THAT(ToStatus(StatusBuilder(absl::AbortedError("hello"), Locs::kLevel2)
                           .With(ExtraMessage() << "world")),
              Eq(absl::AbortedError("hello; world")));
  EXPECT_THAT(ToStatus(StatusBuilder(absl::AbortedError("hello"), Locs::kLevel2)
                           .With(ExtraMessage("world"))
                           .With(ExtraMessage("!"))),
              Eq(absl::AbortedError("hello; world!")));
  EXPECT_THAT(ToStatus(StatusBuilder(absl::AbortedError("hello"), Locs::kLevel2)
                           .With(ExtraMessage("world, "))
                           .SetPrepend()),
              Eq(absl::AbortedError("world, hello")));

  // The above examples use temporary StatusBuilder rvalues; verify things also
  // work fine when StatusBuilder is an lvalue.
  StatusBuilder builder(absl::AbortedError("hello"), Locs::kLevel2);
  EXPECT_THAT(
      ToStatus(builder.With(ExtraMessage("world")).With(ExtraMessage("!"))),
      Eq(absl::AbortedError("hello; world!")));
}

#line 1337 "/foo/secret.cc"
const SourceLocation Locs::kSecret = SourceLocation::current();
#line 1234 "/tmp/level0.cc"
const SourceLocation Locs::kLevel0 = SourceLocation::current();
#line 1234 "/tmp/level1.cc"
const SourceLocation Locs::kLevel1 = SourceLocation::current();
#line 1234 "/tmp/level2.cc"
const SourceLocation Locs::kLevel2 = SourceLocation::current();
#line 1337 "/foo/foo.cc"
const SourceLocation Locs::kFoo = SourceLocation::current();
#line 1337 "/bar/baz.cc"
const SourceLocation Locs::kBar = SourceLocation::current();
}  // namespace
}  // namespace util
}  // namespace visionai
