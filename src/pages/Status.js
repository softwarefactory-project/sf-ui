import * as React from 'react';
import { Grid } from 'patternfly-react';

const StatusPage = () => (
  <Grid fluid className="container-pf-nav-pf-vertical">
    <Grid.Row>
      <Grid.Col xs={12}>
        <div className="page-header">
          <h1>Status Page</h1>
        </div>
      </Grid.Col>
    </Grid.Row>
  </Grid>
);

export default StatusPage;
