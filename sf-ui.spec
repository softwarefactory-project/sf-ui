%global debug_package %{nil}

# Enable fedora built rpm to be installable on el7
%define _source_payload w9.gzdio
%define _binary_payload w9.gzdio

Name:		sf-ui
Version:	0.2.1
Release:	1%{?dist}
Summary:	The software-factory web interface

License:	APACHE-2
URL:	        https://www.softwarefactory-project.io
Source0:	HEAD.tgz
BuildArch:      noarch

%description
The software-factory web interface

%prep
%autosetup -n dist

%build
# Built outside

%install
install -d -m 755 %{buildroot}/usr/share/sf-ui
mv * %{buildroot}/usr/share/sf-ui/

%files
/usr/share/sf-ui

%changelog
* Tue Mar  1 2022 Fabien Boucher <fboucher@redhat.com> - 0.2.1-1
- Bump to force update of re-sf (to fix gitweb->gitiles location)

* Thu Mar  4 2021 Tristan Cacqueray <tdecacqu@redhat.com> - 0.2.0-1
- Update sf-ui file locations

* Wed Oct  7 2020 Tristan Cacqueray <tdecacqu@redhat.com> - 0.1.0-1
- Initial packaging
