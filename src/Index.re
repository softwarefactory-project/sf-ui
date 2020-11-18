// A Fetcher implementation using `bs-fetch`
module BsFetch = {
  let fetch = url => {
    Fetch.fetch(url) |> Js.Promise.then_(Fetch.Response.json);
  };
  let fetchWithInit = (url, requestInit: Fetch.requestInit) => {
    Fetch.fetchWithInit(url, requestInit);
  };
};

module RealApp = App.Main(BsFetch);

switch (ReactDOM.querySelector("#root")) {
| Some(root) => ReactDOM.render(<RealApp />, root)
| None => Js.log("Can't find #root element!")
};
