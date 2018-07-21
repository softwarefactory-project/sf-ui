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
import { withRouter } from 'react-router-dom';
import {
  Button,
  ListView,
  Table,
} from 'patternfly-react';
import AnomaliesData from '../mock/anomalies';
import AnomalyData from '../mock/anomaly';

import './LogClassify.css';


class LogLine extends React.Component {
  constructor() {
    super();
    this.state = {
      background: '#FFFFFF'
    };
    this.updateConfidence = this.updateConfidence.bind(this);
  }
  leftPad(n, width) {
    n = n + '';
    return n.length >= width ? n : new Array(
      width - n.length + 1).join('0') + n;
  }

  componentDidMount() {
    this.setBackground();
  }

  setBackground() {
    var r = Math.floor(255 - 142 * this.props.line[0]).toString(16);
    if (r.length === 1) {
      r = 'F' + r;
    }
    this.setState({background: '#FF' + r + r});
  }

  updateConfidence(evt) {
    this.props.line[0] = evt.target.value / 100;
    this.setBackground();
  }

  render() {
    const { line } = this.props;
    if (!line) {
      return (<p>Loading...</p>);
    }
    const { background } = this.state;
    return (
      <span>
        <input
          type="range"
          min="0"
          max="100"
          value={line[0] * 100}
          className="LogSlider"
          onChange={this.updateConfidence}
          />
        <p className="LogLine" style={{background: background}}>
          {this.leftPad(line[1], 3)} | {line[2]}
        </p>
      </span>
    );
  }
}

class LogFile extends React.Component {
  constructor() {
    super();
    this.myRef = React.createRef();
    this.submit = this.submit.bind(this);
  }
  submit() {
    const { anomaly, logfile } = this.props;
    console.log("Submitting " + anomaly.uuid + " -> " + logfile.lines);
  }
  componentDidMount() {
    this.myRef.current.setState({expanded: true});
  }
  render() {
    const { anomaly, logfile } = this.props;
    const AdditionalInfo = [(
      <ListView.InfoItem key={1}>
        <span className="pficon pficon-zone" />
        <strong>{logfile.lines.length}</strong> anomalies
      </ListView.InfoItem>
    )];
    return (
      <ListView.Item
        heading={logfile.path}
        additionalInfo={AdditionalInfo}
        active="true"
        ref={this.myRef}
        expanded
        >
        {logfile.lines.map((line, idx) => (
          <span key={idx}>
            <LogLine line={line} />
            {(idx > 0 && logfile.lines[idx - 1][1] + 1 !== line[1]) &&
            <hr />}
          </span>
        ))}
      {(anomaly.status !== 'archived') &&
        <Button
          bsStyle="primary"
          onClick={this.submit}>
          Submit
        </Button>
      }
      </ListView.Item>
    );
  }
}

class LogClassifyView extends React.Component {
  constructor() {
    super();

    this.state = {
      anomaly: null,
    };
    this.approve = this.approve.bind(this);
  }

  approve() {
    const { anomaly } = this.state;
    console.log("Approving " + anomaly.uuid);
  }

  componentDidMount() {
    const { anomaly } = this.props;
    console.log("Did mount called for " + anomaly);

    const anomalyData = JSON.parse(AnomalyData);
    this.setState({anomaly: anomalyData});
  }

  render() {
    const { anomaly } = this.state;
    if (!anomaly) {
      return (<p>Loading...</p>);
    }
    console.log(anomaly);
    return (
      <div>
        <ul>
          <li>Name: {anomaly.name}</li>
          <li>Status: {anomaly.status}</li>
          {anomaly.status === 'archived' &&
            <li>Archive: <a href={anomaly.url}>{anomaly.uuid}</a></li>
          }
          {anomaly.status === 'reviewed' &&
            <li>
              <Button onClick={this.approve} bsStyle="success">
                Approve
              </Button>
            </li>}
        </ul>
        <ListView>
        {anomaly.logfiles.map((logfile, idx) => (
          <LogFile key={idx} logfile={logfile} anomaly={anomaly} />
        ))}
        </ListView>
      </div>
    );
  }
}

class LogClassifyList extends React.Component {
  constructor() {
    super();

    this.state = {
      anomalies: [],
    };
  }

  componentDidMount() {
    console.log("Did mount called for list");
    const anomalies = JSON.parse(AnomaliesData);
    this.setState({anomalies: anomalies});
  }

  handleClick(anomaly) {
    const { history } = this.props;
    history.push('/log-classify/' + anomaly['uuid']);
  }

  render() {
    const { anomalies } = this.state;
    if (anomalies.length === 0) {
      return (<p>Loading...</p>);
    }
    const headerFormat = value => <Table.Heading>{value}</Table.Heading>;
    const cellFormat = (value, { rowData }) => (
      <Table.Cell onClick={() => this.handleClick(rowData)}>
        {value}
      </Table.Cell>);
    const columns = [];
    const myColumns = ['name', 'reporter', 'status', 'report_date'];
    myColumns.forEach(column => {
      columns.push({
        header: {label: column,
                 formatters: [headerFormat]},
        property: column,
        cell: {formatters: [cellFormat]},
      });
    });

    return (
      <Table.PfProvider
        striped
        bordered
        hover
        columns={columns}
        >
        <Table.Header/>
        <Table.Body
          rows={anomalies}
          rowKey="uuid"
          />
      </Table.PfProvider>);
  }
}

const LogClassifyListWithRouter = withRouter(LogClassifyList);

class LogClassify extends React.Component {
  constructor(props) {
    super(props);

    this.state = {
      tenants: null,
      tenant: null,
    };
  }

  componentDidMount() {
    this.setState({
    });
  }

  render() {
    const { anomaly } = this.props;
    if (anomaly) {
      return (<LogClassifyView anomaly={anomaly} />);
    }
    return (<LogClassifyListWithRouter />);
  }
}

export default withRouter(LogClassify);
