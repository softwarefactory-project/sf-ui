type state('a) =
  | NotAsked
  | Loading
  | Success('a)
  | Failure(string);
type decoder_t('a, 'b) = Js.Json.t => Belt.Result.t('a, 'b);
type request('a) = {
  qs: list((string, string)),
  body: Js.Json.t,
};
type action('a) =
  | Post(request('a))
  | Put(request('a));
type t('a) = (state('a), action('a) => unit);

// https://stackoverflow.com/questions/50383744/how-to-pass-query-string-parameters-with-bs-fetch
[@bs.val] external encodeURIComponent: string => string;

let buildUrl = (url, params) => {
  let encodeParam = ((key, value)) =>
    encodeURIComponent(key) ++ "=" ++ encodeURIComponent(value);

  let params =
    params
    |> List.map(encodeParam)
    |> Belt.List.toArray
    |> Js.Array.joinWith("&");

  switch (params) {
  | "" => url
  | _ => {j|$url?$params|j}
  };
};

module Hook = (Fetcher: Dependencies.Fetcher) => {
  let actionToVerb = (action: action('a)) => {
    switch (action) {
    | Post(_) => Fetcher.(POST)
    | Put(_) => Fetcher.(PUT)
    };
  };
  let use = (endpoint: string, decoder: decoder_t('a, 'b)): t('a) => {
    let (state, setState) = React.useState(() => NotAsked);
    let call = (action: action('a)): unit => {
      setState(_ => Loading);
      switch (action) {
      | Post(request)
      | Put(request) =>
        Js.Promise.(
          Fetcher.post2(
            buildUrl(endpoint, request.qs),
            action->actionToVerb,
            request.body,
          )
          |> then_(maybeJson => {
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
             })
          |> ignore
        )
      };
      ();
    };
    (state, call);
  };
};
