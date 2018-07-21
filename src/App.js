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
import { Link, Route, Redirect, Switch } from 'react-router-dom';
import {
  VerticalNav,
  VerticalNavItem,
  VerticalNavSecondaryItem,
  VerticalNavMasthead,
  VerticalNavBrand,
  VerticalNavIconBar,
  Dropdown,
  Icon,
  MenuItem
} from 'patternfly-react';
import logo from './img/logo.png';
import { routes } from './routes';

class App extends React.Component {
  constructor() {
    super();

    this.menu = routes();
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

  render() {
    const { location } = this.props;
    const activeItem = this.menu.find(
      item => location.pathname === item.to
    );
    const vertNavItems = this.menu
            .filter(item => item.iconClass)
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

    const dropdownComponentClass = props => (
      <li className={props.className}>{props.children}</li>
    );

    return (
      <React.Fragment>
        <VerticalNav persistentSecondary={false}>
          <VerticalNavMasthead>
            <Link to={'/welcome'}><VerticalNavBrand iconImg={logo} /></Link>
            <VerticalNavIconBar>
              <Dropdown componentClass={dropdownComponentClass} id="help">
                <Dropdown.Toggle
                  className="nav-item-iconic"
                  bsStyle="link"
                  noCaret
                >
                  <Icon type="pf" name="help" />
                </Dropdown.Toggle>
                <Dropdown.Menu>
                  <MenuItem eventKey="1">Help</MenuItem>
                  <MenuItem eventKey="2">About</MenuItem>
                </Dropdown.Menu>
              </Dropdown>
              <Dropdown componentClass={dropdownComponentClass} id="user">
                <Dropdown.Toggle className="nav-item-iconic" bsStyle="link">
                  <Icon type="pf" name="user" />{' '}
                  <span className="dropdown-title">TristanC</span>
                </Dropdown.Toggle>
                <Dropdown.Menu>
                  <MenuItem eventKey="1">Preferences</MenuItem>
                  <MenuItem eventKey="2">Logout</MenuItem>
                </Dropdown.Menu>
               </Dropdown>
            </VerticalNavIconBar>
          </VerticalNavMasthead>
          {vertNavItems}
        </VerticalNav>
        {this.renderContent()}
      </React.Fragment>
    );
  }
}

App.propTypes = {
  history: PropTypes.object.isRequired,
  location: PropTypes.object.isRequired
};

export default withRouter(App);
