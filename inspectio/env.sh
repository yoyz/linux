export    INSPECTIO_PATH=$HOME/lib/inspectio				# 
export    INSPECTIO_DUMP=$PWD/LOG.${SLURM_JOBID}                        # ou ou dump les log
mkdir -p $INSPECTIO_DUMP						# on cree le repertoire
export    LD_PRELOAD=$INSPECTIO_PATH/inspectio.so:$LD_PRELOAD		# on dit a inspectio ou il va dump
unset     INSPECTIO_ALL                                                	# on desactive le plus gros du log debug

grep_inspectio_log()
{
	grep ^ LOG.${SLURM_JOBID}/*|grep -v nompi
}
