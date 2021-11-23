#!/usr/bin/env python3
"""A group of functions for ZFS operations such as creating mounting.

Operations include:
- creating a ZFS aggregate
- mounting a ZFS
- unmounting a ZFS
- gathering information about a ZFS
- deleteing a ZFS

"""
import configparser
import logging
import os
import re
import subprocess
import time
from importlib import util as importlib

from zoautil_py import datasets

# Setup ConfigParser & read properties file
config = configparser.RawConfigParser()
config.read(os.path.join(os.path.dirname(__file__), 'ConfigFile.properties'))

# Setup logger
logging.basicConfig(level=config.get('LoggingSection', 'log.level'),
                    format=' %(asctime)s | %(levelname)-8s | %(message)s',
                    datefmt='%m/%d/%Y %H:%M')

# Print logger header & starting message
if config.get('LoggingSection', 'log.show_header') == 'True':
    print(" time             | level    | message")
    print("------------------+----------+------------------------------------")
logging.info(f"Starting {os.path.basename(__file__)}")

# Check if this is a dry-run or not and set a flag
dry_run_flag = False
if config.get('RunSection', 'run.dryrun').lower() == 'true':
    dry_run_flag = True

# Check that the zoautil_py module is installed
zoautil = importlib.find_spec("zoautil_py")
if not zoautil:
    logging.warn("Python package zoautil_py does not appear to be installed on your system.")
    logging.warn("Please note that while you can still use this code, some functions may not work.")
    logging.warn("Currently, the following functions rely on zoautil_py: delete_zfs")

def aggr_fsinfo(fs_name: str):
    """
    Display detailed information about a zFS file system.

        Parameters:
            fs_name (str): FS aggregate name
        Returns:
            Python dictionary corresponds to zfsadm fsinfo -aggregate omvs command
        Doc:
            https://www.ibm.com/docs/en/zos/2.4.0?topic=commands-zfsadm-fsinfo
    """
    fsinfo_results_dic = {}

    fsinfo_cmd = subprocess.run(
        ["zfsadm", "fsinfo", "-aggregate", fs_name], capture_output=True
    )

    if fsinfo_cmd.returncode != 0:
        logging.error(f"RC={fsinfo_cmd.returncode}")
        logging.error("The zfsadm command's return code was non-zero indicating an error.")
        logging.error("Please refer to the output below for an explanation of the error.")
        fsinfo_cmd = str(fsinfo_cmd.stderr)
        logging.error(fsinfo_cmd)
        return fsinfo_cmd

    fsinfo_cmd = str(fsinfo_cmd.stdout)
    fsinfo = fsinfo_cmd.strip().split("\\n")
    # Iterate over each line of the output to make adjustments to
    # turn into a Python dictionary
    for linew in fsinfo[3:]:
        linew = re.split(r": |\s\s\s", linew)

        # remove empty entries in the list
        while "" in linew:
            linew.remove("")

        # add to Python dictionary
        fsinfo_results_dic.update(dict(zip(linew[::2], linew[1::2])))

    return fsinfo_results_dic


def allocate_zfs(fs_name: str, volume: str, fs_size: int, extended_size: int):
    """
    Create a zFS Aggregate and File System.

        Parameters:
                  fs_name (str): aggregate name for new zFS file system
                   volume (str): size in megabytes for the VSAM linear data set
                  fs_size (int): secondary size in megabytesfor VSAM data set
            extended_size (int): volume the VSAM linear data set can have space
        Returns:
            Resulting return code of zfsadm define command
            -2 = invalid fs_size or extended_size parameter
        Note: The issuer of the zfsadm define command requires sufficient
              authority to create the VSAM linear data set
        Doc:
            https://www.ibm.com/docs/en/zos/2.4.0?topic=commands-zfsadm-define
    """
    if fs_size <= 0 or extended_size < 0:
        return -2

    if dry_run_flag:
        logging.info(f"DRY RUN COMMAND: zfsadm define -aggregate -volumes {volume}" +
            f" -megabytes {fs_size} {extended_size}")
        return 0
    else:
        alloc_cmd = subprocess.run(
            [
                "zfsadm",
                "define",
                "-aggregate",
                fs_name,
                "-volumes",
                volume,
                "-megabytes",
                str(fs_size),
                str(extended_size),
            ]
        )
        return alloc_cmd.returncode


def allocate_zfs_by_datatype(fs_name: str, fs_size: int, extended_size=0,
                             volume='default', data_type='default'):
    """
    Create a zFS Aggregate and File System by a specified data type.

        Parameters:
                  fs_name (str): aggregate name for new zFS file system
                  fs_size (int): secondary size in megabytesfor VSAM data set
            extended_size (int): volume the VSAM linear data set can have space
                                 Optional, default = 0
                   volume (str): size in megabytes for the VSAM linear data set
                                 Optional, default = no volume specified
                data_type (str): data type for defining the size of the ds
                                 can be one of: cylinders, kilobytes, megabytes,
                                 records, tracks
                                 Optional, default = megabytes
        Returns:
            Resulting return code of zfsadm define command
            1 = allocation of data set failed
            -1 = invalid data_type parameter
            -2 = invalid fs_size or extended_size parameter
        Note: The issuer of the zfsadm define command requires sufficient
              authority to create the VSAM linear data set
        Doc:
            https://www.ibm.com/docs/en/zos/2.4.0?topic=commands-zfsadm-define
    """
    cmd_list = ['zfsadm', 'define', '-aggregate', fs_name]

    if volume != 'default':
        cmd_list.append('-volumes')
        cmd_list.append(volume)

    data_type = data_type.lower()
    data_type_allowed = ['cylinders', 'kilobytes', 'megabytes', 'records', 'tracks']

    if data_type in data_type_allowed:
        cmd_list.append('-' + data_type)
    elif data_type == 'default':
        cmd_list.append('-megabytes')
    else:
        logging.error(f"Invalid value {data_type} for parameter data_type passed to zfsadm define command")
        logging.error(f"Expected one of {data_type_allowed}")
        return -1

    if fs_size > 0:
        cmd_list.append(str(fs_size))
    else:
        return -2

    if extended_size > 0:
        cmd_list.append(str(extended_size))
    elif extended_size < 0:
        return -2

    logging.debug(cmd_list)

    if dry_run_flag:
        cmd_str = ''
        for cmd in cmd_list:
            cmd_str = cmd_str + str(cmd) + ' '
        logging.info(f"DRY RUN COMMAND: {cmd_str}")
        return 0
    else:
        alloc_cmd = subprocess.run(cmd_list)
        return alloc_cmd.returncode


def bpxwmigf(source_zfs_name: str, target_zfs_name: str, source_zfs_renamed: str):
    """
    Issue bpxwmigf command to migrate one ZFS to another.

        Parameters:
               source_zfs_name (str): name of the ZFS currently in use that needs changed
               target_zfs_name (str): name of the ZFS to copy the contents of source_zfs_name into
            source_zfs_renamed (str): the new name of source_zfs_name after the swap
        Returns:
            Resulting output of bpxwmigf command
        Note:
            This command requires superuser authority.
            It will also rename target_zfs_name to source_zfs_name automatically and
            complete the content swap for you. Naming changes will be transparent to the user.
        Doc:
            https://www.ibm.com/docs/en/zos/2.4.0?topic=SSLTBW_2.4.0/com.ibm.zos.v2r4.bpxa500/shell_bpxwmigf.htm
    """
    logging.info("bpxwmigf will run as follows: ")
    logging.info(f"bpxwmigf -source {source_zfs_name} -target {target_zfs_name} " +
          f"-srename {source_zfs_renamed} -trename {source_zfs_name} -swap")

    if dry_run_flag:
        logging.info("DRY RUN COMMAND: see above")
        return 0
    else:
        bpxwmigf_cmd = subprocess.run(
            [
                "bpxwmigf",
                "-source",
                source_zfs_name,
                "-target",
                target_zfs_name,
                "-srename",
                source_zfs_renamed,
                "-trename",
                source_zfs_name,
                "-swap"
            ],
            capture_output=True
        )

        time.sleep(int(config.get('TimingSection', 'time.bpxwmigf_sleep')))
        if bpxwmigf_cmd.returncode != 0:
            logging.error(f"RC={bpxwmigf_cmd.returncode}")
            logging.error("The bpxwmigf command's return code was non-zero indicating an error.")
            logging.error("Please refer to the output below for an explanation of the error.")
        bpxwmigf_cmd = str(bpxwmigf_cmd.stdout)
        return bpxwmigf_cmd


def delete_zfs(fs_name: str):
    """
    Delete a ZFS via ZOAU.

        Parameters:
            zfs_name (str): ZFS to be deleted
        Returns:
            Result of ZOAU datasets.delete() command
            1 = delete failed
            0 = delete successful
        Note:
            This function relies on Z Open Automation Utilities (ZOAU)
        Doc:
            https://www.ibm.com/docs/en/zoau/1.1.1?topic=apis-delete-data-sets
    """
    if dry_run_flag:
        logging.info(f"DRY RUN COMMAND: deleteing {fs_name} via ZOAU")
        return 0
    else:
        return datasets.delete(fs_name)


def display_filesystem_info(path_name: str):
    """
    Issue df -kvP command on the system to get relevant filesystem info.

        Parameters:
            path_name (str): file name or directory to get information for
        Returns:
            Python dictionary containing results of the df -kvP omvs command
        Doc:
            https://www.ibm.com/docs/en/zos/2.4.0?topic=scd-df-display-amount-free-space-in-file-system
    """
    fsinfo_results_dic = {}

    fsinfo_cmd = subprocess.run(
        ["df", "-kvP", fs_name], capture_output=True
    )

    if fsinfo_cmd.returncode != 0:
        logging.error(f"RC={fsinfo_cmd.returncode}")
        logging.error("The df -kvP command's return code was non-zero indicating an error.")
        logging.error("Please refer to the output below for an explanation of the error.")
        fsinfo_cmd = str(fsinfo_cmd.stderr)
        logging.error(fsinfo_cmd)
        return fsinfo_cmd

    fsinfo_cmd = str(fsinfo_cmd.stdout)
    fsinfo = fsinfo_cmd.strip().split("\\n")
    # Iterate over each line of the output to make adjustments to
    # turn into a Python dictionary
    for linew in fsinfo[3:]:
        linew = re.split(r": |\s\s\s", linew)

        # remove empty entries in the list
        while "" in linew:
            linew.remove("")

        # add to Python dictionary
        fsinfo_results_dic.update(dict(zip(linew[::2], linew[1::2])))

    return fsinfo_results_dic


def get_sysname():
    """
    Issue "sysvar SYSNAME" to get the name of the current system.

        Returns:
            Resulting output of "sysvar SYSNAME" command
        Doc:
            https://www.ibm.com/docs/en/zos/2.4.0?topic=descriptions-sysvar-display-static-system-symbols
    """
    fsinfo_cmd = subprocess.run(
        ["sysvar", "SYSNAME"], capture_output=True
    )

    if fsinfo_cmd.returncode != 0:
        logging.error(f"RC={fsinfo_cmd.returncode}")
        logging.error("The sysvar SYSNAME command's return code was non-zero indicating an error.")
        logging.error("Please refer to the output below for an explanation of the error.")
        fsinfo_cmd = str(fsinfo_cmd.stderr)
        logging.error(fsinfo_cmd)
        return fsinfo_cmd

    fsinfo_cmd = str(fsinfo_cmd.stdout)
    return fsinfo_cmd


def mount_fs(fs_name: str, mountpath: str):
    """
    Pass zFS argregate name and mountpoint to mount the file system.

        Parameters:
              fs_name (str): zFS aggregate name
            mountpath (str): path to mount zFS
        Returns:
            Resulting return code of mount command
    """
    mount_cmd_location = config.get('FsutilsSection', 'fsutils.mount')

    if dry_run_flag:
        logging.info(f"DRY RUN COMMAND: {mount_cmd_location} -f {fs_name} -t zfs {mountpath}")
        return 0
    else:
        mount_cmd = subprocess.run(
            [mount_cmd_location, "-f", fs_name, "-t", "zfs", mountpath]
        )
        return mount_cmd.returncode


def unmount_fs(fs_name: str):
    """
    Pass zFS mountpoint to unmount the file system.

        Parameters:
            fs_name (str): path to mounted zFS
        Returns:
            Resulting return code of mount command
    """
    unmount_cmd_location = config.get('FsutilsSection', 'fsutils.unmount')

    if dry_run_flag:
        logging.info(f"DRY RUN COMMAND: {unmount_cmd_location} -f {fs_name}")
        return 0
    else:
        unmount_cmd = subprocess.run([unmount_cmd_location, "-f", fs_name])
        return unmount_cmd.returncode
