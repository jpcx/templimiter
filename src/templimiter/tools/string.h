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
 * @file string.h
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides templimiter::tools functions that simplify operations with
 * std::strings
 * @date created 2019-02-05
 * @date modified 2019-02-05
 */

#pragma once

#include <string>
#include <vector>

namespace templimiter {

namespace tools {

/**
 * @brief Splits a string into a vector of strings using ch delimiter (does not
 * include ch in vector)
 *
 * @param str String to split
 * @param ch Char to split on
 * @return std::vector<std::string>
 */
std::vector<std::string> split(const std::string &str, char ch);

/**
 * @brief Checks whether or not a string matches a pattern (using asterisk
 * matching)
 *
 * @param pattern Pattern to use
 * @param test String to check
 * @return true if test matches pattern
 * @return false if test does not match pattern
 */
bool matches_pattern(const std::string &pattern, const std::string &test);

}  // namespace tools

}  // namespace templimiter
