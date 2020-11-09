// A hook to manage authentication
type action =
  | Login(string)
  | Logout;

type user = {name: string};

type state = option(user);

type t = (state, action => unit);

module Hook = {
  let use = (): t =>
    React.useReducer(
      (_state, action) =>
        switch (action) {
        | Login(name) =>
          Js.log("Login " ++ name);
          // TODO: ensure cookie is set
          {name: name}->Some;
        | Logout =>
          Js.log("Logout");
          // TODO: remove cookie
          None;
        },
      // TODO: validate cookie and set default user
      None,
    );
};
