open Jest;
open Expect;
open ReactTestUtils;

let stubResources = Node.Fs.readFileSync("tests/api/resources.json", `utf8);
let stubInfo = Node.Fs.readFileSync("tests/api/info.json", `utf8);

module StubFetch = {
  let fetch = url => {
    Js.log("Stubing: " ++ url);
    (
      switch (url) {
      | "/api/info.json" => stubInfo
      | "/api/resources.json" => stubResources
      | _OtherUrl => "false"
      }
    )
    |> Js.Json.parseExn
    |> Js.Promise.resolve;
  };
};

let testComponentHook =
    (
      container: Dom.element,
      component: React.element,
      assertion: Dom.element => unit,
    ) => {
  ignore(
    actAsync(() =>
      Js.Promise.resolve(ReactDOMRe.render(component, container))
    )
    |> Js.Promise.then_(() => {assertion(container) |> Js.Promise.resolve}),
  );
};

describe("Basic test", () => {
  let container = ref(None);
  beforeEach(prepareContainer(container));
  afterEach(cleanupContainer(container));

  testAsync("can render resources", finish => {
    module StubApp = App.Main(StubFetch);
    let container = getContainer(container);
    testComponentHook(container, <StubApp />, container => {
      finish(
        expect(
          container->DOM.findBySelectorAndTextContent("span", "rdoproject.org")
          |> Belt.Option.isSome,
        )
        |> toBe(true),
      )
    });
  });
});
