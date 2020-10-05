module TenantList = {
  [@react.component]
  let make = (~tenants: list(SF.Tenant.tenant)) => {
    <Patternfly.List>
      {Belt.List.map(tenants, tenant =>
         <Patternfly.ListItem key={tenant.name}>
           {tenant.name |> React.string}
         </Patternfly.ListItem>
       )
       |> Belt.List.toArray
       |> React.array}
    </Patternfly.List>;
  };
};

module App = {
  [@react.component]
  let make = () => {
    let resource = Resources.use();
    <div>
      <h1> {"Welcome to software-factory!" |> React.string} </h1>
      {switch (resource) {
       | Resources.Loading => <p> {"Loading..." |> React.string} </p>
       | Resources.Loaded(resources) =>
         <TenantList tenants={resources.resources.tenants} />
       }}
    </div>;
  };
};

switch (ReactDOM.querySelector("#root")) {
| Some(root) => ReactDOM.render(<App />, root)
| None => Js.log("oops!")
};
