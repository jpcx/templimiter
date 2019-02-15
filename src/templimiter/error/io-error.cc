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
 * @file io-error.cc
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides the templimiter::error::IOError class
 * @date created 2019-02-04
 * @date modified 2019-02-04
 */

#include "templimiter/error/io-error.h"

#include <string>

#include "templimiter/error/error.h"

namespace templimiter {

namespace error {

const char *IOError::name_ = "IOError";

const std::string IOError::make_description_(
    const std::string &file_path, const std::string &operation,
    const std::string &additional_details) {
  std::string description = "[" + std::string(name_) + "]";

  if (file_path == "" || operation == "") {
    description += " Unspecified I/O error.";
  } else {
    description += " Unable to perform I/O operation";
    if (operation != "") description += " (" + operation + ")";
    if (file_path != "") description += " on file \"" + file_path + "\"";
    description += ".";
  }

  if (additional_details != "") description += " " + additional_details;

  return description;
}

const std::string IOError::name() const { return name_; }

IOError::IOError(const std::string &file_path, const std::string &operation,
                 const std::string &additional_details)
    : Error(make_description_(file_path, operation, additional_details)) {}
IOError::~IOError() {}

}  // namespace error

}  // namespace templimiter
