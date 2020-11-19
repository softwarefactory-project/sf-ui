[@decco]
type user = {
  username: string,
  fullname: string,
  email: string,
  idp_sync: bool,
};
[@decco]
type apiKey = {api_key: string};

type t = (state, stateKey)
and hook = unit => t
and state =
  | Loading
  | Loaded(user)
and stateKey =
  | KeyLoading
  | KeyLoaded(apiKey);

module Hook = (Fetcher: Dependencies.Fetcher) => {
  // Todo: record previously fetched user info
  let use = (): t => {
    let (state, setState) = React.useState(() => Loading);
    React.useEffect0(() => {
      switch (state) {
      | Loading =>
        Js.log("Loading user settings...");
        Js.Promise.(
          Fetcher.fetch("/api/user.json")
          |> then_(json =>
               (
                 switch (json |> user_decode) {
                 | Ok(user) => setState(_ => Loaded(user))
                 | Error(e) => Js.log(e)
                 }
               )
               |> resolve
             )
          |> ignore
        );
      };
      None;
    });
    let (keyState, keySetState) = React.useState(() => KeyLoading);
    React.useEffect0(() => {
      switch (state) {
      | Loading =>
        Js.log("Loading user apiKey...");
        Js.Promise.(
          Fetcher.fetch("/api/apikey.json")
          |> then_(json =>
               (
                 switch (json |> apiKey_decode) {
                 | Ok(key) => keySetState(_ => KeyLoaded(key))
                 | Error(e) => Js.log(e)
                 }
               )
               |> resolve
             )
          |> ignore
        );
      };
      None;
    });
    (state, keyState);
  };
};
