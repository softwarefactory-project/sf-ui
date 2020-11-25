// A module to manage remote apis on top of RemoteData

open Belt;

module WebData = {
  // Copied from MargaretKrutikova/advice-app-re
  type t('a) = RemoteData.t('a, option('a), string);

  type action_t('a) =
    | RequestLoading
    | RequestError(string)
    | RequestSuccess('a);

  let toLoading = (data: t('a)): t('a) =>
    RemoteData.(Loading(data |> map(d => Some(d)) |> withDefault(None)));

  let updateWebData = (data: t('a), action: action_t('a)): t('a) => {
    switch (action) {
    | RequestLoading => data |> toLoading
    | RequestError(error) => RemoteData.Failure(error)
    | RequestSuccess(response) => RemoteData.Success(response)
    };
  };
};

// Helper types
type json_t = Js.Json.t
and remote_t('a) = WebData.t('a)
and decoder_t('a) = json_t => decode_t('a)
and result_t('a, 'b) = Result.t('a, 'b)
and decode_t('a) = result_t('a, Decco.decodeError)
and response_t('a) = result_t('a, string)
and promise_t('a) = Js.Promise.t('a)
and dispatch_t('a) = WebData.action_t('a) => unit;

// Helper functions
let note = (o: option('a), e: 'e): result_t('a, 'e) =>
  switch (o) {
  | Some(v) => v->Ok
  | None => e->Error
  };

let deccoErrorToResponse = (r: decode_t('a)): response_t('a) =>
  switch (r) {
  | Ok(v) => v->Ok
  // Todo: better format error
  | Error(e) => e.message->Error
  };

let responseToAction = (response: response_t('a)): WebData.action_t('a) =>
  switch (response) {
  | Ok(r) => r->WebData.RequestSuccess
  | Error(e) => e->WebData.RequestError
  };

// The low-level module
module API = (Fetcher: Dependencies.Fetcher) => {
  let get = (url: string, decode: decoder_t('a), dispatch: dispatch_t('a)) => {
    dispatch(WebData.RequestLoading);
    Js.Promise.(
      Fetcher.fetch(url)
      |> then_(json =>
           json
           // TODO: improve the Fetcher to return a result when real network error happens
           ->note("Network error!")
           ->Result.flatMap(json => json->decode->deccoErrorToResponse)
           ->responseToAction
           ->dispatch
           ->resolve
         )
    );
  };

  let delete = (url: string, dispatch: dispatch_t(unit)) => {
    dispatch(WebData.RequestLoading);
    Js.Promise.(
      Fetcher.delete(url)
      |> then_(result =>
           (
             switch (result) {
             | Ok(_resp) => dispatch(WebData.RequestSuccess())
             | Error(e) => dispatch(e->WebData.RequestError)
             }
           )
           ->resolve
         )
    );
  };

  let post =
      (
        url: string,
        data: option(json_t),
        decode: decoder_t('a),
        dispatch: dispatch_t('a),
      ) => {
    dispatch(WebData.RequestLoading);
    Js.Promise.(
      Fetcher.post(url, data)
      |> then_(resp =>
           resp
           ->Result.flatMap(mjson =>
               mjson
               ->note("Need json!")
               ->Result.flatMap(json => json->decode->deccoErrorToResponse)
               ->responseToAction
               ->dispatch
               ->Ok
             )
           ->resolve
         )
    );
  };

  // A standalone hook to simply returns the remote data
  let simpleGet = (url: string, decoder: decoder_t('a)): remote_t('a) => {
    let (state, setState) = React.useState(() => RemoteData.NotAsked);
    let set_state = s => setState(_prevState => s);

    // Trigger get when not asked
    React.useEffect0(() => {
      switch (state) {
      | RemoteData.NotAsked =>
        get(url, decoder, r => state->WebData.updateWebData(r)->set_state)
        ->ignore
      | _ => ()
      };
      None;
    });
    state;
  };

  // A standalone hook to similar to simpleGet but include post callback
  let simplePost =
      (url: string, decoder: decoder_t('a))
      : (remote_t('a), option(json_t) => promise_t(response_t(unit))) => {
    let (state, setState) = React.useState(() => RemoteData.NotAsked);
    let set_state = s => setState(_prevState => s);

    React.useEffect0(() => {
      switch (state) {
      | RemoteData.NotAsked =>
        get(url, decoder, r => state->WebData.updateWebData(r)->set_state)
        ->ignore
      | _ => ()
      };
      None;
    });

    (
      state,
      obj =>
        post(url, obj, decoder, r =>
          state->WebData.updateWebData(r)->set_state
        ),
    );
  };
};
