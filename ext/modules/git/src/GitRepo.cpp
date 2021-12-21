
#include <Alpackage/Util/Logging.hpp>

#include <Kal/Format.hpp>

#include <cstdio>
#include <cstring>
#include <git2.h>
#include <git2/graph.h>
#include <git2/remote.h>
#include <git2/revparse.h>
#include <git2/status.h>
#include <git2/types.h>

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
                     void*            _payload) {
  char* username = nullptr;
  char* password = nullptr;
  char* privkey  = nullptr;
  char* pubkey   = nullptr;
  int   error    = 1;

  UNUSED (url);
  auto* payload = (GitRepo::fetchPayload*) _payload;

  if (username_from_url != nullptr) {
    if ((username = strdup (username_from_url)) == nullptr) { goto out; }
  } else if ((error = ask (&username, "Username: ", 0)) < 0) {
    goto out;
  }

  if ((allowed_types & GIT_CREDENTIAL_SSH_KEY) != 0u) {
    if (!payload->usedSSH) payload->usedSSH = true;
    if (!payload->triedSSHAgent) {
      error = git_credential_ssh_key_from_agent (out, username);
      payload->triedSSHAgent = true;
      goto out;
    }


    int n;
    if (payload->triedDefaultKey) payload->defaultKey == nullptr;
    if (payload->defaultKey != nullptr && !payload->triedDefaultKey) {
      pubkey = strdup ((payload->defaultKey));
      if (pubkey == nullptr) goto out;
      n       = strlen (pubkey) - 4;
      privkey = (char*) malloc (n + 1);
      if (privkey == nullptr) goto out;
      strncpy (privkey, pubkey, n);
      privkey[n]               = '\0';
      payload->triedDefaultKey = true;
      error = git_credential_ssh_key_new (out, username, pubkey, privkey, "");
      goto out;
    } else if (payload->keys.atEnd ( )) {
      if ((error = ask (&privkey, "SSH Key:", 0)) < 0) goto out;
      if ((n = snprintf (nullptr, 0, "%s.pub", privkey)) < 0
          || (pubkey = (char*) malloc (n + 1)) == nullptr
          || (n = snprintf (pubkey, n + 1, "%s.pub", privkey)) < 0)
        goto out;
    } else if (!payload->triedSSHPasswordless) {
      ++(payload->keys);
      if (payload->keys.atEnd ( )) {
        payload->triedSSHPasswordless = true;
        payload->keys.reset ( );
        goto out;
      }
      pubkey = strdup ((*payload->keys).path ( ).c_str ( ));
      if (pubkey == nullptr) goto out;
      n       = strlen (pubkey) - 4;
      privkey = (char*) malloc (n + 1);
      if (privkey == nullptr) goto out;
      strncpy (privkey, pubkey, n);
      privkey[n] = '\0';

      error = git_credential_ssh_key_new (out, username, pubkey, privkey, "");
      goto out;
    } else {
      if (payload->triedSSHWithPassword >= 3) {
        payload->triedSSHWithPassword = 0;
        ++(payload->keys);
      }
      pubkey = strdup ((*payload->keys).path ( ).c_str ( ));
      if (pubkey == nullptr) goto out;
      n       = strlen (pubkey) - 4;
      privkey = (char*) malloc (n + 1);
      if (privkey == nullptr) goto out;
      strncpy (privkey, pubkey, n);
      privkey[n] = '\0';
    }

    if (payload->triedSSHWithPassword >= 3) { goto out; }

    if ((error = ask (&password, "Password:", 0)) < 0) goto out;

    error
      = git_credential_ssh_key_new (out, username, pubkey, privkey, password);

    payload->triedSSHWithPassword++;

  } else if ((allowed_types & GIT_CREDENTIAL_USERPASS_PLAINTEXT) != 0U) {
    if ((error = ask (&password, "Password:", 1)) < 0) goto out;

    error = git_credential_userpass_plaintext_new (out, username, password);
  } else if ((allowed_types & GIT_CREDENTIAL_USERNAME) != 0U) {
    error = git_credential_username_new (out, username);
  }

out:
  free (username);
  free (password);
  free (privkey);
  free (pubkey);
  return error;
}


ErrorOr<void> GitRepo::buildPkg ( ) {
  if (!repo) TRY (setUpRepo ( ));
  if (build.empty ( )) return { };
  return executeShellCommand (build, dir);
}

ErrorOr<void> GitRepo::installPkg ( ) {
  // Does this always need to rebuild?
  TRY (buildPkg ( ));
  if (install.empty ( ))
    return format ("No command provided to install '{}-{}' with.",
                   name,
                   version);
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

  auto* payload = ((fetchPayload*) fopts.callbacks.payload);

  if (payload->usedSSH) {
    const char* pubkey = nullptr;

    if (payload->triedDefaultKey && payload->defaultKey != nullptr) {
      printf ("Using default key: ");
      pubkey = conf.sshKey.c_str ( );
    } else {
      pubkey = (*payload->keys).path ( ).c_str ( );
    }

    size_t n       = strlen (pubkey) - 4;
    char*  privkey = (char*) malloc (n + 1);
    strncpy (privkey, pubkey, n);
    privkey[n] = '\0';
    printf ("Authenticated to '%s' with ssh key '%s'.\n",
            remote.c_str ( ),
            privkey);
    free (privkey);
  }

  free (fopts.callbacks.payload);

  TRY (handleGitError (errcode, "Could not fetch remote 'origin'"));

  git_remote_free (gremote);

  return { };
};

ErrorOr<GitRepo::AheadBehindResult> GitRepo::checkIfBehind ( ) {
  if (!repo) TRY (setUpRepo ( ));

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
  git_graph_ahead_behind (&res.ahead, &res.behind, repo, headId, fheadId);

  return res;
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
