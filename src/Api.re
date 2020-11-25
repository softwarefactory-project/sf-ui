// The SF remote api hooks on top of RemoteApi

type resources_hook_t = (RemoteApi.state_t(SF.V2.t), unit => unit);

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
    type state = {
      api_key: state_t(apiKey),
      delete_goal: string,
      delete_request: state_t(unit),
    };
    let initialState = {
      api_key: RemoteData.NotAsked,
      delete_request: RemoteData.NotAsked,
      delete_goal: "",
    };

    // Hook internal state update action
    type action =
      // Get the key
      | ApiKeyRequest(action_t(apiKey))
      // Delete the key
      | ApiKeyRegenerate(action_t(unit))
      | ApiKeyDelete(action_t(unit));

    let goal =
      fun
      | ApiKeyRegenerate(_) => "Regenerate"
      | ApiKeyDelete(_) => "Delete"
      | ApiKeyRequest(_) => "Request";

    // Hook internal reducer to manage state update
    let reducer = (state: state, action: action): state =>
      switch (action) {
      | ApiKeyRequest(r) => {
          ...state,
          api_key: state.api_key->updateRemoteData(r),
        }
      | ApiKeyRegenerate(r)
      | ApiKeyDelete(r) => {
          ...state,
          delete_goal: action->goal,
          delete_request: state.delete_request->updateRemoteData(r),
        }
      };

    // Hook internal functions to manage the key
    let get = dispatch =>
      RemoteApi.get("/api/apikey.json", apiKey_decode, r =>
        r->ApiKeyRequest->dispatch
      );
    let delete = (dispatch, intent) =>
      RemoteApi.delete("/api/apikey.json", r => r->intent->dispatch);
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
      let (state, dispatch) = React.useReducer(reducer, initialState);

      // Fetch initial key
      React.useEffect0(
        getWhenNeeded(state.api_key, () => get(dispatch)->ignore),
      );

      // The function the component calls to perform actions
      let callback = (cbAction: callbackAction) =>
        (
          switch (cbAction) {
          | Delete => delete(dispatch, x => x->ApiKeyDelete)->ignore
          | Regenerate =>
            delete(dispatch, x => x->ApiKeyRegenerate)
            ->chainCall(() => create(dispatch))
            ->ignore
          }
        )
        ->ignore;
      (state, callback);
    };
  };
};
