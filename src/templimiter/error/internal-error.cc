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
 * @file internal-error.cc
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides the templimiter::error::InternalError class
 * @date created 2019-02-04
 * @date modified 2019-02-04
 */

#include "templimiter/error/internal-error.h"

#include <string>

#include "templimiter/error/error.h"

namespace templimiter {

namespace error {

const char *InternalError::name_ = "InternalError";

const std::string InternalError::make_description_(const std::string &details) {
  std::string description = "[" + std::string(name_) + "]";

  if (details == "") {
    description += " Unspecified argument error.";
  } else {
    description += " " + details;
  }

  return description;
}

const std::string InternalError::name() const { return name_; }

InternalError::InternalError(const std::string &details)
    : Error(make_description_(details)) {}
InternalError::~InternalError() {}

}  // namespace error

}  // namespace templimiter
