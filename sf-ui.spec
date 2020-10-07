%global debug_package %{nil}

Name:		sf-ui
Version:	0.1.0
Release:	1%{?dist}
Summary:	The software-factory web interface

License:	APACHE-2
URL:	        https://www.softwarefactory-project.io
Source0:	dist.tar.gz
BuildArch:      noarch

%description
The software-factory web interface

%prep
%autosetup -n dist

%build
# Built outside

%install
install -d -m 755 %{buildroot}/var/www/sf-ui
mv * %{buildroot}/var/www/sf-ui/

%files
/var/www/sf-ui/

%changelog
* Wed Oct  7 2020 Tristan Cacqueray <tdecacqu@redhat.com>
- Initial packaging
