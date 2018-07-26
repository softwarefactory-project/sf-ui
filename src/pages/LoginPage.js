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

import React from 'react';
import {
  Card,
  CardTitle,
  CardBody,
  Form,
  FormGroup,
  FormControl,
  Grid,
  Button,
} from 'patternfly-react';

class LoginApp extends React.Component {
  constructor() {
    super();
    this.login = this.login.bind(this);
    this.cancel = this.cancel.bind(this);
  }
  login() {
    console.log("Login!");
    this.props.app.setState({loginPage: false});
  }
  cancel() {
    this.props.app.setState({loginPage: false});
  }

  componentWillMount() {
    document.body.classList.add("login-pf");
  }
  componentWillUnmount() {
    document.body.classList.remove("login-pf");
  }
  render() {
    const loginCard = (
      <Card>
        <CardTitle className="login-pf-header">Login to your account</CardTitle>
        <CardBody>
          <Form>
            <Button
              type="submit"
              bsStyle="primary col-md-12"
              style={{'margin-bottom': 25}}>
              <i className="fa fa-lg fa-github"></i>
              &nbsp;&nbsp;Log in with Github
              <i className="fa fa-sign-in fa-fw"></i>
            </Button>
            <FormGroup controlId="username" placeholder="Username" disabled={false}>
              <FormControl type="username" placeholder="Username" disabled={false} />
            </FormGroup>
            <FormGroup controlId="password" placeholder="Password" disabled={false}>
              <FormControl type="password" placeholder="Password" disabled={false} />
            </FormGroup>
            <Button style={{'margin-top': 0}}
              bsStyle="primary"
              onClick={this.login}>Login</Button>
            <Button
              onClick={this.cancel}>Cancel</Button>
          </Form>
        </CardBody>
      </Card>
    );
    return (
      <div className="col-sm-8 col-sm-offset-2 col-md-6 col-md-offset-3 col-lg-6 col-lg-offset-3">
        <div className="toast-notifications-list-pf" />
        <div className="login-pf-page">
          <Grid fluid>
              <Grid.Col>
                <header className="login-pf-page-header">Page Header</header>
                <Grid.Row>
                  {loginCard}
                </Grid.Row>
              </Grid.Col>
          </Grid>
        </div>
      </div>
    );
  }
}

export default LoginApp;
