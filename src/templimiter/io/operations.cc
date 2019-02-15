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
 * @file operations.cc
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides templimiter::io functions that perform I/O operations.
 * @date created 2019-02-05
 * @date modified 2019-02-14
 */

#include "templimiter/io/operations.h"

#include <glob.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "templimiter/error/io-error.h"
#include "templimiter/tools/string.h"
#include "templimiter/tools/type-convert.h"
#include "templimiter/tools/vector.h"
#include "version.h"

namespace templimiter {

namespace io {

void out_helptext() {
  const std::string version = tools::to_string(TEMPLIMITER_VER_MAJOR) + "." +
                              tools::to_string(TEMPLIMITER_VER_MINOR) + "." +
                              tools::to_string(TEMPLIMITER_VER_PATCH);
  std::string helptext =
      "\ntemplimiter " + version +
      "\n\n"
      "Limits system temperature by reading thermal files, throttling the cpu, "
      "and sending SIGSTOP and SIGCONT signals.\n"
      "Please see man templimiter(8) and README.md for more details.\n\n"
      "Options:\n"
      "  -h --help\n"
      "         Print this text and exit.\n"
      "  -d --debug\n"
      "         Run normally and log debugging information to console.\n"
      "  -v --version\n"
      "         Print the version number and exit.\n"
      "  -h --which-conf\n"
      "         Print the configuration file path and exit.";
  std::cout << helptext << std::endl;
}

void out_confpath(const char *config_path) {
  std::cout << config_path << std::endl;
}

void out_version() {
  const std::string version = tools::to_string(TEMPLIMITER_VER_MAJOR) + "." +
                              tools::to_string(TEMPLIMITER_VER_MINOR) + "." +
                              tools::to_string(TEMPLIMITER_VER_PATCH);
  std::cout << version << std::endl;
}

bool file_exists(const std::string &file_path) {
  struct stat buffer;
  return (stat(file_path.c_str(), &buffer) == 0);
}

void mkdir_deep(const std::string &dir_path) {
  std::vector<std::string> spl = tools::split(dir_path, '/');
  for (size_t depth = 0; depth < spl.size(); depth++) {
    std::string cur =
        '/' + tools::join(tools::subvect(spl, 0, ptrdiff_t(depth + 1)), "/");
    if (!file_exists(cur)) {
      if (mkdir(cur.c_str(), S_IRUSR | S_IWUSR | S_IXUSR) == -1) {
        throw error::IOError(cur, "mkdir");
      }
    }
  }
}

void ensure_deep_parent(const std::string &file_path) {
  std::vector<std::string> spl = tools::split(file_path, '/');
  std::string parent_path =
      '/' + tools::join(tools::subvect(spl, 0, ptrdiff_t(spl.size() - 1)), "/");
  if (!file_exists(parent_path)) {
    mkdir_deep(parent_path);
  }
}

std::vector<std::string> ls(const std::string &pattern, bool include_paths) {
  std::vector<std::string> files;
  glob_t glob_result;
  glob(pattern.c_str(), GLOB_TILDE | GLOB_MARK, NULL, &glob_result);
  for (size_t i = 0; i < glob_result.gl_pathc; ++i) {
    std::string path = glob_result.gl_pathv[i];
    if (include_paths) {
      files.push_back(path);
    } else {
      auto spl = tools::split(path, '/');
      auto sub = tools::subvect(spl, ptrdiff_t(spl.size() - 1), 1);
      files.push_back(tools::join(sub, "/"));
    }
  }
  globfree(&glob_result);
  return files;
}

}  // namespace io

}  // namespace templimiter
