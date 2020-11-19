// A module type to define the `fetch` capability
module type Fetcher = {
  let fetch: string => Js.Promise.t(option(Js.Json.t));
  let post: (string, Js.Json.t) => Js.Promise.t(Fetch.response);
};
