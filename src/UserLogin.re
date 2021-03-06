open Patternfly;
open Patternfly.Layout;

let notEmpty = xs =>
  switch (xs) {
  | [] => false
  | _ => true
  };

module Page = {
  [@react.component]
  let make = (~info: SF.Info.t, ~auth: Auth.t, ~back: string) => {
    let (authState, authDispatch) = auth;
    // Ensure we are logged-out when rendering the login form
    React.useEffect0(() => {
      authDispatch(Auth.Logout);
      None;
    });
    let (useLocalAccount, toggleLocalAccount) = React.useState(_ => false);

    // Username
    let (usernameValue, setUsernameValue) = React.useState(_ => "");
    let onChangeUsername = (value, _) => setUsernameValue(_ => value);

    // Password
    let (passwordValue, setPasswordValue) = React.useState(_ => "");
    let onChangePassword = (value, _) => setPasswordValue(_ => value);

    // Is idp available
    let idpAvailable =
      info.auths.oauth->notEmpty || info.auths.other->notEmpty;

    let loginSubtitle =
      if (useLocalAccount) {
        "Sign in with your username and password";
      } else if (idpAvailable) {
        "Sign in with your external account";
      } else {
        "Only internal accounts are available";
      };

    let footerListItems =
      <>
        <ListItem>
          <LoginFooterItem href="#">
            {"Terms of Use" |> React.string}
          </LoginFooterItem>
        </ListItem>
        <ListItem>
          <LoginFooterItem href="#">
            {"Help" |> React.string}
          </LoginFooterItem>
        </ListItem>
        <ListItem>
          <LoginFooterItem href="#">
            "Privacy Policy"->React.string
          </LoginFooterItem>
        </ListItem>
      </>;
    let helperText =
      <>
        <Icons.ExclamationCircle />
        "Invalid login credentials."->React.string
      </>;

    let mkOauthButton = (backend, icon, text) =>
      <form key=text method="post" action="/auth/login" target="_top">
        <input type_="hidden" name="back" value=back />
        <input
          type_="hidden"
          name="method"
          value={backend->Cauth.backendMethod}
        />
        <Button isBlock=true _type=`Submit> icon text->React.string </Button>
      </form>;

    let mkOauthButton' = (backend, text) =>
      mkOauthButton(backend, React.null, text);

    let oauthButton =
      fun
      | "github" =>
        mkOauthButton(Cauth.GitHub, <Icons.Github />, " Log in with Github")
      | "google" =>
        mkOauthButton(Cauth.Google, <Icons.Google />, " Log in with Google")
      | "bitbucket" =>
        mkOauthButton'(Cauth.BitBucket, "Log in with BitBucket")
      | _ => React.null;

    let otherButton =
      fun
      | SF.Info.{name: "openid", text} => mkOauthButton'(Cauth.OpenID, text)
      | SF.Info.{name: "openid_connect", text} =>
        mkOauthButton'(Cauth.OpenIDConnect, text)
      | SF.Info.{name: "SAML2", text} => mkOauthButton'(Cauth.SAML, text)
      | _ => React.null;

    let externalIdp =
      Belt.List.concat(
        info.auths.oauth->Belt.List.map(oauthButton),
        info.auths.other->Belt.List.map(otherButton),
      )
      ->Belt.List.toArray
      ->React.array;

    let onLoginButtonClick = (e: ReactEvent.Mouse.t) => {
      e->ReactEvent.Mouse.preventDefault;
      authDispatch(
        Auth.Login(
          Auth.CauthLogin(
            Cauth.(
              Password({username: usernameValue, password: passwordValue})
            ),
            back,
          ),
        ),
      );
    };

    let loginForm =
      <LoginForm
        helperText
        usernameValue
        onChangeUsername
        passwordValue
        onChangePassword
        onLoginButtonClick
      />;
    switch (authState.auth_request) {
    | RemoteData.Failure(_)
    | RemoteData.Loading(_)
    | RemoteData.NotAsked =>
      <LoginPage
        footerListVariants=`Inline
        footerListItems
        brandImgSrc={"data:image/png;base64," ++ info.splash_image_b64data}
        loginTitle="Log in to your account"
        loginSubtitle>
        {authState.auth_request->RemoteData.isLoading
           ? <Spinner /> : React.null}
        authState.auth_request->RemoteApi.renderError
        <Stack hasGutter=true>
          {useLocalAccount ? loginForm : externalIdp}
          <a onClick={_ => toggleLocalAccount(_ => !useLocalAccount)}>
            "Toggle login form"->React.string
          </a>
        </Stack>
      </LoginPage>
    | RemoteData.Success(_) =>
      RescriptReactRouter.push(back);
      // // Force a react router call is not enough
      Webapi.Dom.(Location.reload(location));
      <p> "If you are not redirected, click /"->React.string </p>;
    };
  };
};

module Logout = {
  [@react.component]
  let make = (~auth: Auth.t) => {
    React.useEffect0(() => {
      let (_, dispatch) = auth;
      dispatch(Auth.Logout);
      RescriptReactRouter.replace("/");
      Some(() => ());
    });
    <p> "Logout in progress ..."->React.string </p>;
  };
};

module Header = {
  [@react.component]
  let make = (~auth: Auth.t) => {
    <PageHeaderToolsGroup>
      {switch (auth) {
       | ({auth_request: _, user: Some({name})}, _) =>
         <PageHeaderToolsGroup>
           <PageHeaderToolsItem>
             <p> {("Welcome " ++ name)->React.string} </p>
           </PageHeaderToolsItem>
           <PageHeaderToolsItem>
             <Button
               variant=`Plain
               onClick={_ => RescriptReactRouter.push("/auth/settings")}>
               <Icons.Cog />
             </Button>
           </PageHeaderToolsItem>
           <PageHeaderToolsItem>
             <Button
               variant=`Secondary
               onClick={_ => RescriptReactRouter.push("/logout")}>
               {"Logout" |> React.string}
             </Button>
           </PageHeaderToolsItem>
         </PageHeaderToolsGroup>
       | (_, dispatch) =>
         <Button
           variant=`Secondary
           onClick={_ => {
             // Ensure we are logged-out when clicking login.
             dispatch(Auth.Logout);
             RescriptReactRouter.push("/login");
           }}>
           {"Login" |> React.string}
         </Button>
       }}
    </PageHeaderToolsGroup>;
  };
};
