// A module type to define the `fetch` capability
module type Fetcher = {
  type postVerb =
    | POST
    | PUT;
  let fetch: string => Js.Promise.t(option(Js.Json.t));
  let post: (string, Js.Json.t) => Js.Promise.t(Fetch.response);
  let post2:
    (string, postVerb, Js.Json.t) => Js.Promise.t(option(Js.Json.t));
};
