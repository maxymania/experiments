#ifndef SHARED_INFORMATION_H
#define SHARED_INFORMATION_H

#define ROCK_LSM_PRCTL_MAGIC 0x123000


// arg2 of prctl
#define ROCK2_DENY_OTHER     1
#define ROCK2_DENY_GROUP     2
#define ROCK2_DENY_USER      3

#define ROCK2_DENY_SETUID    4
#define ROCK2_DENY_SETGID    5
#define ROCK2_DENY_SETNICE   6
#define ROCK2_DENY_SETSCHED  7
#define ROCK2_DENY_SHMCTL    8
#define ROCK2_DENY_SHMAT     9
#define ROCK2_DENY_SEMCTL    10
#define ROCK2_DENY_SEMOP     11
#define ROCK2_DENY_UNLINK    12
#define ROCK2_DENY_RMDIR     13
#define ROCK2_DENY_RENAME    14
#define ROCK2_DENY_MSGCTL    15
#define ROCK2_DENY_MSGSND    16
#define ROCK2_DENY_MSGRCV    17
#define ROCK2_DENY_SETIOPRIO   18
#define ROCK2_DENY_SETRLIMIT   19
#define ROCK2_DENY_MOVEMEMORY  20



// arg3 of prctl
// use with ROCK2_DENY_{OTHER,GROUP,USER}
#define ROCK3_READ  1
#define ROCK3_WRITE 2
#define ROCK3_EXEC  3

#endif

