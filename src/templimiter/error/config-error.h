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
 * @file config-error.h
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides the templimiter::error::ConfigError class
 * @date created 2019-02-04
 * @date modified 2019-02-04
 */

#pragma once

#include <string>
#include <vector>

#include "templimiter/error/error.h"

namespace templimiter {

namespace error {

/** @brief Thrown in response to configuration errors */
class ConfigError : public Error {
  /** @brief Name of the error */
  static const char *name_;

  /**
   * @brief Creates description for instatiation with std::runtime_error
   *
   * @param tag Configuration tag
   * @param value Configuration value
   * @param additional_details Additional details, if any
   * @return const std::string
   */
  static const std::string make_description_(
      const std::string &tag, const std::string &value,
      const std::string &additional_details);

 public:
  const std::string name() const override;

  /**
   * @brief Construct a new Config Error object
   *
   * @param tag Configuration tag
   * @param value Configuration value
   * @param additional_details Additional details, if any
   */
  ConfigError(const std::string &tag = "", const std::string &value = "",
              const std::string &additional_details = "");

  /** @brief Destroy the ConfigError object */
  ~ConfigError() override;
};

}  // namespace error

}  // namespace templimiter
