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
 * @file main.cc
 * @author Justin Collier (jpcxist@gmail.com)
 * @brief Limits system temperature by reading thermal files, throttling
 * the cpu, and sending SIGSTOP and SIGCONT signals.
 * @version 0.1.0
 * @date created 2019-01-31
 * @date modified 2019-02-14
 * @copyright Copyright (c) 2019 Justin Collier
 */

#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>

#include "templimiter/daemon/config.h"
#include "templimiter/daemon/logger.h"
#include "templimiter/daemon/monitor.h"
#include "templimiter/daemon/pid.h"
#include "templimiter/error/error.h"
#include "templimiter/io/operations.h"

#ifndef TEMPLIMITER_CONFIG_PATH
#define TEMPLIMITER_CONFIG_PATH "/usr/local/etc/conf.d/templimiter.conf"
#endif

/**
 * @brief Main function for templimiter daemon
 *
 * @return int
 */
int main(int argc, char *argv[]) {
  using namespace templimiter;

  if (argc > 2) {
    io::err(
        "Multiple arguments supplied to templimiter. Only the first will be "
        "accepted.");
  }

  // Check for --help flag
  if (argc > 1 &&
      (strncmp(argv[1], "-h", 3) == 0 || strncmp(argv[1], "--help", 7) == 0)) {
    io::out_helptext();
    return 0;
  }

  // Check for --version flag
  if (argc > 1 && (strncmp(argv[1], "-v", 3) == 0 ||
                   strncmp(argv[1], "--version", 10) == 0)) {
    io::out_version();
    return 0;
  }

  // Check for --which-conf flag
  if (argc > 1 && (strncmp(argv[1], "-w", 3) == 0 ||
                   strncmp(argv[1], "--which-conf", 7) == 0)) {
    io::out_confpath(TEMPLIMITER_CONFIG_PATH);
    return 0;
  }

  // Try/catch initialization procedures; logs go to stdout
  try {
    bool is_debug_mode = argc > 1 && (strncmp(argv[1], "-d", 3) == 0 ||
                                      strncmp(argv[1], "--debug", 8) == 0);

    std::shared_ptr<daemon::Config> cfg =
        std::make_shared<daemon::Config>(TEMPLIMITER_CONFIG_PATH);
    std::shared_ptr<daemon::Logger> out =
        std::make_shared<daemon::Logger>(cfg, is_debug_mode);

    // Try/catch daemon::Monitor; logs go to established logger
    try {
      const daemon::Monitor mon(cfg, out);
    } catch (const error::Error &e) {
      out->err(e.what());
      return 1;
    } catch (const std::runtime_error &e) {
      out->err("templimiter has encountered an unknown exception:");
      out->err(e.what());
      return 1;
    } catch (const char *e) {
      out->err("templimiter has encountered an unknown exception:");
      out->err(e);
      return 1;
    } catch (const std::string &e) {
      out->err("templimiter has encountered an unknown exception:");
      out->err(e);
      return 1;
    } catch (...) {
      out->err(
          "templimiter has encountered an unknown exception. No details "
          "available.");
      return 1;
    }
  } catch (const error::Error &e) {
    io::err(e.what());
    return 1;
  } catch (const std::runtime_error &e) {
    io::err("templimiter has encountered an unknown exception:");
    io::err(e.what());
    return 1;
  } catch (const char *e) {
    io::err("templimiter has encountered an unknown exception:");
    io::err(e);
    return 1;
  } catch (const std::string &e) {
    io::err("templimiter has encountered an unknown exception:");
    io::err(e);
    return 1;
  } catch (...) {
    io::err(
        "templimiter has encountered an unknown exception. No details "
        "available.");
    return 1;
  }
  return 0;
}
