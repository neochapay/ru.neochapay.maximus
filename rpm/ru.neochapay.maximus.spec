%define __provides_exclude_from ^%{_datadir}/%{name}/.*$
%define __requires_exclude ^lib.*\\.*$

Name:       ru.neochapay.maximus
Summary:    Клиент мессенджера макс
Version:    0.0.8
Release:    1
License:    BSD-3-Clause
URL:        https://neochapay.ru
Source0:    %{name}-%{version}.tar.bz2

Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  pkgconfig(auroraapp)
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)

%description
OpenSource клиент мессенджера максдля ОС Аврора

%prep
%autosetup

%build
%qmake5
%make_build

%install
%make_install
for size in 86 108 128 172
do
   mkdir -p %{buildroot}%{_datadir}/icons/hicolor/${size}x${size}/apps/
   cp -r %{_sourcedir}/../src_aurora/icons/${size}x${size}/* %{buildroot}%{_datadir}/icons/hicolor/${size}x${size}/apps/
done

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%defattr(644,root,root,-)
%{_datadir}/%{name}/
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
