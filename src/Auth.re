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
  | Keycloak
and loginStatus =
  | Unknown
  | CauthStatus(Cauth.state);

module Hook = (Fetcher: Dependencies.Fetcher) => {
  module Cauth' = Cauth.Hook(Fetcher);

  let use = (~defaultBackend: backend): t => {
    // First we check what is the current backend:
    let (authBackend, setAuthBackend) = React.useState(_ => defaultBackend);
    // Then we initialize the backends hook:
    let (cauthState, cauthDispatch) = Cauth'.use();
    // And we create a reducer to manage action dispatch:
    let (_loginState, authDispatch) =
      React.useReducer(
        (_state, action) =>
          switch (action) {
          | Login(loginInfos) =>
            // User tries to login
            switch (loginInfos) {
            | CauthLogin(cauthInfos) =>
              // Update the current backend
              setAuthBackend(_ => Cauth);
              // Dispatch cauth action
              cauthDispatch(cauthInfos->Cauth.Login);
              CauthStatus(cauthState);
            }
          | Logout =>
            // User logout
            switch (authBackend) {
            | Cauth => cauthDispatch(Cauth.Logout)
            | Keycloak => ()
            };
            Unknown;
          },
        // Starts with an unknown login status
        Unknown,
      );
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
