# Defines a constant that is used later by the spec file.
%define shortname uchroma

# Name, brief description, and version 
Summary: uChroma - 2D/3D visualisation of scientific data
Name: %{shortname}
Version: 0.2.1
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
BuildRequires: gcc-c++ bison Mesa-devel hdf5-devel ftgl-devel libQt5Core-devel libQt5Core5 libQt5OpenGL-devel libQt5OpenGL5 libQt5Widgets5 libQt5Widgets-devel libQt5Svg5

# In-depth description.
%description
uChroma is designed to plot data for presentation-quality 2D or 3D images. 

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
/usr/share/uchroma/

%changelog
* Sun Aug 23 2015 Tristan Youngs <tris@projectaten.net> 
- initial version.
