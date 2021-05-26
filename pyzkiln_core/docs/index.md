# pyzkiln_core Documentation 

## pyzkiln_core.zcmds

Return the output from a set of z/os operator commands in an array of python dictionaries.
Dictionary keys match the columns that would normally appear in response to the operator command. 

Operator commands included:

- displayActiveJobs.displayActiveJobs() : The system is to display detailed information for active jobs and started tasks
    - Output: list of dictionaries with response to DISPLAY J,LIST
- getParmlib.getParmlib() : Display the specifications in the active IKJTSOxx member of SYS1.PARMLIB 
    - Output: list of dictionaries with response to Display IKJTSO,ALL
- getASM.getAsm() : Displaying auxiliary storage information
    - Output: list of dictionaries response to Display ASM, ALL




## pyzkiln_core.zutils
- joblist.listJobs(*user_id*) : list all jobs from a given user.  
    - Input (Optional): userID (str) 
- mountFS: 
    -  mountFS.allocateZFS(*fs_name, volume, fs_size, extended_size*) : Create a zFS Aggregate
        - Input: fs_name (str), volume (str), fs_size (int), extended_size (int)
        - Note: size is specified in megabytes
    -  mountFS.mountFS(*fs_name, mountpath*) : Mount a ZFS file system 
        - Input: fs_name (str), mountpath (str)
    -  mountFS.unmountFS(*fs_name*) :  Unmount a ZFS file system  
        - Input: fs_name (str)
    -  mountFS.aggr_fsinfo(*fs_name*) : Displays detailed information about a zFS file system
        - Input: fs_name: (str)
        - Output: python dictionary corresponds to zfsadm aggrinfo -aggregate omvs command 
- findActiveJobs.displayActiveJobs(searchFor,searchKey) : list all active jobs that meet the criteria seachFor in each dictionary with the searchKey
    - Input: searchFor(str), searchKey (str) (Optional)
    - Output: joblist (list of dictionaries)
