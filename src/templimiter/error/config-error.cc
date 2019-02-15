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
 * @file config-error.cc
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides the templimiter::error::ConfigError class
 * @date created 2019-02-04
 * @date modified 2019-02-04
 */

#include "templimiter/error/config-error.h"

#include <string>

#include "templimiter/error/error.h"
#include "templimiter/tools/vector.h"

namespace templimiter {

namespace error {

const char *ConfigError::name_ = "ConfigError";

const std::string ConfigError::make_description_(
    const std::string &tag, const std::string &value,
    const std::string &additional_details) {
  std::string description = "[" + std::string(name_) + "]";

  if (tag == "" && value == "") {
    description += " Unspecified configuration error.";
  } else {
    description += " Invalid configuration value";
    if (value != "") description += " (" + value + ")";
    if (tag != "") description += " at tag <" + tag + ">";
    description += ".";
  }

  if (additional_details != "") description += " " + additional_details;

  return description;
}

const std::string ConfigError::name() const { return name_; }

ConfigError::ConfigError(const std::string &tag, const std::string &value,
                         const std::string &additional_details)
    : Error(make_description_(tag, value, additional_details)) {}
ConfigError::~ConfigError() {}

}  // namespace error

}  // namespace templimiter
