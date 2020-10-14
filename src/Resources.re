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
        // todo: do not update if already set
        repos
        ->Belt.List.keep(repo => repo.name == sr.name)
        ->Belt.List.map(repo => repo.description)
        ->Belt.List.head
      },
  };
};

let resolveProject =
    (
      default_tenant: string,
      repos: list(SF.Repo.repo),
      project: SF.Project.project,
    )
    : SF.Project.project => {
  {
    ...project,
    source_repositories:
      project.source_repositories->Belt.List.map(resolveSR(repos)),
  };
};

/* resolve update the resources schemas to propagate repository description and project tenant information */
let resolve =
    (default_tenant: string, resources: SF.Resources.top): SF.Resources.top => {
  {
    resources: {
      ...resources.resources,
      projects:
        resources.resources.projects
        ->Belt.List.map(
            resolveProject(default_tenant, resources.resources.repos),
          ),
    },
  };
};

module Hook = (Fetcher: Dependencies.Fetcher) => {
  type state =
    | Loading
    | Loaded(SF.Resources.top);

  let use = (default_tenant: string) => {
    let (state, setState) = React.useState(() => Loading);
    let updateState = resources => {
      setState(_ => Loaded(resolve(default_tenant, resources)));
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
