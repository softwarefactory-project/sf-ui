open Belt;

module RemoteData = {
  // Copied from remotedata-re until sf-ui.RemoteData is removed
  type t('a, 'p, 'e) =
    | NotAsked
    | Loading('p)
    | Failure('e)
    | Success('a);
};

// Some helpers
let note = (o: option('a), e: 'e): Result.t('a, 'e) =>
  switch (o) {
  | Some(v) => v->Ok
  | None => e->Error
  };

let deccoError = (r: Result.t('a, Decco.decodeError)): Result.t('a, string) =>
  switch (r) {
  | Ok(v) => v->Ok
  // Todo: better format error
  | Error({path, message, value}) => message->Error
  };

// A remote api definition needs to provide:
module type RemoteApi = {
  type success
  and loading
  and post_data;

  // Component can provide data (such as the default_tenant name)
  type user_data;

  let path: string;
  let get:
    option((user_data, Js.Json.t) => Result.t(success, Decco.decodeError));
  let post:
    option(
      (
        (user_data, post_data) => option(Js.Json.t),
        (user_data, option(Js.Json.t)) =>
        Result.t(success, Decco.decodeError),
      ),
    );
};

// Interpreter can be bs-fetch or mocked
module type Interpreter = {
  let get: string => Js.Promise.t(option(Js.Json.t));
  let post:
    (string, option(Js.Json.t)) =>
    Js.Promise.t(Result.t(option(Js.Json.t), string));

  let put:
    (string, option(Js.Json.t)) =>
    Js.Promise.t(Result.t(option(Js.Json.t), string));
  let delete: string => Js.Promise.t(Result.t(Fetch.response, string));
};

// The Hook is the component-facing API
module Hook = (RA: RemoteApi, I: Interpreter) => {
  type t = (remote_data, dispatch)
  and remote_data = RemoteData.t(RA.success, RA.loading, string)
  and action =
    | Get
    | Post(RA.post_data)
  and dispatch = action => unit
  and set_state = remote_data => unit;

  let use = (user_data: RA.user_data, init: option(action)): t => {
    // The slot to store the remote data
    let (state, setState) = React.useState(() => RemoteData.NotAsked);
    let set_state = s => setState(_prevState => s)->Ok;

    // An utility function to dispatch an initial action, (such as Get for infos endpoint)
    let dispatchInit = (dispatch, action) => {
      React.useEffect0(() => {
        dispatch(action);
        None;
      });
      None;
    };

    // An utility function to handle get result
    let handleGet = (maybeJson: option(Js.Json.t)) => {
      // Declaring that Get action requires the decodeGet function is tricky to do with module type.
      // Instead we'll just assume that the RemoteApi set the optional
      let decodeGet = RA.get->Option.getExn;

      maybeJson
      ->note("Need json!")
      ->Result.flatMap(json => decodeGet(user_data, json)->deccoError)
      ->Result.flatMap(r => set_state(r->RemoteData.Success));
    };

    // An utility function to handle post preparation and result
    let handlePostInit = (post_data: RA.post_data): option(Js.Json.t) => {
      let (encodePost, _decodePost) = RA.post->Option.getExn;
      encodePost(user_data, post_data);
    };

    let handlePost = (postResult: Result.t(option(Js.Json.t), string)) => {
      let (_encodePost, decodePost) = RA.post->Option.getExn;
      switch (postResult) {
      | Ok(maybeJson) =>
        decodePost(user_data, maybeJson)
        ->Result.flatMap(r => r->RemoteData.Success->set_state)
      | Error(e) => e->RemoteData.Failure->set_state
      };
    };

    let dispatch = (action: action): unit =>
      switch (action) {
      | Get =>
        Js.Promise.(I.get(RA.path) |> then_(json => json->handleGet->resolve))
        ->ignore
      | Post(post_data) =>
        Js.Promise.(
          I.post(RA.path, post_data->handlePostInit)
          |> then_(res => res->handlePost->resolve)
        )
        ->ignore
      };
    init->Option.flatMap(dispatchInit(dispatch))->ignore;
    (state, dispatch);
  };
};

// temporarly keep the SF remote api definition here
module SF = {
  module Info: RemoteApi = {
    type success = SF.V2.t
    and loading = option(SF.V2.t)
    and user_data = string
    and post_data = unit;
    let path = "/api/resources.json";
    let post = None;
    let get =
      Some(
        (default_tenant, json) =>
          json
          ->SF.Resources.decode
          ->Result.flatMap(resv1 =>
              resv1->SF.V2.fromV1(~defaultTenant=default_tenant)->Ok
            ),
      );
  };
  module ApiKey: RemoteApi = {
    type success = {api_key: string}
    and loading = unit
    and user_data = unit
    and post_data = {username: string};
    let path = "/api/apikey.json";
    let get = None;
    let postEncode = (_: user_data, _data: post_data): option(Js.Json.t) =>
      None;
    let postDecode =
        (_: user_data, _maybe_json: option(Js.Json.t))
        : Result.t(success, Decco.decodeError) =>
      {api_key: "todo"}->Ok;
    let post = Some((postEncode, postDecode));
  };
};
