# sf-ui

## Setup

Install yarnpkg and nodejs: `sudo dnf install -y nodejs yarnpkg`

Then setup the project:

```
for dep in re-sf re-patternfly res-remoteapi; do
  git clone https://softwarefactory-project.io/r/software-factory/${dep} ../${dep}
done
podman run --rm quay.io/software-factory/nodejs-builder cat /usr/libexec/shake/yarn.lock > yarn.lock
pnpm install --shamefully-hoist
pnpm start
```

Run test with:

```
pnpm test
```

Setup live hot-reload:

```
pnpm serve
# Open browser on http://localhost:1234
```

Distribute with:

```
pnpm dist
# Open browser in dist/
```

## RPM Build

```
rm -Rf dist
pnpm dist
tar -czf HEAD.tgz dist
mkdir -p ~/rpmbuild/RPMS ~/rpmbuild/SOURCES ~/rpmbuild/SRPMS
mv HEAD.tgz ~/rpmbuild/SOURCES/
yum install -y rpm-build
rpmbuild -ba sf-ui.spec
```
