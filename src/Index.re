// A Fetcher implementation using `bs-fetch`
module BsFetch = {
  let handleAPICallError =
      (promise: Js.Promise.t(Fetch.Response.t))
      : Js.Promise.t(Belt.Result.t(Fetch.response, string)) => {
    promise
    |> Js.Promise.then_(r =>
         r |> Fetch.Response.ok || r |> Fetch.Response.status == 404
           ? Ok(r)->Js.Promise.resolve
           : Error("API call failed: " ++ Fetch.Response.statusText(r))
             ->Js.Promise.resolve
       );
  };

  let extractJson =
      (promise: Js.Promise.t(Belt.Result.t(Fetch.Response.t, string)))
      : Js.Promise.t(Belt.Result.t(option(Js.Json.t), string)) => {
    promise
    |> Js.Promise.then_(result =>
         switch (result) {
         | Ok(resp) =>
           resp
           |> Fetch.Response.json
           |> Js.Promise.then_(decoded =>
                Ok(decoded->Some)->Js.Promise.resolve
              )
           |> Js.Promise.catch(_ => Ok(None)->Js.Promise.resolve)
         | Error(e) => Error(e)->Js.Promise.resolve
         }
       );
  };

  let raiseOnNok = (promise: Js.Promise.t(Fetch.Response.t)) => {
    promise
    |> Js.Promise.then_(r =>
         r |> Fetch.Response.ok || r |> Fetch.Response.status == 404
           ? promise : Js.Exn.raiseError(Fetch.Response.statusText(r))
       );
  };

  let get =
      (url: string): Js.Promise.t(Belt.Result.t(option(Js.Json.t), string)) =>
    Fetch.fetch(url) |> handleAPICallError |> extractJson;

  let postOrPut =
      (verb, url: string, body: option(Js.Json.t))
      : Js.Promise.t(Belt.Result.t(option(Js.Json.t), string)) => {
    let headers =
      Fetch.HeadersInit.make({
        "Accept": "*",
        "Content-Type": "application/json",
      });
    let req =
      switch (body) {
      | None => Fetch.RequestInit.make(~method_=verb, ~headers, ())
      | Some(json) =>
        Fetch.RequestInit.make(
          ~method_=verb,
          ~body=json->Js.Json.stringify->Fetch.BodyInit.make,
          ~headers,
          (),
        )
      };
    Fetch.fetchWithInit(url, req) |> handleAPICallError |> extractJson;
  };
  let put = postOrPut(Put);
  let post = postOrPut(Post);
  let delete =
      (url: string): Js.Promise.t(Belt.Result.t(Fetch.response, string)) => {
    let req =
      Fetch.RequestInit.make(
        ~method_=Delete,
        ~headers=Fetch.HeadersInit.make({"Accept": "*"}),
        (),
      );
    Fetch.fetchWithInit(url, req) |> handleAPICallError;
  };
};

module RealApp = App.Main(BsFetch);

switch (ReactDOM.querySelector("#root")) {
| Some(root) => ReactDOM.render(<RealApp />, root)
| None => Js.log("Can't find #root element!")
};
