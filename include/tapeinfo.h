extern ULONG MT_stat;

#ifdef VMS

#define	MT$M_SEREXCP	1
#define	MT$M_ENSEREXCP	4
#define	MT$M_PARITY	8
#define	MT$M_FORMAT	240
#define	MT$M_DENSITY	7936
#define	MT$M_LOGSOFT	16384
#define	MT$M_LOGSOFTOG	32768
#define	MT$M_BOT	65536
#define	MT$M_EOF	131072
#define	MT$M_EOT	262144
#define	MT$M_HWL	524288
#define	MT$M_LOST	1048576
#define	MT$M_SUP_NRZI	2097152
#define	MT$M_SUP_PE	4194304
#define	MT$M_SUP_GCR	8388608
#define	MT$M_SPEED	-16777216
#define	MT$K_DEFAULT	0
#define	MT$K_NORMAL11	12
#define	MT$K_CORDMP11	13
#define	MT$K_NORMAL15	14
#define	MT$K_NRZI_800	3
#define	MT$K_PE_1600	4
#define	MT$K_GCR_6250	5
#define	MT$K_BLK_833	17
#define	MT$M_MSCP_NRZI_800	1
#define	MT$M_MSCP_PE_1600	2
#define	MT$M_MSCP_GCR_6250	4
#define	MT$M_MSCP_BLK_833	8
#define	MT$K_SPEED_DEF	0
#define	MT$K_SPEED_25	25
#define	MT$K_SPEED_75	75
#define	MT$S_MTDEF	4
#define	MT$V_SEREXCP	0
#define	MT$V_ENSEREXCP	2
#define	MT$V_PARITY	3
#define	MT$S_FORMAT	4
#define	MT$V_FORMAT	4
#define	MT$S_DENSITY	5
#define	MT$V_DENSITY	8
#define	MT$V_LOGSOFT	14
#define	MT$V_LOGSOFTOG	15
#define	MT$V_BOT	16
#define	MT$V_EOF	17
#define	MT$V_EOT	18
#define	MT$V_HWL	19
#define	MT$V_LOST	20
#define	MT$V_SUP_NRZI	21
#define	MT$V_SUP_PE	22
#define	MT$V_SUP_GCR	23
#define	MT$S_SPEED	8
#define	MT$V_SPEED	24
#define	MT$V_MSCP_NRZI_800	0
#define	MT$V_MSCP_PE_1600	1
#define	MT$V_MSCP_GCR_6250	2
#define	MT$V_MSCP_BLK_833	3

#else


#endif

#ifdef VMS

#define TAPE_EOT	MT$M_EOT

#else

#define TAPE_EOT 	0

#endif
