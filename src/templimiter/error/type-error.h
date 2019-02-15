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
 * @file type-error.h
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides the templimiter::error::TypeError class
 * @date created 2019-02-04
 * @date modified 2019-02-07
 */

#pragma once

#include <string>

#include "templimiter/error/error.h"

namespace templimiter {

namespace error {

/** @brief Thrown in response to type conversion errors */
class TypeError : public Error {
  /** @brief Name of the error */
  static const char *name_;

  /**
   * @brief Creates description for instatiation with std::runtime_error
   *
   * @param type_to Type attempting to convert to
   * @param type_from Type attempting to convert from
   * @param additional_details Additional details, if any
   * @return const std::string
   */
  static const std::string make_description_(
      const std::string &type_to, const std::string &type_from,
      const std::string &additional_details);

 public:
  const std::string name() const override;

  /**
   * @brief Construct a new TypeError object
   *
   * @param type_to Type attempting to convert to
   * @param type_from Type attempting to convert from
   * @param additional_details Additional details, if any
   */
  TypeError(const std::string &type_to = "", const std::string &type_from = "",
            const std::string &additional_details = "");

  /** @brief Destroy the Type Error object */
  ~TypeError() override;
};

}  // namespace error

}  // namespace templimiter
