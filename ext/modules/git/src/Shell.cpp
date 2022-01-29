#include <Alpackage/Util/Logging.hpp>

#include <Kal/ErrorOr.hpp>
#include <Kal/Format.hpp>

#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <git2.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

#include "Shell.hpp"


ErrorOr<void> check_waitpid_error (pid_t pid, int status) {
  perror ("waitpid");
  switch (errno) {
    case EAGAIN: {
      return format ("Calling 'waitpid' on the nonblocking process '{}'. This "
                     "shouldn't happen.",
                     pid);
    }
    case ECHILD: {
      return format (
        "Process '{}' does not exist or is not a child of this process.",
        pid);
    }
    case EINTR: {
      return format ("Process '{}' caught an unblocked signal or SIGCHILD.",
                     pid);
    }
    case EINVAL: {
      return format (
        R"(Invalid value for 'options' of "waitpid('{}', X, options"'. We passed '0' so it should never happen.)",
        pid);
    }
    case ESRCH: {
      return format ("PID '{}' equals INT_MIN ({})", pid, INT_MIN);
    }
    default: {
      return format ("Unknown error '{}' for 'waitpid'", errno);
    }
  }
}

ErrorOr<void> check_fork_error ( ) {
  perror ("fork");
  switch (errno) {
    case EAGAIN: {
      return format ("Reached a system imposed limit on number of threads.");
    }
    case ENOMEM: {
      return format ("Out of memory.");
    }
    case ENOSYS: {
      return format ("Operation 'fork' not supported.");
    }
    default: {
      return format ("Unknown error '{}' for 'fork'", errno);
    }
  }
}


ErrorOr<void> executeShellCommand (std::string const& cmd,
                                   std::string const& workingDir) {
  Log::info ("Executing command '%s'.", cmd);
  auto pid = fork ( );

  if (pid == 0) {
    chdir (workingDir.c_str ( ));

    const char* shell = getenv ("SHELL");
    if (shell == nullptr) { shell = "/bin/sh"; }

    if (execl (shell, shell, "-c", cmd.c_str ( ), nullptr) == -1) {
      perror ("execl");
      // TODO: Check for all, yes all, possible errors from execl
      return format ("execl error {}", errno);
    }
  }

  if (pid == -1) { return check_fork_error ( ); }

  int status = 0;
  if (waitpid (pid, &status, 0) == -1) {
    return check_waitpid_error (pid, status);
  };

  if (WIFEXITED (status)) {
    auto estatus = WEXITSTATUS (status);
    if (estatus != 0) {
      return format ("Command '{}' exited with non-zero exit code: {}",
                     cmd,
                     estatus);
    }
  };

  return { };
}

ErrorOr<void> executeShellCommand (std::string const& cmd) {
  return executeShellCommand (cmd,
                              std::filesystem::current_path ( ).string ( ));
}
