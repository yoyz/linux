%define myversion	30.0
%define myrelease 	14.0%{?dist}
%define name		ipallocation
%define src_dir		%{name}-%{myversion}
%define src_tarall	%{src_dir}.tgz

%define is_sles %(test -e /etc/SuSE-release && echo 1 || echo 0)
%if %is_sles
%define python_sitelib /usr/lib64/python2.6/site-packages
%endif


Summary:	ipallocation - An allocation tool to set and modify IPv4 address to the clusterdb from the command line
Name:		%{name}
Version:	%{myversion}
Release:	%{myrelease}%{?ts}
License:	none
BuildArch: 	noarch
Source:		%{src_tarall}
Group:		CLM
URL:            http://forge.fr
BuildRoot:      /%_tmppath/%{src_dir}Build

Requires: python
Requires: python-sqlalchemy
Requires: clusterdb-schema
Requires: python-psycopg2
Requires: python-psycopg2-doc
%description
An allocation tool to set and modify IPv4 address to the clusterdb   

%define _libdir /usr/lib
%define _bcmprefix 		%{_libdir}/clustmngt
%define _clusterdbBinIpalloc	%{_bcmprefix}/clusterdb/bin
%{lib_python}
rpm --showrc|grep python
#%define _clusterdbLibIpalloc 	/usr/lib/python2.6/site-packages/CLM/DB/ipallocation
%define _clusterdbLibIpalloc 	%{python_sitelib}/CLM/DB/ipallocation
%define _clusterdbMan 		/usr/share/man/man8/
%define _clusterdbInst		%{_bcmprefix}/clusterdb/install/
%define _clusterdbInstEx	%{_bcmprefix}/clusterdb/install/example
###############################################################################
# Prepare the files to be compiled
%prep
rm -rf $RPM_BUILD_DIR/%{src_dir}
rm -rf $RPM_BUILD_ROOT
tar -xzf $RPM_SOURCE_DIR/%{src_tarall}

%setup -n %{src_dir}

%build

%install
install -d $RPM_BUILD_ROOT/%{_clusterdbLibIpalloc}
install -d $RPM_BUILD_ROOT/%{_clusterdbBinIpalloc}
install -d $RPM_BUILD_ROOT/%{_clusterdbMan}
install -d $RPM_BUILD_ROOT/%{_clusterdbInst}
install -d $RPM_BUILD_ROOT/%{_clusterdbInstEx}
install -d ${RPM_BUILD_ROOT}/%{_mandir}/man1

install -m 755 lib/apply_xml.py  			$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}
install -m 755 lib/clusterdb_sql_request.py  		$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}
install -m 755 lib/equipment.py  			$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}
install -m 755 lib/allocation_rule.py  			$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}
install -m 755 lib/rule.py  				$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}
install -m 755 lib/handle_xml_ipallocation.py  		$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}
install -m 755 lib/clusterdb_glue.py  			$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}
install -m 755 lib/ip_range.py  			$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}
install -m 755 lib/ipaddr.py  				$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}
install -m 755 lib/network_range.py  			$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}
install -m 755 lib/xml_element.py  			$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}
install -m 755 lib/clusterdb_sqlalchemy.py  		$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}
install -m 755 lib/error_code.py  			$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}
install -m 755 lib/cmd_line_argument.py  		$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}
install -m 755 lib/manage_db_instance.py  		$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}
install -m 755 lib/__init__.py  			$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}
install -m 755 lib/alloc_fetch_update.py  		$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}
install -m 755 lib/misc.py  				$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}
install -m 755 lib/operation_on_fetched_equipment.py  	$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}
install -m 755 lib/alloc_engine.py  			$RPM_BUILD_ROOT%{_clusterdbLibIpalloc}


install -m 755 	bin/ipallocation				$RPM_BUILD_ROOT%{_clusterdbBinIpalloc}

install -m 644  example/ip_allocation.dtd               $RPM_BUILD_ROOT%{_clusterdbInstEx}
install -m 644  example/README.ipallocation             $RPM_BUILD_ROOT%{_clusterdbInstEx}
install -m 644  example/allocation_sample_staging.xml   $RPM_BUILD_ROOT%{_clusterdbInstEx}
install -m 644  example/example_one_management_network.xml                      $RPM_BUILD_ROOT%{_clusterdbInstEx}
install -m 644  example/example_two_management_network.xml                      $RPM_BUILD_ROOT%{_clusterdbInstEx}
install -m 644  example/example_two_management_and_secondary_ib_network.xml     $RPM_BUILD_ROOT%{_clusterdbInstEx}
install -m 644  example/demo.xml                                                $RPM_BUILD_ROOT%{_clusterdbInstEx}
install -m 644  example/netmap-extend-demo-phase1.xml   $RPM_BUILD_ROOT%{_clusterdbInstEx}

install -m 644	man/ipallocation.html			$RPM_BUILD_ROOT%{_clusterdbInstEx}
install -m 644	man/ipallocation.1			$RPM_BUILD_ROOT%{_mandir}/man1/ipallocation.1

%files
%defattr(-,root,root)
/%{_clusterdbLibIpalloc}
/%{_clusterdbBinIpalloc}
/%{_clusterdbMan}
/%{_clusterdbInst}
%{_mandir}/man1/ipallocation.1.gz
#/usr/lib/python2.6/site-packages/CLM/DB/ipallocation/
#/usr/lib/clustmngt/clusterdb/bin/

%post
touch /%{_clusterdbLibIpalloc}/__init__.py

%clean
rm -rf $RPM_BUILD_ROOT

%changelog
- First release

