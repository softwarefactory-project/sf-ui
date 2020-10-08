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
