open Patternfly;

let renderIfSome = (x: option('t), f: 't => React.element) =>
  switch (x) {
  | None => React.null
  | Some(x') => f(x')
  };

let renderIf = (pred: bool, elem: React.element) => pred ? elem : React.null;

let renderIfNot = pred => renderIf(!pred);

let listToReactArray = xs => xs->Belt.List.toArray->React.array;

let renderList = (xs, f) => xs->Belt.List.map(f)->listToReactArray;

let str = React.string;

// Need to create the PF bindind for colors palette
// https://www.patternfly.org/v4/guidelines/colors
let pf_global__palette__light_blue_400 = "#008BAD";

let boxTitleStyle =
  ReactDOM.Style.make(
    ~backgroundColor=pf_global__palette__light_blue_400,
    (),
  );

module SRCard = {
  let getConnection =
      (
        sr: SF.Project.sourceRepository,
        project_connection: option(SF.Connection.connection),
        connections: list(SF.Connection.connection),
      )
      : option(SF.Connection.connection) => {
    switch (sr.connection) {
    | Some(connection_id) =>
      Belt.List.keep(connections, cnx => cnx.name == connection_id)
      ->Belt.List.get(0)
    | None => project_connection
    };
  };

  [@react.component]
  let make =
      (
        ~sr: SF.Project.sourceRepository,
        ~project_connection: option(SF.Connection.connection),
        ~connections: list(SF.Connection.connection),
      ) => {
    <Card isCompact=true>
      <CardBody>
        <span> {sr.name |> str} </span>
        {switch (getConnection(sr, project_connection, connections)) {
         | None => React.null
         | Some(connection) => <span> {connection.name |> str} </span>
         }}
      </CardBody>
    </Card>;
  };
};

module SRsCard = {
  [@react.component]
  let make =
      (
        ~srs: list(SF.Project.sourceRepository),
        ~project_connection: option(SF.Connection.connection),
        ~connections: list(SF.Connection.connection),
      ) => {
    <Card>
      <CardTitle style=boxTitleStyle> "Projects' repositories" </CardTitle>
      <CardBody>
        {srs->renderList(sr =>
           <SRCard key={sr.name} sr project_connection connections />
         )}
      </CardBody>
    </Card>;
  };
};

module ProjectCard = {
  // Display basic information
  // The idea is to let the user click to get a new Component displayed
  // that will contain the full listing of repo + useful links ...
  let handleClick =
      (project_id: string, isClickable: bool, _: ReactEvent.Mouse.t) => {
    isClickable ? ReasonReactRouter.push("project/" ++ project_id) : ();
  };
  let getItem = ((item, label: string, link: bool)) =>
    renderIfSome(item, item =>
      <ListItem key=label>
        <b> {label ++ ": " |> str} </b>
        {link ? <a href=item> {item |> str} </a> : item |> str}
      </ListItem>
    );

  let getContactsItem = (contacts, label: string) =>
    renderIfSome(contacts, contacts =>
      <ListItem key=label>
        <b> {label ++ ": " |> str} </b>
        {contacts->renderList(contact =>
           <span key=contact>
             <a href={"mailto:" ++ contact}> {contact |> str} </a>
             {" " |> str}
           </span>
         )}
      </ListItem>
    );

  let getTenant =
      (tenant_id: string, tenants: list(SF.Tenant.tenant))
      : option(SF.Tenant.tenant) => {
    tenants
    ->Belt.List.keep(tenant => tenant.name == tenant_id)
    ->Belt.List.get(0);
  };

  let getTenantConnection =
      (tenant: SF.Tenant.tenant, connections: list(SF.Connection.connection))
      : option(SF.Connection.connection) => {
    switch (tenant.default_connection) {
    | None => None
    | Some(connection_id) =>
      Belt.List.keep(connections, connection =>
        connection.name == connection_id
      )
      ->Belt.List.get(0)
    };
  };

  let getConnectionFromTenantById =
      (
        tenant_id: string,
        tenants: list(SF.Tenant.tenant),
        connections: list(SF.Connection.connection),
      )
      : option(SF.Connection.connection) => {
    let maybeTenant = getTenant(tenant_id, tenants);
    switch (maybeTenant) {
    | None => None
    | Some(tenant) => getTenantConnection(tenant, connections)
    };
  };

  let getConnection =
      (
        project: SF.Project.project,
        tenants: list(SF.Tenant.tenant),
        connections: list(SF.Connection.connection),
      )
      : option(SF.Connection.connection) => {
    switch (project.connection) {
    | None =>
      switch (project.tenant) {
      | None => getConnectionFromTenantById("local", tenants, connections)
      | Some(tenant_id) =>
        getConnectionFromTenantById(tenant_id, tenants, connections)
      }
    | Some(connection_id) =>
      Belt.List.keep(connections, connection =>
        connection_id == connection.name
      )
      ->Belt.List.get(0)
    };
  };

  [@react.component]
  let make =
      (
        ~project: SF.Project.project,
        ~tenants: list(SF.Tenant.tenant),
        ~connections: list(SF.Connection.connection),
        ~isSmall: bool=false,
      ) =>
    <Card
      key={project.name}
      isCompact=true
      isSelectable=isSmall
      onClick={handleClick(project.name, isSmall)}>
      <CardTitle>
        <span> {project.name |> str} </span>
        <span> {" - " |> str} </span>
        <span> {project.description |> str} </span>
      </CardTitle>
      <CardBody>
        <List>
          {[
             (project.website, "Website", true),
             (project.issue_tracker_url, "Issue-tracker", true),
           ]
           ->renderList(getItem)}
          {renderIfNot(
             isSmall,
             [
               (project.tenant, "Tenant", false)->getItem,
               (project.documentation, "Documentation", true)->getItem,
               getContactsItem(project.contacts, "Contacts"),
               getContactsItem(project.mailing_lists, "Mailing-lists"),
             ]
             ->listToReactArray,
           )}
        </List>
        <br />
        {let project_connection =
           getConnection(project, tenants, connections);
         <SRsCard
           srs={project.source_repositories}
           project_connection
           connections
         />}
      </CardBody>
    </Card>;
};

module TenantCard = {
  [@react.component]
  let make =
      (
        ~tenant: SF.Tenant.tenant,
        ~tenant_projects: list(SF.Project.project),
        ~connections: list(SF.Connection.connection),
      ) => {
    <Card key={tenant.name}>
      <CardTitle style=boxTitleStyle>
        <span> {tenant.name |> str} </span>
        <span> {" - " |> str} </span>
        <span>
          {Belt.Option.getWithDefault(
             tenant.description,
             "The " ++ tenant.name ++ " tenant",
           )
           |> str}
        </span>
      </CardTitle>
      <CardBody>
        <Bullseye> "This tenant owns the following projects" </Bullseye>
        {tenant_projects->renderList(project =>
           <ProjectCard
             key={project.name}
             project
             tenants=[tenant]
             connections
             isSmall=true
           />
         )}
      </CardBody>
    </Card>;
  };
};

module TenantList = {
  [@react.component]
  let make =
      (
        ~tenants: list(SF.Tenant.tenant),
        ~projects: list(SF.Project.project),
        ~connections: list(SF.Connection.connection),
      ) =>
    <Grid hasGutter=true>
      {tenants->renderList(tenant => {
         <GridItem key={tenant.name}>
           <TenantCard
             tenant
             connections
             tenant_projects={
               projects |> SF.Project.filterProjectsByTenant(tenant.name)
             }
           />
         </GridItem>
       })}
    </Grid>;
};

module WelcomePage = {
  [@react.component]
  let make = (~info: SF.Info.info, ~resources: SF.Resources.top) => {
    <Grid hasGutter=true>
      <Bullseye>
        <h1>
          {"Welcome to software-factory " ++ info.version ++ "!" |> str}
        </h1>
      </Bullseye>
      <GridItem offset=Column._1 span=Column._10>
        <TenantList
          tenants={resources.resources.tenants}
          projects={resources.resources.projects}
          connections={resources.resources.connections}
        />
      </GridItem>
    </Grid>;
  };
};

module ProjectPage = {
  [@react.component]
  let make = (~project_id: string, ~resources: SF.Resources.top) => {
    let maybeProject =
      Belt.List.keep(resources.resources.projects, project =>
        project.name == project_id
      );
    switch (maybeProject) {
    | [] => <p> {"Project " ++ project_id ++ " not found" |> str} </p>
    | [project, ..._] =>
      <ProjectCard
        project
        tenants={resources.resources.tenants}
        connections={resources.resources.connections}
      />
    };
  };
};

module Menu = {
  [@react.component]
  let make = (~services: list(SF.Info.service)) => {
    <Nav variant=`Horizontal>
      <NavList>
        {services->renderList(service =>
           <NavItem
             key={service.name} isActive=false onClick={ev => Js.log(ev)}>
             {service.name |> str}
           </NavItem>
         )}
      </NavList>
    </Nav>;
  };
};

module MainRouter = {
  [@react.component]
  let make = (~info: SF.Info.info, ~resources: SF.Resources.top) =>
    <PageSection isFilled=true>
      {switch (ReasonReactRouter.useUrl().path) {
       | [] => <WelcomePage info resources />
       | ["project", project_id] => <ProjectPage project_id resources />
       | _ => <p> {"Not found" |> str} </p>
       }}
    </PageSection>;
};

module Main = (Fetcher: Dependencies.Fetcher) => {
  module Res = Resources.Hook(Fetcher);
  module Inf = Info.Hook(Fetcher);

  module MainWithContext = {
    [@react.component]
    let make = (~info: SF.Info.info) =>
      <Page
        header={
          <PageHeader logo="logo" topNav={<Menu services={info.services} />} />
        }>
        {switch (Res.use()) {
         | Res.Loading => <p> {"Loading resources..." |> str} </p>
         | Res.Loaded(resources) => <MainRouter info resources />
         }}
      </Page>;
  };

  [@react.component]
  let make = () =>
    switch (Inf.use()) {
    | Inf.Loading => <p> {"Loading..." |> str} </p>
    | Inf.Loaded(info) => <MainWithContext info />
    };
};
