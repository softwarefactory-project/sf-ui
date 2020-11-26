// A hook to manage authentication

// The hook return this type t:
type t = (state, action => unit)
and action =
  | Login(loginInfos)
  | Logout
and loginInfos =
  | CauthLogin(Cauth.backend)
and state = option(user)
and user = {name: string}
// Internally, the hook supports two backends:
and backend =
  | Cauth
  | Keycloak;

module Hook = (Fetcher: Dependencies.Fetcher) => {
  module Cauth' = Cauth.Hook(Fetcher);

  let use = (~defaultBackend: backend): t => {
    // Initialize the backends hook:
    let (_cauthState, cauthDispatch) = Cauth'.use();
    // Check what is the current backend:
    let (authBackend, setAuthBackend) = React.useState(_ => defaultBackend);
    // Create the callback:
    let authDispatch =
      fun
      | Login(loginInfos) =>
        // User tries to login
        switch (loginInfos) {
        | CauthLogin(cauthInfos) =>
          // Update the current backend
          setAuthBackend(_ => Cauth);
          // Dispatch cauth action
          cauthDispatch(cauthInfos->Cauth.Login);
        }
      | Logout =>
        // User logout
        switch (authBackend) {
        | Cauth => cauthDispatch(Cauth.Logout)
        | Keycloak => ()
        };

    // Finally we check the auth status based on the selected backend:
    let authState =
      switch (authBackend) {
      | Cauth =>
        Cauth.getUser()->Belt.Option.flatMap(uid => {name: uid}->Some)
      | Keycloak => None
      };
    (authState, authDispatch);
  };
};
