// The SF remote api hooks on top of RemoteApi

type resources_hook_t = (RemoteApi.remote_t(SF.V2.t), unit => unit);

module Hook = (Fetcher: Dependencies.Fetcher) => {
  open RemoteApi;
  module RemoteApi = RemoteApi.API(Fetcher);

  // Info is a simple get hook
  module Info = {
    let use = () =>
      RemoteApi.Hook.useAutoGet("/api/info.json", SF.Info.decode);
  };

  // Resource is a simple get hook with a customized decoder based on
  // the default tenant name (known from info endpoint)
  module Resources = {
    let use = (default_tenant: string): resources_hook_t =>
      RemoteApi.Hook.useGet("/api/resources.json", json =>
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

    let use = () =>
      RemoteApi.Hook.useSimplePost("/api/user.json", user_decode);
  };

  // ApiKey is a hook that manage two states: get and delete
  module ApiKey = {
    [@decco]
    type apiKey = {api_key: string};

    // Hook internal state
    type state =
      | Get(remote_t(apiKey))
      | Delete(remote_t(unit));

    // Hook internal state update action
    type action =
      // Get the key
      | ApiKeyRequest(action_t(apiKey))
      // Delete the key
      | ApiKeyDelete(action_t(unit));

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
        newState->updateRemoteData(r)->Get;
      | ApiKeyDelete(r) =>
        // When deleting the key, discard previous state
        ()->RemoteData.Success->updateRemoteData(r)->Delete
      };

    // Hook internal functions to manage the key
    let get = dispatch =>
      RemoteApi.get("/api/apikey.json", apiKey_decode, r =>
        r->ApiKeyRequest->dispatch
      );
    let delete = dispatch =>
      RemoteApi.delete("/api/apikey.json", r => r->ApiKeyDelete->dispatch);
    let create = dispatch =>
      RemoteApi.post("/api/apikey.json", None, apiKey_decode, r =>
        r->ApiKeyRequest->dispatch
      );

    // Component actions:
    type callbackAction =
      | Delete
      | Regenerate;

    let use = () => {
      // Create state store
      let (state, dispatch) =
        React.useReducer(reducer, RemoteData.NotAsked->Get);

      // Fetch initial key
      React.useEffect0(() => {
        switch (state) {
        | Get(RemoteData.NotAsked) => get(dispatch)->ignore
        | _ => ()
        };
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
