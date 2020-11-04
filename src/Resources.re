// A hook to fetch resources
module Hook = (Fetcher: Dependencies.Fetcher) => {
  type state =
    | Loading
    | Loaded(SF.V2.t);

  let use = (default_tenant: string) => {
    let (state, setState) = React.useState(() => Loading);
    let updateState = resources => {
      setState(_ => Loaded(resources));
    };
    React.useEffect0(() => {
      Js.log("Running Resources effect...");
      switch (state) {
      | Loading =>
        Js.log("Fetching resources...");
        Js.Promise.(
          Fetcher.fetch("/api/resources.json")
          |> then_(json =>
               json
               |> SF.Resources.decode
               |> Belt.Result.getExn
               |> SF.V2.fromV1(~defaultTenant=default_tenant)
               |> updateState
               |> resolve
             )
          |> ignore
        );
      | _ => ()
      };
      None;
    });

    state;
  };
};
