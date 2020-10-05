switch (ReactDOM.querySelector("#root")) {
| Some(root) => ReactDOM.render(<App.Main />, root)
| None => Js.log("Can't find #root element!")
};
