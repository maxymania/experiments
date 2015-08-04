/*
 * Copyright(C) 2015 Simon Schmidt
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2.
 */
#ifndef LSM_KMHOOKS_H
#define LSM_KMHOOKS_H

#include <linux/version.h>
#include <linux/lsm_audit.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,2,0)
/*
 * There is this nasty "security_hook_list" refactoring, that breaks the
 * backwarts-compatibility with older thirdparty lsm-modules.
 * It is introduced after Linux 4.1 (linux 4.2-rc)
 */

// The regular NEW Way

#include <linux/security.h>
#include <linux/lsm_hooks.h>

#define LSMHOOK(K,V) LSM_HOOK_INIT(K, V),
#define LSMHOOKS_START(VAR,NAME) struct security_hook_list VAR[] = {
#define LSMHOOKS_END };

#define LSMENABLE(VAR,NAME) security_module_enable(NAME)
#define LSMADDHOOKS(VAR,NAME) security_add_hooks(VAR, ARRAY_SIZE(VAR))

#else

// The regular OLD Way

#include <linux/security.h>

#define LSMHOOK(K,V) .K = V,
#define LSMHOOKS_START(VAR,NAME) struct security_operations VAR = { \
	.name = NAME,
#define LSMHOOKS_END };

#define LSMENABLE(VAR,NAME) security_module_enable(&VAR)
#define LSMADDHOOKS(VAR,NAME) if(register_security(&VAR))panic(NAME ": Unable to register with kernel.\n")

#endif // LINUX_VERSION_CODE >= KERNEL_VERSION(4,2,0)

#endif


