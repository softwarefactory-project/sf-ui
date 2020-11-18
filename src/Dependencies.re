// A module type to define the `fetch` capability
module type Fetcher = {
  let fetch: string => Js.Promise.t(Js.Json.t);
  let fetchWithInit:
    (string, Fetch.requestInit) => Js.Promise.t(Fetch.response);
};
