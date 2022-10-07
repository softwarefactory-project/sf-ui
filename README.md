# sf-ui

## Setup

Checkout a nix-shell to get a pnpm version than work fine for that project (6.20.1). The CI is currenlty using the same version:

```
nix-shell -I nixpkgs=https://github.com/NixOS/nixpkgs/archive/0a4ac64c8f0a55510f4b89eb64080912508410ee.tar.gz -p ocamlPackages.merlin nodePackages.pnpm
```

Alternatively to the nix's command you should be able to run "pmpm install pnpm@6.20.1".

Then setup the project:

```
for dep in re-sf re-patternfly res-remoteapi; do
  git clone https://softwarefactory-project.io/r/software-factory/${dep} ../${dep}
done
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

## VSCode
Run vscode from your nix-shell and install the reason plugin named: `jaredly.reason-vscode`.
Syntax hightlight and check should work out of the box.

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
