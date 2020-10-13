open Patternfly;

// Need to create the PF bindind for colors palette
// https://www.patternfly.org/v4/guidelines/colors
let pf_global__palette__light_blue_400 = "#008BAD";

module ProjectCard = {
  // Display basic information
  // The idea is to let the user click to get a new Component displayed
  // that will contain the full listing of repo + useful links ...
  let handleClick =
      (project_id: string, isClickable: bool, _: ReactEvent.Mouse.t) => {
    isClickable ? ReasonReactRouter.push("project/" ++ project_id) : ();
  };
  let getLinkItem = ((item, label: string)) => {
    switch (item) {
    | None => ReasonReact.null
    | _ =>
      let url = Belt.Option.getWithDefault(item, "");
      <ListItem key=label>
        <b> {label ++ ": " |> React.string} </b>
        <a href=url> {url |> React.string} </a>
      </ListItem>;
    };
  };
  let getContactsItem = (contacts, label: string) => {
    switch (contacts) {
    | None => ReasonReact.null
    | Some(contacts) =>
      <ListItem>
        <b> {label ++ ": " |> React.string} </b>
        {Belt.List.map(contacts, contact =>
           <span>
             <a href={"mailto:" ++ contact}> {contact |> React.string} </a>
             {" " |> React.string}
           </span>
         )
         |> Belt.List.toArray
         |> React.array}
      </ListItem>
    };
  };
  [@react.component]
  let make = (~project: SF.Project.project, ~isSmall: bool=false) => {
    <Card
      key={project.name}
      isCompact=true
      isSelectable=isSmall
      onClick={handleClick(project.name, isSmall)}>
      <CardTitle>
        <span> {project.name |> React.string} </span>
        <span> {" - " |> React.string} </span>
        <span> {project.description |> React.string} </span>
      </CardTitle>
      <CardBody>
        <List>
          {Belt.List.map(
             [
               (project.website, "Website"),
               (project.issue_tracker_url, "Issue-tracker"),
             ],
             getLinkItem,
           )
           |> Belt.List.toArray
           |> React.array}
          {isSmall
             ? ReasonReact.null
             : {
               [
                 (project.documentation, "Documentation")->getLinkItem,
                 getContactsItem(project.contacts, "Contacts"),
                 getContactsItem(project.mailing_lists, "Mailing-lists")
               ]
               |> Belt.List.toArray
               |> React.array;
             }}
        </List>
      </CardBody>
    </Card>;
  };
};

module TenantCard = {
  [@react.component]
  let make =
      (~tenant: SF.Tenant.tenant, ~tenant_projects: list(SF.Project.project)) => {
    let titleStyle =
      ReactDOM.Style.make(
        ~backgroundColor=pf_global__palette__light_blue_400,
        (),
      );
    <Card key={tenant.name}>
      <CardTitle style=titleStyle>
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
      <CardBody>
        <Bullseye> "This tenant owns the following projects" </Bullseye>
        {Belt.List.map(tenant_projects, project =>
           <ProjectCard key={project.name} project isSmall=true />
         )
         |> Belt.List.toArray
         |> React.array}
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
      ) => {
    let items =
      Belt.List.map(tenants, tenant => {
        <GridItem key={tenant.name}>
          <TenantCard
            tenant
            tenant_projects={
              projects |> SF.Project.filterProjectsByTenant(tenant.name)
            }
          />
        </GridItem>
      })
      |> Belt.List.toArray
      |> React.array;
    <Grid hasGutter=true> items </Grid>;
  };
};

module Main = (Fetcher: Dependencies.Fetcher) => {
  module Res = Resources.Hook(Fetcher);
  module Inf = Info.Hook(Fetcher);

  module MainPage = {
    [@react.component]
    let make = (~info: Info.info, ~resource: Res.state) => {
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
           | Res.Loading => <p> {"Loading resources..." |> React.string} </p>
           | Res.Loaded(resources) =>
             <TenantList
               tenants={resources.resources.tenants}
               projects={resources.resources.projects}
             />
           }}
        </GridItem>
      </Grid>;
    };
  };

  module ProjectPage = {
    [@react.component]
    let make = (~project_id: string, ~resource: Res.state) => {
      switch (resource) {
      | Res.Loading => <p> {"Loading resources..." |> React.string} </p>
      | Res.Loaded(resources) =>
        let maybeProject =
          Belt.List.keep(resources.resources.projects, project =>
            project.name == project_id
          );
        switch (maybeProject) {
        | [] =>
          <p> {"Project " ++ project_id ++ " not found" |> React.string} </p>
        | [project, ..._] => <ProjectCard project />
        };
      };
    };
  };

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
      let url = ReasonReactRouter.useUrl();
      let header =
        <PageHeader logo="logo" topNav={<Menu services={info.services} />} />;
      <Page header>
        <PageSection isFilled=true>
          {switch (url.path) {
           | [] => <MainPage info resource />
           | ["project", project_id] => <ProjectPage project_id resource />
           | _ => <p> {"Not found" |> React.string} </p>
           }}
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
