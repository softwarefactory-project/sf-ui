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

import Axios from 'axios';

const apiUrl = '/manage/about';

export const fetchAboutSuccess = (about) => {
  return {
    type: 'FETCH_ABOUT_SUCCESS',
    about
  }
};

function sleeper(ms) {
  return function(x) {
    return new Promise(resolve => setTimeout(() => resolve(x), ms));
  };
}

export const fetchAbout = () => {
  return (dispatch) => {
    return Axios.get(apiUrl)
      .then(sleeper(2)).then(response => {
        dispatch(fetchAboutSuccess(response.data))
      })
      .catch(error => {
        throw(error);
      });
  };
};
