Name:           sdgyrodsu
Version:        {{{ git_dir_version }}}
Release:        1%{?dist}
Summary:        DSU server for motion data running on Steam Deck. 
License:        MIT
URL:            https://github.com/KyleGospo/SteamDeckGyroDSU

VCS:            {{{ git_dir_vcs }}}
Source:         {{{ git_dir_pack }}}
Patch0:         fedora.patch

BuildRequires:  systemd-rpm-macros
BuildRequires:  gcc
BuildRequires:  gcc-c++
BuildRequires:  ncurses-devel

%global debug_package %{nil}

%description
DSU (cemuhook protocol) server for motion data for Steam Deck.

%prep
{{{ git_dir_setup_macro }}}
%patch 0

%build
%set_build_flags
%make_build

%install
install -Dsm 755 bin/release/%{name} %{buildroot}%{_bindir}/%{name}
install -Dm 755 pkg/%{name}.service %{buildroot}%{_userunitdir}/%{name}.service
install -Dm 644 pkg/51-deck-controls.rules %{buildroot}%{_udevrulesdir}/51-deck-controls.rules

%post
%systemd_user_post %{name}.service
%udev_rules_update

%preun
%systemd_user_preun %{name}.service

%postun
%udev_rules_update

%files
%license LICENSE
%{_bindir}/%{name}
%{_userunitdir}/%{name}.service
%{_udevrulesdir}/51-deck-controls.rules

%changelog
{{{ git_dir_changelog }}}
