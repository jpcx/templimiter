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
 * @file error.h
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides the templimiter::error::Error virtual class
 * @date created 2019-02-04
 * @date modified 2019-02-04
 */

#pragma once

#include <stdexcept>
#include <string>

namespace templimiter {

namespace error {

/** @brief Virtual class for all errors */
class Error : public std::runtime_error {
 public:
  /**
   * @brief Returns the name of the error
   *
   * @return const std::string
   */
  virtual const std::string name() const = 0;

  /**
   * @brief Construct a new Error object
   *
   * @param description Message to use when constructing std::runtime_error
   */
  Error(const std::string &description);

  /** @brief Destroy the Error object */
  virtual ~Error();
};

}  // namespace error

}  // namespace templimiter
