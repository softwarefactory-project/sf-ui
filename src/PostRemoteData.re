type state('a) =
  | NotAsked
  | Loading
  | Success('a)
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
  let use = (endpoint: string, decoder: decoder_t('a, 'b)): t('a) => {
    let (state, setState) = React.useState(() => NotAsked);
    let call = (action: action): unit => {
      setState(_ => Loading);
      switch (action) {
      | Delete(request) =>
        Js.Promise.(
          Fetcher.delete(buildUrl(endpoint, request.qs))
          |> then_(_ => Success(None) |> resolve)
          |> catch(_ => Failure("Network error") |> resolve)
          |> ignore
        )
      | Post(request)
      | Put(request) =>
        Js.Promise.(
          Fetcher.post2(buildUrl(endpoint, request.qs), request.body)
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
