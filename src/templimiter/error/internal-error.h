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
 * @file internal-error.h
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides the templimiter::error::InternalError class
 * @date created 2019-02-04
 * @date modified 2019-02-04
 */

#pragma once

#include <string>

#include "templimiter/error/error.h"

namespace templimiter {

namespace error {

/** @brief Thrown in response to internal errors */
class InternalError : public Error {
  /** @brief Name of the error */
  static const char *name_;

  /**
   * @brief Creates description for instatiation with std::runtime_error
   *
   * @param details Details of the error
   * @return const std::string
   */
  static const std::string make_description_(const std::string &details);

 public:
  const std::string name() const override;

  /**
   * @brief Construct a new InternalError object
   *
   * @param details Details of the error
   */
  InternalError(const std::string &details = "");

  /** @brief Destroy the InternalError object */
  ~InternalError() override;
};

}  // namespace error

}  // namespace templimiter
