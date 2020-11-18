module type CookieFetcher = {let getRawCookie: unit => option(string);};

module BrowserCookieFetcher: CookieFetcher = {
  let getRawCookie = () =>
    JustgageReasonCookie.(Cookie.getAsString("auth_pubtkt"));
};

module AbstractCauthCookie = (CF: CookieFetcher) => {
  let getCookie = () => {
    CF.getRawCookie()
    ->Belt.Option.flatMap(c => c->Js.Global.decodeURIComponent->Some);
  };

  let getUser = (): option(string) => {
    let captureUid = kv => {
      let rex = Js.Re.fromString("^uid=(.*)$")->Js.Re.exec_(kv);
      switch (rex) {
      | Some(r) => Js.Nullable.toOption(Js.Re.captures(r)[1])
      | None => None
      };
    };
    // If we get a auth_pubtkt payload then let's assume an uid key is present
    getCookie()
    ->Belt.Option.flatMap(c =>
        {
          String.split_on_char(';', c)
          ->Belt.List.map(captureUid)
          ->Belt.List.keep(Belt.Option.isSome);
        }
        ->Belt.List.head
      )
    ->Belt.Option.getWithDefault(None);
  };

  let remove = () => {
    JustgageReasonCookie.(Cookie.remove("auth_pubtkt"));
  };
};

module CauthCookie = AbstractCauthCookie(BrowserCookieFetcher);
