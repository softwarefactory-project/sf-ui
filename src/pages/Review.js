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

import ServiceViewer from '../components/ServiceViewer';

/* Add this to GerritSiteHeader.html
    if (window.parent === window) {
      console.log("Setting location to " + window.location.href.replace('/r/', '/ui/#/review/'));
      window.location.href = window.location.href.replace('/r/', '/ui/#/review/');
    }
*/

class ReviewPage extends ServiceViewer {
  getServiceUrl() {
    return "/r/" + this.props.location.hash;
  }
  updateUrl(newUrl) {
    console.log("Pushing new url [" + newUrl + "]");
    // window.history.pushState("Test", "SF", newUrl);
  }
}

export default ReviewPage;
