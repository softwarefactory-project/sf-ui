open Patternfly;

module Page = {
  [@react.component]
  let make = (~info: SF.Info.t, ~auth: Auth.t) => {
    // TODO: add (and use) idp to info type
    ignore(info);
    let (_authState, authDispatch) = auth;
    let (useLocalAccount, toggleLocalAccount) = React.useState(_ => false);

    // Username
    let (usernameValue, setUsernameValue) = React.useState(_ => "");
    let onChangeUsername = (value, _) => setUsernameValue(_ => value);

    // Password
    let (passwordValue, setPasswordValue) = React.useState(_ => "");
    let onChangePassword = (value, _) => setPasswordValue(_ => value);

    let loginSubtitle =
      "Sign in with "
      ++ (useLocalAccount ? "you username and password" : "external account");
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

    let externalIdp =
      <>
        <Button
          isBlock=true
          onClick={_ =>
            authDispatch(Auth.Login(Auth.CauthLogin(Cauth.GitHub)))
          }>
          <Icons.Github />
          " Log in with Github"->React.string
        </Button>
        <Button isBlock=true>
          <Icons.Google />
          " Log in with Google"->React.string
        </Button>
      </>;

    let onLoginButtonClick = (e: ReactEvent.Mouse.t) => {
      e->ReactEvent.Mouse.preventDefault;
      authDispatch(
        Auth.Login(
          Auth.CauthLogin(
            Cauth.(
              Password({username: usernameValue, password: passwordValue})
            ),
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

    <LoginPage
      footerListVariants=`Inline
      footerListItems
      loginTitle="Log in to your account"
      loginSubtitle>
      {useLocalAccount ? loginForm : externalIdp}
      <br />
      <a onClick={_ => toggleLocalAccount(_ => !useLocalAccount)}>
        "Toggle login form"->React.string
      </a>
    </LoginPage>;
  };
};

module Header = {
  [@react.component]
  let make = (~auth: Auth.t) => {
    <PageHeaderToolsGroup>
      {switch (auth) {
       | (Some({name}), dispatch) =>
         <>
           <PageHeaderToolsGroup>
             <PageHeaderToolsItem>
               <p> {("Welcome " ++ name)->React.string} </p>
             </PageHeaderToolsItem>
             <PageHeaderToolsItem>
               <Button
                 variant=`Plain
                 onClick={_ => ReasonReactRouter.push("/auth/settings")}>
                 <Icons.Cog />
               </Button>
             </PageHeaderToolsItem>
             <PageHeaderToolsItem>
               <Button
                 variant=`Secondary onClick={_ => dispatch(Auth.Logout)}>
                 {"Logout" |> React.string}
               </Button>
             </PageHeaderToolsItem>
           </PageHeaderToolsGroup>
         </>
       | (None, _) =>
         <Button
           variant=`Secondary
           onClick={_ => ReasonReactRouter.push("/auth/login")}>
           {"Login" |> React.string}
         </Button>
       }}
    </PageHeaderToolsGroup>;
  };
};
