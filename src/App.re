module TenantList = {
  [@react.component]
  let make = (~tenants: list(SF.Tenant.tenant)) => {
    <>
      <h2> {"Tenant list" |> React.string} </h2>
      <Patternfly.List>
        {Belt.List.map(tenants, tenant =>
           <Patternfly.ListItem key={tenant.name}>
             {tenant.name |> React.string}
           </Patternfly.ListItem>
         )
         |> Belt.List.toArray
         |> React.array}
      </Patternfly.List>
    </>;
  };
};

module Main = (Fetcher: Dependencies.Fetcher) => {
  module Res = Resources.Hook(Fetcher);
  module Inf = Info.Hook(Fetcher);

  module MainWithContext = {
    [@react.component]
    let make = (~info: Info.info, ~resource: Res.state) => {
      Js.log("Info:");
      Js.log(info.services |> Belt.List.toArray);
      <>
        <h1>
          {React.string("Welcome to software-factory " ++ info.version ++ "!")}
        </h1>
        {switch (resource) {
         | Res.Loading => <p> {"Loading resources..." |> React.string} </p>
         | Res.Loaded(resources) =>
           <TenantList tenants={resources.resources.tenants} />
         }}
      </>;
    };
  };

  [@react.component]
  let make = () => {
    let info = Inf.use();
    let resource = Res.use();
    <div>
      {switch (info) {
       | Inf.Loading => <p> {"Loading..." |> React.string} </p>
       | Inf.Loaded(info) => <MainWithContext info resource />
       }}
    </div>;
  };
};
