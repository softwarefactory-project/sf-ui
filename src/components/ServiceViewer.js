// Copyright 2018 Red Hat, Inc
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use this file except in compliance with the License. You may obtain
// a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

import * as React from 'react';
import ReactDOM from 'react-dom';
const { PureComponent } = React;
const PropTypes = require("prop-types");
const objectAssign = require("object-assign");
function noop() {}

// Class imported from https://github.com/svenanders/react-iframe
const Iframe = class extends PureComponent {
  render() {
    const props = {
      ref: "iframe",
      frameBorder: "0",
      src: this.props.url,
      target: "_parent",
      allowFullScreen: this.props.allowFullScreen || false,
      style: objectAssign(
        {},
        {
          position: this.props.position || "absolute",
          display: this.props.display || "block",
          height: "100%",
          width: "100%",
          top: 0,
          left: 0,
          border: 0,
        },
        this.props.styles || {}
      ),
      height: this.props.height || "100%",
      name: this.props.name || "",
      width: this.props.width || "100%",
      onLoad: this.props.onLoad || noop
    };

    return React.createElement(
      "iframe",
      objectAssign(
        props,
        this.props.id ? { id: this.props.id } : {},
        this.props.className ? { className: this.props.className } : {})
    );
  }
};

Iframe.propTypes = {
  url: PropTypes.string.isRequired,
  id: PropTypes.string,
  className: PropTypes.string,
  width: PropTypes.string,
  position: PropTypes.string,
  display: PropTypes.string,
  name: PropTypes.string,
  height: PropTypes.string,
  onLoad: PropTypes.func,
  styles: PropTypes.object,
  allowFullScreen: PropTypes.bool
};

class ServiceViewer extends React.Component {
  constructor(props) {
    super(props);
    this.myRef = React.createRef();
    this.frameLoaded = this.frameLoaded.bind(this);
    this.frameHashChanged = this.frameHashChanged.bind(this);
  }
  frameLoaded(evt) {
    console.log("frameLoaded: " + this.serviceUrl);
    const iframe = ReactDOM.findDOMNode(this.myRef.current);
    iframe.contentWindow.window.addEventListener(
      "hashchange", this.frameHashChanged);
    this.updateUrl(this.serviceUrl);
  }
  frameHashChanged(evt) {
    console.log(evt);
    this.updateUrl(evt.newURL);
  }
  render() {
    this.serviceUrl = this.getServiceUrl();
    console.log("rendering ui from [" + this.serviceUrl + "]");
    // TODO: find a better solution for iframe height than window.innerHeight
    return (
      <div className="container-pf-nav-pf-vertical" height="100%"
           style={{position: 'relative', height: window.innerHeight, display: 'block'}}>
        <div className="row row-cards-pf">
          <Iframe
            ref={this.myRef}
            url={this.serviceUrl}
            scrolling="no"
            frameborder={0}
            display="initial"
            width="100%"
            height="100%"
            onLoad={this.frameLoaded}
            position="absolute"
            />
        </div>
      </div>
    );
  }
}

export default ServiceViewer;
