import * as React from 'react';
import { Grid } from 'patternfly-react';
import Iframe from 'react-iframe';
import ReactDOM from 'react-dom';

/* Add this to GerritSiteHeader.html
    if (window.parent === window) {
      console.log("Setting location to " + window.location.href.replace('/r/', '/ui/#/review/'));
      window.location.href = window.location.href.replace('/r/', '/ui/#/review/');
    }
*/

class ReviewPage extends React.Component {
  constructor(props) {
    super(props);
    this.myRef = React.createRef();
    this.frameLoaded = this.frameLoaded.bind(this);
    this.frameHashChanged = this.frameHashChanged.bind(this);
    this.idx = 0;
  }
  frameLoaded(evt) {
    console.log("frameLoaded");
    const iframe = ReactDOM.findDOMNode(this.myRef.current);
    iframe.contentWindow.window.addEventListener(
      "hashchange", this.frameHashChanged);
    this.updateUrl(this.gerritPath);
  }
  updateUrl(newUrl) {
    console.log("Pusing new url [" + newUrl + "]");
    window.history.pushState("Test" + this.idx, "SF", newUrl);
    this.idx += 1;
  }
  frameHashChanged(evt) {
    console.log(evt);
    this.updateUrl(evt.newURL);
  }
  render() {
    this.gerritPath = "/r/" + this.props.location.hash;
    console.log("Loading gerrit ui to [" + this.gerritPath + "]");
    //this.updateUrl(gerritPath);
    return (
      <Grid fluid className="container-pf-nav-pf-vertical">
        <Grid.Row>
          <Iframe
            ref={this.myRef}
            url={this.gerritPath}
            width="90%"
            height="90%"
            display="initial"
            onLoad={this.frameLoaded}
            />
        </Grid.Row>
      </Grid>
    );
  }
}

export default ReviewPage;
