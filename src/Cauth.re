// A hook to authenticate with Cauth

// The hook returns this type t:
type t('a) = (state('a), action => unit)
and state('a) =
  | Loading
  | Loaded(auth('a))
  | Unloaded
and auth('a) = Belt.Result.t('a, string)
and action =
  | Login(backend)
  | Logout
and backend =
  | Password(credentials)
  | GitHub
  | Google
  | BitBucket
  | OpenID
  | OpenIDConnect
  | SAML
and credentials = {
  username: string,
  password: string,
};

[@decco]
type cauthArgs = {
  username: option(string),
  password: option(string),
};

[@decco]
type cauthPost = {
  args: cauthArgs,
  method: string,
  back: string,
};

// TODO: get this from query string
let getBack = () => "/";

let idpParams = method => {
  method,
  back: getBack(),
  args: {
    username: None,
    password: None,
  },
};

let createParams = (backend: backend): cauthPost =>
  switch (backend) {
  | Password(creds) => {
      method: "Password",
      back: getBack(),
      args: {
        username: creds.username->Some,
        password: creds.password->Some,
      },
    }
  | GitHub => "GitHub"->idpParams
  | Google => "Google"->idpParams
  | BitBucket => "BitBucket"->idpParams
  | OpenID => "OpenID"->idpParams
  | OpenIDConnect => "OpenIDConnect"->idpParams
  | SAML => "SAML2"->idpParams
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
  | _ => ()
  };
  ReasonReactRouter.push(getBack());
};

let getUser = SFCookie.CauthCookie.getUser;

module Hook = (Fetcher: Dependencies.Fetcher) => {
  let use = (): t('a) => {
    // This hook uses a single state
    let (state, updateState) = React.useState(() => Loading);
    let setState = s => updateState(_ => s);
    // And returns this convenient function to trigger state update
    let authenticate = (action: action) => {
      switch (action) {
      | Login(backend) =>
        setState(Loading);
        Js.Promise.(
          Fetcher.post(
            "/auth/login",
            createParams(backend)->cauthPost_encode->Some,
          )
          |> then_(result => {result->Loaded->setState->resolve})
        )
        |> ignore;
      // fakeLogin(backend);
      | Logout =>
        SFCookie.CauthCookie.remove();
        setState(Unloaded);
      };
    };
    (state, authenticate);
  };
};
