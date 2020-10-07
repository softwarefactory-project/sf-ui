open Patternfly;

module TenantList = {
  [@react.component]
  let make = (~tenants: list(SF.Tenant.tenant)) => {
    <>
      <h2> {"Tenant list" |> React.string} </h2>
      <List>
        {Belt.List.map(tenants, tenant =>
           <Card key={tenant.name}>
             <CardTitle> {tenant.name |> React.string} </CardTitle>
             <CardBody> {"body" |> React.string} </CardBody>
             <CardFooter> {"footer" |> React.string} </CardFooter>
           </Card>
         )
         |> Belt.List.toArray
         |> React.array}
      </List>
    </>;
  };
};

module Main = (Fetcher: Dependencies.Fetcher) => {
  module Res = Resources.Hook(Fetcher);
  module Inf = Info.Hook(Fetcher);

  module Menu = {
    [@react.component]
    let make = (~services: list(Info.service)) => {
      <Nav variant=`Horizontal>
        <NavList>
          {Belt.List.map(services, service =>
             <NavItem
               key={service.name} isActive=false onClick={ev => Js.log(ev)}>
               {service.name |> React.string}
             </NavItem>
           )
           |> Belt.List.toArray
           |> React.array}
        </NavList>
      </Nav>;
    };
  };
  module MainWithContext = {
    [@react.component]
    let make = (~info: Info.info, ~resource: Res.state) => {
      <>
        <PageHeader logo="logo" topNav={<Menu services={info.services} />} />
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
    <Page>
      {switch (info) {
       | Inf.Loading => <p> {"Loading..." |> React.string} </p>
       | Inf.Loaded(info) => <MainWithContext info resource />
       }}
    </Page>;
  };
};
