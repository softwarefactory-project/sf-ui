// A hook to fetch resources
module Hook = (Fetcher: Dependencies.Fetcher) => {
  type state =
    | Loading
    | Loaded(SF.Resources.top);

  let use = () => {
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
               json |> SF.Resources.parse |> updateState |> resolve
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
