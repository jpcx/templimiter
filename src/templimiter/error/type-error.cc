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
 * @file type-error.cc
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides the templimiter::error::TypeError class
 * @date created 2019-02-04
 * @date modified 2019-02-07
 */

#include "templimiter/error/type-error.h"

#include <string>

#include "templimiter/error/error.h"

namespace templimiter {

namespace error {

const char *TypeError::name_ = "TypeError";

const std::string TypeError::make_description_(
    const std::string &type_to, const std::string &type_from,
    const std::string &additional_details) {
  std::string description = "[" + std::string(name_) + "]";

  if (type_to == "" && type_from == "" && additional_details == "")
    description += " Unspecified type error.";
  else if (type_to != "" && type_from != "")
    description +=
        " Error converting to <" + type_to + "> from <" + type_from + ">.";
  else if (type_to != "")
    description += " Error converting to <" + type_to + ">.";
  else if (type_from != "")
    description += " Error converting from <" + type_from + ">.";

  if (additional_details != "") description += " " + additional_details;

  return description;
}

const std::string TypeError::name() const { return name_; }

TypeError::TypeError(const std::string &type_to, const std::string &type_from,
                     const std::string &additional_details)
    : Error(make_description_(type_to, type_from, additional_details)) {}
TypeError::~TypeError() {}

}  // namespace error

}  // namespace templimiter
