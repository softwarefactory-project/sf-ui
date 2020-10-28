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

// See https://github.com/reasonml/reason-react/issues/627
[@bs.val]
external toActualPromise: Js.Promise.t('a) => Js.Promise.t('a) =
  "Promise.resolve";

let testComponentHook =
    (
      container: Dom.element,
      component: React.element,
      assertion: Dom.element => 'a,
    ) => {
  toActualPromise(
    actAsync(() =>
      Js.Promise.resolve(ReactDOMRe.render(component, container))
    ),
  )
  |> Js.Promise.then_(() =>
       expect(assertion(container)) |> toBe(true) |> Js.Promise.resolve
     );
};
describe("Basic test", () => {
  let container = ref(None);
  beforeEach(prepareContainer(container));
  afterEach(cleanupContainer(container));

  testPromise("can render resources", () => {
    module StubApp = App.Main(StubFetch);
    let container = getContainer(container);
    testComponentHook(container, <StubApp />, container =>
      container->DOM.findBySelectorAndTextContent("span", "rdoproject.org")
      |> Belt.Option.isSome
    );
  });
});
