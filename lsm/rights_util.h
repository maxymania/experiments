
static fmode_t perm_mask_credentials(const uid_t *uids,const gid_t *gids){
	return
		((uids[0]==uids[1])?0700:0) |
		((gids[0]==gids[1])?070:0) | 7;
}
static fmode_t perm_mask_rock(const uint32_t deny){
	return
		((deny&D_READ_USER )?0:0400) |
		((deny&D_WRITE_USER)?0:0200) |
		((deny&D_EXEC_USER )?0:0100) |
		((deny&D_READ_GROUP )?0:040) |
		((deny&D_WRITE_GROUP)?0:020) |
		((deny&D_EXEC_GROUP )?0:010) |
		((deny&D_READ_OTHER )?0:4) |
		((deny&D_WRITE_OTHER)?0:2) |
		((deny&D_EXEC_OTHER )?0:1) ;
}
static int perm_check_RW(const u8 acc_mode,fmode_t mode){
	if((acc_mode & MAY_READ)&&!(mode&0444))return -EPERM;
	if((acc_mode & MAY_WRITE)&&!(mode&0222))return -EPERM;
	return 0;
}

static int perm_check_exec(fmode_t mode){
	if(mode&0111)return -EPERM;
	return 0;
}


