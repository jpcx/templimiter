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
 * @file file-collection.h
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides the templimiter::io::FileCollection template class
 * @date created 2019-02-06
 * @date modified 2019-02-06
 */

#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include "templimiter/error/argument-error.h"
#include "templimiter/io/file.h"
#include "templimiter/io/operations.h"

namespace templimiter {

namespace io {

/**
 * @brief Provides a container for multiple io::File objects
 *
 * @tparam T Any type to construct File objects with
 */
template <typename T>
class FileCollection {
 private:
  /** @brief Vector of shared pointers to templimiter::io::File objects */
  std::vector<std::shared_ptr<File<T>>> files_;

  /** @brief Last retreived contents from templimiter::io::File objects */
  std::vector<T> contents_;

  /** @brief Updates contents_ */
  void update_contents_() {
    contents_.clear();
    for (const auto &file : files_) {
      const std::vector<T> &file_contents = file->read();
      for (const auto &line : file_contents) {
        contents_.push_back(line);
      }
    }
  }

 public:
  /**
   * @brief Construct a new FileCollection object
   *
   * @param file_paths Vector of file paths to create templimiter::io::File
   * objects with
   * @throw templimiter::error::ArgumentError if file_paths.size() == 0
   */
  FileCollection(const std::vector<std::string> &file_paths) {
    if (file_paths.size() > 0) {
      for (const auto &path : file_paths) {
        files_.push_back(std::make_shared<File<T>>(path));
      }
    } else {
      throw error::ArgumentError(
          "file_paths", "std::vector", "{ \"/dev/null\" }",
          "No file paths provided during construction of the FileCollection.");
    }
  }

  /**
   * @brief Construct a new FileCollection object
   *
   * @param pattern Pattern to match files to and create templimiter::io::File
   * objects with
   * @throw templimiter::error::ArgumentError if no file paths were found given
   * the matcher.
   */
  FileCollection(const std::string &pattern) {
    std::vector<std::string> results = ls(pattern);
    if (results.size() > 0) {
      for (const auto &path : results) {
        files_.push_back(std::make_shared<File<T>>(path));
      }
    } else {
      throw error::ArgumentError(
          "pattern", "std::string", "\"/dev/n*ll\"",
          "No file paths found using the provided pattern used to construct "
          "the FileCollection.");
    }
  }

  /** @brief Destroy the FileCollection object */
  ~FileCollection() {}

  /** @brief Returns the number of contained file objects */
  size_t size() { return files_.size(); }

  /**
   * @brief Updates contents_ and returns a reference to it
   *
   * @return const std::vector<T>&
   */
  const std::vector<T> &read() {
    update_contents_();
    return contents_;
  }

  /**
   * @brief Appends a line to all files
   *
   * @param line Line to append
   */
  void append(const T &line) {
    for (const auto &v : files_) {
      v->append(line);
    }
  }

  /**
   * @brief Appends a line to one file
   *
   * @param file_index Index of file chosen
   * @param line Line to append
   * @throw templimiter::error::ArgumentError if file_index is greater than
   * number of files present.
   */
  void append(size_t file_index, const T &line) {
    if (file_index < files_.size()) {
      files_[file_index]->append(line);
    } else {
      throw error::ArgumentError(
          "file_index", "size_t", "2",
          "File index must be less than FileCollection size.");
    }
  }

  /**
   * @brief Overwrites a line to all files
   *
   * @param line Line to overwrite
   */
  void overwrite(const T &line) {
    for (const auto &v : files_) {
      v->overwrite(line);
    }
  }

  /**
   * @brief Overwrites a line to one file
   *
   * @param file_index Index of file chosen
   * @param line Line to overwrite
   * @throw templimiter::error::ArgumentError if file_index is greater than
   * number of files present.
   */
  void overwrite(size_t file_index, const T &line) {
    if (file_index < files_.size()) {
      files_[file_index]->overwrite(line);
    } else {
      throw error::ArgumentError(
          "file_index", "size_t", "2",
          "File index must be less than FileCollection size.");
    }
  }

  /**
   * @brief Finds the maximum value of any line retrieved from all files
   *
   * @return const T
   * @throw templimiter::error::ArgumentError if std::max_element fails (type
   * cannot be used with std::max_element)
   */
  const T max_line() {
    update_contents_();
    try {
      return *std::max_element(contents_.begin(), contents_.end());
    } catch (...) {
      throw error::ArgumentError(
          "", "(numeric)", "42",
          "Invalid argument provided to std::max_element");
    }
  }
};

}  // namespace io

}  // namespace templimiter
