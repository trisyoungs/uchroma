# Defines a constant that is used later by the spec file.
%define shortname uchroma

# Name, brief description, and version 
Summary: uChroma - 2D / 3D Scientific data visualisation
Name: %{shortname}
Version: 0.197
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

# Boolean that specifies if you want to automatically determine some
# dependencies.
AutoReq: yes

# Basic package dependencies are listed here. For RedHat-based distros, libqt4 = qt4, and libqt4-devel = qt4-devel
BuildRequires: gcc-c++

# For OpenSUSE distros
%if 0%{?suse_version}
BuildRequires: libqt4 libqt4-devel Mesa-devel libhdf5-devel
%endif

# For SLES-based distros, libqt4 = qt4, and libqt4-devel = qt4-devel
%if 0%{?sles_version} == 11
BuildRequires: Mesa-devel libqt4 libqt4-devel
%endif
%if 0%{?sles_version} == 10
BuildRequires: Mesa-devel libqt4-devel hdf5-devel qt4
%endif

# For RedHat-based distros, libqt4 = qt4, and libqt4-devel = qt4-devel
%if 0%{?fedora} || 0%{?rhel_version} || 0%{?centos_version}
BuildRequires: Mesa-devel qt4 qt4-devel hdf5-devel
%endif

# For Mandriva Linux
%if 0%{?mandriva_version} > 2006
BuildRequires: qt4-devel mesa-common-devel hdf5-devel
%endif

# In-depth description.
%description
uChroma is designed to allow multiple XY datasets to be plotted together easily, forming (optionally) a 3D surface representation of the data. This surface can be sliced, equations fitted to it etc.

%prep
%setup -q

# For the build, RedHat distros seem to need the path to the Qt4 binaries set explicitly. SuSE is fine.
%build

# Configure

%if 0%{?suse_version}
    export LIBS="$LIBS -lGL"
    ./configure --with-build-dir=$RPM_BUILD_ROOT --with-install-dir=/usr --prefix=$RPM_BUILD_ROOT/usr 
%endif

%if 0%{?sles_version}
    ./configure --with-build-dir=$RPM_BUILD_ROOT --with-install-dir=/usr --prefix=$RPM_BUILD_ROOT/usr 
%endif


%if 0%{?mandriva_version}
    %ifarch x86_64
        ./configure --with-build-dir=$RPM_BUILD_ROOT --with-install-dir=/usr --prefix=$RPM_BUILD_ROOT/usr --with-qtmoc=/usr/lib/qt4/bin/moc --with-qtrcc=/usr/lib/qt4/bin/rcc --with-qtuic=/usr/lib/qt4/bin/uic
    %else
        ./configure --with-build-dir=$RPM_BUILD_ROOT --with-install-dir=/usr --prefix=$RPM_BUILD_ROOT/usr --with-qtmoc=/usr/lib/qt4/bin/moc --with-qtrcc=/usr/lib/qt4/bin/rcc --with-qtuic=/usr/lib/qt4/bin/uic
    %endif
%endif

%if 0%{?fedora}
    %ifarch x86_64
        export LDFLAGS="-lGL"
        ./configure --with-build-dir=$RPM_BUILD_ROOT --with-install-dir=/usr --prefix=$RPM_BUILD_ROOT/usr --with-qtmoc=moc-qt4 --with-qtrcc=rcc --with-qtuic=uic-qt4
    %else
        export LDFLAGS="-lGL"
        ./configure --with-build-dir=$RPM_BUILD_ROOT --with-install-dir=/usr --prefix=$RPM_BUILD_ROOT/usr --with-qtmoc=moc-qt4 --with-qtrcc=rcc --with-qtuic=uic-qt4
    %endif
%endif

%if 0%{?rhel_version}
    %ifarch x86_64
        ./configure --with-build-dir=$RPM_BUILD_ROOT --with-install-dir=/usr --prefix=$RPM_BUILD_ROOT/usr --with-qtmoc=/usr/lib64/qt4/bin/moc --with-qtrcc=/usr/lib64/qt4/bin/rcc --with-qtuic=/usr/lib64/qt4/bin/uic --with-oldqt
    %else
        ./configure --with-build-dir=$RPM_BUILD_ROOT --with-install-dir=/usr --prefix=$RPM_BUILD_ROOT/usr --with-qtmoc=/usr/lib/qt4/bin/moc --with-qtrcc=/usr/lib/qt4/bin/rcc --with-qtuic=/usr/lib/qt4/bin/uic --with-oldqt
    %endif
%endif

%if 0%{?centos_version}
    %ifarch x86_64
        ./configure --with-build-dir=$RPM_BUILD_ROOT --with-install-dir=/usr --prefix=$RPM_BUILD_ROOT/usr --with-qtmoc=/usr/lib64/qt4/bin/moc --with-qtrcc=/usr/lib64/qt4/bin/rcc --with-qtuic=/usr/lib64/qt4/bin/uic --with-oldqt
    %else
        ./configure --with-build-dir=$RPM_BUILD_ROOT --with-install-dir=/usr --prefix=$RPM_BUILD_ROOT/usr --with-qtmoc=/usr/lib/qt4/bin/moc --with-qtrcc=/usr/lib/qt4/bin/rcc --with-qtuic=/usr/lib/qt4/bin/uic --with-oldqt
    %endif
%endif

%if 0%{?ubuntu_version}
    %ifarch x86_64
        ./configure --with-build-dir=$RPM_BUILD_ROOT --with-install-dir=/usr --prefix=$RPM_BUILD_ROOT/usr --with-qtmoc=moc-qt4 --with-qtrcc=rcc-qt4 --with-qtuic=uic-qt4
    %else
        ./configure --with-build-dir=$RPM_BUILD_ROOT --with-install-dir=/usr --prefix=$RPM_BUILD_ROOT/usr --with-qtmoc=moc-qt4 --with-qtrcc=rcc-qt4 --with-qtuic=uic-qt4
    %endif
%endif

make

%install
make install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc README TODO COPYING ChangeLog
/usr/share/applications/uChroma.desktop
/usr/bin/uchroma
/usr/share/uchroma/

%changelog
* Fri Oct 10 2014 Tristan Youngs <tris@projectaten.net> 
- added checks to build on different distros with the SuSE build service.
- initial version.
