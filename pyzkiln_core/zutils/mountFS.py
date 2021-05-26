#!/usr/bin/env python3
import subprocess
import re


###################################################################
# Create a zFS Aggregate and File System                          #
#   Input: fs_name(string) aggregate name for new zFS file system #
#          fs_size- size in megabytes for the VSAM linear data set#
#          extended_size- secondary size in megabytesfor VSAM ds  #
#          volume- volume the VSAM linear data set can have space #
#                                                                 #
#  Note: The issuer of the zfsadm define command requires         #
#  sufficient authority to create the VSAM linear data set        #
###################################################################
def allocateZFS(fs_name: str, volume: str, fs_size: int, extended_size: int):
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


####################################################################
# Pass zFS argregate name and mountpoint to mount the file system  #
#   Input:  fs_name(string) - zFS aggregate name                   #
#           mountpath(string) - path to mount zFS                  #
####################################################################
def mountFS(fs_name: str, mountpath: str):
    mount_cmd = subprocess.run(
        ["/usr/sbin/mount", "-f", fs_name, "-t", "zfs", mountpath]
    )
    return mount_cmd.returncode


####################################################################
# Pass zFS mountpoint to unmount the file system                   #
#   Input: fs_name (string) path to mounted zFS                    #
####################################################################
def unmountFS(fs_name: str):
    unmount_cmd = subprocess.run(["/usr/sbin/unmount", "-f", fs_name])
    return unmount_cmd.returncode


####################################################################
# Displays detailed information about a zFS file system            #
#  Input: fs_name(string) FS aggregate name                        #
#  Return: python dictionary                                       #
####################################################################
def aggr_fsinfo(fs_name: str):
    fsinfo_results_dic = {}

    fsinfo_cmd = subprocess.run(
        ["zfsadm", "fsinfo", "-aggregate", fs_name], capture_output=True
    )

    fsinfo_cmd = str(fsinfo_cmd.stdout)
    fsinfo = fsinfo_cmd.strip().split("\\n")
    # Iterate over each line of the output to make adjustements to
    # turn into a python dictionary
    for linew in fsinfo[3:]:
        linew = re.split(r": |\s\s\s", linew)

        # remove empty entries in the list
        while "" in linew:
            linew.remove("")

        # add to python dictionary
        fsinfo_results_dic.update(dict(zip(linew[::2], linew[1::2])))

    return fsinfo_results_dic
