// A Fetcher implementation using `bs-fetch`
module BsFetch = {
  let raiseOnNok2 =
      (promise: Js.Promise.t(Fetch.Response.t))
      : Js.Promise.t(Belt.Result.t(Fetch.response, string)) => {
    promise
    |> Js.Promise.then_(r =>
         r |> Fetch.Response.ok
           ? Ok(r)->Js.Promise.resolve
           : Error("API call failed: " ++ Fetch.Response.statusText(r))
             ->Js.Promise.resolve
       );
  };

  let jsonToResult =
      (promise: Js.Promise.t(Belt.Result.t(Fetch.Response.t, string)))
      : Js.Promise.t(Belt.Result.t(Js.Json.t, string)) => {
    promise
    |> Js.Promise.then_(result =>
         switch (result) {
         | Ok(resp) =>
           resp
           |> Fetch.Response.json
           |> Js.Promise.then_(decoded => Ok(decoded)->Js.Promise.resolve)
           |> Js.Promise.catch(_ =>
                Error("Malformed JSON Document")->Js.Promise.resolve
              )
         | Error(e) => Error(e)->Js.Promise.resolve
         }
       );
  };

  let promiseToOptionalJson2 =
      (promise: Js.Promise.t(Fetch.response))
      : Js.Promise.t(Belt.Result.t(Js.Json.t, string)) => {
    promise |> raiseOnNok2 |> jsonToResult;
  };

  let raiseOnNok = (promise: Js.Promise.t(Fetch.Response.t)) => {
    promise
    |> Js.Promise.then_(r =>
         r |> Fetch.Response.ok
           ? promise : Js.Exn.raiseError(Fetch.Response.statusText(r))
       );
  };
  let promiseToOptionalJson =
      (promise: Js.Promise.t(Fetch.response))
      : Js.Promise.t(option(Js.Json.t)) => {
    promise
    |> raiseOnNok
    |> Js.Promise.then_(Fetch.Response.json)
    |> Js.Promise.then_(v => v->Some->Js.Promise.resolve)
    |> Js.Promise.catch(e => {
         Js.log2("Unexpected error: ", e);
         None->Js.Promise.resolve;
       });
  };
  let fetch = (url: string): Js.Promise.t(option(Js.Json.t)) => {
    Fetch.fetch(url) |> promiseToOptionalJson;
  };
  let post = (url: string, body: Js.Json.t) => {
    let req =
      Fetch.RequestInit.make(
        ~method_=Post,
        ~body=body->Js.Json.stringify->Fetch.BodyInit.make,
        ~headers=
          Fetch.HeadersInit.make({
            "Accept": "*",
            "Content-Type": "application/json",
          }),
        (),
      );
    Fetch.fetchWithInit(url, req);
  };
  let post2 =
      (url: string, body: Js.Json.t)
      : Js.Promise.t(Belt.Result.t(Js.Json.t, string)) => {
    let req =
      Fetch.RequestInit.make(
        ~method_=Post,
        ~body=body->Js.Json.stringify->Fetch.BodyInit.make,
        ~headers=
          Fetch.HeadersInit.make({
            "Accept": "*",
            "Content-Type": "application/json",
          }),
        (),
      );
    Fetch.fetchWithInit(url, req) |> promiseToOptionalJson2;
  };
  let put =
      (url: string, body: Js.Json.t)
      : Js.Promise.t(Belt.Result.t(Js.Json.t, string)) => {
    let req =
      Fetch.RequestInit.make(
        ~method_=Put,
        ~body=body->Js.Json.stringify->Fetch.BodyInit.make,
        ~headers=
          Fetch.HeadersInit.make({
            "Accept": "*",
            "Content-Type": "application/json",
          }),
        (),
      );
    Fetch.fetchWithInit(url, req) |> promiseToOptionalJson2;
  };
  let delete =
      (url: string): Js.Promise.t(Belt.Result.t(Fetch.response, string)) => {
    let req =
      Fetch.RequestInit.make(
        ~method_=Delete,
        ~headers=Fetch.HeadersInit.make({"Accept": "*"}),
        (),
      );
    Fetch.fetchWithInit(url, req) |> raiseOnNok2;
  };
};

module RealApp = App.Main(BsFetch);

switch (ReactDOM.querySelector("#root")) {
| Some(root) => ReactDOM.render(<RealApp />, root)
| None => Js.log("Can't find #root element!")
};
