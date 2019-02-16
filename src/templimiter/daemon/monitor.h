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
 * @file monitor.h
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides the templimiter::daemon::Monitor class
 * @date created 2019-02-09
 * @date modified 2019-02-15
 */

#pragma once

#include <memory>
#include <vector>

#include "templimiter/daemon/config.h"
#include "templimiter/daemon/logger.h"
#include "templimiter/daemon/pid.h"

namespace templimiter {

namespace daemon {

/** @brief Continuously monitors temperature and executes responses */
class Monitor {
 private:
  /** @brief Matcher used for searching /proc directory */
  const std::string PROC_FILES_MATCHER_ = "/proc/*";

  /** @brief Shared pointer to the execution configuration */
  std::shared_ptr<Config> cfg_;

  /** @brief Shared pointer to the execution logger */
  std::shared_ptr<Logger> out_;

  /** @brief Vector of Pid shared pointers */
  std::vector<std::shared_ptr<Pid>> pids_;

  /** @brief Vector of all Pid objects that have been sent SIGSTOP */
  std::vector<std::shared_ptr<Pid>> self_stopped_pids_;

  /**
   * @brief Number of iterations to wait before throttling again when
   * throttling is not behaving as expected
   */
  u_short unexpected_frequency_cooldown_ = 20;

  /**
   * @brief Tracks the number of iterations spent cooling down after an 
   * unexpected frequency encounter
   */
  u_short cooldown_ct_ = 0;

  /**
   * @brief Tracks the expected values of the scaling_max_freq files based on 
   * throttling actions in order to determine whether or not the files are 
   * being modified elsewhere
   */
  std::vector<u_long> expected_frequencies_;

  /**
   * @brief Whether or not the program has found a cpu frequency reading that
   * is not as expected (based on previous throttle actions) and is waiting a
   * few iterations
   */
  bool found_unexpected_frequency_ = false;

  /**
   * @brief Sums the current cpu time and returns it
   * @return u_long
   */
  u_long sum_cur_cpu_time_();

  /** @brief Updates the pids_ using current information from /proc/ */
  void update_pids_();

  /**
   * @brief Returns a vector of shared pointers to Pid objects that can be sent
   * SIGSTOP signals
   *
   * @return std::vector<std::shared_ptr<Pid>>
   */
  std::vector<std::shared_ptr<Pid>> find_SIGSTOP_available_pids_() const;

  /**
   * @brief Checks whether or not any cpu is currently throttled
   *
   * @param cur_speeds Vector of current CPU speeds
   * @return true if any cpu is throttled
   * @return false if no cpu is throttled
   */
  bool is_below_max_speed_(const std::vector<u_long> &cur_speeds);

  /**
   * @brief Checks whether or not any cpu is currently clocked higher than
   * minimum
   *
   * @param cur_speeds Vector of current CPU speeds
   * @return true if any cpu is throttled
   * @return false if no cpu is throttled
   */
  bool is_above_min_speed_(const std::vector<u_long> &cur_speeds);

  /**
   * @brief Sends a SIGCONT signal to the next lowest-consuming non-whitelisted
   * process
   */
  void send_next_SIGCONT_();

  /**
   * @brief Sends a SIGSTOP signal to the next highest-consuming non-whitelisted
   * process
   *
   * @param available_pids Vector of pointers to Pids that can be sent SIGSTOP
   * signals
   */
  void send_next_SIGSTOP_(
      const std::vector<std::shared_ptr<Pid>> &available_pids);

  /** @brief Sends a SIGCONT signal to all self stopped processes */
  void send_all_SIGCONTs_();

  /**
   * @brief Sends a SIGSTOP signal to all non-whitelisted processes
   *
   * @param available_pids Vector of pointers to Pids that can be sent SIGSTOP
   * signals
   */
  void send_all_SIGSTOPs_(
      const std::vector<std::shared_ptr<Pid>> &available_pids);

  /** @brief Performs the SIGCONT operation based on the configuration */
  void exec_SIGCONT_();

  /** @brief Performs the SIGSTOP operation based on the configuration */
  void exec_SIGSTOP_();

  /**
   * @brief Dethrottles each cpu to the next available higher scaling_avail freq
   *
   * @param cur_speeds Vector of current CPU speeds
   */
  void dethrottle_next_higher_(const std::vector<u_long> &cur_speeds);

  /**
   * @brief Throttles each cpu to the next available lower scaling_avail freq
   *
   * @param cur_speeds Vector of current CPU speeds
   */
  void throttle_next_lower_(const std::vector<u_long> &cur_speeds);

  /** 
   * @brief Dethrottles each cpu to the highest possible frequency
   * 
   * @param cur_speeds Vector of current CPU speeds
   */
  void dethrottle_highest_(const std::vector<u_long> &cur_speeds);

  /** 
   * @brief Throttles each cpu to the lowest possible frequency
   * 
   * @param cur_speeds Vector of current CPU speeds
   */
  void throttle_lowest_(const std::vector<u_long> &cur_speeds);

  /** @brief Performs the dethrottle operation based on the configuration */
  void exec_dethrottle_();

  /** @brief Performs the throttle operation based on the configuration */
  void exec_throttle_();

  /**
   * @brief Loops forever until an error is thrown, checking temperature and
   * executing responses
   */
  [[noreturn]] void run_();

 public:
  /**
   * @brief Construct a new Monitor object
   *
   * @param cfg Shared pointer to the execution configuration
   * @param out Shared pointer to the execution logger
   */
  [[noreturn]] Monitor(const std::shared_ptr<Config> &cfg,
                       const std::shared_ptr<Logger> &out);

  /**
   * @brief Destroy the Monitor object
   */
  ~Monitor();
};

}  // namespace daemon

}  // namespace templimiter
