// The SF remote api hooks

module Hook = (Fetcher: Dependencies.Fetcher) => {
  open RemoteApi;
  module API = RemoteApi.API(Fetcher);

  // Info is a simple get hook
  module Info = {
    let use = () => API.simpleGet("/api/info.json", SF.Info.decode);
  };

  // Resource is a simple get hook with a customized decoder based on
  // the default tenant name (known from info endpoint)
  module Resources = {
    let use = default_tenant =>
      API.simpleGet("/api/resources.json", json =>
        json
        ->SF.Resources.decode
        ->Belt.Result.flatMap(resv1 =>
            resv1->SF.V2.fromV1(~defaultTenant=default_tenant)->Ok
          )
      );
  };

  // UserSettings is a simple get/post hook
  module UserSettings = {
    [@decco]
    type user = {
      username: string,
      fullname: string,
      email: string,
      idp_sync: bool,
    };

    let use = () => API.simplePost("/api/user.json", user_decode);
  };

  // ApiKey is a hook that manage two states: get and delete
  module ApiKey = {
    [@decco]
    type apiKey = {api_key: string};

    // Hook internal state
    type state =
      | Get(WebData.t(apiKey))
      | Delete(WebData.t(unit));

    // Hook internal state update action
    type action =
      // Get the key
      | ApiKeyRequest(WebData.action_t(apiKey))
      // Delete the key
      | ApiKeyDelete(WebData.action_t(unit));

    // Hook internal reducer to manage state update
    let reducer = (state: state, action: action): state =>
      switch (action) {
      | ApiKeyRequest(r) =>
        let newState =
          switch (state) {
          | Get(v) => v
          // If the key was deleted, then discard previous state
          | Delete(_) => RemoteData.NotAsked
          };
        newState->WebData.updateWebData(r)->Get;
      | ApiKeyDelete(r) =>
        // When deleting the key, discard previous state
        ()->RemoteData.Success->WebData.updateWebData(r)->Delete
      };

    // Hook internal functions to manage the key
    let get = dispatch =>
      API.get("/api/apikey.json", apiKey_decode, r =>
        r->ApiKeyRequest->dispatch
      );
    let delete = dispatch =>
      API.delete("/api/apikey.json", r => r->ApiKeyDelete->dispatch);
    let create = dispatch =>
      API.post("/api/apikey.json", None, apiKey_decode, r =>
        r->ApiKeyRequest->dispatch
      );

    // Component actions:
    type callbackAction =
      | Delete
      | Regenerate;

    let use = () => {
      let (state, dispatch) =
        React.useReducer(reducer, RemoteData.NotAsked->Get);
      React.useEffect0(() => {
        get(dispatch)->ignore;
        None;
      });
      // The function the component calls to perform actions
      let callback = (cbAction: callbackAction) =>
        (
          switch (cbAction) {
          | Delete => delete(dispatch)->ignore
          | Regenerate =>
            Js.Promise.(delete(dispatch) |> then_(() => create(dispatch)))
            ->ignore
          }
        )
        ->ignore;
      (state, callback);
    };
  };
};
