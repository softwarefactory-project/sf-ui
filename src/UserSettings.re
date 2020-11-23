open Patternfly;

module SettingForm = {
  [@react.component]
  let make =
      (
        ~user: Api.UserSettings.user,
        ~userSettingsPost: Api.UserSettings.saveHook,
      ) => {
    let (idpSync, setIdpSync) = React.useState(_ => user.idp_sync);
    let (fullname, setFullname) = React.useState(() => user.fullname);
    let (email, seteMail) = React.useState(() => user.email);
    let (state, postForm) = userSettingsPost();
    <Form>
      <h1> "User information"->React.string </h1>
      {switch (state) {
       | Loading => <Alert title="Save in progress ..." variant=`Default />
       | Success(_) => <Alert title="Success" variant=`Info />
       | Failure(title) => <Alert title variant=`Danger />
       | NotAsked => React.null
       }}
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
          onClick={_ => {
            let user: Api.UserSettings.user = {
              username: user.username,
              fullname,
              email,
              idp_sync: idpSync,
            };
            postForm(
              Put({
                body: Api.UserSettings.(user_encode(user)),
                qs: [("username", user.username)],
              }),
            );
          }}>
          "Save"->React.string
        </Button>
      </ActionGroup>
    </Form>;
  };
};

module ApiForm = {
  [@react.component]
  let make =
      (
        ~apiKey: Api.UserSettings.apiKey,
        ~apiKeyPost: Api.UserSettings.saveHook,
      ) => {
    <Form>
      <h1> "API Key"->React.string </h1>
      <FormGroup label={"Key"->React.string} fieldId="key">
        <TextInput value={apiKey.api_key} isReadOnly=true id="key" />
      </FormGroup>
      <ActionGroup>
        <Button variant=`Primary>
          "Generate new API key"->React.string
        </Button>
      </ActionGroup>
    </Form>;
  };
};

module Page = {
  [@react.component]
  let make =
      (
        ~userSettings: Api.UserSettings.useHook,
        ~userSettingsPost: Api.UserSettings.saveHook,
        ~apiKeyPost: Api.UserSettings.saveHook,
      ) => {
    // TODO: add auth prop to check if user is actually logged-in
    let state = userSettings();
    switch (state) {
    | RemoteData.Loading => <p> "Loading..."->React.string </p>
    | RemoteData.Success((user, apiKey)) =>
      <>
        <SettingForm user userSettingsPost />
        <br />
        <ApiForm apiKey apiKeyPost />
      </>
    | RemoteData.Failure(title) => <Alert variant=`Danger title />
    };
  };
};
