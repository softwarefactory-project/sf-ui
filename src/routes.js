import Welcome from './pages/Welcome';
import Review from './pages/Review';
// CI
import Status from './pages/Status';
import Jobs from './pages/Jobs';
import Builds from './pages/Builds';

const baseName = '/';

const routes = () => [
  {
    title: 'Welcome',
    to: '/welcome',
    component: Welcome
  },
  {
    iconClass: 'fa pficon-in-progress',
    title: 'Review',
    to: '/review',
    component: Review
  },
  {
    iconClass: 'fa fa-dashboard',
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
];

export { baseName, routes };
