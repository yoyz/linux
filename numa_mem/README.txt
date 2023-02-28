make memset
numactl --membind=0 ./memset
[root@dell-r740-001 ~]# /tmp/numastat.pl < /proc/$(pidof memset)/numa_maps
N0        :         2582 (  0.01 GB)
N1        :          281 (  0.00 GB)
active    :            1 (  0.00 GB)
anon      :         2581 (  0.01 GB)
dirty     :         2582 (  0.01 GB)
kernelpagesize_kB:           60 (  0.00 GB)
mapmax    :          385 (  0.00 GB)
mapped    :          281 (  0.00 GB)


numactl --membind=1 ./memset
[root@dell-r740-001 ~]# /tmp/numastat.pl < /proc/$(pidof memset)/numa_maps
N0        :            1 (  0.00 GB)
N1        :         2864 (  0.01 GB)
active    :            1 (  0.00 GB)
anon      :         2582 (  0.01 GB)
dirty     :         2582 (  0.01 GB)
kernelpagesize_kB:           60 (  0.00 GB)
mapmax    :          387 (  0.00 GB)
mapped    :          283 (  0.00 GB)

