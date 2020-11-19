open Patternfly;
open Patternfly.Layout;

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

let displayImg = (width: string, height, src: string, alt: string) => {
  <img
    src
    alt
    width
    height
    style={ReactDOM.Style.make(~marginLeft="5px", ~marginRight="5px", ())}
  />;
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
    let avatar =
      require(
        "@patternfly/react-core/src/components/Avatar/examples/avatarImg.svg",
      );
  };
};

module Service = {
  let getLogoImg = displayImg("75", "75");

  module Logo = {
    [@react.component]
    let make = (~name: string, ~link: string) => {
      let displayLogo = (src: string, alt: string) =>
        <>
          <a href=link> {getLogoImg(src, alt)} </a>
          <center> {alt |> React.string} </center>
        </>;

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
  [@react.component]
  let make = (~sr: SF.V2.SourceRepository.t) => {
    let cloneUrl = {
      "git clone " ++ sr.location->SF.V2.Connection.cloneUrl(sr.name) |> str;
    };
    let webUrl =
      sr.location->SF.V2.Connection.webUrl(sr.name)->buildURL("Browse tree");
    let commitUrl =
      sr.location
      ->SF.V2.Connection.commitUrl(sr.name)
      ->buildURL("Last commits");
    let changesUrl =
      sr.location
      ->SF.V2.Connection.changesUrl(sr.name)
      ->buildURL("Open changes");
    <Card isCompact=true style=boxStyle>
      <CardTitle>
        <span> <b> {sr.name |> str} </b> </span>
        {renderIfSome(sr.description, desc =>
           <span> {" - " |> str} {desc |> str} </span>
         )}
      </CardTitle>
      <CardBody>
        <>
          <div> <span> cloneUrl </span> </div>
          <div>
            <span> webUrl </span>
            <span> " / "->str </span>
            commitUrl
            <span> " / "->str </span>
            changesUrl
          </div>
        </>
      </CardBody>
    </Card>;
  };
};

module SRsCard = {
  [@react.component]
  let make = (~srs: list(SF.V2.SourceRepository.t)) => {
    <Card isCompact=true style=boxStyle>
      <CardTitle style=boxTitleStyle> "Projects' repositories" </CardTitle>
      <CardBody>
        <br />
        <Grid hasGutter=true>
          {srs->renderList(sr => {
             <GridItem key={sr.name} span=Column._6>
               <SRCard key={sr.name} sr />
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

  let project_attrs = (project: SF.V2.Project.t) => [
    ("Website", maybe_link(project.website)),
    ("Issue-tracker", maybe_link(project.issue_tracker_url)),
  ];

  let project_full_attrs = (project: SF.V2.Project.t) => [
    ("Documentation", maybe_link(project.documentation)),
    ("Contacts", maybe_contacts(project.contacts)),
    ("Mailing-lists", maybe_contacts(project.mailing_lists)),
  ];

  [@react.component]
  let make = (~project: SF.V2.Project.t, ~isSmall: bool=false) =>
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
        {renderIfNot(isSmall, <SRsCard srs={project.source_repositories} />)}
      </CardBody>
    </Card>;
};

module TenantCard = {
  [@react.component]
  let make = (~tenant: SF.V2.Tenant.t) => {
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
        {tenant.projects
         ->renderList(project =>
             <ProjectCard key={project.name} project isSmall=true />
           )}
      </CardBody>
    </Card>;
  };
};

module WelcomePage = {
  module TenantList = {
    [@react.component]
    let make = (~tenants: list(SF.V2.Tenant.t)) =>
      <Grid hasGutter=true>
        {tenants->renderList(tenant => {
           <GridItem key={tenant.name}> <TenantCard tenant /> </GridItem>
         })}
      </Grid>;
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

  let splashLogo = (info: SF.Info.t) =>
    <Bullseye>
      <img
        src={"data:image/png;base64," ++ info.splash_image_b64data}
        width="250"
        height="250"
      />
    </Bullseye>;

  [@react.component]
  let make = (~info: SF.Info.t, ~resources: SF.V2.t) => {
    <>
      {splashLogo(info)}
      <Menu services={info.services} />
      <Grid hasGutter=true>
        <GridItem offset=Column._1 span=Column._10>
          <TenantList tenants={resources.tenants} />
        </GridItem>
      </Grid>
    </>;
  };
};

module ProjectPage = {
  [@react.component]
  let make = (~project_id: string, ~resources: SF.V2.t) => {
    let maybeProject =
      resources.tenants
      ->Belt.List.map(tenant => tenant.projects)
      ->Belt.List.flatten
      ->Belt.List.keep(project => project.name == project_id)
      ->Belt.List.head;
    switch (maybeProject) {
    | Some(project) => <ProjectCard project />
    | None => <p> {"Project " ++ project_id ++ " not found" |> str} </p>
    };
  };
};

module Main = (Fetcher: Dependencies.Fetcher) => {
  module Res = Resources.Hook(Fetcher);
  module Inf = Info.Hook(Fetcher);
  module Auth' = Auth.Hook(Fetcher);
  module Managesf' = Managesf.Hook(Fetcher);

  let getHeaderLogo = (info: SF.Info.t) =>
    <Brand
      alt="SF"
      src={"data:image/png;base64," ++ info.header_logo_b64data}
      onClick={_ => ReasonReactRouter.push("/")}
    />;

  module MainWithContext = {
    [@react.component]
    let make = (~info: SF.Info.t, ~resources: SF.V2.t) => {
      let auth = Auth'.use(~defaultBackend=Auth.Cauth);
      let header =
        <PageHeader
          logo={getHeaderLogo(info)}
          headerTools={
            <PageHeaderTools>
              <PageHeaderToolsItem>
                <Button variant=`Plain> <Icons.Help /> </Button>
              </PageHeaderToolsItem>
              <UserLogin.Header auth />
            </PageHeaderTools>
          }
        />;

      <Page header>
        <PageSection isFilled=true>
          {switch (ReasonReactRouter.useUrl().path) {
           | [] => <WelcomePage info resources />
           | ["project", project_id] => <ProjectPage project_id resources />
           | ["auth", "login"] => <UserLogin.Page info auth />
           | ["auth", "settings"] =>
             <UserSettings.Page managesf=Managesf'.use />
           | _ => <p> {"Not found" |> str} </p>
           }}
        </PageSection>
      </Page>;
    };
  };

  module MainWithInfo = {
    [@react.component]
    let make = (~info: SF.Info.t) =>
      switch (Res.use("local")) {
      | Res.Loading => <p> {"Loading resources..." |> str} </p>
      | Res.Loaded(resources) => <MainWithContext info resources />
      };
  };

  [@react.component]
  let make = () => {
    switch (Inf.use()) {
    | Inf.Loading => <p> {"Loading..." |> str} </p>
    | Inf.Loaded(info) => <MainWithInfo info />
    };
  };
};
