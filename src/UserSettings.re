open Patternfly;
open Patternfly.Layout;

module Page = (Fetcher: Dependencies.Fetcher) => {
  module Hook = Api.Hook(Fetcher);

  let boxStyle = ReactDOM.Style.make(~borderRadius="10px", ());
  let boxTitleStyle =
    ReactDOM.Style.make(
      ~borderTopLeftRadius="10px",
      ~borderTopRightRadius="10px",
      ~backgroundColor="var(--pf-global--palette--blue-100)",
      (),
    );

  module SettingForm = {
    [@react.component]
    let make = (~user: Hook.UserSettings.user, ~callback) => {
      let (idpSync, setIdpSync) = React.useState(_ => user.idp_sync);
      let (fullname, setFullname) = React.useState(() => user.fullname);
      let (email, seteMail) = React.useState(() => user.email);
      <Card isCompact=true style=boxStyle>
        <CardTitle style=boxTitleStyle> "User information" </CardTitle>
        <CardBody>
          <Form>
            <FormGroup label={"Username"->React.string} fieldId="username">
              <TextInput value={user.username} isReadOnly=true id="username" />
            </FormGroup>
            <FormGroup label={"Full name"->React.string} fieldId="fullName">
              <TextInput
                value=fullname
                isReadOnly=idpSync
                id="fullName"
                onChange={(value, _) => {setFullname(_ => value)}}
              />
            </FormGroup>
            <FormGroup label={"eMail"->React.string} fieldId="eMail">
              <TextInput
                value=email
                isReadOnly=idpSync
                id="eMail"
                onChange={(value, _) => {seteMail(_ => value)}}
              />
            </FormGroup>
            <FormGroup
              label={"Manage Identity Provider"->React.string} fieldId="idp">
              <Checkbox
                label="Synchronize user data with the Identity Provider"
                isChecked=idpSync
                onChange={(_, _) => setIdpSync(_ => !idpSync)}
                id="idpSync"
              />
            </FormGroup>
            <ActionGroup>
              <Button
                variant=`Primary
                onClick={_ =>
                  callback(
                    Hook.UserSettings.{
                      username: user.username,
                      fullname,
                      email,
                      idp_sync: idpSync,
                    }
                    ->Hook.UserSettings.user_encode
                    ->Some,
                  )
                  ->ignore
                }>
                "Save"->React.string
              </Button>
            </ActionGroup>
          </Form>
        </CardBody>
      </Card>;
    };
  };

  module ApiForm = {
    [@react.component]
    let make = () => {
      let (state, callback) = Hook.ApiKey.use();
      let (message, apiKeyStr) =
        switch (state.delete_request, state.api_key) {
        | (RemoteData.Failure(error), _) => (
            <Alert
              variant=`Danger
              title={(state.delete_goal ++ " failed: " ++ error)->React.string}
            />,
            (
              state.api_key
              |> RemoteData.withDefault(
                   Hook.ApiKey.{api_key: "unknown"}->Some,
                 )
            )
            ->Belt.Option.flatMap(ak => ak.api_key->Some)
            ->Belt.Option.getWithDefault(""),
          )
        | (RemoteData.Loading(_), _) => (
            <Alert variant=`Info title={"Deleting..."->React.string} />,
            "",
          )
        | (RemoteData.Success(_), RemoteData.Success(Some(apiKey))) => (
            <Alert variant=`Info title={"New key generated"->React.string} />,
            apiKey.api_key,
          )
        | (RemoteData.Success(_), RemoteData.NotAsked)
        | (RemoteData.Success(_), RemoteData.Success(None)) => (
            <Alert variant=`Info title={"Key deleted"->React.string} />,
            "",
          )
        | (_, RemoteData.Loading(None))
        | (_, RemoteData.Loading(Some(None)))
        | (_, RemoteData.NotAsked) => (<p> "loading"->React.string </p>, "")
        | (_, RemoteData.Loading(Some(Some(apiKey))))
        | (_, RemoteData.Success(Some(apiKey))) => (
            React.null,
            apiKey.api_key,
          )
        | (_, RemoteData.Success(None)) => (React.null, "")
        | (_, RemoteData.Failure(title)) => (
            <Alert variant=`Danger title />,
            "",
          )
        };

      <Card isCompact=true style=boxStyle>
        <CardTitle style=boxTitleStyle> "API Key" </CardTitle>
        <CardBody>
          <Form>
            message
            <FormGroup label={"Key"->React.string} fieldId="key">
              <TextInput value=apiKeyStr isReadOnly=true id="key" />
            </FormGroup>
            <ActionGroup>
              <Button
                variant=`Primary
                onClick={_ => callback(Hook.ApiKey.Regenerate)}>
                "Generate new API key"->React.string
              </Button>
              {apiKeyStr == ""
                 ? React.null
                 : <Button
                     variant=`Primary
                     onClick={_ => callback(Hook.ApiKey.Delete)}>
                     "Delete API key"->React.string
                   </Button>}
            </ActionGroup>
          </Form>
        </CardBody>
      </Card>;
    };
  };

  [@react.component]
  let make = (~username: string) => {
    let (state, callback) = Hook.UserSettings.use(username);
    <Stack hasGutter=true>
      {switch (state) {
       | RemoteData.Loading(Some(user))
       | RemoteData.Success(user) => <SettingForm user callback />
       | RemoteData.Loading(None)
       | RemoteData.NotAsked => <p> "Loading"->React.string </p>
       | RemoteData.Failure(title) => <Alert variant=`Danger title />
       }}
      <ApiForm />
    </Stack>;
  };
};
