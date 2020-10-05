// A module type to define the `fetch` capability
module type Fetcher = {let fetch: string => Js.Promise.t(Js.Json.t);};
