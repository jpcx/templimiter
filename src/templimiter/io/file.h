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
 * @brief Provides the templimiter::io::File template class
 * @date created 2019-02-06
 * @date modified 2019-02-09
 */

#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "templimiter/error/argument-error.h"
#include "templimiter/error/io-error.h"
#include "templimiter/io/operations.h"
#include "templimiter/tools/type-convert.h"

namespace templimiter {

namespace io {

/**
 * @brief Creates objects that can manipulate a single file of a given type
 *
 * @tparam T Any type to automatically convert to and from
 */
template <typename T>
class File {
 private:
  /** @brief Path of the file */
  std::string path_;

  /** @brief Reusable std::ifstream for read operations */
  std::ifstream read_stream_;

  /** @brief Reusable std::ofstream for append or overwrite operations */
  std::ofstream write_stream_;

  /** @brief Whether or not the read_stream_ is open */
  bool is_open_read_ = false;

  /** @brief Whether or not the write_stream_ is open */
  bool is_open_write_ = false;

  /** @brief Vector of all file contents, line by line */
  std::vector<T> contents_;

  /** @brief Whether or not the file exists */
  bool exists_ = false;

  /**
   * @brief Asserts that the provided file path is a valid absolute filepath
   * @throw templimiter::error::ArgumentError if path_ is blank
   * @throw templimiter::error::ArgumentError if path_ is not prefixed with a
   * forward slash
   * @throw templimiter::error::ArgumentError if path_ is trailed with a forward
   * slash
   */
  void validate_() {
    if (path_.size() == 0) {
      throw error::ArgumentError(
          "file_path", "string", "/dev/null",
          "File path provided during construction of File object is blank.");
    }
    if (path_.front() != '/') {
      throw error::ArgumentError(
          "file_path", "string", "/dev/null",
          "File object must be constructed with an absolute filepath.");
    }
    if (path_.back() == '/') {
      throw error::ArgumentError(
          "file_path", "string", "/dev/null",
          "File object was constructed with a directory path.");
    }
  }

  /** @brief Closes read stream, if open */
  void close_read_() {
    if (is_open_read_) {
      read_stream_.close();
      is_open_read_ = false;
    }
  }

  /** @brief Closes write stream, if open */
  void close_write_() {
    if (is_open_write_) {
      write_stream_.close();
      is_open_write_ = false;
    }
  }

  /**
   * @brief Opens read stream
   * @throw templimiter::error::IOError if file does not exist
   * @throw templimiter::error::IOError if opening the read_stream_ fails
   */
  void open_read_() {
    if (is_open_read_) close_read_();
    if (file_exists(path_)) {
      read_stream_.open(path_);
      is_open_read_ = true;
      if (!read_stream_) {
        close_read_();
        throw error::IOError(path_, "read");
      }
    } else {
      throw error::IOError(path_, "read", "File does not exist.");
    }
  }

  /**
   * @brief Opens write stream
   *
   * @param append Whether or not to append to the file or overwrite
   * @throw templimiter::error::IOError if opening the write_stream_ fails
   */
  void open_write_(bool append = true) {
    if (is_open_write_) close_write_();
    ensure_deep_parent(path_);
    if (append) {
      write_stream_.open(path_, std::ios::app);
      is_open_write_ = true;
    } else {
      write_stream_.open(path_);
      is_open_write_ = true;
    }
    if (!write_stream_) {
      close_write_();
      throw error::IOError(path_, "write");
    }
  }

  /** @brief Updates contents_ */
  void read_to_contents_() {
    open_read_();
    std::string line;
    contents_.clear();
    try {
      while (std::getline(read_stream_, line)) {
        // always set precision to line.size()
        contents_.push_back(
            tools::convert<T>(line, std::streamsize(line.size())));
      }
      close_read_();
    } catch (...) {
      close_read_();
      throw;
    }
  }

  /**
   * @brief Updates contents_ with precise data
   *
   * @param precision Precision to use
   */
  void read_to_contents_(std::streamsize precision) {
    std::streamsize precision_base = read_stream_.precision();
    read_stream_.precision(precision);
    try {
      read_to_contents_();
      read_stream_.precision(precision_base);
    } catch (...) {
      read_stream_.precision(precision_base);
      throw;
    }
  }

 public:
  /**
   * @brief Construct a new File object
   *
   * @param file_path File path to construct file with
   */
  explicit File(const std::string &file_path)
      : path_(file_path), exists_(file_exists(file_path)) {
    validate_();
  }

  /** @brief Destroy the File object */
  ~File() {
    close_read_();
    close_write_();
  }

  /** @brief Gets whether or not the file exists */
  bool exists() const { return exists_; }

  /** @brief Gets the file path */
  const std::string &path() const { return path_; }

  /**
   * @brief Updates contents_ and returns a reference to it
   *
   * @return const std::vector<T>&
   */
  const std::vector<T> &read() {
    read_to_contents_();
    return contents_;
  }

  /**
   * @brief Updates contents_ with precise data and returns a reference to it
   *
   * @param precision Precision to use
   * @return const std::vector<T>&
   */
  const std::vector<T> &read(std::streamsize precision) {
    read_to_contents_(precision);
    return contents_;
  }

  /**
   * @brief Appends data to the file
   *
   * @param data Data to append
   * @throw templimiter::error::IOError if writing to the write_stream_ fails
   */
  void append(const T &data) {
    open_write_();
    try {
      write_stream_ << data << std::endl;
      close_write_();
    } catch (...) {
      close_write_();
      throw error::IOError(path_, "write", "Cannot append to file.");
    }
  }

  /**
   * @brief Appends precise data to the file
   *
   * @param data Data to append
   * @param precision Precision to use
   */
  void append(const T &data, std::streamsize precision) {
    std::streamsize precision_base = write_stream_.precision();
    write_stream_.precision(precision);
    try {
      append(data);
      write_stream_.precision(precision_base);
    } catch (...) {
      write_stream_.precision(precision_base);
      throw;
    }
  }

  /**
   * @brief Appends a vector of data to the file, line by line
   *
   * @param data Data to append
   * @throw templimiter::error::IOError if writing to the write_stream_ fails
   */
  void append(const std::vector<T> &data) {
    open_write_();
    try {
      for (const auto &v : data) {
        write_stream_ << v << std::endl;
      }
      close_write_();
    } catch (...) {
      close_write_();
      throw error::IOError(path_, "write", "Cannot append to file.");
    }
  }

  /**
   * @brief Appends a vector of precise data to the file, line by line
   *
   * @param data Data to append
   * @param precision Precision to use
   */
  void append(const std::vector<T> &data, std::streamsize precision) {
    std::streamsize precision_base = write_stream_.precision();
    write_stream_.precision(precision);
    try {
      append(data);
      write_stream_.precision(precision_base);
    } catch (...) {
      write_stream_.precision(precision_base);
      throw;
    }
  }

  /**
   * @brief Overwrites all data in a file
   *
   * @param data Data to overwrite with
   * @throw templimiter::error::IOError if writing to the write_stream_ fails
   */
  void overwrite(const T &data) {
    open_write_(false);
    try {
      write_stream_ << data << std::endl;
      close_write_();
    } catch (...) {
      close_write_();
      throw error::IOError(path_, "write", "Cannot overwrite file.");
    }
  }

  /**
   * @brief Overwrites all data in a file with precise data
   *
   * @param data Data to overwrite with
   * @param precision Precision to use
   */
  void overwrite(const T &data, std::streamsize precision) {
    std::streamsize precision_base = write_stream_.precision();
    write_stream_.precision(precision);
    try {
      overwrite(data);
      write_stream_.precision(precision_base);
    } catch (...) {
      write_stream_.precision(precision_base);
      throw;
    }
  }

  /**
   * @brief overwrites a vector of data to the file, line by line
   *
   * @param data Data to overwrite with
   * @throw templimiter::error::IOError if writing to the write_stream_ fails
   */
  void overwrite(const std::vector<T> &data) {
    open_write_(false);
    try {
      for (const auto &v : data) {
        write_stream_ << v << std::endl;
      }
      close_write_();
    } catch (...) {
      close_write_();
      throw error::IOError(path_, "write", "Cannot overwrite file.");
    }
  }

  /**
   * @brief overwrites a vector of precise data to the file, line by line
   *
   * @param data Data to overwrite with
   */
  void overwrite(const std::vector<T> &data, std::streamsize precision) {
    std::streamsize precision_base = write_stream_.precision();
    write_stream_.precision(precision);
    try {
      overwrite(data);
      write_stream_.precision(precision_base);
    } catch (...) {
      write_stream_.precision(precision_base);
      throw;
    }
  }
};

}  // namespace io

}  // namespace templimiter
