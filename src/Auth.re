// A hook to manage authentication

type action =
  | Login
  | Logout;

type user = {name: string};

type state = option(user);

type t = (state, action => unit);

module Hook = {
  let readCookieAndSetUser = (): state => {
    switch (SFCookie.CauthCookie.getUser()) {
    | Some(uid) =>
      Js.log("Login: " ++ uid);
      {name: uid}->Some;
    | None => None
    };
  };
  let use = (): t =>
    React.useReducer(
      (_state, action) =>
        switch (action) {
        | Login => readCookieAndSetUser()
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
