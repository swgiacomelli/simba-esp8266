#!/usr/bin/env python

import sys
import time
from ConfigParser import ConfigParser
from collections import OrderedDict


MAJOR=1
MINOR=0

SETTINGS_H_FILENAME = "settings.h"
SETTINGS_BIN_FILENAME = "settings.bin"

H_FMT = """/**
 * @file {filename}
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Simba project.
 */

/**
 * This file was generated by settings.py {major}.{minor} {date}.
 */

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

{addresses}
{sizes}
{types}
{values}

#endif
"""

def parse_settings_file(filename):
    settings_parser = ConfigParser()
    settings_parser.read(filename)

    addresses = []
    sizes = []
    values = []
    types = []

    for item in settings_parser.items("addresses"):
        addresses.append((item[0], int(item[1], 0)))

    for item in settings_parser.items("sizes"):
        sizes.append((item[0], int(item[1], 0)))

    for item in settings_parser.items("types"):
        types.append((item[0], item[1]))

    for item in settings_parser.items("values"):
        values.append((item[0], item[1]))

    addresses.sort(key=lambda item: item[1])

    return addresses, sizes, types, values


def create_settings_dict(addresses, sizes, types, values):
    settings = OrderedDict()

    for name, address in addresses:
        settings[name] = {"address": address}

    for name, size in sizes:
        if name not in settings:
            sys.stderr.write("{}: no address for settings\n".format(name))
            sys.exit(1)
        settings[name]["size"] = size

    for name, type in types:
        if name not in settings:
            sys.stderr.write("{}: no address for settings\n".format(name))
            sys.exit(1)
        settings[name]["type"] = type

    for name, value in values:
        if name not in settings:
            sys.stderr.write("{}: no address for settings\n".format(name))
            sys.exit(1)
        settings[name]["value"] = value

    return settings


def create_binary_file(settings):
    # create the settings file content
    content = ""
    for name, item in settings.items():
        # add padding between previous setting and this one
        content += "\xff" * (item["address"] - len(content))
        # add the value
        if item["type"] == "string":
            if item["size"] <= len(item["value"]):
                sys.stderr.write("{}: value does not fit in size {}\n".format(item["value"],
                                                                              item["size"]))
                sys.exit(1)
            content += item["value"]
            # null termination
            content += "\x00"
        else:
            sys.stderr.write("{}: bad type\n".format(item["type"]))
            sys.exit(1)

    # write the content to the settings file
    with open(SETTINGS_BIN_FILENAME, "wb") as fout:
        fout.write(content)


def create_header_file(settings):

    addresses = []
    sizes = []
    types = []
    values = []

    for name, item in settings.items():
        addresses.append("#define SETTINGS_{name}_ADDR {value}"
                         .format(name=name.upper(), value=item["address"]))
        sizes.append("#define SETTINGS_{name}_SIZE {value}"
                     .format(name=name.upper(), value=item["size"]))
        types.append("#define SETTINGS_{name}_TYPE {value}"
                     .format(name=name.upper(), value=item["type"]))
        values.append("#define SETTINGS_{name}_VALUE {value}"
                      .format(name=name.upper(), value=item["value"]))

    now = time.strftime("%Y-%m-%d %H:%M %Z")

    # write to settings header file
    with open(SETTINGS_H_FILENAME, "w") as fout:
        fout.write(H_FMT.format(filename=SETTINGS_H_FILENAME,
                                date=now,
                                major=MAJOR,
                                minor=MINOR,
                                addresses="\n".join(addresses),
                                sizes="\n".join(sizes),
                                types="\n".join(types),
                                values="\n".join(values)))


if __name__ == "__main__":
    items = parse_settings_file(sys.argv[1])
    settings = create_settings_dict(*items)

    create_header_file(settings)
    create_binary_file(settings)
