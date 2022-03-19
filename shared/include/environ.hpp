#pragma once

#include <string>
#include <vector>

const std::string shell_env(void);
void  new_environ(const char *ch_env[], std::vector<std::string> &valuepairs);
