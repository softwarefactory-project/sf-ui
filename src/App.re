open Patternfly;

module ProjectCard = {
  // Display basic information
  // The idea is to let the user click to get a new Component displayed
  // that will contain the full listing of repo + useful links ...
  [@react.component]
  let make = (~project: SF.Project.project) => {
    <Card key={project.name}>
      <CardTitle> {project.name |> React.string} </CardTitle>
      <CardBody> {"body" |> React.string} </CardBody>
    </Card>;
  };
};

module TenantCard = {
  [@react.component]
  let make = (~tenant: SF.Tenant.tenant) => {
    <Card key={tenant.name}>
      <CardTitle>
        <span> {tenant.name |> React.string} </span>
        <span> {" - " |> React.string} </span>
        <span>
          {Belt.Option.getWithDefault(
             tenant.description,
             "The " ++ tenant.name ++ " tenant",
           )
           |> React.string}
        </span>
      </CardTitle>
      // Write a fonction to discover tenant's projects
      // Display one ProjectCard by project
      <CardBody> {"List the tenant's projects" |> React.string} </CardBody>
    </Card>;
  };
};

module TenantList = {
  [@react.component]
  let make = (~tenants: list(SF.Tenant.tenant)) => {
    let items =
      Belt.List.map(tenants, tenant => {
        <GridItem key={tenant.name}> <TenantCard tenant /> </GridItem>
      })
      |> Belt.List.toArray
      |> React.array;
    <Grid hasGutter=true> items </Grid>;
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
      let header =
        <PageHeader logo="logo" topNav={<Menu services={info.services} />} />;
      <Page header>
        <PageSection isFilled=true>
          <Grid hasGutter=true>
            <Bullseye>
              <h1>
                {React.string(
                   "Welcome to software-factory " ++ info.version ++ "!",
                 )}
              </h1>
            </Bullseye>
            <GridItem offset=Column._1 span=Column._10>
              {switch (resource) {
               | Res.Loading =>
                 <p> {"Loading resources..." |> React.string} </p>
               | Res.Loaded(resources) =>
                 <TenantList tenants={resources.resources.tenants} />
               }}
            </GridItem>
          </Grid>
        </PageSection>
      </Page>;
    };
  };

  [@react.component]
  let make = () => {
    let info = Inf.use();
    let resource = Res.use();
    switch (info) {
    | Inf.Loading => <p> {"Loading..." |> React.string} </p>
    | Inf.Loaded(info) => <MainWithContext info resource />
    };
  };
};
