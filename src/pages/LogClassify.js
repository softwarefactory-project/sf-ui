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
import { Link } from 'react-router-dom';
import { Alert, Grid } from 'patternfly-react';

import logo from '../img/log-classify.png';
import LogClassify from '../components/LogClassify';

class LogClassifyPage extends React.Component {
  render () {
    return (
      <Grid fluid className="container-pf-nav-pf-vertical">
        <Grid.Row>
          <Grid.Col xs={12}>
            <div className="page-header">
              <h1>
                <Link to={'/log-classify'}><img src={logo} alt="logo" /></Link>
                Log Classify
              </h1>
              <Alert type="warning">
                This service is a tech-preview under development
              </Alert>
            </div>
          </Grid.Col>
        </Grid.Row>
        <Grid.Row>
          <Grid.Col xs={12}>
            <LogClassify anomaly={this.props.match.params.anomalyId}/>
          </Grid.Col>
        </Grid.Row>
      </Grid>
    );
  }
};

export default LogClassifyPage;
