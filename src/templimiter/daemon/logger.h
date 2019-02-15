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
 * @file logger.h
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides templimiter::daemon::Logger class
 * @date created 2019-01-31
 * @date modified 2019-02-14
 */

#pragma once

#include <string>

#include "templimiter/daemon/config.h"
#include "templimiter/io/file.h"
#include "templimiter/io/operations.h"
#include "templimiter/tools/type-convert.h"
#include "templimiter/tools/vector.h"

namespace templimiter {

namespace daemon {

/** @brief Provides a logging channel to the configured logfile */
class Logger {
 private:
  /** @brief Execution configruation */
  std::shared_ptr<Config> cfg_;

  /** @brief Whether or not logs should go to stdout as well */
  bool is_debug_mode_;

  /** @brief Logfile file object */
  io::File<std::string> logfile_;

  /**
   * @brief Generates a string timestamp
   *
   * @return std::string
   */
  std::string gen_timestamp_();

  /** @brief Logs a welcome text with title and version */
  void write_welcome_text_();

 public:
  /**
   * @brief Construct a new Logger object
   *
   * @param cfg Execution configuration
   * @param is_debug_mode Whether or not logs should go to stdout as well
   * (enabled by --debug flag)
   */
  explicit Logger(const std::shared_ptr<Config> &cfg, bool is_debug_mode);

  /**
   * @brief Logs any kind of non-vector data
   *
   * @tparam T Type of data
   * @param data Date to log
   */
  template <typename T>
  void log(const T &data) {
    std::string line = "[" + gen_timestamp_() + "] " + tools::to_string(data);
    logfile_.append(line);
    if (is_debug_mode_) io::log(line);
  }

  /**
   * @brief Logs any kind of vector data as a block with one timestamp
   *
   * @tparam T Type of vector
   * @param data Vector to log
   */
  template <typename T>
  void log(const std::vector<T> &data) {
    std::string lines_header = "[" + gen_timestamp_() + "] ";
    std::vector<std::string> lines = {};
    lines.push_back(lines_header + tools::to_string(data[0]));
    for (size_t i = 1; i < data.size(); i++) {
      // pad each new line with spaces
      lines.push_back(
          tools::join(tools::fill<std::string>(lines_header.size(), " "), "") +
          tools::to_string(data[i]));
    }
    logfile_.append(lines);
    if (is_debug_mode_) io::log(lines);
  }

  /**
   * @brief Errors out any kind of non-vector data
   *
   * @tparam T Type of data
   * @param data Date to error out
   */
  template <typename T>
  void err(const T &data) {
    std::string timestamp = "[" + gen_timestamp_() + "] ";
    std::string err_header = "<!--- An error has occurred! ---!>";
    std::string err_line = timestamp + err_header;
    std::string line = timestamp + tools::to_string(data);
    logfile_.append(err_line);
    logfile_.append(line);
    if (is_debug_mode_) io::err(err_line);
    if (is_debug_mode_) io::err(line);
  }

  /**
   * @brief Errors out any kind of vector data as a block with one timestamp
   *
   * @tparam T Type of vector
   * @param data Vector to error out
   */
  template <typename T>
  void err(const std::vector<T> &data) {
    std::string timestamp = "[" + gen_timestamp_() + "] ";
    std::string err_header = "<!---- An error has occurred! ----!>";
    std::vector<std::string> lines = {};
    lines.push_back(timestamp + err_header);
    for (size_t i = 0; i < data.size(); i++) {
      // pad each new line with spaces
      lines.push_back(
          tools::join(tools::fill<std::string>(timestamp.size(), " "), "") +
          tools::to_string(data[i]));
    }
    logfile_.append(lines);
    if (is_debug_mode_) io::err(lines);
  }
};

}  // namespace daemon

}  // namespace templimiter
