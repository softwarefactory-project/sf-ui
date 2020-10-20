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

let buildURL = (url: string, label: string) => {
  <a href=url> {label |> str} </a>;
};

let boxStyle = ReactDOM.Style.make(~borderRadius="10px", ());

let boxTitleStyle =
  ReactDOM.Style.make(
    ~borderTopLeftRadius="10px",
    ~borderTopRightRadius="10px",
    ~backgroundColor="var(--pf-global--palette--blue-100)",
    (),
  );

let getConnectionById =
    (
      connections: list(SF.Connection.connection),
      connection_id: option(string),
    )
    : option(SF.Connection.connection) => {
  switch (connection_id) {
  | Some(connection_id) =>
    Belt.List.keep(connections, cnx => cnx.name == connection_id)
    ->Belt.List.head
  | None => None
  };
};

let displayImg = (width: string, height, src: string, alt: string) => {
  <img src alt width height />;
};

module Asset = {
  // require binding does not support complex string manipulation
  [@bs.val] external require: string => string = "require";
  let asset = path => require("../assets/" ++ path);

  module Logo = {
    let logo = name => asset("logos/" ++ name);
    // the assets
    let gerrit = logo("Gerrit_icon.svg");
    let zuul = logo("Zuul_icon.svg");
    let paste = logo("Paste_icon.svg");
    let etherpad = logo("Etherpad_icon.svg");
    let kibana = logo("Kibana_icon.svg");
    let mumble = logo("Mumble_icon.svg");
    let cgit = logo("CGIT_icon.svg");
  };
};

module Service = {
  let getLogoImg = displayImg("75", "75");

  module Logo = {
    [@react.component]
    let make = (~name: string, ~link: string) => {
      let displayLogo = (src: string, alt: string) => {
        <a href=link> {getLogoImg(src, alt)} </a>;
      };
      switch (name) {
      | "gerrit" => displayLogo(Asset.Logo.gerrit, "Gerrit")
      | "zuul" => displayLogo(Asset.Logo.zuul, "Zuul")
      | "paste" => displayLogo(Asset.Logo.paste, "Paste")
      | "etherpad" => displayLogo(Asset.Logo.etherpad, "Eherpad")
      | "kibana" => displayLogo(Asset.Logo.kibana, "Kibana")
      | "mumble" => displayLogo(Asset.Logo.mumble, "Mumble")
      | "cgit" => displayLogo(Asset.Logo.cgit, "CGIT")
      | _ => React.null
      };
    };
  };
};

module SRCard = {
  let buildCloneURLE =
      (connection: SF.Connection.connection, name: string): React.element => {
    switch (connection.base_url) {
    | None => React.null
    | Some(base_url) =>
      <span> {"git clone " ++ base_url ++ "/" ++ name |> str} </span>
    };
  };

  let buildGitwebURLBrowserE =
      (connection: SF.Connection.connection, name: string): React.element => {
    let toA = (url: string) => {
      buildURL(url, "Browse tree");
    };
    switch (connection.base_url, connection.connection_type) {
    | (Some(base_url), GERRIT) =>
      base_url ++ "/gitweb?p=" ++ name ++ ".git;a=tree" |> toA
    | (Some(base_url), GITHUB | GITLAB) => base_url ++ "/" ++ name |> toA
    | (Some(base_url), PAGURE) => base_url ++ "/" ++ name ++ "/tree" |> toA
    | (_, GIT) => React.null
    | (None, _) => React.null
    };
  };

  let buildGitwebURLLastCommitsE =
      (connection: SF.Connection.connection, name: string): React.element => {
    let toA = (url: string) => {
      buildURL(url, "Last commits");
    };
    switch (connection.base_url, connection.connection_type) {
    | (Some(base_url), GERRIT) =>
      base_url ++ "/gitweb?p=" ++ name ++ ".git;a=shortlog" |> toA
    | (Some(base_url), GITHUB | GITLAB | PAGURE) =>
      base_url ++ "/" ++ name ++ "/commits" |> toA
    | (_, GIT) => React.null
    | (None, _) => React.null
    };
  };

  let buildOpenChangesE =
      (connection: SF.Connection.connection, name: string): React.element => {
    let toA = (url: string) => {
      buildURL(url, "Open changes");
    };
    switch (connection.base_url, connection.connection_type) {
    | (Some(base_url), GERRIT) =>
      base_url ++ "/q/status:open+project:" ++ name |> toA
    | (Some(base_url), GITHUB) => base_url ++ "/" ++ name ++ "/pulls" |> toA
    | (Some(base_url), PAGURE) =>
      base_url ++ "/" ++ name ++ "/pull-requests" |> toA
    | (Some(base_url), GITLAB) =>
      base_url ++ "/" ++ name ++ "/merge_requests" |> toA
    | (_, GIT) => React.null
    | (None, _) => React.null
    };
  };

  let getConnection =
      (
        sr: SF.Project.sourceRepository,
        project_connection: option(SF.Connection.connection),
        connections: list(SF.Connection.connection),
      )
      : option(SF.Connection.connection) => {
    switch (sr.connection) {
    | Some(connection_id) =>
      getConnectionById(connections, Some(connection_id))
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
    let connection = getConnection(sr, project_connection, connections);
    <Card isCompact=true style=boxStyle>
      <CardTitle>
        <span> <b> {sr.name |> str} </b> </span>
        {renderIfSome(sr.description, desc =>
           <span> {" - " |> str} {desc |> str} </span>
         )}
      </CardTitle>
      <CardBody>
        {renderIfSome(connection, connection => {
           <>
             <div> {connection->buildCloneURLE(sr.name)} </div>
             <div>
               <span> {connection->buildGitwebURLBrowserE(sr.name)} </span>
               <span> {" / " |> str} </span>
               <span>
                 {connection->buildGitwebURLLastCommitsE(sr.name)}
               </span>
               <span> {" / " |> str} </span>
               <span> {connection->buildOpenChangesE(sr.name)} </span>
             </div>
           </>
         })}
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
    <Card isCompact=true style=boxStyle>
      <CardTitle style=boxTitleStyle> "Projects' repositories" </CardTitle>
      <CardBody>
        <br />
        <Grid hasGutter=true>
          {srs->renderList(sr => {
             <GridItem key={sr.name} span=Column._6>
               <SRCard key={sr.name} sr project_connection connections />
             </GridItem>
           })}
        </Grid>
      </CardBody>
    </Card>;
  };
};

module ProjectCard = {
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
         | Link(link) => link |> buildURL(link)
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
  let make =
      (
        ~project: SF.Project.project,
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
          {project_attrs(project)->renderList(renderAttribute)}
          {renderIfNot(
             isSmall,
             project_full_attrs(project)->renderList(renderAttribute),
           )}
        </List>
        <br />
        {renderIfNot(
           isSmall,
           <SRsCard
             srs={project.source_repositories}
             project_connection={getConnectionById(
               connections,
               project.connection,
             )}
             connections
           />,
         )}
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
    <Card key={tenant.name} isCompact=true style=boxStyle>
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
           <ProjectCard key={project.name} project connections isSmall=true />
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
  let make = (~resources: SF.Resources.top) => {
    <Grid hasGutter=true>
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
      <ProjectCard project connections={resources.resources.connections} />
    };
  };
};

module Menu = {
  [@react.component]
  let make = (~services: list(SF.Info.service)) => {
    <Bullseye>
      {services->renderList(service =>
         <GridItem key={service.name} span=Column._1>
           <Service.Logo name={service.name} link={service.path} />
         </GridItem>
       )}
    </Bullseye>;
  };
};

module MainRouter = {
  [@react.component]
  let make = (~resources: SF.Resources.top) =>
    <PageSection isFilled=true>
      {switch (ReasonReactRouter.useUrl().path) {
       | [] => <WelcomePage resources />
       | ["project", project_id] => <ProjectPage project_id resources />
       | _ => <p> {"Not found" |> str} </p>
       }}
    </PageSection>;
};

module Main = (Fetcher: Dependencies.Fetcher) => {
  module Res = Resources.Hook(Fetcher);
  module Inf = Info.Hook(Fetcher);

  let getHeaderLogo = (info: SF.Info.info) =>
    <Brand
      alt="SF"
      src={"data:image/png;base64," ++ info.splash_image_b64data}
    />;

  module MainWithContext = {
    [@react.component]
    let make = (~info: SF.Info.info) =>
      <Page>
        <PageHeader logo={getHeaderLogo(info)} />
        <Bullseye>
          <h1>
            <img
              src={"data:image/png;base64," ++ info.splash_image_b64data}
              width="250"
              height="250"
            />
          </h1>
        </Bullseye>
        <Menu services={info.services} />
        {switch (Res.use("local")) {
         | Res.Loading => <p> {"Loading resources..." |> str} </p>
         | Res.Loaded(resources) => <MainRouter resources />
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
