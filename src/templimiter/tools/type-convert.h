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
 * @file type-convert.h
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides templimiter::tools functions that convert between types
 * @date created 2019-02-05
 * @date modified 2019-02-09
 */

#pragma once

#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "templimiter/error/type-error.h"
#include "templimiter/tools/vector.h"

namespace templimiter {

namespace tools {

/**
 * @brief Converts any value (not a string or a const char * or a bool) to a
 * string
 *
 * @tparam T Any type (not a string or const char * or a bool)
 * @param input Input to convert to string
 * @return std::string
 */
template <typename T, std::enable_if_t<!std::is_same_v<std::string, T> &&
                                       !std::is_same_v<const char *, T> &&
                                       !std::is_same_v<bool, T>> * = nullptr>
std::string to_string(const T &input) {
  std::ostringstream oss;
  oss << input;
  return oss.str();
}

/**
 * @brief Returns the input of any (string to string) and (const char * to
 * string) conversion attempts
 *
 * @tparam T std::string or const char *
 * @param input Input attempting to convert to string
 * @return std::string
 */
template <typename T,
          std::enable_if_t<std::is_same_v<std::string, T> ||
                           std::is_same_v<const char *, T>> * = nullptr>
std::string to_string(const T &input) {
  return input;
}

/**
 * @brief Converts any bool to string
 *
 * @tparam T bool
 * @param input Input attempting to convert to string
 * @return std::string
 */
template <typename T, std::enable_if_t<std::is_same_v<bool, T>> * = nullptr>
std::string to_string(const T &input) {
  std::ostringstream oss;
  oss << std::boolalpha << input;
  return oss.str();
}

/**
 * @brief Converts any precise value (not a string or a const char * or a bool)
 * to a string
 *
 * @tparam T Any type (not a string or const char * or a bool)
 * @param input Input to convert to string
 * @param precision Precision specified for ostringstream
 * @return std::string
 */
template <typename T, std::enable_if_t<!std::is_same_v<std::string, T> &&
                                       !std::is_same_v<const char *, T> &&
                                       !std::is_same_v<bool, T>> * = nullptr>
std::string to_string(const T &input, const std::streamsize precision) {
  std::ostringstream oss;
  oss.precision(precision);
  oss << input;
  return oss.str();
}

/**
 * @brief Returns the input of any (string to string) and (const char * to
 * string) precise conversion attempts
 *
 * @tparam T std::string or const char *
 * @param input Input attempting to convert to string
 * @param precision Precision specified for ostringstream (unused)
 * @return std::string
 */
template <typename T,
          std::enable_if_t<std::is_same_v<std::string, T> ||
                           std::is_same_v<const char *, T>> * = nullptr>
std::string to_string(const T &input,
                      [[maybe_unused]] std::streamsize precision) {
  return input;
}

/**
 * @brief Converts any bool to string (with precision mistakenly specified)
 *
 * @tparam T bool
 * @param input Input attempting to convert to string
 * @param precision Precision specified for ostringstream (unused)
 * @return std::string
 */
template <typename T, std::enable_if_t<std::is_same_v<bool, T>> * = nullptr>
std::string to_string(const T &input,
                      [[maybe_unused]] std::streamsize precision) {
  std::ostringstream oss;
  oss << std::boolalpha << input;
  return oss.str();
}

/**
 * @brief Converts any value of any type to another type, so long as the types
 * to/from are not the same and the conversion is not from a const char * to a
 * string, a bool to a string, or a string to a bool
 *
 * @tparam T_to Type converting to
 * @tparam T_from Type converting from
 * @param input Input to convert
 * @return T_to
 * @throw templimiter::error::TypeError if type conversion fails
 */
template <
    typename T_to, typename T_from,
    std::enable_if_t<
        !((std::is_same_v<const char *, T_from> &&
           std::is_same_v<std::string, T_to>) ||
          (std::is_same_v<bool, T_from> && std::is_same_v<std::string, T_to>) ||
          (std::is_same_v<std::string, T_from> && std::is_same_v<bool, T_to>) ||
          std::is_same_v<T_to, T_from>)> * = nullptr>
T_to convert(const T_from &input) {
  // define output
  T_to output;
  // create conversion stream
  std::istringstream iss(to_string(input));
  // stream iss_input to output
  iss >> output;
  // throw if output does not match input
  if (to_string(output) != iss.str()) {
    throw error::TypeError(typeid(T_to).name(), typeid(T_from).name());
  }
  return output;
}

/**
 * @brief Returns the input of any conversion attempts from/to the same type and
 * from a const char * to a string
 *
 * @tparam T_to Type converting to
 * @tparam T_from Type converting from
 * @param input Input to convert
 * @return T_to
 */
template <typename T_to, typename T_from,
          std::enable_if_t<(std::is_same_v<const char *, T_from> &&
                            std::is_same_v<std::string, T_to>) ||
                           std::is_same_v<T_to, T_from>> * = nullptr>
T_to convert(const T_from &input) {
  return input;
}

/**
 * @brief Handles any bool to string conversion attempts
 *
 * @tparam T_to Type converting to
 * @tparam T_from Type converting from
 * @param input Input to convert
 * @return T_to
 */
template <typename T_to, typename T_from,
          std::enable_if_t<std::is_same_v<bool, T_from> &&
                           std::is_same_v<std::string, T_to>> * = nullptr>
T_to convert(const T_from &input) {
  return to_string(input);
}

/**
 * @brief Handles any string to bool conversion attempts
 *
 * @tparam T_to Type converting to
 * @tparam T_from Type converting from
 * @param input Input to convert
 * @return T_to
 */
template <typename T_to, typename T_from,
          std::enable_if_t<std::is_same_v<std::string, T_from> &&
                           std::is_same_v<bool, T_to>> * = nullptr>
T_to convert(const T_from &input) {
  bool output;
  std::istringstream iss;
  iss.str(input);
  iss >> std::boolalpha >> output;
  // check if output has been converted successfully
  if (to_string(output) != iss.str()) {
    throw error::TypeError(typeid(T_to).name(), typeid(T_from).name());
  }
  return output;
}

/**
 * @brief Converts any precise value of any type to another type, so long as the
 * types to/from are not the same and the conversion is not from a const char *
 * to a string, a bool to a string, or a string to a bool
 *
 * @tparam T_to Type converting to
 * @tparam T_from Type converting from
 * @param input Input to convert
 * @param precision Precision specified for ostringstream
 * @return T_to
 * @throw templimiter::error::TypeError if type conversion fails
 */
template <
    typename T_to, typename T_from,
    std::enable_if_t<
        !((std::is_same_v<const char *, T_from> &&
           std::is_same_v<std::string, T_to>) ||
          (std::is_same_v<bool, T_from> && std::is_same_v<std::string, T_to>) ||
          (std::is_same_v<std::string, T_from> && std::is_same_v<bool, T_to>) ||
          std::is_same_v<T_to, T_from>)> * = nullptr>
T_to convert(const T_from &input, std::streamsize precision) {
  // define output
  T_to output;
  // create conversion stream
  std::istringstream iss;
  iss.precision(precision);
  iss.str(to_string(input, precision));
  // stream iss_input to output
  iss >> output;
  // throw if output does not match input
  if (to_string(output, precision) != iss.str()) {
    throw error::TypeError(typeid(T_to).name(), typeid(T_from).name());
  }
  return output;
}

/**
 * @brief Returns the input of any conversion attempts from/to the same type and
 * from a const char * to a string
 *
 * @tparam T_to Type converting to
 * @tparam T_from Type converting from
 * @param input Input to convert
 * @param precision Precision specified for ostringstream (unused)
 * @return T_to
 */
template <typename T_to, typename T_from,
          std::enable_if_t<(std::is_same_v<const char *, T_from> &&
                            std::is_same_v<std::string, T_to>) ||
                           std::is_same_v<T_to, T_from>> * = nullptr>
T_to convert(const T_from &input, [[maybe_unused]] std::streamsize precision) {
  return input;
}

/**
 * @brief Handles any bool to string conversion attempts (with precision
 * mistakenly specified)
 *
 * @tparam T_to Type converting to
 * @tparam T_from Type converting from
 * @param input Input to convert
 * @param precision Precision specified for ostringstream (unused)
 * @return T_to
 */
template <typename T_to, typename T_from,
          std::enable_if_t<std::is_same_v<bool, T_from> &&
                           std::is_same_v<std::string, T_to>> * = nullptr>
T_to convert(const T_from &input, [[maybe_unused]] std::streamsize precision) {
  return to_string(input);
}

/**
 * @brief Handles any string to bool conversion attempts (with precision
 * mistakenly specified)
 *
 * @tparam T_to Type converting to
 * @tparam T_from Type converting from
 * @param input Input to convert
 * @return T_to
 */
template <typename T_to, typename T_from,
          std::enable_if_t<std::is_same_v<std::string, T_from> &&
                           std::is_same_v<bool, T_to>> * = nullptr>
T_to convert(const T_from &input, [[maybe_unused]] std::streamsize precision) {
  bool output;
  std::istringstream iss;
  iss.str(input);
  iss >> std::boolalpha >> output;
  // check if output has been converted successfully
  if (to_string(output) != iss.str()) {
    throw error::TypeError(typeid(T_to).name(), typeid(T_from).name());
  }
  return output;
}

/**
 * @brief Converts any vector of any type to a vector of another type, so long
 * as the types to/from are not the same and the conversion is not from a const
 * char * to a string
 *
 * @tparam T_to Type converting to
 * @tparam T_from Type converting from
 * @param input Input to convert
 * @return T_to
 * @throw templimiter::error::TypeError if type conversion fails
 */
template <typename T_to, typename T_from,
          std::enable_if_t<!((std::is_same_v<const char *, T_from> &&
                              std::is_same_v<std::string, T_to>) ||
                             std::is_same_v<T_to, T_from>)> * = nullptr>
std::vector<T_to> convert(const std::vector<T_from> &input) {
  std::vector<T_to> output;
  for (const auto &v : input) {
    output.push_back(convert<T_to>(v));
  }
  return output;
}

/**
 * @brief Returns the input of any conversion attempts from/to the same type and
 * from a const char * to a string
 *
 * @tparam T_to Type converting to
 * @tparam T_from Type converting from
 * @param input Input to convert
 * @return T_to
 */
template <typename T_to, typename T_from,
          std::enable_if_t<(std::is_same_v<const char *, T_from> &&
                            std::is_same_v<std::string, T_to>) ||
                           std::is_same_v<T_to, T_from>> * = nullptr>
std::vector<T_to> convert(const std::vector<T_from> &input) {
  return input;
}

/**
 * @brief Converts any vector of any type to a vector of another type, so long
 * as the types to/from are not the same and the conversion is not from a const
 * char * to a string
 *
 * @tparam T_to Type converting to
 * @tparam T_from Type converting from
 * @param input Input to convert
 * @param precision Precision specified for ostringstream
 * @return T_to
 * @throw templimiter::error::TypeError if type conversion fails
 */
template <typename T_to, typename T_from,
          std::enable_if_t<!((std::is_same_v<const char *, T_from> &&
                              std::is_same_v<std::string, T_to>) ||
                             std::is_same_v<T_to, T_from>)> * = nullptr>
std::vector<T_to> convert(const std::vector<T_from> &input,
                          std::streamsize precision) {
  std::vector<T_to> output;
  for (const auto &v : input) {
    output.push_back(convert<T_to>(v, precision));
  }
  return output;
}

/**
 * @brief Returns the input of any conversion attempts from/to the same type and
 * from a const char * to a string
 *
 * @tparam T_to Type converting to
 * @tparam T_from Type converting from
 * @param input Input to convert
 * @param precision Precision specified for ostringstream (unused)
 * @return T_to
 */
template <typename T_to, typename T_from,
          std::enable_if_t<(std::is_same_v<const char *, T_from> &&
                            std::is_same_v<std::string, T_to>) ||
                           std::is_same_v<T_to, T_from>> * = nullptr>
std::vector<T_to> convert(const std::vector<T_from> &input,
                          [[maybe_unused]] std::streamsize precision) {
  return input;
}

}  // namespace tools

}  // namespace templimiter
