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
 * @file logger.cc
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Provides templimiter::daemon::Logger class
 * @date created 2019-01-31
 * @date modified 2019-02-14
 */

#include "templimiter/daemon/logger.h"

#include <string>

#include "templimiter/io/file.h"
#include "templimiter/tools/type-convert.h"
#include "version.h"

namespace templimiter {

namespace daemon {

std::string Logger::gen_timestamp_() {
  time_t now;
  time(&now);
  char buf[25];
  strftime(buf, 25, "%FT%T%z", localtime(&now));
  return buf;
}

void Logger::write_welcome_text_() {
  const std::string START_TXT = "    Starting Templimiter ";
  const std::string BORDER_TL = "╔";
  const std::string BORDER_TR = "╗";
  const std::string BORDER_BL = "╚";
  const std::string BORDER_BR = "╝";
  const std::string BORDER_HM = "═";
  const std::string BORDER_VM = "║";
  const std::string RIGHT_PAD = "    ";
  std::string version = tools::to_string(TEMPLIMITER_VER_MAJOR) + "." +
                        tools::to_string(TEMPLIMITER_VER_MINOR) + "." +
                        tools::to_string(TEMPLIMITER_VER_PATCH);
  std::string top;
  std::string middle = BORDER_VM + START_TXT + version + RIGHT_PAD + BORDER_VM;
  std::string bottom;

  top += BORDER_TL;
  bottom += BORDER_BL;
  for (size_t i = 0; i < 34 + version.size() - 5; i++) {
    top += BORDER_HM;
    bottom += BORDER_HM;
  }
  top += BORDER_TR;
  bottom += BORDER_BR;

  log(top);
  log(middle);
  log(bottom);
}

Logger::Logger(const std::shared_ptr<Config> &cfg, bool is_debug_mode)
    : cfg_(cfg),
      is_debug_mode_(is_debug_mode),
      logfile_(io::File<std::string>(cfg->log_file_path())) {
  write_welcome_text_();
}

}  // namespace daemon

}  // namespace templimiter
