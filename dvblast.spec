%global github_owner Gujs
%global github_branch biss-descrambling

Name:           dvblast
Version:        3.5
Release:        1.biss%{?dist}
Summary:        Simple and powerful streaming application

License:        GPLv2+
URL:            https://github.com/%{github_owner}/dvblast
Source0:        https://github.com/%{github_owner}/dvblast/archive/%{github_branch}/dvblast-%{github_branch}.tar.gz
Source1:        https://github.com/glenvt18/libdvbcsa/archive/master/libdvbcsa-master.tar.gz

BuildRequires:  gcc
BuildRequires:  make
BuildRequires:  autoconf
BuildRequires:  automake
BuildRequires:  libtool
BuildRequires:  bitstream-devel >= 1.4
BuildRequires:  libev-devel


%description
DVBlast is a simple and powerful streaming application based on the
linux-dvb API. It opens a DVB device, tunes it, places PID filters,
configures a CAM module, and demultiplexes the packets to several RTP
outputs. It supports the new S2API of linux-dvb (compile option).

This build includes BISS-1 descrambling support via libdvbcsa.


%prep
%setup -q -n dvblast-%{github_branch}
%setup -q -T -D -a 1
# Prepare dvbiscovery
sed -i -e 's|/usr/local|/usr|' extra/dvbiscovery/dvbiscovery.sh
install -pm 0644 extra/dvbiscovery/README README.dvbiscovery


%build
# Build libdvbcsa as static lib in local prefix
pushd libdvbcsa-master
./bootstrap
./configure --enable-static --disable-shared --prefix=%{_builddir}/dvbcsa-local
make %{?_smp_mflags}
make install
popd

# Build dvblast with BISS support
export CFLAGS="%{optflags} -DHAVE_DVBCSA -I%{_builddir}/dvbcsa-local/include"
export LDFLAGS="%{?__global_ldflags}"
export LDLIBS_DVBLAST="-lpthread -lev -L%{_builddir}/dvbcsa-local/lib -ldvbcsa"
%make_build V=1


%install
%make_install PREFIX=%{_prefix}

# Add missing dvbiscovery scripts
mkdir -p %{buildroot}%{_datadir}/%{name}/dvbiscovery
install -pm 0644 extra/dvbiscovery/*.conf \
  %{buildroot}%{_datadir}/%{name}/dvbiscovery
install -pm 0755 extra/dvbiscovery/dvbiscovery.sh %{buildroot}%{_bindir}



%files
%doc AUTHORS COPYING NEWS README TODO
%doc README.dvbiscovery
%license COPYING
%{_bindir}/dvbiscovery.sh
%{_bindir}/dvblast
%{_bindir}/dvblastctl
%{_bindir}/dvblast_mmi.sh
%{_mandir}/man1/dvblast.1.*
%{_datadir}/%{name}

%changelog
* Sat Mar 15 2026 Gregor <gregor@localhost> - 3.5-1.biss
- Upgrade to 3.5 from Gujs/dvblast biss-descrambling branch
- Add BISS-1 descrambling support (libdvbcsa bundled as static lib)
- Per-output /biss=<key> config option for inline DVB-CSA decryption

* Wed Jul 21 2021 Fedora Release Engineering <releng@fedoraproject.org> - 3.4-7
- Rebuilt for https://fedoraproject.org/wiki/Fedora_35_Mass_Rebuild

* Tue Jan 26 2021 Fedora Release Engineering <releng@fedoraproject.org> - 3.4-6
- Rebuilt for https://fedoraproject.org/wiki/Fedora_34_Mass_Rebuild

* Mon Jul 27 2020 Fedora Release Engineering <releng@fedoraproject.org> - 3.4-5
- Rebuilt for https://fedoraproject.org/wiki/Fedora_33_Mass_Rebuild

* Tue Jan 28 2020 Fedora Release Engineering <releng@fedoraproject.org> - 3.4-4
- Rebuilt for https://fedoraproject.org/wiki/Fedora_32_Mass_Rebuild

* Wed Jul 24 2019 Fedora Release Engineering <releng@fedoraproject.org> - 3.4-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_31_Mass_Rebuild

* Thu Jan 31 2019 Fedora Release Engineering <releng@fedoraproject.org> - 3.4-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_30_Mass_Rebuild

* Mon Sep 24 2018 Nicolas Chauvet <kwizart@gmail.com> - 3.4-1
- Update to 3.4

* Tue Jul 17 2018 Nicolas Chauvet <kwizart@gmail.com> - 3.3-3
- Add missing cc

* Thu Jul 12 2018 Fedora Release Engineering <releng@fedoraproject.org> - 3.3-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_29_Mass_Rebuild

* Wed Jul 04 2018 Nicolas Chauvet <kwizart@gmail.com> - 3.3-1
- Update to 3.3

* Tue Feb 06 2018 Nicolas Chauvet <kwizart@gmail.com> - 3.1-2
- Rebuilt for bitsteam 1.3

* Thu Sep 14 2017 Nicolas Chauvet <kwizart@gmail.com> - 3.1-1
- Update to 3.1

* Wed Aug 02 2017 Fedora Release Engineering <releng@fedoraproject.org> - 3.0-6
- Rebuilt for https://fedoraproject.org/wiki/Fedora_27_Binutils_Mass_Rebuild

* Wed Jul 26 2017 Fedora Release Engineering <releng@fedoraproject.org> - 3.0-5
- Rebuilt for https://fedoraproject.org/wiki/Fedora_27_Mass_Rebuild

* Fri Feb 10 2017 Fedora Release Engineering <releng@fedoraproject.org> - 3.0-4
- Rebuilt for https://fedoraproject.org/wiki/Fedora_26_Mass_Rebuild

* Tue Oct 04 2016 Nicolas Chauvet <kwizart@gmail.com> - 3.0-3
- Add dvbiscovery scripts

* Tue Oct 04 2016 Nicolas Chauvet <kwizart@gmail.com> - 3.0-2
- Backport fix for LDFLAGS from upstream

* Thu Sep 15 2016 Nicolas Chauvet <kwizart@gmail.com> - 3.0-1
- Update to 3.0

* Thu Aug 23 2012 Nicolas Chauvet <kwizart@gmail.com> - 2.2-1
- Update to 2.2

* Mon Jan 16 2012 Nicolas Chauvet <kwizart@gmail.com> - 2.1.0-1
- Update to 2.1.0

* Sat Apr 24 2010 Nicolas Chauvet <kwizart@fedoraproject.org> - 1.2-1
- Update to 1.2

* Mon Nov 30 2009 Nicolas Chauvet <kwizart@fedoraproject.org> - 1.1-1
- Initial spec file
