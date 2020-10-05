open Jest;
open Expect;
open ReactTestUtils;

[@bs.module "fs"]
external readFileSync:
  (~name: string, [@bs.string] [ | `utf8 | [@bs.as "ascii"] `useAscii]) =>
  string =
  "readFileSync";

let stubResources = readFileSync(~name="tests/api/resources.json", `utf8);

module StubFetch = {
  let fetch = url => {
    Js.log("Stubing: " ++ url);
    Js.Promise.resolve(Js.Json.parseExn(stubResources));
  };
};

describe("Basic test", () => {
  let container = ref(None);
  beforeEach(prepareContainer(container));
  afterEach(cleanupContainer(container));

  test("can render resources", () => {
    let container = getContainer(container);

    act(() => {
      module StubApp = SfUi.App.Main(StubFetch);
      ReactDOMRe.render(<StubApp />, container);
    });
    // TODO: fix update happening outside of act() call...
    expect(
      container
      ->DOM.findBySelectorAndTextContent("p", "Loading...")
      ->Belt.Option.isSome,
    )
    |> toBe(true);
  });
});
