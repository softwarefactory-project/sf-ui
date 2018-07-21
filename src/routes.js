import Welcome from './pages/Welcome';
import Review from './pages/Review';
// CI
import Status from './pages/Status';
import Jobs from './pages/Jobs';
import Builds from './pages/Builds';

import CiExplorer from './pages/CiExplorer';

// Log-Classify
import LogClassify from './pages/LogClassify';

const baseName = '/';

const routes = () => [
  {
    iconClass: 'fa pficon-in-progress',
    title: 'Review',
    to: '/review',
    component: Review
  },
  {
    title: 'Review',
    to: '/review/:path(.*)',
    component: Review
  },
  {
    iconClass: 'fa pficon-integration',
    title: 'CI',
    to: '/ci',
    subItems: [
      {
        iconClass: 'fa pficon-in-progress',
        title: 'Status',
        to: '/ci/status',
        component: Status
      },
      {
        iconClass: 'fa fa-list',
        title: 'Jobs',
        to: '/ci/jobs',
        component: Jobs
      },
      {
        iconClass: 'fa fa-table',
        title: 'Builds',
        to: '/ci/builds',
        component: Builds
      }
    ]
  },
  {
    iconClass: 'fa fa-tachometer',
    title: 'Status CI',
    to: '/status/ci',
    component: CiExplorer
  },
  {
    iconClass: 'fa pficon-zone',
    title: 'Log Classify',
    to: '/log-classify',
    component: LogClassify
  },
  {
    title: 'Log Classify',
    to: '/log-classify/:anomalyId',
    component: LogClassify
  },
  {
    title: 'Welcome',
    to: '/welcome',
    component: Welcome
  },
];

export { baseName, routes };
