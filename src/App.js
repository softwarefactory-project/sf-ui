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
import PropTypes from 'prop-types';
import { withRouter } from 'react-router';
import { connect } from 'react-redux';
import { Route, Redirect, Switch } from 'react-router-dom';
import cookie from 'react-cookies';
import {
  Grid,
  VerticalNav,
  VerticalNavItem,
  VerticalNavSecondaryItem,
  VerticalNavIconBar,
  NavItem,
  Masthead,
  Icon,
  MenuItem
} from 'patternfly-react';
import logo from './img/logo.png';
import { routes } from './routes';
import LoginApp from './pages/LoginPage';


class App extends React.Component {
  constructor() {
    super();

    this.menu = routes();
    this.state = {
      about: null,
      loginPage: true, // false
    };
    this.onLogin = this.onLogin.bind(this);
    this.onLogout = this.onLogout.bind(this);
  }

  onLogin() {
    this.setState({loginPage: true});
  };

  onLogout() {
    console.log("logout");
  };

  componentWillMount() {
    this.setState({
      userId: cookie.load('userId'),
    });
  }
  renderContent = () => {
    const allRoutes = [];
    this.menu.map((item, index) => {
      allRoutes.push(
        <Route key={index} exact path={item.to} component={item.component} />
      );
      if (item.subItems) {
        item.subItems.map((secondaryItem, subIndex) =>
          allRoutes.push(
            <Route
              key={subIndex}
              exact
              path={secondaryItem.to}
              component={secondaryItem.component}
            />
          )
        );
      }
      return allRoutes;
    });

    return (
      <Switch>
        {allRoutes}
        <Redirect from="*" to="/welcome" key="default-route" />
      </Switch>
    );
  };

  navigateTo = path => {
    const { history } = this.props;
    history.push(path);
  };

  renderMain() {
    const { userId } = this.state;
    const { about, location } = this.props;

    const activeItem = this.menu.find(
      item => location.pathname === item.to
    );
    const vertNavItems = this.menu
          .filter(item => item.iconClass && (
            !item.serviceName ||
              about.services.indexOf(item.serviceName) !== -1))
            .map(item => (
      <VerticalNavItem
        key={item.to}
        title={item.title}
        iconClass={item.iconClass}
        active={item === activeItem}
        onClick={() => this.navigateTo(item.to)}
      >
        {item.subItems &&
          item.subItems.map(secondaryItem => (
            <VerticalNavSecondaryItem
              key={secondaryItem.to}
              title={secondaryItem.title}
              iconClass={secondaryItem.iconClass}
              active={secondaryItem.to === location.pathname}
              onClick={() => this.navigateTo(secondaryItem.to)}
            />
          ))}
      </VerticalNavItem>
    ));

    let loginNav;

    if (userId) {
      loginNav = (
        <Masthead.Dropdown
          id="user"
          title={
              <span>
                  <Icon type="pf" name="user" />{' '}
                  <span className="dropdown-title">tristanC</span>
              </span>
              }
              >
          <MenuItem eventKey="1">User Preferences</MenuItem>
          <MenuItem eventKey="2" onClick={this.onLogout()}>Logout</MenuItem>
        </Masthead.Dropdown>
      );
    } else {
      loginNav = (
        <NavItem
          id="login"
          title="login"
          onClick={this.onLogin}><p>Login</p></NavItem>
      );
    }


    return (
      <React.Fragment>
        <VerticalNav persistentSecondary={false}>
          <VerticalNav.Masthead iconImg={logo}>
            <VerticalNavIconBar>
              <Masthead.Dropdown
                id="help"
                title={<Icon type="pf" name="help" />}
                noCaret
                >
                <MenuItem eventKey="1">Help</MenuItem>
                <MenuItem eventKey="2">About</MenuItem>
              </Masthead.Dropdown>
              {loginNav}
            </VerticalNavIconBar>
          </VerticalNav.Masthead>
          {vertNavItems}
        </VerticalNav>
        {this.renderContent()}
      </React.Fragment>
    );
  }

  render() {
    const { about } = this.props;

    let content;

    if (this.state.loginPage) {
      content = (<LoginApp app={this} />);
    } else if (about.services) {
      content = this.renderMain();
    } else {
      content = (
        <React.Fragment>
          <VerticalNav persistentSecondary={false}>
            <VerticalNav.Masthead iconImg={logo}>
            </VerticalNav.Masthead>
          </VerticalNav>

          <Grid fluid className="container-pf-nav-pf-vertical">
            <Grid.Row>
              <Grid.Col xs={12}>
                  <h1>Loading...</h1>
              </Grid.Col>
            </Grid.Row>
          </Grid>
        </React.Fragment>
      );
    }
    return content;
  }
  handleNavClick = (event: Event) => {
    event.preventDefault();
    const target = (event.currentTarget: any);
    const { history } = this.props;
    if (target.getAttribute) {
      const href = target.getAttribute('href');
      console.log("Pushing " + href);
      history.push(href);
    }
  };
}

App.propTypes = {
  history: PropTypes.object.isRequired,
  location: PropTypes.object.isRequired
};

const mapStateToProps = (state, ownProps) => {
  return {
    about: state.about
  }
};

const mapDispatchToProps = (dispatch) => {
  return {
  }
};

export default connect(mapStateToProps, mapDispatchToProps)(withRouter(App));
