import * as React from 'react';
import {
  AggregateStatusNotifications,
  AggregateStatusNotification,
  Icon,
  Nav,
  CardGrid,
  Card,
  CardTitle,
  CardBody,
  NavItem,
  TabContainer,
  TabPane,
  TabContent,
  // Grid,
  // Row,
  // Col,
  ListView,
} from 'patternfly-react';
import StatusData from '../mock/status';

class ProjectExplorer extends React.Component {
  render () {
    const { project } = this.props;
    var labels = [];
    project.jobs.forEach(job => (
        job.labels.forEach(label => {
          if (labels.indexOf(label) === -1) {
              labels.push(label);
          }
        })
    ));
    const renderAdditionalInfoItems = function() {
      return [(
        <ListView.InfoItem key={1}>
          <span className="pficon pficon-cluster" />
          <strong>{project.jobs.length}</strong> jobs
        </ListView.InfoItem>
      ), (
        <ListView.InfoItem key={2}>
          <span className="pficon pficon-image" />
          <strong>{labels.length}</strong> labels
        </ListView.InfoItem>
      ), (
        <ListView.InfoItem key={3}>
          <span className="pficon pficon-bundle" />
          <strong>{project.pipelines.length}</strong> pipelines
        </ListView.InfoItem>
      )];
    };
    return (
      <ListView.Item
        heading={project.name}
        key={project.name}
        additionalInfo={renderAdditionalInfoItems()}
        >
        <p>Job configured:</p>
        <ul>
          {project.jobs.map((job, idx) => (
            <li key={idx}>{job.name}</li>
          ))}
        </ul>
        <p>Label used:</p>
        <ul>
        {labels.map((name, idx) => (
            <li key={idx}>{name}</li>
          ))}
        </ul>
      </ListView.Item>
    );
  }
};

class ProjectsExplorer extends React.Component {
  render () {
    const { projects } = this.props;
    return (
      <ListView>
        {projects.map((project, idx) => (
          <ProjectExplorer project={project} key={idx} />
       ))}
      </ListView>
    );
  }
};

class JobsExplorer extends React.Component {
  render () {
    const { jobs } = this.props;
    return (
      <CardGrid fluid={true}>
        <CardGrid.Row>
          {Object.entries(jobs).map(([name, project_list], idx) => (
            <CardGrid.Col xs={12} md={4} key={idx}>
              <Card accented aggregated>
                <CardTitle>
                  <Icon name="shield" />
                  {name}

                </CardTitle>
                <CardBody>
                  <AggregateStatusNotifications>
                    <AggregateStatusNotification>
                      <Icon type="pf" name="ok" />
                      {project_list.length}
                    </AggregateStatusNotification>
                  </AggregateStatusNotifications>
                </CardBody>
              </Card>
            </CardGrid.Col>
          ))}
        </CardGrid.Row>
      </CardGrid>
    );
  }
}

class LabelsExplorer extends React.Component {
  render () {
    const { labels } = this.props;
    return (
      <CardGrid fluid={true}>
        <CardGrid.Row>
          {Object.entries(labels).map(([name, project_list], idx) => (
            <CardGrid.Col xs={12} md={4} key={idx}>
              <Card accented aggregated>
                <CardTitle>
                  <Icon name="shield" />
                  {name}

                </CardTitle>
                <CardBody>
                  <AggregateStatusNotifications>
                    <AggregateStatusNotification>
                      <Icon type="pf" name="ok" />
                      {project_list.length}
                    </AggregateStatusNotification>
                  </AggregateStatusNotifications>
                </CardBody>
              </Card>
            </CardGrid.Col>
          ))}
        </CardGrid.Row>
      </CardGrid>
    );
  }
}



class TenantExplorer extends React.Component {
  constructor(props) {
    super(props);

    this.state = {
      page: 'projects',
    };
  }
  handleSelect (page) {
    this.setState({page: page});
  }

  render () {
    const { page } = this.state;
    const { tenant } = this.props;
    let tabContent;
    if (page === 'projects') {
      tabContent = (<ProjectsExplorer projects={tenant.projects} />);
    }
    else if (page === 'jobs') {
      tabContent = (<JobsExplorer jobs={tenant.jobs} />);
    }
    else if (page === 'labels') {
      tabContent = (<LabelsExplorer labels={tenant.labels} />);
    }
    return (
      <TabContainer id="tenant-status-list">
        <div>
          <Nav bsClass="nav nav-tabs nav-tabs-pf">
            <NavItem onClick={() => this.handleSelect("projects")}>
              <div><p>Projects</p></div>
            </NavItem>
            <NavItem onClick={() => this.handleSelect("jobs")}>
              <div><p>Jobs</p></div>
            </NavItem>
            <NavItem onClick={() => this.handleSelect("labels")}>
              <div><p>Labels</p></div>
            </NavItem>
          </Nav>
          <TabContent>
            <TabPane>{tabContent}</TabPane>
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
      tenant: null,
    };
  }

  componentDidMount() {
    const tenants = JSON.parse(StatusData).tenants;
    // Generate job and label lists
    tenants.forEach(tenant => {
      tenant.jobs = {};
      tenant.labels = {};
      tenant.projects.forEach(project => {
        project.jobs.forEach(job => {
          if (!tenant.jobs[job.name]) {
            tenant.jobs[job.name] = [];
          }
          tenant.jobs[job.name].push(project);
          job.labels.forEach(label => {
            if (!tenant.labels[label]) {
              tenant.labels[label] = [];
            }
            tenant.labels[label].push(project);
          });
        });
      });
    });
    this.setState({
      tenants: tenants,
      // devel: auto select a tenant
      // tenant: tenants[1]
    });
  }

  handleSelect (tenant) {
    this.setState({tenant: tenant});
  }

  render() {
    const { tenant, tenants } = this.state;
    if (!tenants) {
      return <div>Loading...</div>;
    }
    let tabContent;

    if (!tenant) {
      tabContent = (<p>Select a tenant ^</p>);
    } else {
      tabContent = (<TenantExplorer tenant={tenant} />);
    }
    return (
      <TabContainer id="tenant-list">
        <div>
          <Nav bsClass="nav nav-tabs nav-tabs-pf">
            {tenants.map((tenant, idx) => (
              <NavItem key={idx} onClick={() => this.handleSelect(tenant)}>
                <div><p>{tenant.name}</p></div>
              </NavItem>
            ))}
          </Nav>
          <TabContent>
            <TabPane>{tabContent}</TabPane>
          </TabContent>
        </div>
      </TabContainer>
    );
  }
}


export default CiExplorer;
