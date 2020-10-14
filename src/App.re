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

type attribute_type =
  | Flat(string)
  | Link(string)
  | Contact(list(string))
  | Empty;

let is_empty = v =>
  switch (v) {
  | Empty => true
  | _ => false
  };

let maybe_attribute = (attribute, ctr) =>
  switch (attribute) {
  | Some(attr) => ctr(attr)
  | None => Empty
  };

let maybe_flat = v => maybe_attribute(v, v => Flat(v));
let maybe_link = v => maybe_attribute(v, v => Link(v));
let maybe_contacts = v => maybe_attribute(v, v => Contact(v));

module ProjectCard = {
  // Display basic information
  // The idea is to let the user click to get a new Component displayed
  // that will contain the full listing of repo + useful links ...
  let handleClick =
      (project_id: string, isClickable: bool, _: ReactEvent.Mouse.t) => {
    isClickable ? ReasonReactRouter.push("project/" ++ project_id) : ();
  };
  let renderAttribute =
      ((label: string, attribute: attribute_type)): React.element =>
    renderIfNot(
      is_empty(attribute),
      <ListItem key=label>
        <b> {label ++ ": " |> str} </b>
        {switch (attribute) {
         | Flat(value) => value |> str
         | Link(link) => <a href=link> {link |> str} </a>
         | Contact(links) =>
           links->renderList(contact =>
             <span key=contact>
               <a href={"mailto:" ++ contact}> {contact |> str} </a>
               {" " |> str}
             </span>
           )
         | Empty => React.null // should not happen
         }}
      </ListItem>,
    );

  let project_attrs = (project: SF.Project.project) => [
    ("Website", maybe_link(project.website)),
    ("Issue-tracker", maybe_link(project.issue_tracker_url)),
  ];

  let project_full_attrs = (project: SF.Project.project) => [
    ("Tenant", maybe_flat(project.tenant)),
    ("Documentation", maybe_link(project.documentation)),
    ("Contacts", maybe_contacts(project.contacts)),
    ("Mailing-lists", maybe_contacts(project.mailing_lists)),
  ];

  [@react.component]
  let make = (~project: SF.Project.project, ~isSmall: bool=false) =>
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
          {project_attrs(project)->renderList(renderAttribute)}
          {renderIfNot(
             isSmall,
             project_full_attrs(project)->renderList(renderAttribute),
           )}
        </List>
      </CardBody>
    </Card>;
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
           <ProjectCard key={project.name} project isSmall=true />
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
      ) =>
    <Grid hasGutter=true>
      {tenants->renderList(tenant => {
         <GridItem key={tenant.name}>
           <TenantCard
             tenant
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
    | [project, ..._] => <ProjectCard project />
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
