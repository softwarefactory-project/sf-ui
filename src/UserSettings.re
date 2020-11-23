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
        ~user: Api.UserSettings.user,
        ~apiKey: Api.UserSettings.apiKey,
        ~apiKeyPost: Api.UserSettings.saveApiKeyHook,
      ) => {
    let (apikey, setApikey) = React.useState(() => apiKey.api_key);
    let (state, postForm) = apiKeyPost();
    <Form>
      <h1> "API Key"->React.string </h1>
      {switch (state) {
       | Loading => <Alert title="Generating API Key ..." variant=`Default />
       | Success(ak) =>
         setApikey(_ => ak.api_key);
         <Alert title="Done ..." variant=`Success />;
       | Failure(title) => <Alert title variant=`Danger />
       | NotAsked => React.null
       }}
      <FormGroup label={"Key"->React.string} fieldId="key">
        <TextInput value=apikey isReadOnly=true id="key" />
      </FormGroup>
      <ActionGroup>
        <Button
          variant=`Primary
          onClick={_ => {postForm(Regenerate(user.username))}}>
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
        ~apiKeyPost: Api.UserSettings.saveApiKeyHook,
      ) => {
    // TODO: add auth prop to check if user is actually logged-in
    let state = userSettings();
    switch (state) {
    | RemoteData.Loading => <p> "Loading..."->React.string </p>
    | RemoteData.Success((user, apiKey)) =>
      <>
        <SettingForm user userSettingsPost />
        <br />
        <ApiForm user apiKey apiKeyPost />
      </>
    | RemoteData.Failure(title) => <Alert variant=`Danger title />
    };
  };
};
