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
 * @file string.cc
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides templimiter::tools functions that simplify operations with
 * std::strings
 * @date created 2019-02-05
 * @date modified 2019-02-14
 */

#include <numeric>
#include <string>
#include <vector>

#include "templimiter/tools/string.h"
#include "templimiter/tools/vector.h"

namespace templimiter {

namespace tools {

// splits values separated by any number of occurences of ch.
// observes backslash escapes.
std::vector<std::string> split(const std::string &str, char ch) {
  std::vector<std::string> chunks;
  std::string cur = "";
  bool waiting_next = false;
  for (size_t i = 0; i < str.length(); i++) {
    if (str[i] == '\\' && str.length() > i + 1) {
      // escape, increment, continue
      cur.push_back(str[++i]);
      continue;
    }
    if (str[i] == ch) {
      // push changes
      // wait for next non-ch
      if (!waiting_next) {
        if (cur.length() > 0) {
          chunks.push_back(cur);
          cur = "";
          waiting_next = true;
        }
      }
    } else {
      // collect
      if (waiting_next) {
        waiting_next = false;
      }
      cur += str[i];
    }
  }
  if (cur.length() > 0) {
    chunks.push_back(cur);
  }
  return chunks;
}

bool matches_pattern(const std::string &pattern, const std::string &test) {
  if (pattern.find('*') == std::string::npos) {
    // no asterisks; must match exactly
    return pattern == test;
  } else {
    // asterisks. split and try to match.
    std::vector<std::string> patt_spl = split(pattern, '*');
    // patt_spl[0] must be found at position 0 if first char is not '*'
    if (pattern[0] != '*' &&
        test.compare(0, patt_spl[0].size(), patt_spl[0]) != 0) {
      return false;
    }
    // patt_spl values must be continuously found in string
    // start from last found index each iteration
    for (size_t i = 0; i < patt_spl.size(); i++) {
      auto slice = subvect(patt_spl, 0, ptrdiff_t(i));
      size_t next_idx_after_last = std::accumulate(
          slice.begin(), slice.end(), size_t(0),
          [](size_t sum, const auto &v) { return sum + v.size(); });
      size_t rem_find_idx = test.find(patt_spl[i], next_idx_after_last);
      if (rem_find_idx == std::string::npos) {
        return false;
      }
    }
    // last patt_spl must be found at last possible point in test_str
    if (pattern[pattern.length() - 1] != '*' &&
        test.find(patt_spl[patt_spl.size() - 1]) !=
            test.length() - patt_spl[patt_spl.size() - 1].length()) {
      return false;
    }
    return true;
  }
}

}  // namespace tools

}  // namespace templimiter
