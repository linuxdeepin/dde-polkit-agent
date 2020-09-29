%global repo dde-polkit-agent

Name:           deepin-polkit-agent
Version:        5.3.0.2
Release:        1%{?dist}
Summary:        Deepin Polkit Agent
License:        GPLv3
URL:            https://github.com/linuxdeepin/dde-polkit-agent
%if 0%{?fedora}
Source0:        %{url}/archive/%{version}/%{repo}-%{version}.tar.gz
%else
Source0:        %{name}_%{version}.orig.tar.xz
%endif

BuildRequires:  gcc-c++
BuildRequires:  dtkwidget-devel >= 5.1.1
BuildRequires:  pkgconfig(dframeworkdbus)
BuildRequires:  pkgconfig(polkit-qt5-1)
BuildRequires:  pkgconfig(Qt5)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(gsettings-qt)
BuildRequires:  pkgconfig(Qt5Multimedia)
BuildRequires:  pkgconfig(Qt5Multimedia)
BuildRequires:  pkgconfig(Qt5MultimediaWidgets)
BuildRequires:  pkgconfig(Qt5X11Extras)
BuildRequires:  qt5-linguist

%description
DDE Polkit Agent is the polkit agent used in Deepin Desktop Environment.

%package devel
Summary:        Development package for %{name}
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description devel
Header files and libraries for %{name}.

%prep
%setup -q -n %{repo}-%{version}
sed -i 's|/usr/lib|%{_libexecdir}|' dde-polkit-agent.pro polkit-dde-authentication-agent-1.desktop \
    pluginmanager.cpp

%build
# help find (and prefer) qt5 utilities, e.g. qmake, lrelease
export PATH=%{_qt5_bindir}:$PATH
%qmake_qt5 PREFIX=%{_prefix}
%make_build

%install
%make_install INSTALL_ROOT=%{buildroot}

%files
%doc README.md
%license LICENSE
%{_libexecdir}/polkit-1-dde/
%{_datadir}/%{repo}/

%files devel
%{_includedir}/dpa/

%changelog
* Wed Jun 10 2020 uoser <uoser@uniontech.com> - 5.0.9
- Update to 5.0.9
