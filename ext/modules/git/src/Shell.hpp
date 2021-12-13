#pragma once


#include <Kal/ErrorOr.hpp>

#include <string>

ErrorOr<void> executeShellCommand (std::string const& cmd,
                                   std::string const& workingDir);

ErrorOr<void> executeShellCommand (std::string const& cmd);
