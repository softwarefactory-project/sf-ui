open Patternfly;
module Page = (Fetcher: Dependencies.Fetcher) => {
  module Hook = Api.Hook(Fetcher);

  module SettingForm = {
    [@react.component]
    let make = (~user: Hook.UserSettings.user, ~callback) => {
      let (idpSync, setIdpSync) = React.useState(_ => user.idp_sync);
      let (fullname, setFullname) = React.useState(() => user.fullname);
      let (email, seteMail) = React.useState(() => user.email);
      <Form>
        <h1> "User information"->React.string </h1>
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
      </Form>;
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
            "unknown",
          )
        | (RemoteData.Loading(_), _) => (
            <Alert variant=`Info title={"Deleting..."->React.string} />,
            "",
          )
        | (RemoteData.Success(_), RemoteData.Success(apiKey)) => (
            <Alert variant=`Info title={"New key generated"->React.string} />,
            apiKey.api_key,
          )
        | (_, RemoteData.Loading(None))
        | (_, RemoteData.NotAsked) => (<p> "loading"->React.string </p>, "")
        | (_, RemoteData.Loading(Some(apiKey)))
        | (_, RemoteData.Success(apiKey)) => (React.null, apiKey.api_key)
        | (_, RemoteData.Failure(title)) => (
            <Alert variant=`Danger title />,
            "",
          )
        };

      <Form>
        <h1> "API Key"->React.string </h1>
        message
        <FormGroup label={"Key"->React.string} fieldId="key">
          <TextInput value=apiKeyStr isReadOnly=true id="key" />
        </FormGroup>
        <ActionGroup>
          <Button
            variant=`Primary onClick={_ => callback(Hook.ApiKey.Regenerate)}>
            "Generate new API key"->React.string
          </Button>
          <Button
            variant=`Primary onClick={_ => callback(Hook.ApiKey.Delete)}>
            "Delete API key"->React.string
          </Button>
        </ActionGroup>
      </Form>;
    };
  };

  [@react.component]
  let make = () => {
    let (state, callback) = Hook.UserSettings.use();

    <>
      {switch (state) {
       | RemoteData.Success(user) => <SettingForm user callback />
       | _ => <p> "Loading"->React.string </p>
       }}
      <ApiForm />
    </>;
  };
};
