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
 * @file argument-error.cc
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides the templimiter::error::ArgumentError class
 * @date created 2019-02-04
 * @date modified 2019-02-04
 */

#include "templimiter/error/argument-error.h"

#include <string>

#include "templimiter/error/error.h"

namespace templimiter {

namespace error {

const char *ArgumentError::name_ = "ArgumentError";

const std::string ArgumentError::make_description_(
    const std::string &arg_name, const std::string &desired_arg_type,
    const std::string &sample_arg_value,
    const std::string &additional_details) {
  std::string description = "[" + std::string(name_) + "]";

  if (arg_name == "" || desired_arg_type == "" || sample_arg_value == "") {
    description += " Unspecified argument error.";
  } else {
    description += " Invalid argument";
    if (arg_name != "") description += " (" + arg_name + ")";
    if (desired_arg_type != "")
      description += ". Expected argument of type " + desired_arg_type;
    if (sample_arg_value != "")
      description += ". Sample: <" + sample_arg_value + ">";
    description += ".";
  }

  if (additional_details != "") description += " " + additional_details;

  return description;
}

const std::string ArgumentError::name() const { return name_; }

ArgumentError::ArgumentError(const std::string &arg_name,
                             const std::string &desired_arg_type,
                             const std::string &sample_arg_value,
                             const std::string &additional_details)
    : Error(make_description_(arg_name, desired_arg_type, sample_arg_value,
                              additional_details)) {}
ArgumentError::~ArgumentError() {}

}  // namespace error

}  // namespace templimiter
