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

let renderLink = (l: SF.Info.link) =>
  <p>
    <a href={l.link} style={ReactDOM.Style.make(~color="#bee1f4", ())}>
      l.name->React.string
    </a>
  </p>;

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
    let nodepool = logo("Nodepool_icon.svg");
    let paste = logo("Paste_icon.svg");
    let etherpad = logo("Etherpad_icon.svg");
    let kibana = logo("Kibana_icon.svg");
    let mumble = logo("Mumble_icon.svg");
    let cgit = logo("CGIT_icon.svg");
    let repoxplorer = logo("repoXplorer_icon.svg");
    let hound = logo("Hound_icon.svg");
    let grafana = logo("Grafana_icon.svg");
    let status = logo("Status_icon.svg");
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
      | "nodepool" => displayLogo(Asset.Logo.nodepool, "Nodepool")
      | "lodgeit" => displayLogo(Asset.Logo.paste, "Paste")
      | "etherpad" => displayLogo(Asset.Logo.etherpad, "Eherpad")
      | "kibana" => displayLogo(Asset.Logo.kibana, "Kibana")
      | "mumble" => displayLogo(Asset.Logo.mumble, "Mumble")
      | "repoxplorer" => displayLogo(Asset.Logo.repoxplorer, "Repoxplorer")
      | "hound" => displayLogo(Asset.Logo.hound, "Hound")
      | "cgit" => displayLogo(Asset.Logo.cgit, "CGIT")
      | "grafana" => displayLogo(Asset.Logo.grafana, "Grafana")
      | "status" => displayLogo(Asset.Logo.status, "Status")
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
             <GridItem key={sr.name} span=PFTypes.Column._6>
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
    isClickable ? RescriptReactRouter.push("project/" ++ project_id) : ();
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
        <Stack hasGutter=true>
          <Bullseye> "This tenant owns the following projects" </Bullseye>
          {tenant.projects
           ->renderList(project =>
               <ProjectCard key={project.name} project isSmall=true />
             )}
        </Stack>
      </CardBody>
    </Card>;
  };
};

module WelcomePage = {
  module TenantList = {
    [@react.component]
    let make = (~tenants: list(SF.V2.Tenant.t)) => {
      <Stack hasGutter=true>
        {tenants->renderList(tenant => {
           <TenantCard key={tenant.name} tenant />
         })}
      </Stack>;
    };
  };

  module SplashLogo = {
    [@react.component]
    let make = (~info: SF.Info.t) => {
      <Bullseye>
        <img
          src={"data:image/png;base64," ++ info.splash_image_b64data}
          width="250"
          height="250"
        />
      </Bullseye>;
    };
  };

  module Menu = {
    [@react.component]
    let make = (~services: list(SF.Info.service)) => {
      <Bullseye>
        {services->renderList(service =>
           <GridItem key={service.name} span=PFTypes.Column._1>
             <Service.Logo name={service.name} link={service.path} />
           </GridItem>
         )}
      </Bullseye>;
    };
  };

  [@react.component]
  let make = (~info: SF.Info.t, ~resourcesHook: Api.resources_hook_t) => {
    let (state, dispatch) = resourcesHook;
    React.useEffect0(RemoteApi.getWhenNeeded(state, dispatch));
    <Stack hasGutter=true>
      <SplashLogo info />
      <Menu services={info.services} />
      <Grid hasGutter=true>
        <GridItem offset=PFTypes.Column._1 span=PFTypes.Column._10>
          {switch (state) {
           | RemoteData.NotAsked
           | RemoteData.Loading(None) => <p> {"Loading..." |> str} </p>
           | RemoteData.Loading(Some(resources))
           | RemoteData.Success(resources) =>
             <TenantList tenants={resources.tenants} />
           | RemoteData.Failure(title) => <Alert variant=`Danger title />
           }}
        </GridItem>
      </Grid>
    </Stack>;
  };
};

module ProjectPage = {
  [@react.component]
  let make = (~project_id: string, ~resourcesHook: Api.resources_hook_t) => {
    let (state, dispatch) = resourcesHook;
    React.useEffect0(RemoteApi.getWhenNeeded(state, dispatch));
    switch (state) {
    | RemoteData.NotAsked
    | RemoteData.Loading(None) => <p> {"Loading..." |> str} </p>
    | RemoteData.Loading(Some(resources))
    | RemoteData.Success(resources) =>
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
    | RemoteData.Failure(title) => <Alert variant=`Danger title />
    };
  };
};

module SFAbout = {
  [@react.component]
  let make = (~info: SF.Info.t, ~isOpen: bool, ~onClose: unit => unit) =>
    <AboutModal
      isOpen
      onClose
      brandImageAlt="SF"
      brandImageSrc={"data:image/png;base64," ++ info.header_logo_b64data}>
      <TextList component=`Dl>
        <TextListItem component=`Dt>
          "Software Factory Version"->React.string
        </TextListItem>
        <TextListItem component=`Dd> info.version->React.string </TextListItem>
        {info.links.contact
         ->UserLogin.notEmpty
         ->renderIf(
             <>
               <TextListItem component=`Dt>
                 "Contacts"->React.string
               </TextListItem>
               <TextListItem component=`Dd>
                 {info.links.contact->renderList(renderLink)}
               </TextListItem>
             </>,
           )}
      </TextList>
    </AboutModal>;
};

module Main = (Fetcher: RemoteAPI.HTTPClient) => {
  module Auth' = Auth.Hook(Fetcher);
  module Hook = Api.Hook(Fetcher);
  module UserSettingsPage = UserSettings.Page(Fetcher);

  let getHeaderLogo = (info: SF.Info.t) =>
    <Brand
      alt="SF"
      src={"data:image/png;base64," ++ info.header_logo_b64data}
      onClick={_ => RescriptReactRouter.push("/")}
    />;

  let getBack = qs => {
    Webapi.Url.(qs |> URLSearchParams.make |> URLSearchParams.get("back"));
  };
  let getBaseUrl = () => Webapi.Dom.(Location.origin(location));

  module Footer = {
    [@react.component]
    let make = (~info: SF.Info.t) => {
      <Stack hasGutter=true>
        <Bullseye>
          <Grid hasGutter=true>
            <GridItem span=PFTypes.Column._4>
              <Bullseye>
                <Stack>
                  <p> <b> "DOCUMENTATION"->React.string </b> </p>
                  {info.links.documentation->renderList(renderLink)}
                </Stack>
              </Bullseye>
            </GridItem>
            <GridItem span=PFTypes.Column._4>
              <Bullseye>
                <Stack>
                  <p> <b> "PLATFORM"->React.string </b> </p>
                  <p> {("version: " ++ info.version)->React.string} </p>
                  {info.links.status->renderList(renderLink)}
                </Stack>
              </Bullseye>
            </GridItem>
            {info.links.contact
             ->UserLogin.notEmpty
             ->renderIf(
                 <GridItem span=PFTypes.Column._4>
                   <Bullseye>
                     <Stack>
                       <p> <b> "CONTACTS"->React.string </b> </p>
                       {info.links.contact->renderList(renderLink)}
                     </Stack>
                   </Bullseye>
                 </GridItem>,
               )}
          </Grid>
        </Bullseye>
      </Stack>;
    };
  };

  module MainWithContext = {
    [@react.component]
    let make = (~info: SF.Info.t, ~resourcesHook: Api.resources_hook_t) => {
      let auth = Auth'.use(~defaultBackend=Auth.Cauth);
      let (modal, setModal) = React.useState(_ => false);
      let header =
        <PageHeader
          logo={getHeaderLogo(info)}
          headerTools={
            <PageHeaderTools>
              <PageHeaderToolsItem>
                <Button variant=`Plain onClick={_ => setModal(_ => true)}>
                  <Icons.Help />
                </Button>
              </PageHeaderToolsItem>
              <UserLogin.Header auth />
            </PageHeaderTools>
          }
        />;

      <Page header>
        <PageSection isFilled=true>
          {let reacturl = RescriptReactRouter.useUrl();
           switch (reacturl.path) {
           | [] => <WelcomePage info resourcesHook />
           | ["project", project_id] =>
             <ProjectPage project_id resourcesHook />
           | ["login"] =>
             let back =
               Belt.Option.getWithDefault(
                 getBack(reacturl.search),
                 getBaseUrl(),
               );
             <UserLogin.Page info auth back />;
           | ["logout"] => <UserLogin.Logout auth />
           | ["auth", "settings"] =>
             switch (auth) {
             | ({user: Some(user)}, _) =>
               <UserSettingsPage username={user.name} />
             | _ => <p> {"You need to login first" |> str} </p>
             }
           | _ => <p> {"Not found" |> str} </p>
           }}
        </PageSection>
        <SFAbout info isOpen=modal onClose={() => setModal(_ => false)} />
        <PageSection variant=`Darker isFilled=true>
          <Footer info />
        </PageSection>
      </Page>;
    };
  };

  module MainWithInfo = {
    [@react.component]
    let make = (~info: SF.Info.t) => {
      // Here we create the resources hook so that
      // the data persists accross navigation
      let resourcesHook = Hook.Resources.use("local");
      <MainWithContext info resourcesHook />;
    };
  };

  [@react.component]
  let make = () =>
    switch (Hook.Info.use()) {
    | RemoteData.NotAsked
    | RemoteData.Loading(None) => <p> {"Loading..." |> str} </p>
    | RemoteData.Loading(Some(info))
    | RemoteData.Success(info) => <MainWithInfo info />
    | RemoteData.Failure(title) => <Alert variant=`Danger title />
    };
};
