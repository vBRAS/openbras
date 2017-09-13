Name:        openbras
Version:     @OPENBRAS_RPM_VERSION
Release:     1
Summary:     OPENBRAS
Vendor:      Nanjing YunEx Network Technology Co., Ltd
Packager:    YunEx.com

Group:       Development/Server
License:     BSD
URL:         http://www.yunex.com
Source0:     %{name}-@OPENBRAS_RPM_VERSION.tar.gz

BuildRoot: %{_tmppath}/%{name}-buildroot
Requires: starcore
Requires: stardlls
Requires: dipc
Requires: protocol
Requires: starlang
%define _binaries_in_noarch_packages_terminate_build   0

%description
OpenBRAS binaries for Red Hat Linux.

%prep
%setup -q

%install
mkdir -p %{buildroot}
cp -R * %{buildroot}


%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
/opt/yunex.com/openbras
%post
echo "/opt/yunex.com/openbras/libs"> /etc/ld.so.conf.d/yunex-openbras-x86_64.conf
ldconfig
echo OpenBRAS successfully

%preun
%postun
#TODO this should be less brute-force
rm -rf /opt/yunex.com/openbras
ldconfig
%changelog
# TODO

