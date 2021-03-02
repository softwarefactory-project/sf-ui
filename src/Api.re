// The SF remote api hooks on top of RemoteApi

type resources_hook_t = (RemoteApi.state_t(SF.V2.t), unit => unit);

let apiKeyEndpoint = "/auth/apikey" ++ (Cauth.isDevelopment ? ".json" : "");

let userSettingEndpoint =
  "/manage/services_users" ++ (Cauth.isDevelopment ? ".json" : "");

module Hook = (Fetcher: RemoteAPI.HTTPClient) => {
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
    [@decco.decode]
    type user = {
      username: string,
      fullname: string,
      email: string,
      idp_sync: bool,
    };

    let user_encode: user => Js.Json.t =
      user =>
        [
          ("username", user.username->Js.Json.string),
          ("full_name", user.fullname->Js.Json.string),
          ("email", user.email->Js.Json.string),
          ("idp_sync", user.idp_sync->Js.Json.boolean),
        ]
        ->Js.Dict.fromList
        ->Js.Json.object_;

    [@decco.decode]
    type managesfUser = {
      username: string,
      full_name: string,
      email: string,
      idp_sync: bool,
    };

    [@decco.decode]
    type userPut = {updated_fields: managesfUser};

    let put_decode = json =>
      json
      ->userPut_decode
      ->Belt.Result.flatMap(up =>
          {
            username: up.updated_fields.username,
            fullname: up.updated_fields.full_name,
            email: up.updated_fields.email,
            idp_sync: up.updated_fields.idp_sync,
          }
          ->Ok
        );

    let use = (user: string) =>
      RemoteApi.Hook.useSimplePut(
        userSettingEndpoint ++ "?username=" ++ user,
        user_decode,
        put_decode,
      );
  };

  // ApiKey is a hook that manage two states: get and delete
  module ApiKey = {
    [@decco]
    type apiKey = {api_key: string};

    // Hook internal state
    type state = {
      api_key: state_t(option(apiKey)),
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
      | ApiKeyRequest(action_t(option(apiKey)))
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
          api_key:
            switch (r) {
            | NetworkRequestSuccess(_) => RemoteData.NotAsked
            | _ => state.api_key
            },
          delete_goal: action->goal,
          delete_request: state.delete_request->updateRemoteData(r),
        }
      };

    // Hook internal functions to manage the key
    let get = dispatch =>
      RemoteApi.getMaybe(apiKeyEndpoint, apiKey_decode, r =>
        r->ApiKeyRequest->dispatch
      );
    let delete = (dispatch, intent) =>
      RemoteApi.delete(apiKeyEndpoint, r => r->intent->dispatch);
    let create = dispatch =>
      RemoteApi.post(apiKeyEndpoint, None, apiKey_decode, r =>
        r->actionMap(x => x->Some)->ApiKeyRequest->dispatch
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
