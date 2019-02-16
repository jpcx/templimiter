# templimiter

Limits system temperature by reading thermal files, throttling the cpu, and sending SIGSTOP and SIGCONT signals.

## Description

This software was written for systems with inadequate hardware cooling. By constantly monitoring system thermal files, it is able to respond to dangerous temperature levels by either throttling the CPU or sending SIGSTOP signals to the highest CPU-consuming processes. If the temperature is below a given threshold, templimiter will dethrottle the CPU to maximum speeds and send SIGCONT signals to the processes stopped by the program.

All configuration is established via the templimiter.conf file. Path is defined during the pre-make `./configure` step. Use the `--which-conf` option to find its location on your system. Please refer to the [configuration](#configuration) section for all settings.

This program is meant to be run as a persistent daemon and is already configured to use systemd if possible. It is advisable to run the program with `--debug` first in order to verify that no configuration issues are preventing execution __before__ enabling the daemon. The daemon is configured to relaunch on exit.

## Installation

### Prerequisites

+ Linux
+ __c++17__
+ pkg-config
  + Necessary for automatic systemd configuration.
  + Manual configuration of systemd directory can still be acheived without pkg-config via `./configure --with-systemdsystemunitdir=DIR`

### Getting the Source

Clone the repo (or download the source archive).

```bash
git clone https://github.com/jpcx/templimiter.git
cd templimiter
```

### Building

_Note: This software uses autotools for building. Due to an issue regarding timestamp preservation, `./configure` may not work immediately._

If make fails with a 'not found' message:

+ Use the source archive instead of cloning the repository (timestamps are preserved).
+ Execute `sh scripts/fix-timestamps.sh` before running configure and make.

```bash
./configure
make
sudo make install
```

### Setup

```bash
# Edit the configuration, if desired
sudo nano $(templimiter --which-conf)
```

### Verification

_NOTE: Setting low and tight thresholds for the following debugging step is recommended._

```bash
# Check for errors
sudo templimiter --debug
# In another terminal, stimulate the CPU
dd if=/dev/zero of=/dev/null bs=8M status=progress
```

_NOTE: During debug mode, if the thresholds are set appropriately you should see output signifying that actions were taken to limit temperature. Use `^C` to exit._

### Daemon Initialization

```bash
# If it runs as expected, enable the service
sudo systemctl enable templimiter
```

## Usage

templimiter is meant to be run as a daemon; simply start the service if you are using systemd.

___Note: Proper debugging is advised before enabling; the daemon is set to relaunch on failure.___

```bash
sudo systemctl start templimiter.service
```

Functionality remains the same if running without systemd.

## Modes

templimiter has two major modes of operation: throttle and SIGSTOP.

### Throttle

Throttle mode modifies scaling_max_freq files in response to temperature changes, which sets a ceiling to CPU speed. There are two methods of doing this- scaling mode and minmax mode:

+ __Scaling mode:__
  + Reads the scaling_available_frequencies file for available frequencies that the CPU can scale to.
    + Each iteration above the throttle threshold temperature will decrease the CPU speeds by one step.
    + Each iteration below the dethrottle threshold temperature will increase the CPU speeds by one step.
  + _This is the most fluid method of temperature limiting, and is recommended if possible._
    + As such, it is enabled by default.
    + If the system does not have scaling_available_frequencies files, the mode will be disabled and warn the console.
+ __Minmax mode:__
  + Reads the cpuinfo_max_freq files and cpuinfo_min_freq files for information on maximum and minimum cpu frequencies.
  + If the temperature exceeds the throttle threshold temperature CPU speed will be set to the minimums.
  + If the temperature exceeds the dethrottle threshold temperature CPU speed will be set to the maximums.

### SIGSTOP

SIGSTOP mode sends SIGSTOP signals to non-whitelisted processes in response to temperatures. There are two configuration options related to the operation: stepwise SIGSTOP and stepwise SIGCONT.

__WARNING: This mode can cause a lot of problems in the system if the whitelist is not configured perfectly. It is recommended that the SIGSTOP threshold be set to a high value or not used. It is disabled by default.__

+ __stepwise_SIGSTOP = true:__
  + Loads all available processes and picks the highest non-whitelisted process for sending the signal. Repeats on each iteration above the SIGSTOP threshold.
+ __stepwise_SIGSTOP = false:__
  + Upon reaching the SIGSTOP threshold, stops all non-whitelisted processes at once.
+ __stepwise_SIGCONT = true:__
  + Upon decreasing past the SIGCONT threshold, sends a SIGCONT signal to the lowest cpu-consuming process in the list of processes that have been sent a SIGSTOP signal by templimiter.
+ __stepwise_SIGCONT = false:__
  + Upon decreasing past the SIGCONT threshold, sends a SIGCONT signal to _all_ of the processes sent a SIGSTOP signal by templimiter.

## Documentation

### Configuration

A default configuration file is loaded into the correct location during the installation steps. To use a custom configuration directory, use `--with-templimiterconfdir=DIR` when running `./configure`.

#### Syntax

Configuration values must not be broken by newlines, but the number of spaces between tags/values does not matter.

__Default configuration:__

```conf
log_file_path            /var/log/templimiter.log
whitelist_comm           dnsmasq systemd (sd-pam) startx xinit Xorg dbus-daemon rtkit-daemon at-spi-bus-laun at-spi2-registr wpa_supplicant dhcpcd systemd-journal lvmetad systemd-udevd upowerd systemd-timesyn systemd-machine firewalld systemd-logind polkitd haveged systemd-resolve systemd-network
whitelist_state          S D Z T t W X x K W P
whitelist_pgrp           0 1
whitelist_max_nice       -21
use_throttle             true
use_SIGSTOP              false
use_scaling_available    true
use_stepwise_SIGSTOP     true
use_stepwise_SIGCONT     false
matcher_thermal          /sys/devices/virtual/thermal/thermal_zone*/temp
matcher_scaling_max_freq /sys/devices/system/cpu/cpu*/cpufreq/scaling_max_freq
matcher_cpuinfo_max_freq /sys/devices/system/cpu/cpu*/cpufreq/cpuinfo_max_freq
matcher_cpuinfo_min_freq /sys/devices/system/cpu/cpu*/cpufreq/cpuinfo_min_freq
matcher_scaling_available_frequencies /sys/devices/system/cpu/cpu*/cpufreq/scaling_available_frequencies
temp_SIGSTOP             70000
temp_SIGCONT             66000
temp_throttle            66000
temp_dethrottle          60000
min_sleep                500
```

___Note: The execution pid is automatically added to the whitelist; the program should not stop itself.___

#### Available Tags

| Tag | Type | Description |
| --- | --- | --- |
| log_file_path | string | Location of the log file to append to or create if blank |
| whitelist_pid | int[] | List of pids to whitelist |
| whitelist_comm | string[] | List of comm values to whitelist (may use * matching) |
| whitelist_state | char[] | List of state values to whitelist |
| whitelist_ppid |  int[] | List of ppid values to whitelist |
| whitelist_pgrp | int[] | List of pgrp values to whitelist |
| whitelist_session |  int[] | List of session values to whitelist |
| whitelist_tty_nr |  int[] | List of tty_nr values to whitelist |
| whitelist_tpgid |  int[] | List of tpgid values to whitelist |
| whitelist_flags |  unsigned[] | List of flags values to whitelist |
| whitelist_max_nice | long | nice values at or below this value will be whitelisted |
| use_throttle | (true \|\| false) | Toggle for throttle mode |
| use_SIGSTOP | (true \|\| false) | Toggle for SIGSTOP mode |
| use_scaling_available | (true \|\| false) | Toggle for scaling mode |
| use_stepwise_SIGSTOP | (true \|\| false) | Toggle for stepwise SIGSTOP mode |
| use_stepwise_SIGCONT | (true \|\| false) | Toggle for stepwise SIGCONT mode |
| matcher_thermal | string | Thermal file matcher (use * matching) |
| matcher_scaling_max_freq | string | scaling_max_freq file matcher (use * matching) |
| matcher_cpuinfo_max_freq | string | cpuinfo_max_freq file matcher (use * matching) |
| matcher_cpuinfo_min_freq | string | cpuinfo_min_freq file matcher (use * matching) |
| matcher_scaling_available_frequencies | string | scaling_available_frequencies file matcher (use * matching) |
| temp_SIGSTOP | unsigned long | maximum temperature found at any sensor to trigger SIGSTOP |
| temp_SIGCONT | unsigned long | minimum temperature found at the HOTTEST sensor to trigger SIGCONT |
| temp_throttle | unsigned long | maximum temperature found at any sensor to trigger throttling |
| temp_dethrottle | unsigned long | minimum temperature found at the HOTTEST sensor to trigger dethrottling |
| min_sleep | unsigned int | minimum time (in milliseconds) between re-scan operations |

### Source Code

All code is documented using Doxygen. If you would like to read the source code docs please run `doxygen Doxyfile` in the package root directory.

## License

Copyright © Justin Collier  License GPLv3+: GNU GPL version 3 or later
<<https://gnu.org/licenses/gpl.html>>.

This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
