#!/usr/bin/env python3
"""Grow a ZFS Filesystem by a specified percentage."""
import configparser
import logging
import os
import random
import string
import time

from .fsutils import (aggr_fsinfo, allocate_zfs_by_datatype, bpxwmigf,
                     delete_zfs, display_filesystem_info, get_sysname, logging)

# Setup ConfigParser & read properties file
config = configparser.RawConfigParser()
config.read(os.path.join(os.path.dirname(__file__), 'ConfigFile.properties'))

# Print logger header & starting message
# logging is imported from fsutils.py
logging.info(f"Starting {os.path.basename(__file__)}")

def grow_zfs(source_zfs_name: str, mounted_on: str, grow_or_shrink: str, percentage: int):
    """
    Grow a ZFS Filesystem by a specified percentage.

        Parameters:
            source_zfs_name (str): ZFS to be grown
                 mounted_on (str): USS directory the ZFS is currently mounted on
             grow_or_shrink (str): whether to grow or shrink the ZFS (values: grow, shrink)
                 percentage (int): % for the ZFS to be grown by
        Returns:
            -1 = Error in processing, please refer to the log for specifics
            0 = Success
        Note:
            This command requires superuser authority
            This command relies on Z Open Automation Utilities (ZOAU)
    """
    logging.info("Starting grow_zfs() routine...")

    # Check if this is a dry-run or not and print a message if so
    if config.get('RunSection', 'run.dryrun').lower() == 'true':
        logging.info("DRY RUN: dry-run flag enabled, this method will continue without making any system changes or updates")

    # get the name of the ZFS that is mounted on the specified mount point
    zfs_name_check = display_filesystem_info(mounted_on)['Aggregate Name ']

    # check that the supplied ZFS name and mounted ZFS name match
    if zfs_name_check != source_zfs_name:
        logging.error(f"ERROR: supplied source_zfs_name ({source_zfs_name})" +
                      f" and ZFS ({zfs_name_check}) mounted on " +
                      f"({mounted_on}) DO NOT match!")
        return -1
    else:
        logging.debug(f"Success: supplied source_zfs_name ({source_zfs_name})" +
                      f" and ZFS ({zfs_name_check}) mounted on ({mounted_on}) match!")

        # get the name of the system the ZFS lives on
        sysname_zfs_mounted_on = display_filesystem_info(mounted_on)['File System Owner ']

        # get the name of the system we are running on
        sysname = get_sysname()
        sysname_were_on = sysname[2:len(sysname)-3]

        # make sure we are on the system that the current mounted ZFS is on
        if sysname_were_on != sysname_zfs_mounted_on:
            logging.error("ERROR: not running on system that owns the " +
                          "source ZFS. Please re-run this code on system " +
                            f"{sysname_zfs_mounted_on}")
            return -1
        else:
            logging.debug("Success: running on correct system")

            # get the current size of the source ZFS
            aggr_fsinfo_dict = aggr_fsinfo(source_zfs_name)
            source_zfs_size = aggr_fsinfo_dict['Size']

            # need to remove the K from the size & convert to an int
            source_zfs_size = int(source_zfs_size[0:len(source_zfs_size)-1])
            logging.debug(f"Source ZFS initial size (KB): {source_zfs_size}")

            # calculate the new desired size
            if grow_or_shrink == 'grow':
                percentage = (percentage / 100)
                logging.debug(f"Percentage to increase size by: {percentage}")
                target_zfs_size = int(source_zfs_size + (source_zfs_size * percentage))
                logging.debug(f"Desired/target size (KB): {target_zfs_size}")

            elif grow_or_shrink == 'shrink':
                percentage = (percentage / 100)
                logging.debug(f"Percentage to decrease size by: {percentage}")
                target_zfs_size = int(source_zfs_size - (source_zfs_size * percentage))
                logging.debug(f"Desired/target size (KB): {target_zfs_size}")

            # allocate temp zfs using a partially random name to avoid data set collisions
            letters = string.ascii_uppercase
            random_name = (''.join(random.choice(letters) for i in range(5)))
            temp_zfs_name = source_zfs_name[0:len(source_zfs_name)-4] + ".TMP" + random_name + ".ZFS"
            logging.debug(f"Temporary ZFS name: {temp_zfs_name}")
            allocate_rc = allocate_zfs_by_datatype(temp_zfs_name, target_zfs_size, data_type='kilobytes')

            if allocate_rc == 1:
                logging.error("ERROR: allocation of temp data set failed, " +
                              "see the above output for more details")
                return -1
            else:
                # create a new name for the old ZFS, also partially random
                random_name = (''.join(random.choice(letters) for i in range(5)))
                source_zfs_renamed = source_zfs_name[0:len(source_zfs_name)-4] + ".RNM" + random_name + ".ZFS"
                logging.debug(f'New name for "old" ZFS: {source_zfs_renamed}')

                # issue bpxwmigf command
                # NOTE: this is the function that requires superuser authority
                bpxwmigf_res = bpxwmigf(source_zfs_name, temp_zfs_name, source_zfs_renamed)
                time.sleep(int(config.get('TimingSection', 'time.bpxwmigf_wait')))
                logging.info(f"Result of bpxwmigf command is: {bpxwmigf_res}")

                # delete old (now renamed) ZFS
                # sleep/pause was added because the delete was running too
                # quickly and the old ZFS was still in use
                delete_result = delete_zfs(source_zfs_renamed)

                if delete_result == 0:
                    logging.info(f"Successfully deleted {source_zfs_renamed}")
                else:
                    logging.error(f"Delete return code was {delete_result} " +
                                  "meaning something went wrong when attempting "
                                  f"to delete {source_zfs_renamed}. Manual cleanup "
                                  f"of {source_zfs_renamed} is necessary")
                    return -1

                logging.info("...ending grow_zfs() routine")
                return 0
