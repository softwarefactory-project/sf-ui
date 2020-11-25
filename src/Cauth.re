// A hook to authenticate with Cauth

// The hook returns this type t:
type t = (state, action => unit)
and state = RemoteApi.state_t(unit)
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
let getBack = () => "https://sftests.com/";

let backendMethod =
  fun
  | Password(_) => "Password"
  | GitHub => "Github"
  | Google => "Google"
  | BitBucket => "BitBucket"
  | OpenID => "OpenID"
  | OpenIDConnect => "OpenIDConnect"
  | SAML => "SAML2";

let idpParams = method => {
  method,
  back: getBack(),
  args: {
    username: None,
    password: None,
  },
};

let createParams = (backend: backend): cauthPost =>
  // TODO: update this code because only the password method params are used,
  // the externalidp are submited through a post form
  // because xhr request doesn't seems to support location redirect
  switch (backend) {
  | Password(creds) => {
      method: "Password",
      back: getBack(),
      args: {
        username: creds.username->Some,
        password: creds.password->Some,
      },
    }
  | GitHub => "Github"->idpParams
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
  open RemoteApi;
  module RemoteApi = RemoteApi.API(Fetcher);

  let use = (): t => {
    // This hook uses a single state
    let (state, setState) = React.useState(() => RemoteData.NotAsked);
    let set_state = s => setState(_ => s);
    // And returns this convenient function to trigger state update
    let authenticate = (action: action): unit => {
      switch (action) {
      | Login(backend) =>
        // Here we just post, and we'll be redirected if the request succeed
        RemoteApi.justPost(
          "/auth/login", createParams(backend)->cauthPost_encode, r =>
          state->updateRemoteData(r)->set_state
        )
        ->ignore

      // fakeLogin(backend);
      | Logout =>
        SFCookie.CauthCookie.remove();
        RemoteData.NotAsked->set_state;
      };
    };
    (state, authenticate);
  };
};
