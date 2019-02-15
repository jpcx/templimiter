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
 * @file pid.cc
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides the templimiter::daemon::Pid class
 * @date created 2019-02-09
 * @date modified 2019-02-14
 */

#include "templimiter/daemon/pid.h"

#include <signal.h>
#include <memory>
#include <string>
#include <vector>

#include "templimiter/daemon/config.h"
#include "templimiter/daemon/logger.h"
#include "templimiter/error/error.h"
#include "templimiter/error/internal-error.h"
#include "templimiter/io/file.h"
#include "templimiter/tools/type-convert.h"
#include "templimiter/tools/vector.h"

namespace templimiter {

namespace daemon {

void Pid::assert_is_ready_() const {
  if (!is_ready_) {
    throw error::InternalError(
        "Attempted to access cpu_pct before cpu_pct was calculated.");
  }
}

void Pid::read_stat_() {
  std::vector<std::string> stat_lines;
  try {
    stat_lines = stat_->read();
    if (stat_lines.size() > 1) {
      throw error::InternalError("/proc/" + pid_str_ +
                                 "/stat file is longer than one line!");
    }
    std::vector<std::string> stat_data = tools::split(stat_lines[0], ' ');
    comm_ = stat_data[1];
    state_ = tools::convert<char>(stat_data[2]);
    ppid_ = tools::convert<pid_t>(stat_data[3]);
    pgrp_ = tools::convert<int>(stat_data[4]);
    session_ = tools::convert<int>(stat_data[5]);
    tty_nr_ = tools::convert<int>(stat_data[6]);
    tpgid_ = tools::convert<int>(stat_data[7]);
    flags_ = tools::convert<uint>(stat_data[8]);
    utime_ = tools::convert<u_long>(stat_data[13]);
    stime_ = tools::convert<u_long>(stat_data[14]);
    cutime_ = tools::convert<u_long>(stat_data[15]);
    cstime_ = tools::convert<u_long>(stat_data[16]);
    nice_ = tools::convert<long>(stat_data[18]);
    is_a_process_ = true;
  } catch (const error::Error &e) {
    if (e.name() == "IOError") {
      is_a_process_ = false;
    } else {
      throw;
    }
  }
}

void Pid::check_whitelist_() {
  // out_->debug("Checking whitelist for pid " + pid_str_ + "...");
  is_whitelisted_ =
      (nice_ < cfg_->whitelist_max_nice() ||
       tools::contains(cfg_->whitelist_pid(), pid_) ||
       tools::contains(cfg_->whitelist_state(), state_) ||
       tools::contains(cfg_->whitelist_ppid(), ppid_) ||
       tools::contains(cfg_->whitelist_pgrp(), pgrp_) ||
       tools::contains(cfg_->whitelist_session(), session_) ||
       tools::contains(cfg_->whitelist_tty_nr(), tty_nr_) ||
       tools::contains(cfg_->whitelist_tpgid(), tpgid_) ||
       tools::contains(cfg_->whitelist_flags(), flags_) ||
       tools::pattern_vect_contains(cfg_->whitelist_comm(), comm_));
}

Pid::Pid(const std::shared_ptr<daemon::Config> &cfg, pid_t pid)
    : cfg_(cfg),
      pid_(pid),
      pid_str_(tools::to_string(pid)),
      stat_(std::make_shared<io::File<std::string>>("/proc/" + pid_str_ +
                                                    "/stat")) {
  read_stat_();
  check_whitelist_();
}

Pid::~Pid() {}

void Pid::update(u_long cpu_time) {
  read_stat_();
  check_whitelist_();
  if (!is_whitelisted_) {
    if (!has_received_first_update_) {
      pid_time_prev_ = utime_ + stime_ + cutime_ + cstime_;
      cpu_time_prev_ = cpu_time;
      has_received_first_update_ = true;
    } else {
      u_long pid_time = utime_ + stime_ + cutime_ + cstime_;
      float pid_time_diff = pid_time - pid_time_prev_;
      float cpu_time_diff = cpu_time - cpu_time_prev_;
      pid_cpu_pct_ = pid_time_diff / cpu_time_diff;
      pid_time_prev_ = pid_time;
      cpu_time_prev_ = cpu_time;
      if (!is_ready_) is_ready_ = true;
    }
  } else {
    pid_time_prev_ = 0;
    cpu_time_prev_ = 0;
    pid_cpu_pct_ = 0;
    has_received_first_update_ = false;
  }
}

pid_t Pid::pid() const { return pid_; }
const std::string &Pid::pid_str() const { return pid_str_; }
bool Pid::is_a_process() const { return is_a_process_; }
bool Pid::is_whitelisted() const { return is_whitelisted_; }
bool Pid::is_ready() const { return is_ready_; }
bool Pid::is_self_stopped() const { return is_self_stopped_; }
const std::string &Pid::comm() const { return comm_; }
char Pid::state() const { return state_; }
pid_t Pid::ppid() const { return ppid_; }
int Pid::pgrp() const { return pgrp_; }
int Pid::session() const { return session_; }
int Pid::tty_nr() const { return tty_nr_; }
int Pid::tpgid() const { return tpgid_; }
uint Pid::flags() const { return flags_; }
u_long Pid::utime() const { return utime_; }
u_long Pid::stime() const { return stime_; }
u_long Pid::cutime() const { return cutime_; }
u_long Pid::cstime() const { return cstime_; }
long Pid::nice() const { return nice_; }
float Pid::pid_cpu_pct() const {
  assert_is_ready_();
  return pid_cpu_pct_;
}

void Pid::send_SIGSTOP() {
  kill(pid_, 19);
  is_self_stopped_ = true;
}

void Pid::send_SIGCONT() {
  kill(pid_, 18);
  is_self_stopped_ = false;
}

}  // namespace daemon

}  // namespace templimiter
