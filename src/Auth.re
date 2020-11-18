// A hook to manage authentication

type action =
  | Login(loginInfos)
  | Logout
and backend =
  | Cauth
  | Keycloak
and loginInfos =
  | CauthLogin(Cauth.cauthBackend)
and loginStatus =
  | Unknown
  | CauthStatus(Cauth.state);

type user = {name: string};

type state = option(user);

type t = (state, action => unit);

module Hook = (Fetcher: Dependencies.Fetcher) => {
  module Cauth = Cauth.Hook(Fetcher);

  let readCookieAndSetUser = (): state => {
    switch (SFCookie.CauthCookie.getUser()) {
    | Some(uid) =>
      Js.log("Got login from cookie: " ++ uid);
      {name: uid}->Some;
    | None => None
    };
  };
  let use = (~defaultBackend: backend): t => {
    let (authBackend, setAuthBackend) = React.useState(_ => defaultBackend);
    let (cauthState, cauthAuthenticate) = Cauth.use();
    Js.log2("Cauth state is", cauthState);
    let (loginState, authDispatch) =
      React.useReducer(
        (_state, action) =>
          switch (action) {
          | Login(loginInfos) =>
            switch (loginInfos) {
            | CauthLogin(cauthInfos) =>
              setAuthBackend(_ => Cauth);
              cauthAuthenticate(cauthInfos);
              CauthStatus(cauthState);
            }
          | Logout =>
            Js.log("Logout");
            // remove cookie
            SFCookie.CauthCookie.remove();
            Unknown;
          },
        Unknown,
      );
    let authState =
      switch (authBackend) {
      | Cauth => readCookieAndSetUser()
      | Keycloak => None
      };
    (authState, authDispatch);
  };
};
