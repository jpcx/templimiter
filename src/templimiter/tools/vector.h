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
 * @file vector.h
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides templimiter::tools functions that simplify std::vector
 * operations
 * @date created 2019-02-05
 * @date modified 2019-02-14
 */

#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "templimiter/error/argument-error.h"

namespace templimiter {

namespace tools {

/**
 * @brief Returns a subvector copy of a given vector
 *
 * @tparam T Type of vector
 * @param vect Vector to subvect
 * @param start_index Start index of operation
 * @param n_elements Number of elements to copy
 * @return std::vector<T>
 * @throw templimiter::error::ArgumentError if start_index is less than zero
 * @throw templimiter::error::ArgumentError if n_elements is less than zero
 * @throw templimiter::error::ArgumentError if start_index falls off data
 * @throw templimiter::error::ArgumentError if n_elements falls off data
 */
template <typename T>
std::vector<T> subvect(const std::vector<T> &vect, ptrdiff_t start_index,
                       ptrdiff_t n_elements) {
  // Throw if starting at negative index
  if (start_index < 0) {
    throw error::ArgumentError(
        "start_index", "ptrdiff_t", "0",
        "Start index of subvection must be at least zero.");
  }

  // Throw if selecting negative elements
  if (n_elements < 0) {
    throw error::ArgumentError(
        "n_elements", "ptrdiff_t", "1",
        "Number of subvection elements must be at least zero.");
  }

  // Return empty vector if not selecting any elements
  if (n_elements == 0) return std::vector<T>();

  // Define new iterators
  auto new_begin = vect.begin() + start_index;
  auto new_end = vect.begin() + start_index + n_elements;

  // Throw if starting off of the vector
  if (new_begin >= vect.end()) {
    throw error::ArgumentError("start_index", "ptrdiff_t", "0",
                               "Start index must reside on the vector.");
  }

  // Throw if selection exceeds vector size
  if (new_end > vect.end()) {
    throw error::ArgumentError("n_elements", "ptrdiff_t", "1",
                               "Too many elements selected for subvection.");
  }

  // Return new vector
  std::vector<T> new_vect(new_begin, new_end);
  return new_vect;
}

/**
 * @brief Performs a lambda operation on each element of a vector and returns a
 * copy with results
 *
 * @tparam T Type of vector
 * @tparam Lambda Lambda function to use (input element) { return modified
 * element }
 * @param vect Vector to perform operations with
 * @param transformer Lambda function to use (input element) { return modified
 * element }
 * @return std::vector<T>
 */
template <typename T, typename Lambda>
std::vector<T> map(const std::vector<T> &vect, const Lambda &&transformer) {
  std::vector<T> new_vect;
  std::transform(vect.begin(), vect.end(), std::back_inserter(new_vect),
                 transformer);
  return new_vect;
}

/**
 * @brief Filters elements of a vector using a lambda function and returns a
 * copy with filtered elements
 *
 * @tparam T Type of vector
 * @tparam Lambda Lambda function to use (input element) { return true if
 * element should remain }
 * @param target Vector to perform operations with
 * @param rule Lambda function to use (input element) { return true if element
 * should remain }
 * @return std::vector<T>
 */
template <typename T, typename Lambda>
std::vector<T> filter(const std::vector<T> &target, Lambda &&rule) {
  std::vector<T> new_vect;
  for (const auto &v : target) {
    if (rule(v)) {
      new_vect.push_back(v);
    }
  }
  return new_vect;
}

/**
 * @brief Checks if a vector contains a value
 *
 * @tparam T Type of vector and value
 * @param target Target vector
 * @param value Value to search for
 * @return true if value has been found
 * @return false if value has not been found
 */
template <typename T>
bool contains(const std::vector<T> &target, const T &value) {
  for (const auto &v : target) {
    if (v == value) return true;
  }
  return false;
}

/**
 * @brief Returns a filled vector of a given type with a given value
 *
 * @tparam T Type of vector and value
 * @param size Size of vector to create
 * @param fill_val Value to fill with
 * @return std::vector<T>
 */
template <typename T>
std::vector<T> fill(size_t size, const T &fill_val) {
  std::vector<T> vect;
  for (size_t i = 0; i < size; i++) {
    vect.push_back(fill_val);
  }
  return vect;
}

/**
 * @brief Checks if a vector of string patterns contains a match for a value
 *
 * @param pattern_vect Pattern vector to search through
 * @param value Value to search for a match
 * @return true if match has been found
 * @return false if match has not been found
 */
bool pattern_vect_contains(const std::vector<std::string> &pattern_vect,
                           const std::string &value);

/**
 * @brief Joins a vector of strings to a single string using delimiter str
 *
 * @param vect Vector to join
 * @param str String to join with
 * @return std::string
 */
std::string join(const std::vector<std::string> &vect, const std::string &str);

}  // namespace tools

}  // namespace templimiter
