module UserSettings = {
  [@decco]
  type user = {
    username: string,
    fullname: string,
    email: string,
    idp_sync: bool,
  };
  [@decco]
  type apiKey = {api_key: string};

  type t = (user, apiKey);
  type useHook = unit => RemoteData.t(t);
  type saveHook = unit => PostRemoteData.t(user);

  type apiKeyState =
    | NotAsked
    | Loading
    | Success(apiKey)
    | Failure(string);
  type apiKeyAction =
    | Regenerate(string);

  type saveApiKey = (apiKeyState, apiKeyAction => unit);
  type saveApiKeyHook = unit => saveApiKey;

  module Hook = (Fetcher: Dependencies.Fetcher) => {
    module RemoteData' = RemoteData.Hook(Fetcher);
    module PostRemoteData' = PostRemoteData.Hook(Fetcher);
    // Todo: record previously fetched user info
    let use = (): RemoteData.t(t) =>
      RemoteData.mzip(
        RemoteData'.use("/api/user.json", user_decode),
        RemoteData'.use("/api/apikey.json", apiKey_decode),
      );
    let use_save_us = (): PostRemoteData.t(user) => {
      PostRemoteData'.use("/manage/services_users", user_decode->Some);
    };
    let use_save_apiKey = (): saveApiKey => {
      let (state, setState) = React.useState(() => NotAsked);
      let call = (action: apiKeyAction): unit => {
        setState(_ => Loading);
        switch (action) {
        | Regenerate(username) =>
          let url =
            PostRemoteData.(
              buildUrl("/auth/apikey", [("username", username)])
            );
          Js.Promise.(
            Fetcher.delete(url)
            |> then_(result =>
                 switch (result) {
                 | Ok(_) =>
                   Fetcher.post(url, None)
                   |> then_(result =>
                        switch (result) {
                        | Ok(maybeJson) =>
                          switch (maybeJson) {
                          | Some(json) =>
                            switch (json |> apiKey_decode) {
                            | Ok(decoded) =>
                              setState(_ => Success(decoded)) |> resolve
                            | Error(_) =>
                              setState(_ => Failure("Unable to decode Json"))
                              |> resolve
                            }
                          | None =>
                            setState(_ => Failure("Missing Json data"))
                            |> resolve
                          }
                        | Error(e) => setState(_ => Failure(e)) |> resolve
                        }
                      )
                 | Error(e) => setState(_ => Failure(e)) |> resolve
                 }
               )
          )
          |> ignore;
        };
      };
      (state, call);
    };
  };
};

module Resources = {
  type t = SF.V2.t
  and hook = string => RemoteData.t(t);

  // A hook to fetch resources
  module Hook = (Fetcher: Dependencies.Fetcher) => {
    module RemoteData' = RemoteData.Hook(Fetcher);

    let use = (default_tenant: string) =>
      RemoteData'.use("/api/resources.json", json =>
        json
        ->SF.Resources.decode
        ->Belt.Result.flatMap(resv1 =>
            resv1->SF.V2.fromV1(~defaultTenant=default_tenant)->Ok
          )
      );
  };
};

module Info = {
  type t = SF.Info.t
  and hook = unit => RemoteData.t(t);

  // A hook to fetch infos
  module Hook = (Fetcher: Dependencies.Fetcher) => {
    module RemoteData' = RemoteData.Hook(Fetcher);

    let use = () => RemoteData'.use("/api/info.json", SF.Info.decode);
  };
};
