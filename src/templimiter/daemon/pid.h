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
 * @file pid.h
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides the templimiter::daemon::Pid class
 * @date created 2019-02-09
 * @date modified 2019-02-14
 */

#pragma once

#include <memory>
#include <string>

#include "templimiter/daemon/config.h"
#include "templimiter/daemon/logger.h"
#include "templimiter/io/file.h"

namespace templimiter {

namespace daemon {

/**
 * @brief Provides an interface for system processes in order to read
 * information, calculate cpu usage, and send signals
 */
class Pid {
 private:
  /** @brief Execution configuration */
  std::shared_ptr<daemon::Config> cfg_;

  /** @brief Execution logger */
  std::shared_ptr<daemon::Logger> out_;

  /** @brief Pid associated with the object */
  pid_t pid_;

  /** @brief String representation of the pid */
  std::string pid_str_;

  /** @brief Stat file pointer */
  std::shared_ptr<io::File<std::string>> stat_;

  /** @brief Whether or not the stat file exists */
  bool is_a_process_ = false;

  /** @brief Whether or not the process is whitelisted */
  bool is_whitelisted_ = false;

  /**
   * @brief Whether or not the process has received its first cpu time update
   */
  bool has_received_first_update_ = false;

  /** @brief Whether or not the process has calculated its first cpu_pct */
  bool is_ready_ = false;

  /** @brief Whether or not the Pid has been sent SIGSTOP */
  bool is_self_stopped_ = false;

  /** @brief comm value of the pid */
  std::string comm_;

  /** @brief state value of the pid */
  char state_;

  /** @brief ppid value of the pid */
  pid_t ppid_;

  /** @brief pgrp value of the pid */
  int pgrp_;

  /** @brief session value of the pid */
  int session_;

  /** @brief tty_nr value of the pid */
  int tty_nr_;

  /** @brief tpgid value of the pid */
  int tpgid_;

  /** @brief flags value of the pid */
  uint flags_;

  /** @brief utime value of the pid */
  u_long utime_;

  /** @brief stime value of the pid */
  u_long stime_;

  /** @brief cutime value of the pid */
  u_long cutime_;

  /** @brief cstime value of the pid */
  u_long cstime_;

  /** @brief nice value of the pid */
  long nice_;

  /** @brief Previously retrieved pid process time */
  u_long pid_time_prev_;

  /** @brief Previously provided CPU process time */
  u_long cpu_time_prev_;

  /** @brief Calculated CPU usage */
  float pid_cpu_pct_;

  /**
   * @brief Asserts that the Pid object has calculated its first cpu_pct
   * @throw templimiter::error::InternalError if object is not ready
   */
  void assert_is_ready_() const;

  /**
   * @brief Reads the stat file and loads internal variables
   * @throws templimiter::error::InternalError if stat file is longer than one
   * line
   */
  void read_stat_();

  /** @brief Checks the pid properties against the whitelist */
  void check_whitelist_();

 public:
  /**
   * @brief Construct a new Pid object
   *
   * @param cfg Execution configuration
   * @param pid Pid associated with object
   */
  Pid(const std::shared_ptr<daemon::Config> &cfg, pid_t pid);

  /** @brief Destroy the Pid object */
  ~Pid();

  /**
   * @brief Updates the pid with new cpu time information
   *
   * @param cpu_time Total cpu time spent processing information
   */
  void update(u_long cpu_time);

  /**
   * @brief Returns the pid
   * @return pid_t
   */
  pid_t pid() const;

  /**
   * @brief Returns the pid string
   * @return const std::string&
   */
  const std::string &pid_str() const;

  /**
   * @brief Returns whether or not the stat file exists
   * @return true if stat file exists
   * @return false if stat file does not exist
   */
  bool is_a_process() const;

  /**
   * @brief Returns whether or not the Pid is whitelisted
   * @return true if whitelisted
   * @return false if not whitelisted
   */
  bool is_whitelisted() const;

  /**
   * @brief Returns whether or not the Pid has calculated its first cpu_pct
   * value
   * @return true if calculated
   * @return false if not calculated
   */
  bool is_ready() const;

  /**
   * @brief Returns whetheror not the process was stopped from within
   * @return true if stopped from within
   * @return false if not stopped from within
   */
  bool is_self_stopped() const;

  /**
   * @brief Returns the comm of the process
   * @return const std::string&
   */
  const std::string &comm() const;

  /**
   * @brief Returns the state of the process
   * @return char
   */
  char state() const;

  /**
   * @brief Returns the ppid of the process
   *
   * @return pid_t
   */
  pid_t ppid() const;

  /**
   * @brief Returns the pgrp of the process
   *
   * @return int
   */
  int pgrp() const;

  /**
   * @brief Returns the session of the process
   *
   * @return int
   */
  int session() const;

  /**
   * @brief Returns the tty_nr of the process
   *
   * @return int
   */
  int tty_nr() const;

  /**
   * @brief Returns the tpgid of the process
   *
   * @return int
   */
  int tpgid() const;

  /**
   * @brief Returns the flags of the process
   *
   * @return uint
   */
  uint flags() const;

  /**
   * @brief Returns the utime of the process
   *
   * @return u_long
   */
  u_long utime() const;

  /**
   * @brief Returns the stime of the process
   *
   * @return u_long
   */
  u_long stime() const;

  /**
   * @brief Returns the cutime of the process
   *
   * @return u_long
   */
  u_long cutime() const;

  /**
   * @brief Returns the cstime of the process
   *
   * @return u_long
   */
  u_long cstime() const;

  /**
   * @brief Returns the nice of the process
   *
   * @return long
   */
  long nice() const;

  /**
   * @brief Returns the calculated pid_cpu_pct
   *
   * @return float
   */
  float pid_cpu_pct() const;

  /** @brief Sends a SIGSTOP signal to the process */
  void send_SIGSTOP();

  /** @brief Sends a SIGCONT signal to the process */
  void send_SIGCONT();
};

}  // namespace daemon

}  // namespace templimiter
