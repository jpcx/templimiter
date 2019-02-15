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
 * @file vector.cc
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides templimiter::tools functions that simplify std::vector
 * operations
 * @date created 2019-02-05
 * @date modified 2019-02-05
 */

#include "templimiter/tools/vector.h"

#include <string>
#include <vector>

#include "templimiter/tools/string.h"

namespace templimiter {

namespace tools {

bool pattern_vect_contains(const std::vector<std::string> &pattern_vect,
                           const std::string &value) {
  for (const auto &v : pattern_vect) {
    if (matches_pattern(v, value)) return true;
  }
  return false;
}

std::string join(const std::vector<std::string> &vect, const std::string &str) {
  std::string result;
  for (size_t i = 0; i < vect.size() - 1; i++) {
    result += vect[i] + str;
  }
  result += vect.back();
  return result;
}

}  // namespace tools

}  // namespace templimiter
