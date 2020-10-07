// A hook to fetch info
type info = {
  services: list(service),
  version: string,
}
and service = {
  name: string,
  path: string,
};

module Decode = {
  let service = json =>
    Json.Decode.{
      name: json |> field("name", string),
      path: json |> field("path", string),
    };

  let info = json =>
    Json.Decode.{
      version: json |> field("version", string),
      services: json |> field("services", list(service)),
    };
};

module Hook = (Fetcher: Dependencies.Fetcher) => {
  type state =
    | Loading
    | Loaded(info);

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
          |> then_(json => json |> Decode.info |> updateInfo |> resolve)
          |> ignore
        );
      | _ => ()
      };
      None;
    });

    state;
  };
};
