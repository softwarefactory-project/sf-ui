import * as React from 'react';
import { Grid } from 'patternfly-react';

const WelcomePage = () => (
  <Grid fluid className="container-pf-nav-pf-vertical">
    <Grid.Row>
      <Grid.Col xs={12}>
        <div className="page-header">
          <h1>Welcome</h1>
          <p>This platform hosts git repositories, a review system and CI components.</p>
        </div>
      </Grid.Col>
    </Grid.Row>
  </Grid>
);

export default WelcomePage;
