open Jest;
open Expect;
open ReactTestUtils;
open SFUI;

let stubResources = Node.Fs.readFileSync("tests/api/resources.json", `utf8);

module StubFetch = {
  let fetch = url => {
    Js.log("Stubing: " ++ url);
    Js.Promise.resolve(Js.Json.parseExn(stubResources));
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
          container->DOM.findBySelectorAndTextContent("li", "rdoproject.org")
          |> Belt.Option.isSome,
        )
        |> toBe(true),
      )
    });
  });
});
