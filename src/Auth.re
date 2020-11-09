// A hook to manage authentication
type action =
  | Login(string)
  | Logout;

type user = {name: string};

type state = option(user);

type t = (state, action => unit);

module Hook = {
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
        | Login(name) =>
          Js.log("Login " ++ name);
          // Ensure cookie is set
          let maybeUser = readCookieAndSetUser();
          switch (maybeUser) {
          | Some(user) => user.name == name ? {name: name}->Some : None
          | None => None
          };
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
