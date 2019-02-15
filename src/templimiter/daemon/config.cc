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
 * @file config.cc
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides templimiter::daemon::Config class
 * @date created 2019-02-01
 * @date modified 2019-02-14
 */

#include "templimiter/daemon/config.h"

#include <algorithm>
#include <limits>
#include <string>
#include <vector>

#include "templimiter/error/argument-error.h"
#include "templimiter/error/config-error.h"
#include "templimiter/error/error.h"
#include "templimiter/error/internal-error.h"
#include "templimiter/io/file-collection.h"
#include "templimiter/io/file.h"
#include "templimiter/tools/string.h"
#include "templimiter/tools/vector.h"

namespace templimiter {

namespace daemon {

void Config::assert_SIGSTOP_mode_(const std::string &value) const {
  if (!use_SIGSTOP_) {
    if (value == "") {
      throw error::InternalError(
          "Attempting to access SIGSTOP-related value without having enabled "
          "SIGSTOP mode.");
    } else {
      throw error::InternalError("Attempting to access " + value +
                                 " without having enabled SIGSTOP mode.");
    }
  }
}

void Config::assert_throttle_mode_(const std::string &value) const {
  if (!use_throttle_) {
    throw error::InternalError("Attempting to access " + value +
                               " without having enabled throttle mode.");
  }
}

void Config::assert_any_mode_() const {
  if (!use_throttle_ && !use_SIGSTOP_) {
    throw error::ConfigError(
        "use_throttle", "false",
        "Cannot choose false for both <use_throttle> and <use_SIGSTOP>.");
  }
}

void Config::assert_throttle_gte_dethrottle_() const {
  if (temp_throttle_ < temp_dethrottle_) {
    throw error::ConfigError(
        "temp_throttle", tools::to_string(temp_throttle_),
        "Throttle temp must not be lower than dethrottle temp.");
  }
}

void Config::assert_scale_max_freq_files_sizey_(size_t scalemax_sz) const {
  if (scalemax_sz == 0) {
    throw error::ConfigError("matcher_scaling_max_freq",
                             matcher_scaling_max_freq_,
                             "Cannot find any scaling_max_freq values using "
                             "the provided matcher.");
  }
}

void Config::assert_scaling_available_frequencies_sizey_() const {
  size_t scaleavail_sz = scaling_available_frequencies_.size();
  if (scaleavail_sz == 0) {
    throw error::ConfigError("matcher_scaling_available_frequencies",
                             matcher_scaling_available_frequencies_,
                             "Cannot find any scaling_available_frequencies "
                             "values using the provided matcher.");
  }
}

void Config::assert_scaling_max_size_eq_scaling_available_(
    size_t scalemax_sz) const {
  size_t scaleavail_sz = scaling_available_frequencies_.size();
  if (scalemax_sz != scaleavail_sz) {
    throw error::ConfigError(
        "matcher_scaling_max_freq", matcher_scaling_max_freq_,
        "Number of matched scaling_max_freq values does not match the "
        "number "
        "of matched scaling_available_frequencies values.");
  }
}

void Config::assert_scaling_max_size_eq_cpu_max_and_min_(
    size_t scalemax_sz) const {
  size_t cpumax_sz = cpuinfo_max_freqs_.size();
  size_t cpumin_sz = cpuinfo_min_freqs_.size();
  if (cpumax_sz == 0) {
    throw error::ConfigError(
        "matcher_cpuinfo_max_freq", matcher_cpuinfo_max_freq_,
        "Cannot find any cpuinfo_max_freq values using the provided matcher.");
  }
  if (cpumin_sz == 0) {
    throw error::ConfigError(
        "matcher_cpuinfo_min_freq", matcher_cpuinfo_max_freq_,
        "Cannot find any cpuinfo_min_freq values using the provided matcher.");
  }

  // Ensure cpu ct is the same for cur, min, and max
  if (scalemax_sz != cpumax_sz) {
    throw error::ConfigError(
        "matcher_cpuinfo_max_freq", matcher_cpuinfo_max_freq_,
        "Number of matched scaling_max_freq values does not match the number "
        "of matched cpuinfo_max_freq values.");
  }
  if (scalemax_sz != cpumin_sz) {
    throw error::ConfigError(
        "matcher_cpuinfo_min_freq", matcher_cpuinfo_min_freq_,
        "Number of matched scaling_max_freq values does not match the number "
        "of matched cpuinfo_min_freq values.");
  }
}

void Config::assert_SIGSTOP_gte_SIGCONT_() const {
  if (temp_SIGSTOP_ < temp_SIGCONT_) {
    throw error::ConfigError(
        "temp_SIGSTOP", tools::to_string(temp_SIGSTOP_),
        "SIGSTOP temp must not be lower than SIGCONT temp.");
  }
}

void Config::assert_proc_stat_file_sizey_(size_t procstat_sz) const {
  if (procstat_sz == 0) {
    throw error::InternalError("Could not load a valid /proc/stat file.");
  }
}

void Config::load_config_lines_(const std::string &config_path) {
  io::File<std::string> config(config_path);
  if (!config.exists()) {
    throw error::InternalError("Cannot find config file.");
  }
  config_lines_ = config.read();
}

void Config::get_own_pid_() {
  // Read /proc/self/stat for own pid information
  io::File<std::string> self_stat(PROC_SELF_STAT_);
  std::string self_pid = tools::split(self_stat.read()[0], ' ')[0];
  own_pid_ = tools::convert<pid_t>(self_pid);
}

std::vector<size_t> Config::indices_of_tag_(const std::string &tag) {
  // Get the index of a tag from the previously loaded config lines
  std::vector<size_t> found;
  for (size_t i = 0; i < config_lines_.size(); i++) {
    // Split the line and check the first text block for the tag
    std::vector<std::string> spl = tools::split(config_lines_[i], ' ');
    if (spl[0] == tag) {
      found.push_back(i);
    }
  }
  return found;
}

void Config::load_config_values_() {
  // Load each tag using load_from_tag_ and hard-coded default value as fallback
  log_file_path_ = load_from_tag_<std::string>("log_file_path", log_file_path_);
  whitelist_pid_ = load_from_tag_<pid_t>("whitelist_pid", whitelist_pid_);
  // whitelist own pid
  whitelist_pid_.push_back(own_pid_);
  whitelist_comm_ =
      tools::map(load_from_tag_<std::string>("whitelist_comm", whitelist_comm_),
                 [](const auto &v) { return "(" + v + ")"; });
  whitelist_state_ = load_from_tag_<char>("whitelist_state", whitelist_state_);
  whitelist_ppid_ = load_from_tag_<pid_t>("whitelist_ppid", whitelist_ppid_);
  whitelist_pgrp_ = load_from_tag_<int>("whitelist_pgrp", whitelist_pgrp_);
  whitelist_session_ =
      load_from_tag_<int>("whitelist_session", whitelist_session_);
  whitelist_tty_nr_ =
      load_from_tag_<int>("whitelist_tty_nr", whitelist_tty_nr_);
  whitelist_tpgid_ = load_from_tag_<int>("whitelist_tpgid", whitelist_tpgid_);
  whitelist_flags_ = load_from_tag_<uint>("whitelist_flags", whitelist_flags_);
  whitelist_max_nice_ =
      load_from_tag_<long>("whitelist_max_nice", whitelist_max_nice_);
  matcher_thermal_ =
      load_from_tag_<std::string>("matcher_thermal", matcher_thermal_);
  matcher_scaling_max_freq_ = load_from_tag_<std::string>(
      "matcher_scaling_max_freq", matcher_scaling_max_freq_);
  matcher_cpuinfo_max_freq_ = load_from_tag_<std::string>(
      "matcher_cpuinfo_max_freq", matcher_cpuinfo_max_freq_);
  matcher_cpuinfo_min_freq_ = load_from_tag_<std::string>(
      "matcher_cpuinfo_min_freq", matcher_cpuinfo_min_freq_);
  matcher_scaling_available_frequencies_ =
      load_from_tag_<std::string>("matcher_scaling_available_frequencies",
                                  matcher_scaling_available_frequencies_);
  use_throttle_ = load_from_tag_<bool>("use_throttle", use_throttle_);
  use_SIGSTOP_ = load_from_tag_<bool>("use_SIGSTOP", use_SIGSTOP_);
  use_scaling_available_ =
      load_from_tag_<bool>("use_scaling_available", use_scaling_available_);
  use_stepwise_SIGSTOP_ =
      load_from_tag_<bool>("use_stepwise_SIGSTOP", use_stepwise_SIGSTOP_);
  use_stepwise_SIGCONT_ =
      load_from_tag_<bool>("use_stepwise_SIGCONT", use_stepwise_SIGCONT_);
  temp_SIGSTOP_ = load_from_tag_<u_long>("temp_SIGSTOP", temp_SIGSTOP_);
  temp_SIGCONT_ = load_from_tag_<u_long>("temp_SIGCONT", temp_SIGCONT_);
  temp_throttle_ = load_from_tag_<u_long>("temp_throttle", temp_throttle_);
  temp_dethrottle_ =
      load_from_tag_<u_long>("temp_dethrottle", temp_dethrottle_);
  min_sleep_ = load_from_tag_<uint>("min_sleep", min_sleep_);
}

void Config::set_and_assert_config_() {
  // Ensure at least one mode is selected
  assert_any_mode_();

  // Load thermal files
  thermal_files_ =
      std::make_shared<io::FileCollection<u_long>>(matcher_thermal_);

  if (use_throttle_) {
    // If throttle mode is selected
    // Ensure throttle temp is gte dethrottle temp
    assert_throttle_gte_dethrottle_();

    // Load cur cpu freq files
    scaling_max_freq_files_ =
        std::make_shared<io::FileCollection<u_long>>(matcher_scaling_max_freq_);

    // Load scaling_max_freq file size for configuration assertions
    size_t scalemax_sz = scaling_max_freq_files_->read().size();

    // Ensure cur cpu freq files are found
    assert_scale_max_freq_files_sizey_(scalemax_sz);

    std::shared_ptr<io::FileCollection<std::string>> test_scaling_files;

    // Test for scaling directory before allowing it to be enabled
    if (use_scaling_available_) {
      try {
        test_scaling_files = std::make_shared<io::FileCollection<std::string>>(
            matcher_scaling_available_frequencies_);
      } catch (const error::Error &e) {
        // Disable if expected error occurs, otherwise throw if unexpected error
        if (e.name() == "ArgumentError") {
          // Warn the user
          io::err(
              "[Warning] Scaling available frequencies file not found! "
              "Disabling scaling.");
          use_scaling_available_ = false;
        } else {
          throw;
        }
      }
    }

    if (use_scaling_available_) {
      // If using frequency scaling
      // Load available frequencies
      scaling_available_frequencies_files_ = test_scaling_files;

      // Create available frequencies matrix
      std::vector<std::string> scalemax_lines =
          scaling_available_frequencies_files_->read();
      for (const auto &v : scalemax_lines) {
        std::vector<u_long> freq_vect =
            tools::convert<u_long>(tools::split(v, ' '));
        scaling_available_frequencies_.push_back(freq_vect);
      }

      // Ensure available frequencies were found and loaded correctly
      assert_scaling_available_frequencies_sizey_();

      // Ensure cpu ct is the same for cur and available
      assert_scaling_max_size_eq_scaling_available_(scalemax_sz);

    } else {
      // If using cpu min/max throttling
      // Load min and max files
      cpuinfo_max_freq_files_ = std::make_shared<io::FileCollection<u_long>>(
          matcher_cpuinfo_max_freq_);
      cpuinfo_min_freq_files_ = std::make_shared<io::FileCollection<u_long>>(
          matcher_cpuinfo_min_freq_);

      // Create min/max frequency vectors
      cpuinfo_max_freqs_ = cpuinfo_max_freq_files_->read();
      cpuinfo_min_freqs_ = cpuinfo_min_freq_files_->read();

      // Ensure min and max are sizey
      assert_scaling_max_size_eq_cpu_max_and_min_(scalemax_sz);
    }
  } else {
    // If throttle mode is not selected
    // Set throttle temp to highest, just in case
    temp_throttle_ = std::numeric_limits<u_long>().max();
    // Set dethrottle temp to lowest, just in case
    temp_dethrottle_ = 0;
  }

  if (use_SIGSTOP_) {
    // If SIGSTOP mode is selected
    // Ensure SIGSTOP temp is gte SIGCONT temp
    assert_SIGSTOP_gte_SIGCONT_();

    // Load CPU stats file
    proc_stat_file_ = std::make_shared<io::File<std::string>>(PROC_STAT_);
    // Ensure proc stat file is sizey
    assert_proc_stat_file_sizey_(proc_stat_file_->read().size());
  } else {
    // If SIGSTOP mode is not selected
    // Set SIGSTOP temp to highest, just in case
    temp_SIGSTOP_ = std::numeric_limits<u_long>().max();
    // Set SIGCONT temp to lowest, just in case
    temp_SIGCONT_ = 0;
  }
}

Config::Config(const std::string &config_path) {
  // Load configuration into memory
  load_config_lines_(config_path);
  // Get execution PID
  get_own_pid_();
  // Load each configuration value
  load_config_values_();
  // Perform verifications
  set_and_assert_config_();
}

const std::string &Config::log_file_path() const { return log_file_path_; }
const std::vector<pid_t> &Config::whitelist_pid() const {
  assert_SIGSTOP_mode_("whitelist_pid");
  return whitelist_pid_;
}
const std::vector<std::string> &Config::whitelist_comm() const {
  assert_SIGSTOP_mode_("whitelist_comm");
  return whitelist_comm_;
}
const std::vector<char> &Config::whitelist_state() const {
  assert_SIGSTOP_mode_("whitelist_state");
  return whitelist_state_;
}
const std::vector<pid_t> &Config::whitelist_ppid() const {
  assert_SIGSTOP_mode_("whitelist_ppid");
  return whitelist_ppid_;
}
const std::vector<int> &Config::whitelist_pgrp() const {
  assert_SIGSTOP_mode_("whitelist_pgrp");
  return whitelist_pgrp_;
}
const std::vector<int> &Config::whitelist_session() const {
  assert_SIGSTOP_mode_("whitelist_session");
  return whitelist_session_;
}
const std::vector<int> &Config::whitelist_tty_nr() const {
  assert_SIGSTOP_mode_("whitelist_tty_nr");
  return whitelist_tty_nr_;
}
const std::vector<int> &Config::whitelist_tpgid() const {
  assert_SIGSTOP_mode_("whitelist_tpgid");
  return whitelist_tpgid_;
}
const std::vector<uint> &Config::whitelist_flags() const {
  assert_SIGSTOP_mode_("whitelist_flags");
  return whitelist_flags_;
}
long Config::whitelist_max_nice() const {
  assert_SIGSTOP_mode_("whitelist_max_nice");
  return whitelist_max_nice_;
}
bool Config::use_throttle() const { return use_throttle_; }
bool Config::use_SIGSTOP() const { return use_SIGSTOP_; }
bool Config::use_scaling_available() const {
  assert_throttle_mode_("use_scaling_available");
  return use_scaling_available_;
}
bool Config::use_stepwise_SIGSTOP() const {
  assert_SIGSTOP_mode_("use_stepwise_SIGSTOP");
  return use_stepwise_SIGSTOP_;
}
bool Config::use_stepwise_SIGCONT() const {
  assert_SIGSTOP_mode_("use_stepwise_SIGCONT");
  return use_stepwise_SIGCONT_;
}
u_long Config::temp_SIGSTOP() const {
  assert_SIGSTOP_mode_("temp_SIGSTOP");
  return temp_SIGSTOP_;
}
u_long Config::temp_SIGCONT() const {
  assert_SIGSTOP_mode_("temp_SIGCONT");
  return temp_SIGCONT_;
}
u_long Config::temp_throttle() const {
  assert_throttle_mode_("temp_throttle");
  return temp_throttle_;
}
u_long Config::temp_dethrottle() const {
  assert_throttle_mode_("temp_dethrottle");
  return temp_dethrottle_;
}
uint Config::min_sleep() const { return min_sleep_; }
const std::shared_ptr<io::FileCollection<u_long>> &Config::thermal_files() {
  return thermal_files_;
}
const std::shared_ptr<io::FileCollection<u_long>>
    &Config::scaling_max_freq_files() {
  assert_throttle_mode_("scaling_max_freq_files");
  return scaling_max_freq_files_;
}
const std::shared_ptr<io::File<std::string>> &Config::proc_stat_file() {
  assert_SIGSTOP_mode_("proc_stat_file");
  return proc_stat_file_;
}
const std::vector<u_long> &Config::cpuinfo_max_freqs() const {
  assert_throttle_mode_("cpuinfo_max_freqs");
  return cpuinfo_max_freqs_;
}
const std::vector<u_long> &Config::cpuinfo_min_freqs() const {
  assert_throttle_mode_("cpuinfo_min_freqs");
  return cpuinfo_min_freqs_;
}
const std::vector<std::vector<u_long>> &Config::scaling_available_frequencies()
    const {
  assert_throttle_mode_("scaling_available_frequencies");
  return scaling_available_frequencies_;
}

}  // namespace daemon

}  // namespace templimiter
