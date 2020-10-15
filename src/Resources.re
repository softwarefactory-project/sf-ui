// A hook to fetch resources

let resolveSR =
    (repos: list(SF.Repo.repo), sr: SF.Project.sourceRepository)
    : SF.Project.sourceRepository => {
  {
    // Set sr description from repo list
    ...sr,
    description:
      switch (sr.description) {
      | Some(description) => Some(description)
      | None =>
        repos
        ->Belt.List.keep(repo => repo.name == sr.name)
        ->Belt.List.map(repo => repo.description)
        ->Belt.List.head
      },
  };
};

let getProjectTenantName =
    (project: SF.Project.project, default_tenant: string) => {
  Some(
    switch (project.tenant) {
    | Some(tenant) => tenant
    | None => default_tenant
    },
  );
};

let getTenant =
    (tenant_id: string, tenants: list(SF.Tenant.tenant))
    : option(SF.Tenant.tenant) => {
  tenants->Belt.List.keep(tenant => tenant.name == tenant_id)->Belt.List.head;
};

let resolveProject =
    (
      default_tenant: string,
      repos: list(SF.Repo.repo),
      tenants: list(SF.Tenant.tenant),
      project: SF.Project.project,
    )
    : SF.Project.project => {
  ...project,
  tenant: getProjectTenantName(project, default_tenant),
  connection:
    switch (project.connection) {
    | Some(connection) => Some(connection)
    | None =>
      switch (getProjectTenantName(project, default_tenant)) {
      | None => None
      | Some(tenant_id) =>
        switch (getTenant(tenant_id, tenants)) {
        | None => None
        | Some(tenant) => tenant.default_connection
        }
      }
    },
  source_repositories:
    project.source_repositories->Belt.List.map(resolveSR(repos)),
};

/* resolve update the resources schemas to propagate repository description and project tenant information */
let resolveResources =
    (default_tenant: string, resources: SF.Resources.top): SF.Resources.top => {
  resources: {
    ...resources.resources,
    projects:
      resources.resources.projects
      ->Belt.List.map(
          resolveProject(
            default_tenant,
            resources.resources.repos,
            resources.resources.tenants,
          ),
        ),
  },
};

module Hook = (Fetcher: Dependencies.Fetcher) => {
  type state =
    | Loading
    | Loaded(SF.Resources.top);

  let use = (default_tenant: string) => {
    let (state, setState) = React.useState(() => Loading);
    let updateState = resources => {
      setState(_ => Loaded(resolveResources(default_tenant, resources)));
    };
    React.useEffect0(() => {
      Js.log("Running Resources effect...");
      switch (state) {
      | Loading =>
        Js.log("Fetching resources...");
        Js.Promise.(
          Fetcher.fetch("/api/resources.json")
          |> then_(json =>
               json |> SF.Resources.parse |> updateState |> resolve
             )
          |> ignore
        );
      | _ => ()
      };
      None;
    });

    state;
  };
};
