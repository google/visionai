// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/status/status_builder.h"

#include "absl/base/log_severity.h"
#include "absl/container/flat_hash_map.h"
#include "absl/strings/str_format.h"
#include "absl/synchronization/mutex.h"
#include "visionai/util/examine_stack.h"

namespace visionai {

StatusBuilder::StatusBuilder() {}

StatusBuilder::Rep::Rep(const absl::Status& s) : status(s) {}
StatusBuilder::Rep::Rep(absl::Status&& s) : status(std::move(s)) {}
StatusBuilder::Rep::~Rep() {}

StatusBuilder::Rep::Rep(const Rep& r)
    : status(r.status),
      logging_mode(r.logging_mode),
      log_severity(r.log_severity),
      verbose_level(r.verbose_level),
      n(r.n),
      period(r.period),
      stream_message(r.stream_message),
      stream(&stream_message),
      should_log_stack_trace(r.should_log_stack_trace),
      message_join_style(r.message_join_style) {}

absl::Status StatusBuilder::JoinMessageToStatus(absl::Status s,
                                                absl::string_view msg,
                                                MessageJoinStyle style) {
  if (s.ok() || msg.empty()) return s;

  std::string new_msg;
  if (style == MessageJoinStyle::kAnnotate) {
    std::string formatted_msg{msg};
    if (!s.message().empty()) {
      new_msg = absl::StrFormat("%s; %s", s.message(), formatted_msg);
    } else {
      new_msg = formatted_msg;
    }
  } else if (style == MessageJoinStyle::kPrepend) {
    new_msg = absl::StrCat(msg, s.message());
  } else {
    new_msg = absl::StrCat(s.message(), msg);
  }
  return absl::Status(s.code(), new_msg);
}

void StatusBuilder::ConditionallyLog(const absl::Status& status) const {
  if (rep_->logging_mode == Rep::LoggingMode::kDisabled) return;

  absl::LogSeverity severity = rep_->log_severity;
  switch (rep_->logging_mode) {
    case Rep::LoggingMode::kVLog: {
      if (!VLOG_IS_ON(rep_->verbose_level)) {
        return;
      }
      severity = absl::LogSeverity::kInfo;
      break;
    }
    case Rep::LoggingMode::kDisabled:
    case Rep::LoggingMode::kLog:
      break;
    case Rep::LoggingMode::kLogEveryN: {
      {
        struct LogSites {
          absl::Mutex mutex;
          absl::flat_hash_map<std::pair<const void*, uint>, uint>
              counts_by_file_and_line ABSL_GUARDED_BY(mutex);
        };
        static auto* log_every_n_sites = new LogSites();

        log_every_n_sites->mutex.Lock();
        const uint count =
            log_every_n_sites
                ->counts_by_file_and_line[{loc_.file_name(), loc_.line()}]++;
        log_every_n_sites->mutex.Unlock();

        if (count % rep_->n != 0) {
          return;
        }
        break;
      }
    }
  }
  const std::string maybe_stack_trace =
      rep_->should_log_stack_trace
          ? absl::StrCat(" ", visionai::CurrentStackTrace())
          : "";
  google::LogMessage(loc_.file_name(), loc_.line(),
    static_cast<int>(severity)).stream() << status << maybe_stack_trace;
}

absl::Status StatusBuilder::CreateStatusAndConditionallyLog() && {
  absl::Status result = JoinMessageToStatus(
      std::move(rep_->status), rep_->stream_message, rep_->message_join_style);
  ConditionallyLog(result);
  // We consumed the status above, we set it to some error just to prevent
  // people relying on it become OK or something.
  rep_->status = absl::UnknownError("");
  rep_ = nullptr;
  return result;
}

std::ostream& operator<<(std::ostream& os, const StatusBuilder& builder) {
  return os << static_cast<absl::Status>(builder);
}

std::ostream& operator<<(std::ostream& os, StatusBuilder&& builder) {
  return os << static_cast<absl::Status>(std::move(builder));
}
}  // namespace visionai
