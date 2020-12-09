# sf-ui

## Setup

Install yarnpkg and nodejs: `sudo dnf install -y nodejs yarnpkg`

Then setup the project:

```
for dep in re-sf re-patternfly; do
  git clone https://softwarefactory-project.io/r/software-factory/${depo} ../${dep}
done
podman run --rm quay.io/software-factory/nodejs-builder cat /usr/libexec/shake/yarn.lock > yarn.lock
yarn install
yarn start
```

Run test with:

```
yarn test
```

Setup live hot-reload:

```
yarn serve
# Open browser on http://localhost:1234
```

Distribute with:

```
yarn dist
# Open browser in dist/
```

## RPM Build

We don't have CI on that packaging for now, so let's build the rpm/rpms with rpmbuild.

```
rm -Rf dist
yarn dist
tar -czf dist.tar.gz dist
mkdir -p ~/rpmbuild/RPMS ~/rpmbuild/SOURCES ~/rpmbuild/SRPMS
mv dist.tar.gz ~/rpmbuild/SOURCES/
yum install -y rpm-build
rpmbuild -ba sf-ui.spec
```
