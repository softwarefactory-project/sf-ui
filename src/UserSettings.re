open Patternfly;

module SettingForm = {
  [@react.component]
  let make = (~user: Managesf.user) => {
    let (idpSync, idpSyncChange) = React.useState(_ => user.idp_sync);
    <Form>
      <h1> "User information"->React.string </h1>
      <FormGroup label={"Username"->React.string} fieldId="username">
        <TextInput value={user.username} isReadOnly=true id="username" />
      </FormGroup>
      <FormGroup label={"Full name"->React.string} fieldId="fullName">
        <TextInput value={user.fullname} isReadOnly=idpSync id="fullName" />
      </FormGroup>
      <FormGroup label={"eMail"->React.string} fieldId="eMail">
        <TextInput value={user.email} isReadOnly=idpSync id="eMail" />
      </FormGroup>
      <FormGroup
        label={"Manage Identity Provider"->React.string} fieldId="idp">
        <Checkbox
          label="Synchronize user data with the Identity Provider"
          isChecked=idpSync
          onChange={(_, _) => idpSyncChange(_ => !idpSync)}
          id="idpSync"
        />
      </FormGroup>
      <ActionGroup>
        <Button variant=`Primary> "Save"->React.string </Button>
      </ActionGroup>
    </Form>;
  };
};

module ApiForm = {
  [@react.component]
  let make = (~apiKey: Managesf.apiKey) => {
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
  let make = (~managesf: Managesf.hook) => {
    // TODO: add auth prop to check if user is actually logged-in
    let state = managesf();
    switch (state) {
    | (_, Managesf.KeyLoading)
    | (Managesf.Loading, _) => <p> "Loading..."->React.string </p>
    | (Managesf.Loaded(user), Managesf.KeyLoaded(apiKey)) =>
      <> <SettingForm user /> <br /> <ApiForm apiKey /> </>
    };
  };
};
