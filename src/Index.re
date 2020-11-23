// A Fetcher implementation using `bs-fetch`
module BsFetch = {
  type postVerb =
    | POST
    | PUT;
  let promiseToOptionalJson = (promise: Js.Promise.t(Fetch.response)) => {
    promise
    |> Js.Promise.then_(r =>
         r |> Fetch.Response.ok
           ? promise : Js.Exn.raiseError(Fetch.Response.statusText(r))
       )
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
      (url: string, verb: postVerb, body: Js.Json.t)
      : Js.Promise.t(option(Js.Json.t)) => {
    let method =
      switch (verb) {
      | POST => Fetch.(Post)
      | PUT => Fetch.(Put)
      };
    let req =
      Fetch.RequestInit.make(
        ~method_=method,
        ~body=body->Js.Json.stringify->Fetch.BodyInit.make,
        ~headers=
          Fetch.HeadersInit.make({
            "Accept": "*",
            "Content-Type": "application/json",
          }),
        (),
      );
    Fetch.fetchWithInit(url, req) |> promiseToOptionalJson;
  };
};

module RealApp = App.Main(BsFetch);

switch (ReactDOM.querySelector("#root")) {
| Some(root) => ReactDOM.render(<RealApp />, root)
| None => Js.log("Can't find #root element!")
};
