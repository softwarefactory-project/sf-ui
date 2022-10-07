// The SF remote api hooks on top of RemoteApi

type resources_hook_t = (RemoteApi.state_t(SF.V2.t), unit => unit);

module Hook = (Fetcher: RemoteAPI.HTTPClient) => {
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
};
