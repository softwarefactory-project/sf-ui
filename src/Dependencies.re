// A module type to define the `fetch` capability
module type Fetcher = {
  let fetch: string => Js.Promise.t(option(Js.Json.t));
  let post:
    (string, Js.Json.t) => Js.Promise.t(Belt.Result.t(Js.Json.t, string));

  let put:
    (string, Js.Json.t) => Js.Promise.t(Belt.Result.t(Js.Json.t, string));
  let delete: string => Js.Promise.t(Belt.Result.t(Fetch.response, string));
};
