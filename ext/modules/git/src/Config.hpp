#pragma once

#include <Alpackage/Util/ConfLine.hpp>
#include <Alpackage/Util/Logging.hpp>

#include <Kal/ErrorOr.hpp>
#include <Kal/Format.hpp>

#include <cstdio>
#include <cstring>
#include <git2.h>
#include <git2/graph.h>
#include <git2/remote.h>
#include <git2/revparse.h>
#include <git2/status.h>
#include <git2/types.h>
#include <string>

#include "Shell.hpp"

#define UNUSED(x)

static int readline (char** out) {
  int   c, error = 0, length = 0, allocated = 0;
  char* line = NULL;

  errno      = 0;

  while ((c = getchar ( )) != EOF) {
    if (length == allocated) {
      allocated += 16;

      if ((line = (char*) realloc (line, allocated)) == NULL) {
        error = -1;
        goto error;
      }
    }

    if (c == '\n') break;

    line[length++] = c;
  }

  if (errno != 0) {
    error = -1;
    goto error;
  }

  line[length] = '\0';
  *out         = line;
  line         = NULL;
  error        = length;
error:
  free (line);
  return error;
}

static int ask (char** out, const char* prompt, char optional) {
  printf ("%s ", prompt);
  fflush (stdout);

  if (!readline (out) && !optional) {
    fprintf (stderr, "Could not read response: %s", strerror (errno));
    return -1;
  }

  return 0;
}

int cred_acquire_cb (git_credential** out,
                     const char*      url,
                     const char*      username_from_url,
                     unsigned int     allowed_types,
                     void*            payload);


class Config {
  git_repository* repo{nullptr};
  public:
  std::string name, pin, description, remote, dir, build, install, version;
  Config ( )      = default;
  Config& operator= (Config const&) = default;
  Config (Config const&)            = default;
  Config (Config&& c) noexcept
      : name (std::move (c.name))
      , pin (std::move (c.pin))
      , description (std::move (c.description))
      , remote (std::move (c.remote))
      , dir (std::move (c.dir))
      , build (std::move (c.build))
      , install (std::move (c.install))
      , version (std::move (c.version)) { }

  ErrorOr<void> buildPkg ( ) {
    if (!repo) TRY (setUpRepo ( ));
    if (build.empty ( )) return { };
    return executeShellCommand (build, dir);
  }

  ErrorOr<void> installPkg ( ) {
    // Does this always need to rebuild?
    TRY (buildPkg ( ));
    if (install.empty ( ))
      return format ("No command provided to install '{}-{}' with.",
                     name,
                     version);
    return executeShellCommand (install, dir);
  }

  ErrorOr<void> setUpRepo ( ) {
    int error = git_repository_open (&repo, dir.c_str ( ));
    if (error != 0) {
      git_clone_options clone_opts                = GIT_CLONE_OPTIONS_INIT;
      clone_opts.fetch_opts.callbacks.credentials = cred_acquire_cb;
      Log::info ("Cloning repository '%s' to '%s'.", remote, dir);
      error = git_clone (&repo, remote.c_str ( ), dir.c_str ( ), &clone_opts);
      if (error != 0) {
        Log::warn ("Clone failed.");
        const git_error* err = git_error_last ( );
        if (err != nullptr) {
          return format ("Failed to clone git repo '{}': ({}) {}",
                         remote,
                         err->klass,
                         err->message);
        }
        return format ("Failed to clone git repo '{}': Error No. {}",
                       remote,
                       error);
      }
      Log::info ("Clone succeeded.");
    } else {
      Log::info ("Repository '%s' already initialized. ", remote);
    }
    return { };
  }

  struct AheadBehindResult {
    size_t ahead, behind;
  };

  ErrorOr<AheadBehindResult> checkIfBehind ( ) {
    if (!repo) TRY (setUpRepo ( ));


    int         errcode = 0;

    git_remote* remote;
    errcode = git_remote_lookup (&remote, repo, "origin");

    if (errcode != 0) {
      const auto* err = git_error_last ( );
      if (err) {
        return format ("Could not find remote 'origin': [{}] {}",
                       err->klass,
                       err->message);
      }
      return format ("Could not find remote 'origin': {}", errcode);
    }

    git_fetch_options fopts     = GIT_FETCH_OPTIONS_INIT;
    fopts.callbacks.credentials = cred_acquire_cb;


    git_remote_fetch (remote, nullptr, &fopts, "update");

    git_object*    headObj;
    git_reference* headRef;
    git_revparse_ext (&headObj, &headRef, repo, "HEAD");

    git_object*    fheadObj;
    git_reference* fheadRef;
    git_revparse_ext (&fheadObj, &fheadRef, repo, "FETCH_HEAD");

    const auto*       headId  = git_object_id (headObj);
    const auto*       fheadId = git_object_id (fheadObj);

    AheadBehindResult res;
    git_graph_ahead_behind (&res.ahead, &res.behind, repo, headId, fheadId);

    return res;
  }

  ErrorOr<bool> modified ( ) {
    git_status_options opts  = GIT_STATUS_OPTIONS_INIT;
    opts.show                = GIT_STATUS_SHOW_WORKDIR_ONLY;
    opts.flags               = GIT_STATUS_OPT_UPDATE_INDEX;
    git_status_list* out     = nullptr;
    int              errcode = 0;
    if ((errcode = git_status_list_new (&out, repo, &opts)) != 0) {
      const auto* err = git_error_last ( );
      if (err) {
        return format ("Git error while initializing Git status list: [{}] {}",
                       err->klass,
                       err->message);
      }
      return format (
        "Failed to initialize Git status list with error code '{}'",
        errcode);
    }

    if (!out) { return format ("Failed to initialize Git status list."); }


    bool res = git_status_list_entrycount (out) > 0;

    if (out) { git_status_list_free (out); }

    return res;
  }

  ~Config ( ) {
    if (repo) { git_repository_free (repo); }
  };
};

bool            validate (Config const&);
ErrorOr<Config> from (std::vector<ConfLine> const&);
