
#include <Alpackage/Util/Logging.hpp>

#include <Kal/Format.hpp>
#include <Kal/FreeLater.hpp>

#include <cstdio>
#include <cstring>
#include <git2.h>
#include <git2/annotated_commit.h>
#include <git2/blob.h>
#include <git2/checkout.h>
#include <git2/graph.h>
#include <git2/merge.h>
#include <git2/oid.h>
#include <git2/remote.h>
#include <git2/repository.h>
#include <git2/revparse.h>
#include <git2/status.h>
#include <git2/types.h>
#include <type_traits>

#include "GitRepo.hpp"
#include "Shell.hpp"

bool validate (GitRepo const& c) {
  // TODO: Implement this.
  return false;
};

ErrorOr<GitRepo> from (std::vector<ConfLine> const& lines, Config const& conf) {
  GitRepo def (conf);
  for (auto const& line : lines) {
    if (line.name == "name") {
      def.name = line.value;
    } else if (line.name == "pin") {
      def.pin = line.value;
    } else if (line.name == "description") {
      def.description = line.value;
    } else if (line.name == "remote") {
      def.remote = line.value;
    } else if (line.name == "cloneto") {
      def.dir = line.value;
    } else if (line.name == "build") {
      def.build = line.value;
    } else if (line.name == "install") {
      def.install = line.value;
    } else if (line.name == "version") {
      def.version = line.value;
    } else {
      return format ("Invalid key-value pair: '{} = {}'",
                     line.name,
                     line.value);
    }
  }

  return def;
}

#define UNUSED(x)

static int readline (char** out) {
  int       c         = 0;
  int       error     = 0;
  int       length    = 0;
  int       allocated = 0;
  char*     line      = nullptr;
  FreeLater lineGuard ((void**) &line);

  errno = 0;

  while ((c = getchar ( )) != EOF) {
    if (length == allocated) {
      allocated += 16;

      if ((line = (char*) realloc (line, allocated)) == NULL) {
        error = -1;
        return error;
      }
    }

    if (c == '\n') break;

    line[length++] = c;
  }

  if (errno != 0) {
    error = -1;
    return error;
  }

  line[length] = '\0';
  *out         = line;
  lineGuard.release ( );
  error = length;

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
                     void*            _payload) {
  int error = 1;

  UNUSED (url);
  auto*     payload  = (GitRepo::fetchPayload*) _payload;

  char*     username = nullptr;
  FreeLater usernameGuard ((void**) &username);
  if (username_from_url != nullptr) {
    if ((username = strdup (username_from_url)) == nullptr) { return error; }
  } else if ((error = ask (&username, "Username: ", 0)) < 0) {
    return error;
  }

  if ((allowed_types & GIT_CREDENTIAL_SSH_KEY) != 0U) {
    if (!payload->usedSSH) { payload->usedSSH = true; }
    if (!payload->triedSSHAgent) {
      error = git_credential_ssh_key_from_agent (out, username);
      payload->triedSSHAgent = true;
      return error;
    }

    char*     privkey = nullptr;
    FreeLater privkeyGuard ((void**) &privkey);

    char*     pubkey = nullptr;
    FreeLater pubkeyGuard ((void**) &pubkey);
    if (payload->triedDefaultKey) { payload->defaultKey = nullptr; }
    if (payload->defaultKey != nullptr && !payload->triedDefaultKey) {
      pubkey = strdup ((payload->defaultKey));

      if (pubkey == nullptr) { return error; }

      size_t n = strlen (pubkey) - 4;

      privkey  = (char*) malloc (n + 1);

      if (privkey == nullptr) { return error; }
      strncpy (privkey, pubkey, n);
      privkey[n]               = '\0';
      payload->triedDefaultKey = true;
      error = git_credential_ssh_key_new (out, username, pubkey, privkey, "");
      return error;
    }

    if (payload->keys.atEnd ( )) {
      size_t n = 0;
      if ((error = ask (&privkey, "SSH Key:", 0)) < 0) { return error; }

      if ((n = snprintf (nullptr, 0, "%s.pub", privkey)) < 0
          || (pubkey = (char*) malloc (n + 1)) == nullptr
          || (n = snprintf (pubkey, n + 1, "%s.pub", privkey)) < 0) {
        return error;
      }

    } else if (!payload->triedSSHPasswordless) {
      ++(payload->keys);
      if (payload->keys.atEnd ( )) {
        payload->triedSSHPasswordless = true;
        payload->keys.reset ( );
        return error;
      }
      pubkey = strdup ((*payload->keys).path ( ).c_str ( ));

      if (pubkey == nullptr) { return error; }

      size_t n = strlen (pubkey) - 4;
      privkey  = (char*) malloc (n + 1);

      if (privkey == nullptr) { return error; }
      strncpy (privkey, pubkey, n);
      privkey[n] = '\0';

      error = git_credential_ssh_key_new (out, username, pubkey, privkey, "");
      return error;
    } else {
      if (payload->triedSSHWithPassword >= 3) {
        payload->triedSSHWithPassword = 0;
        ++(payload->keys);
      }


      pubkey = strdup ((*payload->keys).path ( ).c_str ( ));
      if (pubkey == nullptr) { return error; }
      size_t n = strlen (pubkey) - 4;
      privkey  = (char*) malloc (n + 1);
      if (privkey == nullptr) { return error; }
      strncpy (privkey, pubkey, n);
      privkey[n] = '\0';
    }

    if (payload->triedSSHWithPassword >= 3) { return error; }

    char*     password = nullptr;
    FreeLater passwordGuard ((void**) &password);
    if ((error = ask (&password, "Password:", 0)) < 0) { return error; }

    error
      = git_credential_ssh_key_new (out, username, pubkey, privkey, password);

    payload->triedSSHWithPassword++;

  } else if ((allowed_types & GIT_CREDENTIAL_USERPASS_PLAINTEXT) != 0U) {
    char*     password = nullptr;
    FreeLater passwordGuard ((void**) &password);
    if ((error = ask (&password, "Password:", 1)) < 0) { return error; }

    error = git_credential_userpass_plaintext_new (out, username, password);
  } else if ((allowed_types & GIT_CREDENTIAL_USERNAME) != 0U) {
    error = git_credential_username_new (out, username);
  }

  return error;
}


ErrorOr<void> GitRepo::buildPkg ( ) {
  if (!repo) { TRY (setUpRepo ( )); }
  if (build.empty ( )) { return { }; }
  return executeShellCommand (build, dir);
}

ErrorOr<void> GitRepo::installPkg ( ) {
  // Does this always need to rebuild?
  TRY (buildPkg ( ));
  if (install.empty ( )) {
    return format ("No command provided to install '{}-{}' with.",
                   name,
                   version);
  }
  return executeShellCommand (install, dir);
}

ErrorOr<void> GitRepo::setUpRepo ( ) {
  int error = git_repository_open (&repo, dir.c_str ( ));
  if (error != 0) {
    git_clone_options clone_opts                = GIT_CLONE_OPTIONS_INIT;
    clone_opts.fetch_opts.callbacks.credentials = cred_acquire_cb;
    Log::info ("Cloning repository '%s' to '%s'.", remote, dir);
    error = git_clone (&repo, remote.c_str ( ), dir.c_str ( ), &clone_opts);
    TRY (
      handleGitError (error, format ("Failed to clone git repo '{}'", remote)));
    Log::info ("Clone succeeded.");
  } else {
    Log::info ("Repository '%s' already initialized. ", remote);
  }
  return { };
}

ErrorOr<void> GitRepo::fetchOrigin ( ) {
  int         errcode = 0;
  git_remote* gremote;
  errcode = git_remote_lookup (&gremote, repo, "origin");

  TRY (handleGitError (errcode, "Could not find remote 'origin'"));

  git_fetch_options fopts     = GIT_FETCH_OPTIONS_INIT;
  fopts.callbacks.credentials = cred_acquire_cb;
  fopts.callbacks.payload     = malloc (sizeof (fetchPayload));
  FreeLater payloadGuard (&fopts.callbacks.payload);
  new ((fetchPayload*) fopts.callbacks.payload) fetchPayload ( );
  ((fetchPayload*) fopts.callbacks.payload)->defaultKey = conf.sshKey.c_str ( );
  bool retry                                            = true;

  while (retry) {
    errcode = git_remote_fetch (gremote, nullptr, &fopts, "update");
    switch (errcode) {
      case -1: break;
      default: retry = false; break;
    }
  }

  ((fetchPayload*) fopts.callbacks.payload)->~fetchPayload ( );
  git_remote_free (gremote);
  TRY (handleGitError (errcode, "Could not fetch remote 'origin'"));
  return { };
};

ErrorOr<GitRepo::AheadBehindResult> GitRepo::checkIfBehind ( ) {
  if (!repo) TRY (setUpRepo ( ));

  // FIXME: Figure out a way to avoid doing this fetch on every check.
  TRY (fetchOrigin ( ));
  int            errcode = 0;
  git_object*    headObj;
  git_reference* headRef;
  errcode = git_revparse_ext (&headObj, &headRef, repo, "HEAD");

  TRY (handleGitError (errcode, "Could not find revision 'HEAD'"));

  git_reference_free (headRef);

  const auto* headId = git_object_id (headObj);
  git_object_free (headObj);

  git_object*    fheadObj;
  git_reference* fheadRef;
  errcode = git_revparse_ext (&fheadObj, &fheadRef, repo, "FETCH_HEAD");

  TRY (handleGitError (errcode, "Could not find revision 'FETCH_HEAD'"));

  git_reference_free (fheadRef);

  const auto* fheadId = git_object_id (fheadObj);
  git_object_free (fheadObj);


  AheadBehindResult res{ };

  mergeStatusCache = std::move (TRY (mergeStatus ( )));
  res.mergeStatus  = mergeStatusCache.status;
  if (res.mergeStatus != MergeStatus::Kind::NONE) {
    git_graph_ahead_behind (&res.ahead, &res.behind, repo, headId, fheadId);
  } else {
    res.ahead  = 0;
    res.behind = 0;
  };


  return res;
}

namespace {
struct OIDPayload {
  git_oid* data   = nullptr;
  bool     status = false;
};
}     // namespace

static int collectOIDFromFetchHead (const char*    ref_name,
                                    const char*    remote_url,
                                    const git_oid* oid,
                                    unsigned int   is_merge,
                                    void*          payload) {
  if (is_merge) {
    // printf ("Name: %s\nRemote: %s\nMerge? %u\n",
    //         ref_name,
    //         remote_url,
    //         is_merge);
    memcpy (((OIDPayload*) payload)->data, oid, sizeof (git_oid));
    ((OIDPayload*) payload)->status = true;
  }
  return 0;
}

ErrorOr<void> GitRepo::fastForward ( ) {
  auto ms = TRY (mergeStatus ( ));
  if (ms.status != MergeStatus::Kind::FAST_FORWARD) {
    return format ("Cannot fast-forward this repository");
  }
  git_reference* targetRef = nullptr;
  int            error     = 0;
  error                    = git_repository_head (&targetRef, repo);
  TRY (handleGitError (error, "Failed to get repository head"));

  git_object* target = nullptr;
  error = git_object_lookup (&target, repo, ms.oid, GIT_OBJECT_COMMIT);
  TRY (handleGitError (error,
                       format ("Failed to look up commit object for OID '{}'",
                               git_oid_tostr_s (ms.oid))));

  git_checkout_options ffCheckoutOpts = GIT_CHECKOUT_OPTIONS_INIT;
  ffCheckoutOpts.checkout_strategy    = GIT_CHECKOUT_SAFE;

  error = git_checkout_tree (repo, target, &ffCheckoutOpts);
  TRY (handleGitError (error, "Failed to checkout HEAD."));

  git_reference* newRef = nullptr;
  error
    = git_reference_set_target (&newRef, targetRef, ms.oid, "FF via Alpackage");
  TRY (handleGitError (error, "Failed to move HEAD reference"));

  git_object_free (target);
  git_reference_free (targetRef);
  git_reference_free (newRef);

  return installPkg ( );
}

ErrorOr<GitRepo::MergeStatus> GitRepo::mergeStatus ( ) {
  int        error = 0;
  auto*      oid   = (git_oid*) malloc (sizeof (git_oid));
  FreeLater  oidGuard ((void**) &oid);
  OIDPayload payload = {oid, false};
  error              = git_repository_fetchhead_foreach (repo,
                                            collectOIDFromFetchHead,
                                            &payload);
  TRY (handleGitError (error, "Failed to find merge OID from FETCH_HEAD."));

  if (!payload.status) {
    // HACK:??? Assume that a missing mergeable ref means we are up-to-date.
    return MergeStatus (MergeStatus::Kind::NONE);
  }

  if (mergeStatusCache.oid && git_oid_cmp (oid, mergeStatusCache.oid) == 0) {
    return mergeStatusCache;
  }

  git_annotated_commit* commit = nullptr;
  error = git_annotated_commit_lookup (&commit, repo, oid);
  TRY (handleGitError (
    error,
    format ("Failed to look up annotated commit for {}", name)));

  git_merge_analysis_t        analysis_result;
  git_merge_preference_t      pref  = GIT_MERGE_PREFERENCE_NONE;

  const git_annotated_commit* buf[] = {commit};

  error = git_merge_analysis (&analysis_result, &pref, repo, buf, 1);
  TRY (handleGitError (error, "Failed to run merge analysis."));

  git_annotated_commit_free (commit);
  if (analysis_result & GIT_MERGE_ANALYSIS_FASTFORWARD) {
    return MergeStatus (MergeStatus::Kind::FAST_FORWARD, oid);
  }
  if (analysis_result & GIT_MERGE_ANALYSIS_UP_TO_DATE) {
    return MergeStatus (MergeStatus::Kind::NONE, oid);
  }
  if (analysis_result & GIT_MERGE_ANALYSIS_NORMAL) {
    return MergeStatus (MergeStatus::Kind::MANUAL_MERGE, oid);
  }
  if (analysis_result & GIT_MERGE_ANALYSIS_NONE
      || analysis_result & GIT_MERGE_ANALYSIS_UNBORN) {
    return format ("Invalid result from merge analysis for {}", name);
  }
  return format ("Wot? This should not be reachable.: {}", analysis_result);
}


ErrorOr<bool> GitRepo::modified ( ) {
  git_status_options opts  = GIT_STATUS_OPTIONS_INIT;
  opts.show                = GIT_STATUS_SHOW_WORKDIR_ONLY;
  opts.flags               = GIT_STATUS_OPT_UPDATE_INDEX;

  git_status_list* out     = nullptr;
  int              errcode = 0;

  errcode                  = git_status_list_new (&out, repo, &opts);
  TRY (handleGitError (errcode, "Failed to initialize Git status list"));

  bool res = git_status_list_entrycount (out) > 0;

  git_status_list_free (out);

  return res;
}


ErrorOr<void> GitRepo::handleGitError (int                errcode,
                                       std::string const& message) const {
  if (errcode != 0) {
    const auto* err = git_error_last ( );
    if (err) {
      ErrorOr<void> ret (format ("[{}] {}", err->klass, err->message));
      return ret.propagate (message);
    }
    return format ("{}: {}", message, errcode);
  }
  return { };
}
