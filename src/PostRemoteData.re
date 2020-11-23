type state('a) =
  | NotAsked
  | Loading
  | Success(option('a))
  | Failure(string);
type decoder_t('a, 'b) = Js.Json.t => Belt.Result.t('a, 'b);

type request = {qs: list((string, string))};
type bodyRequest = {
  qs: list((string, string)),
  body: Js.Json.t,
};

type action =
  | Post(bodyRequest)
  | Put(bodyRequest)
  | Delete(request);
type t('a) = (state('a), action => unit);

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
  let use = (endpoint: string, decoder: option(decoder_t('a, 'b))): t('a) => {
    let (state, setState) = React.useState(() => NotAsked);
    let call = (action: action): unit => {
      setState(_ => Loading);
      switch (action) {
      | Delete(request) =>
        Js.Promise.(
          Fetcher.delete(buildUrl(endpoint, request.qs))
          |> then_(result => {
               switch (result) {
               | Ok(_resp) => Success(None) |> resolve
               | Error(e) => Failure(e) |> resolve
               }
             })
          |> ignore
        )
      | Post(request)
      | Put(request) =>
        Js.Promise.(
          Fetcher.put(buildUrl(endpoint, request.qs), request.body->Some)
          |> then_(result =>
               (
                 switch (result) {
                 | Ok(maybeJson) =>
                   switch (maybeJson, decoder) {
                   | (None, _)
                   | (_, None) => setState(_ => Success(None))
                   | (Some(json), Some(decoder)) =>
                     switch (json |> decoder) {
                     | Ok(decoded) => setState(_ => Success(decoded->Some))
                     | Error(_) =>
                       setState(_ =>
                         Failure(
                           "Could not decode JSON data with the given encoder",
                         )
                       )
                     }
                   }
                 | Error(e) => setState(_ => Failure(e))
                 }
               )
               |> resolve
             )
        )
        |> ignore
      };
      ();
    };
    (state, call);
  };
};
