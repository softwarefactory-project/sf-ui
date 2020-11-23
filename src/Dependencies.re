// A module type to define the `fetch` capability
module type Fetcher = {
  let fetch: string => Js.Promise.t(option(Js.Json.t));
  let post: (string, Js.Json.t) => Js.Promise.t(Fetch.response);
  let post2: (string, Js.Json.t) => Js.Promise.t(option(Js.Json.t));
  let put: (string, Js.Json.t) => Js.Promise.t(option(Js.Json.t));
  let delete: (string) => Js.Promise.t(Fetch.response);
};
