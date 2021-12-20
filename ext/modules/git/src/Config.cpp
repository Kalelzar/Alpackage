
#include <Kal/Format.hpp>

#include "Config.hpp"

bool            validate (Config const& c) { return false; };
ErrorOr<Config> from (std::vector<ConfLine> const& lines) {
  Config def = { };
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
  auto* payload  = (Config::fetchPayload*) _payload;

  if (username_from_url != nullptr) {
    if ((username = strdup (username_from_url)) == nullptr) { goto out; }
  } else if ((error = ask (&username, "Username: ", 0)) < 0) {
    goto out;
  }

  if ((allowed_types & GIT_CREDENTIAL_SSH_KEY) != 0u) {
    if (!payload->triedSSHAgent) {
      error = git_credential_ssh_key_from_agent (out, username);
      payload->triedSSHAgent = true;
      goto out;
    }

    int n;

    if (payload->keys.atEnd ( )) {
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
