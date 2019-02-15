/*
    Copyright (c) 2019 Justin Collier
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file monitor.cc
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides the templimiter::daemon::Monitor class
 * @date created 2019-02-09
 * @date modified 2019-02-14
 */

#include "templimiter/daemon/monitor.h"

#include <algorithm>
#include <chrono>
#include <limits>
#include <memory>
#include <numeric>
#include <thread>
#include <vector>

#include "templimiter/daemon/config.h"
#include "templimiter/daemon/logger.h"
#include "templimiter/daemon/pid.h"
#include "templimiter/error/error.h"
#include "templimiter/error/internal-error.h"
#include "templimiter/tools/type-convert.h"
#include "templimiter/tools/vector.h"

namespace templimiter {

namespace daemon {

u_long Monitor::sum_cur_cpu_time_() {
  std::vector<std::string> proc_stat_cpu =
      tools::split(cfg_->proc_stat_file()->read()[0], ' ');
  std::vector<u_long> cpu_times =
      tools::convert<u_long>(tools::subvect(proc_stat_cpu, 1, 4));
  return u_long(std::accumulate(cpu_times.begin(), cpu_times.end(), 0));
}

void Monitor::update_pids_() {
  // update processes
  for (const auto &v : pids_) {
    v->update(sum_cur_cpu_time_());
  }
  // Remove all not_a_process processes
  pids_ = tools::filter(pids_, [](const auto &v) { return v->is_a_process(); });
  self_stopped_pids_ = tools::filter(self_stopped_pids_, [](const auto &v) {
    return v->is_a_process() && v->is_self_stopped();
  });

  std::vector<std::string> files = io::ls(PROC_FILES_MATCHER_, false);

  auto number_fnames = tools::filter(files, [](const auto &v) {
    return v.find_first_not_of("0123456789") == std::string::npos;
  });

  std::vector<pid_t> pid_nums = tools::convert<pid_t>(number_fnames);

  // Add each pid to processes if it does not yet exist there.
  for (auto test_pid : pid_nums) {
    if (!std::any_of(pids_.begin(), pids_.end(), [&test_pid](const auto &v) {
          return v->pid() == test_pid;
        })) {
      // processes do not contain this pid
      pids_.push_back(std::make_shared<Pid>(cfg_, test_pid));
    }
  }
}

std::vector<std::shared_ptr<Pid>> Monitor::find_SIGSTOP_available_pids_()
    const {
  std::vector<std::shared_ptr<Pid>> available;
  for (const auto &v : pids_) {
    if (v->is_a_process() && v->is_ready() && !v->is_whitelisted() &&
        !v->is_self_stopped()) {
      available.push_back(v);
    }
  }
  return available;
}

bool Monitor::is_below_max_speed_(const std::vector<u_long> &cur_speeds) {
  std::vector<u_long> max_speeds;
  if (cfg_->use_scaling_available()) {
    for (const auto &v : cfg_->scaling_available_frequencies()) {
      max_speeds.push_back(*std::max_element(v.begin(), v.end()));
    }
  } else {
    max_speeds = cfg_->cpuinfo_max_freqs();
  }
  if (cur_speeds.size() != max_speeds.size()) {
    throw error::InternalError(
        "scaling_max_freq_files size differs from cpuinfo_max_freq_files size. "
        "This should have been prevented by the initial configuration "
        "verification.");
  }
  for (size_t i = 0; i < cur_speeds.size(); i++) {
    if (cur_speeds[i] < max_speeds[i]) {
      return true;
    }
  }
  return false;
}

bool Monitor::is_above_min_speed_(const std::vector<u_long> &cur_speeds) {
  std::vector<u_long> min_speeds;
  if (cfg_->use_scaling_available()) {
    for (const auto &v : cfg_->scaling_available_frequencies()) {
      min_speeds.push_back(*std::min_element(v.begin(), v.end()));
    }
  } else {
    min_speeds = cfg_->cpuinfo_min_freqs();
  }
  if (cur_speeds.size() != min_speeds.size()) {
    throw error::InternalError(
        "scaling_max_freq_files size differs from cpuinfo_min_freq_files size. "
        "This should have been prevented by the initial configuration "
        "verification.");
  }
  for (size_t i = 0; i < cur_speeds.size(); i++) {
    if (cur_speeds[i] > min_speeds[i]) {
      return true;
    }
  }
  return false;
}

void Monitor::send_next_SIGCONT_() {
  // Sort by pid_cpu_pct descending
  if (self_stopped_pids_.size() > 0) {
    auto min_available =
        std::min_element(self_stopped_pids_.begin(), self_stopped_pids_.end(),
                         [](const auto &a, const auto &b) {
                           return a->pid_cpu_pct() < b->pid_cpu_pct();
                         });
    auto pid_ptr = *min_available;
    out_->log("Sending SIGCONT to pid " + pid_ptr->pid_str() + " " +
              pid_ptr->comm());
    pid_ptr->send_SIGCONT();
  }
}

void Monitor::send_next_SIGSTOP_(
    const std::vector<std::shared_ptr<Pid>> &available_pids) {
  auto max_available =
      std::max_element(available_pids.begin(), available_pids.end(),
                       [](const auto &a, const auto &b) {
                         return a->pid_cpu_pct() > b->pid_cpu_pct();
                       });
  if (max_available != available_pids.end()) {
    auto pid_ptr = *max_available;
    out_->log("Sending SIGSTOP to pid " + pid_ptr->pid_str() + " " +
              pid_ptr->comm());
    pid_ptr->send_SIGSTOP();
    self_stopped_pids_.push_back(pid_ptr);
  }
}

void Monitor::send_all_SIGCONTs_() {
  for (const auto &v : self_stopped_pids_) {
    v->send_SIGCONT();
  }
}

void Monitor::send_all_SIGSTOPs_(
    const std::vector<std::shared_ptr<Pid>> &available_pids) {
  for (const auto &v : available_pids) {
    v->send_SIGSTOP();
    self_stopped_pids_.push_back(v);
  }
}

void Monitor::exec_SIGCONT_() {
  if (self_stopped_pids_.size() > 0) {
    update_pids_();
    if (cfg_->use_stepwise_SIGSTOP()) {
      send_next_SIGCONT_();
    } else {
      send_all_SIGCONTs_();
    }
  }
}

void Monitor::exec_SIGSTOP_() {
  update_pids_();
  auto available_pids = find_SIGSTOP_available_pids_();
  if (available_pids.size() > 0) {
    if (cfg_->use_stepwise_SIGSTOP()) {
      send_next_SIGSTOP_(available_pids);
    } else {
      send_all_SIGSTOPs_(available_pids);
    }
  }
}

void Monitor::dethrottle_next_higher_(const std::vector<u_long> &cur_speeds) {
  std::vector<std::vector<u_long>> scaling_avail =
      cfg_->scaling_available_frequencies();
  for (size_t i = 0; i < cur_speeds.size(); i++) {
    u_long speed = cur_speeds[i];
    std::vector<u_long> avail_speeds = scaling_avail[i];
    u_long ul_max = std::numeric_limits<u_long>().max();
    u_long next = ul_max;
    for (const auto &v : avail_speeds) {
      if (v < next && v > speed) next = v;
    }
    if (next != ul_max) {
      cfg_->scaling_max_freq_files()->overwrite(i, next);
    }
  }
}

void Monitor::throttle_next_lower_(const std::vector<u_long> &cur_speeds) {
  std::vector<std::vector<u_long>> scaling_avail =
      cfg_->scaling_available_frequencies();
  for (size_t i = 0; i < cur_speeds.size(); i++) {
    u_long speed = cur_speeds[i];
    std::vector<u_long> avail_speeds = scaling_avail[i];
    u_long next = 0;
    for (const auto &v : avail_speeds) {
      if (v > next && v < speed) next = v;
    }
    if (next != 0) {
      cfg_->scaling_max_freq_files()->overwrite(i, next);
    }
  }
}

void Monitor::dethrottle_highest_() {
  std::vector<u_long> max_freqs = cfg_->cpuinfo_max_freqs();
  const auto &curfreq_files = cfg_->scaling_max_freq_files();
  for (size_t i = 0; i < curfreq_files->size(); i++) {
    curfreq_files->overwrite(i, max_freqs[i]);
  }
}

void Monitor::throttle_lowest_() {
  std::vector<u_long> min_freqs = cfg_->cpuinfo_min_freqs();
  const auto &curfreq_files = cfg_->scaling_max_freq_files();
  for (size_t i = 0; i < curfreq_files->size(); i++) {
    const auto &cur_reading = curfreq_files->read()[i];
    if (cur_reading > min_freqs[i]) {
      curfreq_files->overwrite(i, min_freqs[i]);
    }
  }
}

void Monitor::exec_dethrottle_() {
  std::vector<u_long> cur_speeds = cfg_->scaling_max_freq_files()->read();
  if (is_below_max_speed_(cur_speeds)) {
    out_->log("Dethrottling CPU.");
    if (cfg_->use_scaling_available()) {
      dethrottle_next_higher_(cur_speeds);
    } else {
      dethrottle_highest_();
    }
  }
}

void Monitor::exec_throttle_() {
  std::vector<u_long> cur_speeds = cfg_->scaling_max_freq_files()->read();
  if (is_above_min_speed_(cur_speeds)) {
    out_->log("Throttling CPU.");
    if (cfg_->use_scaling_available()) {
      throttle_next_lower_(cur_speeds);
    } else {
      throttle_lowest_();
    }
  }
}

[[noreturn]] void Monitor::run_() {
  const auto sleep = [this]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(cfg_->min_sleep()));
  };
  if (cfg_->use_throttle() && cfg_->use_SIGSTOP()) {
    // use throttle && SIGSTOP
    while (true) {
      u_long max_temp = cfg_->thermal_files()->max_line();
      if (max_temp > cfg_->temp_throttle() || max_temp > cfg_->temp_SIGSTOP()) {
        if (max_temp > cfg_->temp_throttle()) {
          exec_throttle_();
        }
        if (max_temp > cfg_->temp_SIGSTOP()) {
          exec_SIGSTOP_();
        }
      } else if (max_temp < cfg_->temp_dethrottle() ||
                 max_temp < cfg_->temp_SIGCONT()) {
        if (max_temp < cfg_->temp_dethrottle()) {
          exec_dethrottle_();
        }
        if (max_temp < cfg_->temp_SIGCONT()) {
          exec_SIGCONT_();
        }
      }
      sleep();
    }
  } else if (cfg_->use_throttle()) {
    // use throttle only
    while (true) {
      u_long max_temp = cfg_->thermal_files()->max_line();
      if (max_temp > cfg_->temp_throttle()) {
        exec_throttle_();
      } else if (max_temp < cfg_->temp_dethrottle()) {
        exec_dethrottle_();
      }
      sleep();
    }
  } else if (cfg_->use_SIGSTOP()) {
    // use SIGSTOP only
    while (true) {
      u_long max_temp = cfg_->thermal_files()->max_line();
      if (max_temp > cfg_->temp_SIGSTOP()) {
        exec_SIGSTOP_();
      } else if (max_temp < cfg_->temp_SIGCONT()) {
        exec_SIGCONT_();
      }
      sleep();
    }
  } else {
    throw error::InternalError(
        "Neither throttling nor SIGSTOP operations are enabled. This should "
        "have been prevented by the initial configuration verification.");
  }
}

[[noreturn]] Monitor::Monitor(const std::shared_ptr<Config> &cfg,
                              const std::shared_ptr<Logger> &out)
    : cfg_(cfg), out_(out) {
  run_();
}

Monitor::~Monitor() {}

}  // namespace daemon

}  // namespace templimiter
