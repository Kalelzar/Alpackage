#pragma once

#include <Alpackage/Util/ConfLine.hpp>

#include <Kal/ErrorOr.hpp>

#include <git2/repository.h>
#include <string>

#include "Config.hpp"
#include "SSHKeys.hpp"


class GitRepo {
  git_repository* repo{nullptr};
  Config          conf;
  public:
  std::string name, pin, description, remote, dir, build, install, version;
  GitRepo (Config const& conf) : conf (conf) { }
  GitRepo& operator= (GitRepo const&) = default;
  GitRepo (GitRepo const&)            = default;
  GitRepo (GitRepo&& c) noexcept
      : conf (std::move (c.conf))
      , name (std::move (c.name))
      , pin (std::move (c.pin))
      , description (std::move (c.description))
      , remote (std::move (c.remote))
      , dir (std::move (c.dir))
      , build (std::move (c.build))
      , install (std::move (c.install))
      , version (std::move (c.version)) { }

  struct AheadBehindResult {
    size_t ahead, behind;
  };

  struct fetchPayload {
    bool             triedSSHAgent        = false;
    bool             triedSSHPasswordless = false;
    bool             triedDefaultKey      = false;
    bool             usedSSH              = false;
    uint8_t          triedSSHWithPassword = 0;
    uint8_t          triedUserpass        = 0;
    const char*      defaultKey           = nullptr;
    SSHKeyIterator<> keys                 = makeSSHKeyIterator ( );
  };

  ErrorOr<void> handleGitError (int errcode, std::string const& message) const;
  ErrorOr<void> buildPkg ( );
  ErrorOr<void> installPkg ( );
  ErrorOr<void> setUpRepo ( );
  ErrorOr<void> fetchOrigin ( );
  ErrorOr<bool> modified ( );
  ErrorOr<AheadBehindResult> checkIfBehind ( );

  ~GitRepo ( ) {
    if (repo) { git_repository_free (repo); }
  };
};

bool             validate (GitRepo const&);
ErrorOr<GitRepo> from (std::vector<ConfLine> const&, Config const&);
