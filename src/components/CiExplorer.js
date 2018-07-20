import * as React from 'react';
import {
  AggregateStatusCount,
  Icon,
  Nav,
  Card,
  CardGrid,
  CardTitle,
  NavItem,
  Uncontroller,
  TabContainer,
  TabPane,
  TabContent,
  Grid,
  Row,
  Col,
  ListView,
  Masthead,
  MenuItem,
} from 'patternfly-react';
import classNames from 'classnames';
import StatusData from './status';

class ProjectExplorer extends React.Component {
  render () {
    const { name, data, idx } = this.props;
    var labels = [];
    Object.values(data.jobs).map(jdata => (
        jdata.labels.map(label => {
          if (labels.indexOf(label) == -1) {
              labels.push(label);
          }
        })
    ));
    const renderAdditionalInfoItems = function() {
      return (
        <span>
        <ListView.InfoItem>
          <span className="pficon pficon-cluster" />
          <strong>{Object.keys(data.jobs).length}</strong> jobs
        </ListView.InfoItem>
        <ListView.InfoItem>
          <span className="pficon pficon-image" />
          <strong>{labels.length}</strong> labels
        </ListView.InfoItem>
        </span>
      )
    };
    return (
      <ListView.Item
        key={idx}
        heading={name}
        additionalInfo={renderAdditionalInfoItems()}
        >
        <p>Job configured:</p>
        <ul>
          {Object.keys(data.jobs).map(name => (
            <li>{name}</li>
          ))}
        </ul>
        <p>Label used:</p>
        <ul>
          {labels.map(name => (
            <li>{name}</li>
          ))}
        </ul>
      </ListView.Item>
    )
  }
};

class ProjectsExplorer extends React.Component {
  render () {
    const { data } = this.props;
    return (
      <ListView>
        {Object.entries(data).map(([name, data], idx) => (
          <ProjectExplorer name={name} data={data} idx={idx} />
       ))}
      </ListView>
    );
  }
};

class TenantExplorer extends React.Component {
  render () {
    const { data } = this.props;
    return (
      <TabContainer id="basic-tabs-pf" defaultActiveKey={1}>
        <div>
          <Nav bsClass="nav nav-tabs nav-tabs-pf">
            <NavItem eventKey={0}>
              <div><p>Projects</p></div>
            </NavItem>
            <NavItem eventKey={1}>
              <div><p>Jobs</p></div>
            </NavItem>
            <NavItem eventKey={2}>
              <div><p>Nodes</p></div>
            </NavItem>
          </Nav>
          <TabContent>
            <TabPane eventKey={0}>
              <ProjectsExplorer data={data.projects} />
            </TabPane>
            <TabPane eventKey={1}>
              <p>Jobs...</p>
            </TabPane>
            <TabPane eventKey={2}>
              <p>Nodes...</p>
            </TabPane>
          </TabContent>
        </div>
      </TabContainer>
    );
  }
}

class CiExplorer extends React.Component {
  constructor(props) {
    super(props);

    this.state = {
      tenants: null,
    };
  }

  componentDidMount() {
    this.setState({tenants: JSON.parse(StatusData).tenants});
  }

  render() {
    const { tenants } = this.state;
    if (!tenants) {
      return <div>Loading...</div>;
    }
    return (
      <div>
        {/*<ListView>
        <ProjectExplorer name="softwarefactory-project.io/DLRN" data={tenants.local.projects["softwarefactory-project.io/DLRN"]} idx={0} />
        </ListView> */}

      <TabContainer id="basic-tabs-pf" defaultActiveKey={1}>
        <div>
          <Nav bsClass="nav nav-tabs nav-tabs-pf">
            {Object.entries(tenants).map(([name, data]) => (
              <NavItem eventKey={name}>
                <div><p>{name}</p></div>
              </NavItem>
            ))}
          </Nav>
          <TabContent>
            {Object.entries(tenants).map(([name, data]) => (
              <TabPane eventKey={name}>
                <TenantExplorer data={data} />
              </TabPane>
            ))}
          </TabContent>
        </div>
      </TabContainer>
      </div>
    );
  }
}


export default CiExplorer;
