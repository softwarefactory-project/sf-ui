// A Fetcher implementation using `bs-fetch`
module BsFetch = {
  let fetch = url => {
    Fetch.fetch(url) |> Js.Promise.then_(Fetch.Response.json);
  };
  let post = (url, body: Js.Json.t) => {
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
};

module RealApp = App.Main(BsFetch);

switch (ReactDOM.querySelector("#root")) {
| Some(root) => ReactDOM.render(<RealApp />, root)
| None => Js.log("Can't find #root element!")
};
