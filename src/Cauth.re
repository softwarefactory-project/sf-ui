// A hook to authenticate with Cauth

type credentials = {
  username: string,
  password: string,
};
type cauthBackend =
  | Password(credentials)
  | GITHUB;
type auth =
  | Ok
  | Error(string);
type state =
  | Loading
  | Loaded(auth)
and cauthParams = list((string, string))
and t = (state, cauthBackend => unit);

let createParams = (backend: cauthBackend): cauthParams => {
  switch (backend) {
  | Password(creds) => [
      ("username", creds.username),
      ("password", creds.password),
      ("method", "password"),
      ("back", "/"),
    ]
  | GITHUB => [("method", "github"), ("back", "/")]
  };
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

module Hook = (Fetcher: Dependencies.Fetcher) => {
  let use = (): t => {
    let (state, updateState) = React.useState(() => Loading);
    let setState = s => updateState(_ => s);
    let authenticate = (cauthInfo: cauthBackend) => {
      let kv = createParams(cauthInfo);
      setState(Loading);
      let params = () => {
        let formData = Fetch.FormData.make();
        kv->Belt.List.map(((k, v)) => Fetch.FormData.set(k, v, formData))
        |> ignore;
        Fetch.RequestInit.make(
          ~method_=Post,
          ~body=Fetch.BodyInit.makeWithFormData(formData),
          ~headers=
            Fetch.HeadersInit.make({
              "Accept": "*",
              "Content-Type": "application/x-www-form-urlencoded",
            }),
          (),
        );
      };
      Js.log("Connecting to Cauth ...");
      Js.Promise.(
        Fetcher.fetchWithInit("/auth/login", params())
        |> then_(resp => resp |> responseToState |> setState |> resolve)
        |> ignore
      );
      // |> catch(err => err->Js.log |> resolve) |> ignore
      // |> ignore
    };
    (state, authenticate);
  };
};
