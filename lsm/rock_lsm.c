#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros


//additional
#include <linux/binfmts.h>

#include "kmhooks.h"
#include "restriction.h"
#include "sharedinf.h"


// header-only mini libraries
#include "rights_util.h"


/*
 *	Check permission before performing a process control operation on the
 *	current process.
 *	@option contains the operation.
 *	@arg2 contains a argument.
 *	@arg3 contains a argument.
 *	@arg4 contains a argument.
 *	@arg5 contains a argument.
 *	Return -ENOSYS if no-one wanted to handle this op, any other value to
 *	cause prctl() to return immediately with that value.
 */
int rock_task_prctl (
	int option,
	unsigned long arg2,
	unsigned long arg3,
	unsigned long arg4,
	unsigned long arg5 )
{
	struct proc_cap* rockpc = current_security();
	if(option==ROCK_LSM_PRCTL_MAGIC){
		switch(arg2){
		case ROCK2_DENY_OTHER:
			switch(arg3){
			case ROCK3_READ: rockpc->deny |= D_READ_OTHER;    break;
			case ROCK3_WRITE: rockpc->deny |= D_WRITE_OTHER;  break;
			case ROCK3_EXEC: rockpc->deny |= D_EXEC_OTHER;    break;
			} break;
		case ROCK2_DENY_GROUP:
			switch(arg3){
			case ROCK3_READ: rockpc->deny |= D_READ_GROUP;    break;
			case ROCK3_WRITE: rockpc->deny |= D_WRITE_GROUP;  break;
			case ROCK3_EXEC: rockpc->deny |= D_EXEC_GROUP;    break;
			} break;
		case ROCK2_DENY_USER:
			switch(arg3){
			case ROCK3_READ: rockpc->deny |= D_READ_USER;     break;
			case ROCK3_WRITE: rockpc->deny |= D_WRITE_USER;   break;
			case ROCK3_EXEC: rockpc->deny |= D_EXEC_USER;     break;
			} break;
		case ROCK2_DENY_SETUID:   rockpc->deny |= D_SETUID;   break;
		case ROCK2_DENY_SETGID:   rockpc->deny |= D_SETGID;   break;
		case ROCK2_DENY_SETNICE:  rockpc->deny |= D_SETNICE;  break;
		case ROCK2_DENY_SETSCHED: rockpc->deny |= D_SETSCHED; break;
		case ROCK2_DENY_SHMCTL:   rockpc->deny |= D_SHMCTL;   break;
		case ROCK2_DENY_SHMAT:    rockpc->deny |= D_SHMAT;    break;
		case ROCK2_DENY_SEMCTL:   rockpc->deny |= D_SEMCTL;   break;
		case ROCK2_DENY_SEMOP:    rockpc->deny |= D_SEMOP;    break;
		case ROCK2_DENY_UNLINK:   rockpc->deny |= D_UNLINK;   break;
		case ROCK2_DENY_RMDIR:    rockpc->deny |= D_RMDIR;    break;
		case ROCK2_DENY_RENAME:   rockpc->deny |= D_RENAME;   break;
		case ROCK2_DENY_MSGCTL:   rockpc->deny |= D_MSGCTL;   break;
		case ROCK2_DENY_MSGSND:   rockpc->deny |= D_MSGSND;   break;
		case ROCK2_DENY_MSGRCV:   rockpc->deny |= D_MSGRCV;   break;
		case ROCK2_DENY_SETIOPRIO:   rockpc->deny |= D_SETIOPRIO;   break;
		case ROCK2_DENY_SETRLIMIT:   rockpc->deny |= D_SETRLIMIT;   break;
		case ROCK2_DENY_MOVEMEMORY:  rockpc->deny |= D_MOVEMEMORY;  break;
		}
		return -ENOSYS;
	}
	return 0;
}

/*
 * @cred_alloc_blank:
 *	@cred points to the credentials.
 *	@gfp indicates the atomicity of any memory allocations.
 *	Only allocate sufficient memory and attach to @cred such that
 *	cred_transfer() will not get ENOMEM.
 */
int rock_cred_alloc_blank (struct cred *cred, gfp_t gfp){
	struct proc_cap* rockpc = kzalloc(sizeof(struct proc_cap), gfp);
	if (rockpc == NULL)
		return -ENOMEM;
	cred -> security = rockpc;
	return 0;
}

/*
 * @cred_free:
 *	@cred points to the credentials.
 *	Deallocate and clear the cred->security field in a set of credentials.
 */
void rock_cred_free (struct cred *cred){
	kfree(cred->security);
	cred->security = 0;
}

/*
 * @cred_prepare:
 *	@new points to the new credentials.
 *	@old points to the original credentials.
 *	@gfp indicates the atomicity of any memory allocations.
 *	Prepare a new set of credentials by copying the data from the old set.
 */
int rock_cred_prepare (struct cred *new, const struct cred *old,gfp_t gfp){
	struct proc_cap* oldrockpc = old->security;
	struct proc_cap* rockpc = kzalloc(sizeof(struct proc_cap), gfp);
	if (rockpc == NULL)
		return -ENOMEM;
	*rockpc = *oldrockpc;
	new -> security = rockpc;
	return 0;

}
/*
 * @cred_transfer:
 *	@new points to the new credentials.
 *	@old points to the original credentials.
 *	Transfer data from original creds to new creds
 */
void rock_cred_transfer (struct cred *new, const struct cred *old){
	struct proc_cap* oldrockpc = old->security;
	struct proc_cap* newrockpc = new->security;
	*newrockpc = *oldrockpc;
}

//int rock_task_fix_setuid (struct cred *new, const struct cred *old,int flags) {}
int rock_task_setpgid (struct task_struct *p, pid_t pgid) {
	struct proc_cap* rockpc = current_security();
	if(rockpc->deny&D_SETGID)return -EPERM;
	return 0;
}
int rock_task_setnice (struct task_struct *p, int nice) {
	struct proc_cap* rockpc = current_security();
	if(rockpc->deny&D_SETNICE)return -EPERM;
	return 0;
}
int rock_task_setioprio (struct task_struct *p, int ioprio) {
	struct proc_cap* rockpc = current_security();
	if(rockpc->deny&D_SETIOPRIO)return -EPERM;
	return 0;
}
int rock_task_setrlimit (struct task_struct *p, unsigned int resource, struct rlimit *new_rlim) {
	struct proc_cap* rockpc = current_security();
	if(rockpc->deny&D_SETRLIMIT)return -EPERM;
	return 0;
}
int rock_task_setscheduler (struct task_struct *p) {
	struct proc_cap* rockpc = current_security();
	if(rockpc->deny&D_SETSCHED)return -EPERM;
	return 0;
}
int rock_task_movememory (struct task_struct *p) {
	struct proc_cap* rockpc = current_security();
	if(rockpc->deny&D_MOVEMEMORY)return -EPERM;
	return 0;
}

int rock_file_open (struct file *file, const struct cred *cred){
	fmode_t m;
	struct proc_cap* rockpc = current_security();
	const u8 acc_mode = ACC_MODE(file->f_flags);
	uid_t uids[2] = {file->f_cred->fsuid.val,current_fsuid().val};
	gid_t gids[2] = {file->f_cred->fsgid.val,current_fsgid().val};
	m = file->f_mode;
	m &= perm_mask_credentials(uids,gids);
	m &= perm_mask_rock(rockpc->deny);
	return perm_check_RW(acc_mode,m);
}

int rock_shm_shmctl (struct shmid_kernel *shp, int cmd)
{
	struct proc_cap* rockpc = current_security();
	if(rockpc->deny&D_SHMCTL)return -EPERM;
	return 0;
}

int rock_shm_shmat (struct shmid_kernel *shp,
	char __user *shmaddr, int shmflg)
{
	struct proc_cap* rockpc = current_security();
	if(rockpc->deny&D_SHMAT)return -EPERM;
	return 0;
}

int rock_bprm_check_security (struct linux_binprm *bprm){
	fmode_t m;
	struct proc_cap* rockpc = current_security();
	uid_t uids[2] = {bprm->file->f_cred->fsuid.val,current_fsuid().val};
	gid_t gids[2] = {bprm->file->f_cred->fsgid.val,current_fsgid().val};
	m = bprm->file->f_mode;
	m &= perm_mask_credentials(uids,gids);
	m &= perm_mask_rock(rockpc->deny);
	return perm_check_exec(m);
}


LSMHOOKS_START(hardy_ops,"hardy")
/* cred */
	LSMHOOK(cred_alloc_blank,rock_cred_alloc_blank)
	LSMHOOK(cred_free,rock_cred_free)
	LSMHOOK(cred_prepare,rock_cred_prepare)
	LSMHOOK(cred_transfer,rock_cred_transfer)

/* task */
	LSMHOOK(task_prctl,rock_task_prctl)
	LSMHOOK(task_setpgid,rock_task_setpgid)
	LSMHOOK(task_setnice,rock_task_setnice)
	LSMHOOK(task_setioprio,rock_task_setioprio)
	LSMHOOK(task_setrlimit,rock_task_setrlimit)
	LSMHOOK(task_setscheduler,rock_task_setscheduler)
	LSMHOOK(task_movememory,rock_task_movememory)

/* file */
	LSMHOOK(file_open,rock_file_open)

/* file */
	LSMHOOK(shm_shmctl,rock_shm_shmctl)
	LSMHOOK(shm_shmat,rock_shm_shmat)
	LSMHOOK(bprm_check_security,rock_bprm_check_security)

/*  */
LSMHOOKS_END


/**
 * rock_init - initialize the rock system
 *
 * Returns 0
 */
static __init int rock_lsm_init(void)
{
	if (LSMENABLE(hardy_ops,"rock"))
		return 0;
	//rock_cred_alloc_blank(get_current_cred(),GFP_KERNEL);
	LSMADDHOOKS(hardy_ops,"rock");
	return 0;
}

/*
static int __init rock_init(void)
{
    printk(KERN_INFO "Module Loaded!\n");
    return 0;
}

static void __exit rock_cleanup(void)
{
    printk(KERN_INFO "Module Unloaded!\n");
}*/

security_initcall(rock_lsm_init);
//module_init(rock_init);
//module_exit(rock_cleanup);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Simon Schmidt");
MODULE_DESCRIPTION("A Linux Security Module");
