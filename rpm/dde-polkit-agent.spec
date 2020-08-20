%global repo dde-polkit-agent

Name:           dde-polkit-agent
Version:        5.0.10
Release:        1
Summary:        Deepin Polkit Agent
License:        GPLv3
URL:            https://github.com/linuxdeepin/dde-polkit-agent
Source0:        %{name}_%{version}.orig.tar.xz

BuildRequires:  gcc-c++
BuildRequires:  dtkwidget-devel >= 5.1.1
#BuildRequires:  pkgconfig(dframeworkdbus) >= 2.0
BuildRequires:  dde-qt-dbus-factory-devel
BuildRequires:  dde-qt-dbus-factory
BuildRequires:  pkgconfig(polkit-qt5-1)
BuildRequires:  qt5-devel
BuildRequires:  qt5-linguist
BuildRequires:  gsettings-qt-devel
BuildRequires:  qt5-qtmultimedia-devel 
BuildRequires:  qt5-qtx11extras-devel
#BuildRequires:  libdframeworkdbus-dev

%description
DDE Polkit Agent is the polkit agent used in Deepin Desktop Environment.

%package devel
Summary:        Development package for %{name}
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description devel
Header files and libraries for %{name}.

%prep
%setup -q -n %{repo}-%{version}
sed -i 's|lrelease|lrelease-qt5|' translate_generation.sh
sed -i 's|/usr/lib|/usr/libexec|' dde-polkit-agent.pro polkit-dde-authentication-agent-1.desktop \
    pluginmanager.cpp

%build
%qmake_qt5 PREFIX=%{_prefix}
%make_build

%install
%make_install INSTALL_ROOT=%{buildroot}

%files
%doc README.md
%license LICENSE
/usr/libexec/polkit-1-dde/dde-polkit-agent
%{_datadir}/%{repo}/

%files devel
%{_includedir}/dpa/agent-extension-proxy.h
%{_includedir}/dpa/agent-extension.h

%changelog
* Wed Jun 10 2020 uoser <uoser@uniontech.com> - 5.0.9
- Update to 5.0.9
