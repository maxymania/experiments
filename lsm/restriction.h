#ifndef __RESTRICTION_H__
#define __RESTRICTION_H__

#include <linux/types.h>

//typedef unsigned int Tflag;

#define D_SETUID       0x00000001
#define D_SETGID       0x00000002
#define D_SETNICE      0x00000004
#define D_SETSCHED     0x00000008

#define D_SHMCTL       0x00000010
#define D_SHMAT        0x00000020
#define D_SEMCTL       0x00000040
#define D_SEMOP        0x00000080

#define D_READ_OTHER   0x00000100
#define D_READ_GROUP   0x00000200
#define D_READ_USER    0x00000400
#define D_WRITE_OTHER  0x00000800
#define D_WRITE_GROUP  0x00001000
#define D_WRITE_USER   0x00002000
#define D_EXEC_OTHER   0x00004000
#define D_EXEC_GROUP   0x00008000
#define D_EXEC_USER    0x00010000

#define D_UNLINK       0x00020000
#define D_RMDIR        0x00040000
#define D_RENAME       0x00080000

#define D_MSGCTL       0x00100000
#define D_MSGSND       0x00200000
#define D_MSGRCV       0x00400000

#define D_SETIOPRIO    0x00800000
#define D_SETRLIMIT    0x01000000
#define D_MOVEMEMORY   0x02000000

#define D_SETIOPRIO    0x00800000
#define D_SETRLIMIT    0x01000000
#define D_MOVEMEMORY   0x02000000

struct proc_cap {
	uint32_t deny;
};



#endif



