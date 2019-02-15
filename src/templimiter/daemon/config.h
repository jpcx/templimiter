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
 * @file config.h
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides templimiter::daemon::Config class
 * @date created 2019-02-08
 * @date modified 2019-02-09
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "templimiter/error/config-error.h"
#include "templimiter/error/error.h"
#include "templimiter/io/file-collection.h"
#include "templimiter/io/file.h"
#include "templimiter/io/operations.h"
#include "templimiter/tools/string.h"
#include "templimiter/tools/vector.h"

namespace templimiter {

namespace daemon {

/** @brief Contains configuration settings and derived objects */
class Config {
 private:
  // Internal variables
  /** @brief String vector used to store loaded configuration text */
  std::vector<std::string> config_lines_;
  /** @brief Holds the pid of the running process */
  pid_t own_pid_;
  /** @brief Location of /proc/self/stat used to find own_pid_ */
  const std::string PROC_SELF_STAT_ = "/proc/self/stat";
  /** @brief Location of /proc/stat used to find cpu information */
  const std::string PROC_STAT_ = "/proc/stat";

  // Internal pre-derivation config values
  /** @brief Matcher to find thermal files */
  std::string matcher_thermal_ =
      "/sys/devices/virtual/thermal/thermal_zone*/temp";
  /** @brief Matcher to find scaling_max_freq files */
  std::string matcher_scaling_max_freq_ =
      "/sys/devices/system/cpu/cpu*/cpufreq/scaling_max_freq";
  /** @brief Matcher to find cpuinfo_max_freq files */
  std::string matcher_cpuinfo_max_freq_ =
      "/sys/devices/system/cpu/cpu*/cpufreq/cpuinfo_max_freq";
  /** @brief Matcher to find cpuinfo_min_freq files */
  std::string matcher_cpuinfo_min_freq_ =
      "/sys/devices/system/cpu/cpu*/cpufreq/cpuinfo_min_freq";
  /** @brief Matcher to find scaling_available_frequencies files */
  std::string matcher_scaling_available_frequencies_ =
      "/sys/devices/system/cpu/cpu*/cpufreq/scaling_available_frequencies";

  // Private component of config values
  /** @brief File to send logging information */
  std::string log_file_path_ = "/var/log/templimiter.log";
  /** @brief Prohibits sending SIGSTOP to processes of this pid */
  std::vector<pid_t> whitelist_pid_;
  /** @brief Prohibits sending SIGSTOP to processes of this comm */
  std::vector<std::string> whitelist_comm_ = {
      "dnsmasq",         "systemd",         "(sd-pam)",
      "startx",          "xinit",           "Xorg",
      "dbus-daemon",     "rtkit-daemon",    "at-spi-bus-laun",
      "at-spi2-registr", "wpa_supplicant",  "dhcpcd",
      "systemd-journal", "lvmetad",         "systemd-udevd",
      "upowerd",         "systemd-timesyn", "systemd-machine",
      "firewalld",       "systemd-logind",  "polkitd",
      "haveged",         "systemd-resolve", "systemd-network"};
  /** @brief Prohibits sending SIGSTOP to processes of this state */
  std::vector<char> whitelist_state_;
  /** @brief Prohibits sending SIGSTOP to processes of this ppid */
  std::vector<pid_t> whitelist_ppid_;
  /** @brief Prohibits sending SIGSTOP to processes of this pgrp */
  std::vector<int> whitelist_pgrp_;
  /** @brief Prohibits sending SIGSTOP to processes of this session */
  std::vector<int> whitelist_session_;
  /** @brief Prohibits sending SIGSTOP to processes of this tty_nr */
  std::vector<int> whitelist_tty_nr_;
  /** @brief Prohibits sending SIGSTOP to processes of this tpgid */
  std::vector<int> whitelist_tpgid_;
  /** @brief Prohibits sending SIGSTOP to processes of this flag */
  std::vector<uint> whitelist_flags_;
  /** @brief Prohibits sending SIGSTOP to processes of a nice lower than this */
  long whitelist_max_nice_ = -21;
  /** @brief Whether or not throttling should be enabled */
  bool use_throttle_ = true;
  /** @brief Whether or not sending SIGSTOP signals should be enabled */
  bool use_SIGSTOP_ = false;
  /**
   * @brief Whether or not scaling_available_frequencies or cpu_min_freq should
   * be used
   */
  bool use_scaling_available_ = false;
  /** @brief Whether or not to stop one process at a time */
  bool use_stepwise_SIGSTOP_ = true;
  /** @brief Whether or not to continue one process at a time */
  bool use_stepwise_SIGCONT_ = false;
  /** @brief Temperature to start sending SIGSTOP signals */
  u_long temp_SIGSTOP_ = 70000;
  /** @brief Temperature to start sending SIGCONT signals */
  u_long temp_SIGCONT_ = 66000;
  /** @brief Temperature to start CPU throttling */
  u_long temp_throttle_ = 66000;
  /** @brief Temperature to start CPU dethrottling */
  u_long temp_dethrottle_ = 60000;
  /** @brief Time to wait between state checks */
  uint min_sleep_ = 500;

  // Derived private components
  /** @brief Files to get thermal data from */
  std::shared_ptr<io::FileCollection<u_long>> thermal_files_;
  /** @brief Files to get/set current CPU speed */
  std::shared_ptr<io::FileCollection<u_long>> scaling_max_freq_files_;
  /** @brief Files to get maximum CPU speed */
  std::shared_ptr<io::FileCollection<u_long>> cpuinfo_max_freq_files_;
  /** @brief Files to get mininum CPU speed */
  std::shared_ptr<io::FileCollection<u_long>> cpuinfo_min_freq_files_;
  /** @brief Files to get supported CPU throttle speeds */
  std::shared_ptr<io::FileCollection<std::string>>
      scaling_available_frequencies_files_;
  /** @brief File to get current CPU clock time information */
  std::shared_ptr<io::File<std::string>> proc_stat_file_;
  /** @brief Holds the vector of cpuinfo_max_freq values */
  std::vector<u_long> cpuinfo_max_freqs_;
  /** @brief Holds the vector of cpuinfo_min_freq values */
  std::vector<u_long> cpuinfo_min_freqs_;
  /** @brief Holds the matrix of available scaling frequencies */
  std::vector<std::vector<u_long>> scaling_available_frequencies_;

  // Internal functions

  // Asserts
  /**
   * @brief Asserts that SIGSTOP mode is enabled before accessing a given value
   *
   * @param value Value attempting to access
   * @throw templimiter::error::InternalError if SIGSTOP mode is not enabled
   */
  void assert_SIGSTOP_mode_(const std::string &value) const;

  /**
   * @brief Asserts that throttle mode is enabled before accessing a given value
   *
   * @param value Value attempting to access
   * @throw templimiter::error::InternalError if throttle mode is not enabled
   */
  void assert_throttle_mode_(const std::string &value) const;

  /**
   * @brief Asserts that at least one response mode is enabled
   *
   * @throw templimiter::error::ConfigError if neither mode is enabled
   */
  void assert_any_mode_() const;

  /**
   * @brief Asserts that throttle mode is gte dethrottle mode
   *
   * @throw templimiter::error::ConfigError if throttle temp is less than
   * dethrottle temp
   */
  void assert_throttle_gte_dethrottle_() const;

  /**
   * @brief Asserts that scaling_max_freq_files_ are sizey
   *
   * @param scalemax_sz Size of the scaling_max_freq_files_ object
   * @throw templimiter::error::ConfigError if files are not sizey
   */
  void assert_scale_max_freq_files_sizey_(size_t scalemax_sz) const;

  /**
   * @brief Asserts that generated scaling_available_frequencies_ matrix is
   * sizey
   *
   * @throw templimiter::error::ConfigError if files are not sizey
   */
  void assert_scaling_available_frequencies_sizey_() const;

  /**
   * @brief Asserts that scaling_max_freq_files_ size matches scaling available
   * matrix size
   *
   * @param scalemax_sz Size of the scaling_max_freq_files_ object
   * @throw templimiter::error::ConfigError if sizes are not equal
   */
  void assert_scaling_max_size_eq_scaling_available_(size_t scalemax_sz) const;

  /**
   * @brief Asserts that scaling_max_freq_files, cpuinfo_max_freq, and
   * cpuinfo_min_freq are the same size
   *
   * @param scalemax_sz Size of the scaling_max_freq_files_ object
   * @throw templimiter::error::ConfigError if cpu_max_freq_files_ size is zero
   * @throw templimiter::error::ConfigError if cpu_min_freq_files_ size is zero
   * @throw templimiter::error::ConfigError if scaling_max_freq_files_ size is
   * not equal to cpu_max_freq_files_ size
   * @throw templimiter::error::ConfigError if scaling_max_freq_files_ size is
   * not equal to cpu_min_freq_files_ size
   */
  void assert_scaling_max_size_eq_cpu_max_and_min_(size_t scalemax_sz) const;

  /**
   * @brief Asserts that SIGSTOP temp is gte SIGCONT temp
   *
   * @throws templimiter::error::ConfigError if SIGSTOP temp is less than
   * SIGCONT temp
   */
  void assert_SIGSTOP_gte_SIGCONT_() const;

  /**
   * @brief Asserts that proc_stat_file_ is sizey
   *
   * @param procstat_sz Size of the proc_stat_file_ object
   * @throws templimiter::error::InternalError if file is not sizey
   */
  void assert_proc_stat_file_sizey_(size_t procstat_sz) const;

  // Procedures
  /**
   * @brief Loads config lines to the private config_lines_
   * @param config_path Path to the config file
   */
  void load_config_lines_(const std::string &config_path);

  /** @brief Loads own pid from PROC_SELF_STAT_ to own_pid_ */
  void get_own_pid_();

  /**
   * @brief Retrieves the index of the tag in the config lines
   *
   * @param tag Tag to search for
   * @return size_t
   */
  std::vector<size_t> indices_of_tag_(const std::string &tag);

  /** @brief Loads all private config values from config lines */
  void load_config_values_();

  /** @brief Verifies all fallable conditions and loads file objects */
  void set_and_assert_config_();

  // Templates
  /**
   * @brief Loads a value from the config lines given a tag
   *
   * @tparam T Type of value
   * @param tag Tag to search for
   * @param defval Default value to return if not found
   * @return T
   * @throw templimiter::error::ConfigError if config has more than one line
   * configuring the tag
   * @throw templimiter::error::ConfigError if config line contains more than
   * one value for the tag
   * @throw templimiter::error::ConfigError if value type is invalid
   */
  template <typename T>
  T load_from_tag_(const std::string &tag, const T &defval) {
    // Try finding the index of the tag in the configuration
    std::vector<size_t> indices = indices_of_tag_(tag);
    if (indices.size() == 0) {
      // Tag is not found, return default
      io::log("Could not find tag <" + tag + "> in config. Using default");
      return defval;
    } else if (indices.size() > 1) {
      throw error::ConfigError(
          tag, "",
          "Multiple indices found for tag. Config should only have one value.");
    }

    // Set index to the only value in indices
    size_t index = indices[0];

    // Split the config line and take all values right of the tag
    std::vector<std::string> spl;
    std::vector<std::string> val_strs;
    spl = tools::split(config_lines_[index], ' ');
    val_strs = tools::subvect(spl, 1, ptrdiff_t(spl.size() - 1));

    // Ensure values have been found
    if (val_strs.size() < 1) {
      io::log("Could not find a value for tag <" + tag +
              "> in config. Using default");
      return defval;
    }

    // Ensure only one value is found
    if (val_strs.size() > 1) {
      throw error::ConfigError(
          tag, tools::join(val_strs, ", "),
          "Expected only one element in configuration setting.");
    }
    std::string val = val_strs.front();

    // Try to convert and return the value
    try {
      auto result = tools::convert<T>(val);
      return result;
    } catch (const error::Error &e) {
      if (e.name() == "TypeError") {
        throw error::ConfigError(tag, val, "Unable to convert type.");
      } else {
        throw;
      }
    }
  }

  /**
   * @brief Loads a vector value from the config lines given a tag
   *
   * @tparam T Type of the vector
   * @param tag Tag to search for
   * @param defval Default value to return if not found
   * @return T
   * @throw templimiter::error::ConfigError if config has more than one line
   * configuring the tag
   * @throw templimiter::error::ConfigError if value type is invalid
   */
  template <typename T>
  std::vector<T> load_from_tag_(const std::string &tag,
                                const std::vector<T> &defval) {
    // Try finding the index of the tag in the configuration
    std::vector<size_t> indices = indices_of_tag_(tag);
    if (indices.size() == 0) {
      // Tag is not found, return default
      io::log("Could not find tag <" + tag + "> in config. Using default");
      return defval;
    } else if (indices.size() > 1) {
      throw error::ConfigError(
          tag, "",
          "Multiple indices found for tag. Config should only have one value.");
    }

    // Set index to the only value in indices
    size_t index = indices[0];

    // Split the line and retrieve values right of tag
    std::vector<std::string> spl;
    std::vector<std::string> val_strs;
    spl = tools::split(config_lines_[index], ' ');
    val_strs = tools::subvect(spl, 1, ptrdiff_t(spl.size() - 1));
    if (val_strs.size() < 1) {
      io::log("Could not find any values for tag <" + tag +
              "> in config. Using default");
      return defval;
    }

    // Try to convert and return the value
    try {
      auto result = tools::convert<T>(val_strs);
      return result;
    } catch (const error::Error &e) {
      if (e.name() == "TypeError") {
        throw error::ConfigError(tag, tools::join(val_strs, ", "),
                                 "Unable to convert type.");
      } else {
        throw;
      }
    }
  }

 public:
  /**
   * @brief Construct a new Config object
   * @param config_path Where to find the configuration file
   */
  explicit Config(const std::string &config_path);

  /**
   * @brief Returns log_file_path configuration setting.
   * @return const std::string&
   */
  const std::string &log_file_path() const;

  /**
   * @brief Returns whitelist_pid configuration setting
   * @return const std::vector< pid_t >&
   */
  const std::vector<pid_t> &whitelist_pid() const;

  /**
   * @brief Returns whitelist_comm configuration setting
   * @return const std::vector< std::string >&
   */
  const std::vector<std::string> &whitelist_comm() const;

  /**
   * @brief Returns whitelist_state configuration setting
   * @return const std::vector< char >&
   */
  const std::vector<char> &whitelist_state() const;

  /**
   * @brief Returns whitelist_ppid configuration setting
   * @return const std::vector< pid_t >&
   */
  const std::vector<pid_t> &whitelist_ppid() const;

  /**
   * @brief Returns whitelist_pgrp configuration setting
   * @return const std::vector< int >&
   */
  const std::vector<int> &whitelist_pgrp() const;

  /**
   * @brief Returns whitelist_session configuration setting
   * @return const std::vector< int >&
   */
  const std::vector<int> &whitelist_session() const;

  /**
   * @brief Returns whitelist_tty_nr configuration setting
   * @return const std::vector< int >&
   */
  const std::vector<int> &whitelist_tty_nr() const;

  /**
   * @brief Returns whitelist_tpgid configuration setting
   * @return const std::vector< int >&
   */
  const std::vector<int> &whitelist_tpgid() const;

  /**
   * @brief Returns whitelist_flags configuration setting
   * @return const std::vector< uint >&
   */
  const std::vector<uint> &whitelist_flags() const;

  /**
   * @brief Returns whitelist_max_nice configuration setting
   * @return long
   */
  long whitelist_max_nice() const;

  /**
   * @brief Returns use_throttle configuration setting
   * @return true if using throttle
   * @return false if not using throttle
   */
  bool use_throttle() const;

  /**
   * @brief Returns use_SIGSTOP configuration setting
   * @return true if using SIGSTOP
   * @return false if not using SIGSTOP
   */
  bool use_SIGSTOP() const;

  /**
   * @brief Returns use_scaling_available configuration setting
   * @return true if using scaling_available
   * @return false if not using scaling_available
   */
  bool use_scaling_available() const;

  /**
   * @brief Returns the use_stepwise_SIGSTOP configuration setting
   *
   * @return true if using stepwise SIGSTOP
   * @return false if not using stepwise SIGSTOP
   */
  bool use_stepwise_SIGSTOP() const;

  /**
   * @brief Returns the use_stepwise_SIGCONT configuration setting
   *
   * @return true if using stepwise SIGCONT
   * @return false if not using stepwise SIGCONT
   */
  bool use_stepwise_SIGCONT() const;

  /**
   * @brief Returns temp_SIGSTOP configuration setting
   * @return u_long
   */
  u_long temp_SIGSTOP() const;

  /**
   * @brief Returns temp_SIGCONT configuration setting
   * @return u_long
   */
  u_long temp_SIGCONT() const;

  /**
   * @brief Returns temp_throttle configuration setting
   * @return u_long
   */
  u_long temp_throttle() const;

  /**
   * @brief Returns temp_dethrottle configuration setting
   * @return u_long
   */
  u_long temp_dethrottle() const;

  /**
   * @brief Returns min_sleep configuration setting
   * @return uint
   */
  uint min_sleep() const;

  /**
   * @brief Returns the constructed thermal_files FileCollection object based on
   * the configured matcher
   *
   * @return const std::shared_ptr< io::FileCollection< u_long > >&
   */
  const std::shared_ptr<io::FileCollection<u_long>> &thermal_files();

  /**
   * @brief Returns the constructed scaling_max_freq_files FileCollection object
   * based on the configured matcher
   *
   * @return const std::shared_ptr< io::FileCollection< u_long > >&
   */
  const std::shared_ptr<io::FileCollection<u_long>> &scaling_max_freq_files();

  /**
   * @brief Returns the constructed proc_stat_file File object based on the
   * hardcoded /proc/stat location
   *
   * @return const std::shared_ptr<io::File<std::string>>&
   */
  const std::shared_ptr<io::File<std::string>> &proc_stat_file();

  /**
   * @brief Returns the constructed vector of cpu max frequencies retrieved
   * during program initialization
   *
   * @return const std::vector< u_long >&
   */
  const std::vector<u_long> &cpuinfo_max_freqs() const;

  /**
   * @brief Returns the constructed vector of cpu min frequencies retrieved
   * during program initialization
   *
   * @return const std::vector< u_long >&
   */
  const std::vector<u_long> &cpuinfo_min_freqs() const;

  /**
   * @brief Returns the constructed vector of cpu available scaling frequencies
   * retrieved during program initialization
   *
   * @return const std::vector< u_long >&
   */
  const std::vector<std::vector<u_long>> &scaling_available_frequencies() const;
};

}  // namespace daemon

}  // namespace templimiter
