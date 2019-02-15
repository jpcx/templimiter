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
 * @file operations.h
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides templimiter::io functions that perform I/O operations.
 * @date created 2019-02-05
 * @date modified 2019-02-14
 */

#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "templimiter/error/argument-error.h"

namespace templimiter {

namespace io {

/**
 * @brief Logs any valid value to std::clog
 *
 * @tparam T Type to log
 * @param input Input to log
 * @throws templimiter::error::ArgumentError if streaming to std::clog fails
 */
template <typename T>
void log(const T &input) {
  try {
    std::clog << input << std::endl;
  } catch (...) {
    throw error::ArgumentError("input", "(streamable)", "foo",
                               "Value must be streamable to std::clog.");
  }
}

/**
 * @brief Logs any valid precise value to std::clog
 *
 * @tparam T Type to log
 * @param input Input to log
 * @param precision Precision to use
 */
template <typename T>
void log(const T &input, std::streamsize precision) {
  std::streamsize precision_base = std::clog.precision();
  try {
    std::clog.precision(precision);
    log(input);
    std::clog.precision(precision_base);
  } catch (...) {
    std::clog.precision(precision_base);
    throw;
  }
}

/**
 * @brief Logs any valid vector to std::clog, line by line
 *
 * @tparam T Type to log
 * @param input Input to log
 * @throws templimiter::error::ArgumentError if streaming to std::clog fails
 */
template <typename T>
void log(const std::vector<T> &input) {
  try {
    for (const auto &v : input) {
      std::clog << v << std::endl;
    }
  } catch (...) {
    throw error::ArgumentError(
        "input", "(streamable)", "foo",
        "Vector values must be streamable to std::clog.");
  }
}

/**
 * @brief Logs any valid vector of precise data to std::clog, line by line
 *
 * @tparam T Type to log
 * @param input Input to log
 * @param precision Precision to use
 */
template <typename T>
void log(const std::vector<T> &input, std::streamsize precision) {
  std::streamsize precision_base = std::clog.precision();
  try {
    std::clog.precision(precision);
    log(input);
    std::clog.precision(precision_base);
  } catch (...) {
    std::clog.precision(precision_base);
    throw;
  }
}

/**
 * @brief Errors out any valid value to std::cerr
 *
 * @tparam T Type to error out
 * @param input Input to error out
 * @throws templimiter::error::ArgumentError if streaming to std::cerr fails
 */
template <typename T>
void err(const T &input) {
  try {
    std::cerr << input << std::endl;
  } catch (...) {
    throw error::ArgumentError("input", "(streamable)", "foo",
                               "Value must be streamable to std::cerr.");
  }
}

/**
 * @brief Errors out any valid precise value to std::cerr
 *
 * @tparam T Type to error out
 * @param input Input to error out
 * @param precision Precision to use
 */
template <typename T>
void err(const T &input, std::streamsize precision) {
  std::streamsize precision_base = std::cerr.precision();
  try {
    std::cerr.precision(precision);
    err(input);
    std::cerr.precision(precision_base);
  } catch (...) {
    std::cerr.precision(precision_base);
    throw;
  }
}

/**
 * @brief Errors out any valid vector to std::cerr, line by line
 *
 * @tparam T Type to error out
 * @param input Input to error out
 * @throws templimiter::error::ArgumentError if streaming to std::cerr fails
 */
template <typename T>
void err(const std::vector<T> &input) {
  try {
    for (const auto &v : input) {
      std::cerr << input << std::endl;
    }
  } catch (...) {
    throw error::ArgumentError(
        "input", "(streamable)", "foo",
        "Vector values must be streamable to std::cerr.");
  }
}

/**
 * @brief Errors out any valid vector of precise data to std::cerr, line by line
 *
 * @tparam T Type to error out
 * @param input Input to error out
 * @param precision Precision to use
 */
template <typename T>
void err(const std::vector<T> &input, std::streamsize precision) {
  std::streamsize precision_base = std::cerr.precision();
  try {
    std::cerr.precision(precision);
    err(input);
    std::cerr.precision(precision_base);
  } catch (...) {
    std::cerr.precision(precision_base);
    throw;
  }
}

/** @brief Prints the helptext to the console */
void out_helptext();

/** @brief Prints the current version to the console */
void out_version();

/**
 * @brief Prints the configuration file path to the console
 * @param config_path Path to configuration file.
 */
void out_confpath(const char *config_path);

/**
 * @brief Check if file exists using stat
 *
 * @param file_path File path to check
 * @return true if file exists
 * @return false if file does not exist
 */
bool file_exists(const std::string &file_path);

/**
 * @brief Makes all directories leading to a provided directory path
 *
 * @param dir_path Directory path to make
 * @throw templimiter::error::IOError if mkdir fails
 */
void mkdir_deep(const std::string &dir_path);

/**
 * @brief Ensures that the provided file path has a parent directory by using
 * mkdir_deep if necessary
 *
 * @param file_path File path to check
 */
void ensure_deep_parent(const std::string &file_path);

/**
 * @brief Returns a list of files that match the provided pattern using glob
 *
 * @param pattern Pattern to search for
 * @param include_paths True to to return absolute paths; false to return
 * filenames
 * @return std::vector<std::string>
 */
std::vector<std::string> ls(const std::string &pattern,
                            bool include_paths = true);

}  // namespace io

}  // namespace templimiter
