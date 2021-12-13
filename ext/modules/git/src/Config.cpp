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
                     void*            payload) {
  char* username = nullptr;
  char* password = nullptr;
  char* privkey  = nullptr;
  char* pubkey   = nullptr;
  int   error    = 1;

  UNUSED (url);
  UNUSED (payload);

  if (username_from_url != nullptr) {
    if ((username = strdup (username_from_url)) == nullptr) { goto out; }
  } else if ((error = ask (&username, "Username:", 0)) < 0) {
    goto out;
  }

  if ((allowed_types & GIT_CREDENTIAL_SSH_KEY) != 0u) {
    int n;

    if ((error = ask (&privkey, "SSH Key:", 0)) < 0
        || (error = ask (&password, "Password:", 1)) < 0)
      goto out;

    if ((n = snprintf (nullptr, 0, "%s.pub", privkey)) < 0
        || (pubkey = (char*) malloc (n + 1)) == nullptr
        || (n = snprintf (pubkey, n + 1, "%s.pub", privkey)) < 0)
      goto out;

    error
      = git_credential_ssh_key_new (out, username, pubkey, privkey, password);
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
