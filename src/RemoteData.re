type t('a) =
  | Loading
  | Success('a)
  | Failure(string);

let mzip = (x: t('a), y: t('b)): t(('a, 'b)) => {
  switch (x, y) {
  | (_, Loading)
  | (Loading, _) => Loading
  | (_, Failure(a))
  | (Failure(a), _) => Failure(a)
  | (Success(a), Success(b)) => Success((a, b))
  };
};

module Hook = (Fetcher: Dependencies.Fetcher) => {
  let use =
      (endpoint: string, decoder: Js.Json.t => Belt.Result.t('a, _)): t('a) => {
    let (state, setState) = React.useState(() => Loading);
    React.useEffect0(() => {
      switch (state) {
      | Loading =>
        Js.log("Loading " ++ endpoint);
        Js.Promise.(
          Fetcher.fetch(endpoint)
          |> then_(maybeJson =>
               (
                 switch (maybeJson) {
                 | Some(json) =>
                   switch (json |> decoder) {
                   | Ok(a) => setState(_ => Success(a))
                   | Error(e) =>
                     Js.log(e);
                     setState(_ => Failure("Could not decode response"));
                   }
                 | None => setState(_ => Failure("Network error"))
                 }
               )
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
