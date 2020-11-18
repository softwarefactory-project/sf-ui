// A hook to authenticate with Cauth

// The hook returns this type t:
type t = (state, action => unit)
and state =
  | Loading
  | Loaded(auth)
  | Unloaded
and auth =
  | Ok
  | Error(string)
and action =
  | Login(backend)
  | Logout
and backend =
  | Password(credentials)
  | GitHub
and credentials = {
  username: string,
  password: string,
};

type cauthParams = list((string, string));

let createParams = (backend: backend): cauthParams =>
  switch (backend) {
  | Password(creds) => [
      ("username", creds.username),
      ("password", creds.password),
      ("method", "password"),
      ("back", "/"),
    ]
  | GitHub => [("method", "github"), ("back", "/")]
  };

// Simulate the result of a cauth authentication success
let fakeLogin = (backend: backend) => {
  switch (backend) {
  | Password(creds) =>
    JustgageReasonCookie.(
      Cookie.setString(
        "auth_pubtkt",
        "cid%3D11%3Buid%3D"
        ++ creds.username
        ++ "%3Bvaliduntil%3D1606139056.416925",
      )
    )
  | GitHub =>
    JustgageReasonCookie.(
      Cookie.setString(
        "auth_pubtkt",
        "cid%3D11%3Buid%3D"
        ++ "GitHub User"
        ++ "%3Bvaliduntil%3D1606139056.416925",
      )
    )
  };
  ReasonReactRouter.push("/");
};

let responseToState = resp =>
  Fetch.Response.ok(resp)
    ? Loaded(Ok)
    : Loaded(
        Error(
          "Code: "
          ++ (Fetch.Response.status(resp) |> string_of_int)
          ++ " ("
          ++ Fetch.Response.statusText(resp)
          ++ ")",
        ),
      );

let getUser = SFCookie.CauthCookie.getUser;

module Hook = (Fetcher: Dependencies.Fetcher) => {
  let use = (): t => {
    // This hook uses a single state
    let (state, updateState) = React.useState(() => Loading);
    let setState = s => updateState(_ => s);
    // And returns this convenient function to trigger state update
    let authenticate = (action: action) => {
      switch (action) {
      | Login(backend) =>
        setState(Loading);
        let toJsonString = ((. x) => x->Js.Json.string);
        let body =
          Js.Dict.map(toJsonString, createParams(backend)->Js.Dict.fromList)
          ->Js.Json.object_;
        Js.Promise.(
          Fetcher.post("/auth/login", body)
          |> then_(resp => resp |> responseToState |> setState |> resolve)
          |> ignore
        );
        // TODO: remove fakeLogin
        fakeLogin(backend);
      | Logout =>
        SFCookie.CauthCookie.remove();
        setState(Unloaded);
      };
    };
    (state, authenticate);
  };
};
