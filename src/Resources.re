// A hook to fetch resources
type state =
  | Loading
  | Loaded(SF.Resources.top);

let use = fetch => {
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
        fetch("/api/resources.json")
        // |> then_(Fetch.Response.json)
        |> then_(json => json |> SF.Resources.parse |> updateState |> resolve)
        |> ignore
      );
    | _ => ()
    };
    None;
  });

  state;
};
