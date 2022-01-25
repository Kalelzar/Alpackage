#pragma once

#include <Alpackage/Util/ConfLine.hpp>

#include <Kal/ErrorOr.hpp>

#include <git2/oidarray.h>
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

  struct MergeStatus {
    enum Kind { FAST_FORWARD, MANUAL_MERGE, NONE, UNDEFINED = -1 };

    Kind     status;
    git_oid* oid;

    MergeStatus ( ) : status (Kind::UNDEFINED), oid (nullptr) { }

    MergeStatus (Kind status, git_oid* oid)
        : status (status)
        , oid ((git_oid*) malloc (sizeof (git_oid))) {
      memcpy (this->oid, oid, sizeof (git_oid));
    }

    MergeStatus (MergeStatus const& other) : status (other.status) {
      if (&other != this) {
        oid = (git_oid*) malloc (sizeof (git_oid));
        memcpy (oid, other.oid, sizeof (git_oid));
      }
    };

    MergeStatus& operator= (MergeStatus const& other) {
      if (&other != this) {
        status = other.status;
        if (oid == nullptr) { oid = (git_oid*) malloc (sizeof (git_oid)); }
        memcpy (oid, other.oid, sizeof (git_oid));
      }
      return *this;
    };

    MergeStatus (MergeStatus&& other) noexcept
        : status (other.status)
        , oid (other.oid) {
      if (&other != this) { other.oid = nullptr; }
    }
    MergeStatus& operator= (MergeStatus&& other) noexcept {
      if (&other != this) {
        status = other.status;
        if (oid) free (oid);
        oid       = other.oid;
        other.oid = nullptr;
      }
      return *this;
    };

    ~MergeStatus ( ) { free (oid); }
  };

  struct AheadBehindResult {
    size_t      ahead, behind;
    MergeStatus mergeStatus;
  };

  [[nodiscard]] ErrorOr<void>              handleGitError (int                errcode,
                                                           std::string const& message) const;
  [[nodiscard]] ErrorOr<void>              buildPkg ( );
  [[nodiscard]] ErrorOr<void>              installPkg ( );
  [[nodiscard]] ErrorOr<void>              setUpRepo ( );
  [[nodiscard]] ErrorOr<void>              fetchOrigin ( );
  [[nodiscard]] ErrorOr<bool>              modified ( );
  [[nodiscard]] ErrorOr<AheadBehindResult> checkIfBehind ( );
  [[nodiscard]] ErrorOr<MergeStatus>       mergeStatus ( );

  ~GitRepo ( ) {
    if (repo) { git_repository_free (repo); }
  };
};

bool             validate (GitRepo const&);
ErrorOr<GitRepo> from (std::vector<ConfLine> const&, Config const&);
