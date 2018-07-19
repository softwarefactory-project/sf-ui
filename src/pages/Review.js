import * as React from 'react';
import { Grid } from 'patternfly-react';

const ReviewPage = () => (
  <Grid fluid className="container-pf-nav-pf-vertical">
    <Grid.Row>
      <Grid.Col xs={12}>
        <div className="page-header">
          <h1>Review Page</h1>
        </div>
      </Grid.Col>
    </Grid.Row>
  </Grid>
);

export default ReviewPage;
