module TenantList = {
  [@react.component]
  let make = (~tenants: list(SF.Tenant.tenant)) => {
    <Patternfly.List>
      {Belt.List.map(tenants, tenant =>
         <Patternfly.ListItem key={tenant.name}>
           {tenant.name |> React.string}
         </Patternfly.ListItem>
       )
       |> Belt.List.toArray
       |> React.array}
    </Patternfly.List>;
  };
};

module Main = (Fetcher: Dependencies.Fetcher) => {
  module Res = Resources.Hook(Fetcher);
  [@react.component]
  let make = () => {
    let resource = Res.use();
    <div>
      <h1> {"Welcome to software-factory!" |> React.string} </h1>
      {switch (resource) {
       | Res.Loading => <p> {"Loading..." |> React.string} </p>
       | Res.Loaded(resources) =>
         <TenantList tenants={resources.resources.tenants} />
       }}
    </div>;
  };
};
