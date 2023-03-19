
# pyzkiln_core Documentation

## pyzkiln_core.zcmds

Return the output from a set of z/os operator commands in an array of Python dictionaries.
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
- fsutils:
  - fsutils.aggr_fsinfo(*fs_name*) : Display detailed information about a zFS file system
    - Parameters:
      - fs_name (str): FS aggregate name
    - Returns:
      - Python dictionary corresponds to zfsadm fsinfo -aggregate omvs command
    - Doc: <https://www.ibm.com/docs/en/zos/2.4.0?topic=commands-zfsadm-fsinfo>
  - fsutils.allocate_zfs(*fs_name, volume, fs_size, extended_size*) : Create a zFS Aggregate and File System
    - Parameters:
      - fs_name (str): aggregate name for new zFS file system
      - volume (str): size in **megabytes** for the VSAM linear data set
      - fs_size (int): secondary size in megabytesfor VSAM data set
    - extended_size (int): volume the VSAM linear data set can have space
    - Returns:
      - Resulting return code of zfsadm define command
      - -2 = invalid fs_size or extended_size parameter
    - Note: The issuer of the zfsadm define command requires sufficient authority to create the VSAM linear data set
    - Doc: <https://www.ibm.com/docs/en/zos/2.4.0?topic=commands-zfsadm-define>
  - fsutils.allocate_zfs_by_datatype(*fs_name, fs_size, extended_size, volume, data_type*) : Create a zFS Aggregate and File System by a specified data type
    - Parameters:
      - fs_name (str): aggregate name for new zFS file system
      - fs_size (int): secondary size in megabytesfor VSAM data set
      - extended_size (int): volume the VSAM linear data set can have space. Optional, default = 0
      - volume (str): size in megabytes for the VSAM linear data set. Optional, default = no volume specified
      - data_type (str): data type for defining the size of the ds. Can be one of: cylinders, kilobytes, megabytes,records, tracks. Optional, default = megabytes
    - Returns:
      - Resulting return code of zfsadm define command
      - 1 = allocation of data set failed
      - -1 = invalid data_type parameter
      - -2 = invalid fs_size or extended_size parameter
    - Note: The issuer of the zfsadm define command requires sufficient authority to create the VSAM linear data set
    - Doc: <https://www.ibm.com/docs/en/zos/2.4.0?topic=commands-zfsadm-define>
  - fsutils.bpxwmigf(*source_zfs_name, target_zfs_name, source_zfs_renamed*) : Issue bpxwmigf command to migrate one ZFS to another
    - Parameters:
      - source_zfs_name (str): name of the ZFS currently in use that needs changed
      - target_zfs_name (str): name of the ZFS to copy the contents of source_zfs_name into
      - source_zfs_renamed (str): the new name of source_zfs_name after the swap
    - Returns:
      - Resulting output of bpxwmigf command
    - Note:
      - This command requires **superuser** authority. It will also rename target_zfs_name to source_zfs_name automatically and complete the content swap for you. Naming changes will be transparent to the user.
    - Doc: <https://www.ibm.com/docs/en/zos/2.4.0?topic=SSLTBW_2.4.0/com.ibm.zos.v2r4.bpxa500/shell_bpxwmigf.htm>
  - fsutils.delete_zfs(*fs_name*) : Delete a ZFS via ZOAU
    - Parameters:
      - zfs_name (str): ZFS to be deleted
    - Returns:
      - Result of ZOAU datasets.delete() command
      - 1 = delete failed
      - 0 = delete successful
    - Note:
      - **This function relies on Z Open Automation Utilities (ZOAU)**
    - Doc: <https://www.ibm.com/docs/en/zoau/1.1.1?topic=apis-delete-data-sets>
  - fsutils.display_filesystem_info(*path_name*) : Issue df -kvP command on the system to get relevant filesystem info
    - Parameters:
      - path_name (str): file name or directory to get information for
    - Returns:
      - Python dictionary containing results of the df -kvP omvs command
    - Doc: <https://www.ibm.com/docs/en/zos/2.4.0?topic=scd-df-display-amount-free-space-in-file-system>
  - fsutils.get_sysname() : Issue "sysvar SYSNAME" to get the name of the current system
    - Returns:
      - Resulting output of "sysvar SYSNAME" command
    - Doc:
            <https://www.ibm.com/docs/en/zos/2.4.0?topic=descriptions-sysvar-display-static-system-symbols>
  - fsutils.mount_fs(*fs_name, mountpath*) : Pass zFS argregate name and mountpoint to mount the file system
    - Parameters:
      - fs_name (str): zFS aggregate name
      - mountpath (str): path to mount zFS
    - Returns:
      - Resulting return code of mount command
  - fsutils.unmount_fs(*fs_name*): Pass zFS mountpoint to unmount the file system  
    - Parameters:
      - fs_name (str): path to mounted zFS
    - Returns:
      - Resulting return code of mount command
- findActiveJobs.displayActiveJobs(*searchFor, searchKey*) : list all active jobs that meet the criteria seachFor in each dictionary with the searchKey
  - Input: searchFor(str), searchKey (str) (Optional)
  - Output: joblist (list of dictionaries)
- zfsadm.grow_zfs(*source_zfs_name, mounted_on, grow_or_shrink, percentage*) : Grow a ZFS Filesystem by a specified percentage
  - Parameters:
    - source_zfs_name (str): ZFS to be grown
    - mounted_on (str): USS directory the ZFS is currently mounted on
    - grow_or_shrink (str): whether to grow or shrink the ZFS (values: grow, shrink)
    - percentage (int): % for the ZFS to be grown by
  - Returns:
    - -1 = Error in processing, please refer to the log for specifics
    - 0 = Success
  - Note:
    - This command requires **superuser** authority
    - This command relies on Z Open Automation Utilities (ZOAU)
- displayActiveJobs(*searchFor,searchKey*) : list all active jobs that meet the criteria seachFor in each dictionary with the key searchKey
  - Input: searchFor(str), searchKey (str) (Optional)
  - Output: joblist (list of Dictionaries)

- runrexx(*authorized, library,program_info, inputdata, outputinfo*): REXX code in a TSO address space
  - Parameters:
    - authorized (bool): A flag to determine if the code run authorized
    - library (str): library that the REXX code is in
    - program_info (str): A string containing the program and its arguments
    - inputdata (list): a list of strings containing input lines without newlines
    - outputinfo (dict): A dictionary containin - "DDName", "Filename", "Dataset"
  - Returns: structure with following keys:
    - returninfo: return code, return messages, and stdout and stderr data
    - systsprtfile: filename that the SYSTSPRT DD pointed to
    - sysprtfile: filename that the SYSPRINT DD pointed to

- create_sysin(inputdata,filename,codepage): Build a file to provide to sysin (or any other input DD)
  - Parameters:
    - inputdata (list): A list of strings that will be separate cards into the file. Each list item is a card.
    - filename (string): The name of a file that will be created with the input from inputdata.
    - codepage (string): The codepage that the text will be written in. The file must have EBCDIC input for the cards to be read by the program. If not specified it will default to cp1047
  - Returns:
        Does not return any data. It creates the file named filename with the input defined in inputdata using the codepage codepage

## py_racf

A Python interface to the library of services provided by RACF.
More documentation (<https://github.com/ambitus/pyzkiln/tree/main/pyzkiln_core/pyracf>)
