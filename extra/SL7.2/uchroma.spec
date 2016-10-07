# Defines a constant that is used later by the spec file.
%define shortname uchroma

# Name, brief description, and version 
Summary: uChroma - 3D Scientific data visualisation and analysis
Name: %{shortname}
Version: 0.2.2
Release: 1
License: GPL
%define fullname %{name}-%{version}
# norootforbuild

# Define buildroot
BuildRoot: /var/tmp/%{fullname}

# Software group
Group: Productivity/Scientific/Chemistry

# Source tar ball.
Source: %{fullname}.tar.gz

# Location of the project's home page.
URL: http://www.projectaten.net

# Owner of the product.
Vendor: Tristan Youngs

# Packager of the product.
Packager: Tristan Youngs

# Boolean that specifies if you want to automatically determine some dependencies
AutoReq: yes

# Build dependencies
BuildRequires: gcc-c++ bison qt5-qtbase-devel qt5-qtsvg-devel hdf5-devel

# In-depth description.
%description
uChroma plots multiple 2D datasets as 3D surfaces, waterfall plots etc., and generates high-quality images for publication.

%prep
%setup -q

%build

# Configure and make
./configure --with-build-dir=$RPM_BUILD_ROOT --with-install-dir=/usr --prefix=$RPM_BUILD_ROOT/usr 
make

%install
make install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc README TODO COPYING ChangeLog
/usr/share/applications/uChroma.desktop
/usr/share/pixmaps/uchroma.png
/usr/bin/uchroma
/usr/share/uchroma

%changelog
* Wed Apr 02 2008 Tristan Youngs <tristan.youngs@stfc.ac.uk> 
- initial version.
