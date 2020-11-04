// A hook to fetch info
module Hook = (Fetcher: Dependencies.Fetcher) => {
  type state =
    | Loading
    | Loaded(SF.Info.t);

  let use = () => {
    let (state, setState) = React.useState(() => Loading);
    let updateInfo = info => {
      setState(_ => Loaded(info));
    };
    React.useEffect0(() => {
      Js.log("Running info effect...");
      switch (state) {
      | Loading =>
        Js.log("Fetching resources...");
        Js.Promise.(
          Fetcher.fetch("/api/info.json")
          |> then_(json =>
               json
               |> SF.Info.decode
               |> Belt.Result.getExn
               |> updateInfo
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
