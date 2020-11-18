// A hook to manage authentication
type credentials = {
  username: string,
  password: string,
};

type cauthBackend =
  | Password(credentials)
  | GITHUB;

type action =
  | CauthLogin(cauthBackend)
  | Logout;

type user = {name: string};

type state = option(user);

type t = (state, action => unit);

module Hook = (Fetcher: Dependencies.Fetcher) => {
  module Cauth = Cauth.Hook(Fetcher);
  let readCookieAndSetUser = (): state => {
    switch (SFCookie.CauthCookie.getUser()) {
    | Some(uid) => {name: uid}->Some
    | None => None
    };
  };
  let use = (): t =>
    React.useReducer(
      (_state, action) =>
        switch (action) {
        | CauthLogin(auth_type) =>
          switch (auth_type) {
          | Password(creds) =>
            let state =
              Cauth.use([
                ("username", creds.username),
                ("password", creds.password),
                ("method", "password"),
                ("back", "/"),
              ]);
            switch (state) {
            | Loading => None
            | Loaded(auth_status) =>
              switch (auth_status) {
              | Ok =>
                let maybeUser = readCookieAndSetUser();
                switch (maybeUser) {
                | Some(user) => user.name == creds.username ? maybeUser : None
                | None => None
                };
              | Error(err) =>
                err->Js.log;
                None;
              }
            };
          | GITHUB => readCookieAndSetUser()
          }
        | Logout =>
          Js.log("Logout");
          // remove cookie
          SFCookie.CauthCookie.remove();
          None;
        },
      // Validate cookie and set default user
      readCookieAndSetUser(),
    );
};
