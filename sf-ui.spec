%global debug_package %{nil}

# Enable fedora built rpm to be installable on el7
%define _source_payload w9.gzdio
%define _binary_payload w9.gzdio

Name:		sf-ui
Version:	0.1.0
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
install -d -m 755 %{buildroot}/var/www/
mv * %{buildroot}/var/www/

%files
/var/www/

%changelog
* Wed Oct  7 2020 Tristan Cacqueray <tdecacqu@redhat.com>
- Initial packaging
