// A hook to fetch resources
module Hook = (Fetcher: Dependencies.Fetcher) => {
  type auth =
    | Ok
    | Error(string);
  type state =
    | Loading
    | Loaded(auth);
    
  type cauthParams = list((string, string));

  let use = (kv: cauthParams) => {
    let (state, setState) = React.useState(() => Loading);
    let updateState = (auth_result: auth) => {
      setState(_ => Loaded(auth_result));
    };
    let params = () => {
      let formData = Fetch.FormData.make();
      kv->Belt.List.map(((k, v))
        => Fetch.FormData.set(k, v, formData))
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
    React.useEffect0(() => {
      Js.log("Running Cauth authentication effect...");
      switch (state) {
      | Loading =>
        Js.log("Connecting to Cauth ...");
        Js.Promise.(
          Fetcher.fetchWithInit("/auth/login", params())
          |> then_(resp =>
               resp
               |> Fetch.Response.ok
               |> {
                 (
                   status =>
                     status
                       ? Ok
                       : Error(
                           "Code: "
                           ++ (Fetch.Response.status(resp) |> string_of_int)
                           ++ " ("
                           ++ Fetch.Response.statusText(resp)
                           ++ ")",
                         )
                 );
               }
               |> updateState
               |> resolve
             )
          // |> catch(err => err->Js.log |> resolve) |> ignore
          |> ignore
        );
      | _ => ()
      };
      None;
    });
    state;
  };
};
