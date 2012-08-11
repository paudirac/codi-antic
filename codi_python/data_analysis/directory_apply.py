#!/usr/bin/python
# -*- coding: UTF-8 -*-

# TODO: canviar lo de l'options, perquè ho agafi de la linia de commandes amb
#       optparse o d'un fitxer d'opcions, rotllo -o=options.txt

"""
Utility to apply a script to a set given in a file.

Us as:
\t$ python directory_apply.py path_one script_path

Utility that finds an archive named directories.txt at path_one and
applies the function "parse_and_apply(arguments)" once for each line in
directories.txt and each time giving the line as argument.

Is responsability of the script's parse_and_apply function to parse the
input and call to script's main or whatever it has to do.

An example is to call the following little script,

\t#!/usr/bin/python
\t# -*- coding: UTF-8 -*-

\tdef main(directori, fitxer):
\t    print directori
\t    print fitxer

\tdef parse_and_apply(algo):
\t    directori, fitxer = algo.split()
\t    main(directori, fitxer)

with the directories.txt file as:

\ty_32x32_v0p01_t0p0001_1000 rug.dat
\ty_32x32_v0p01_t0p0001_1001 rug.dat
\ty_32x32_v0p01_t0p0001_1002 rug.dat
\ty_32x32_v0p01_t0p0001_1003 rug.dat

which results in,

\ty_32x32_v0p01_t0p0001_1000
\trug.dat
\ty_32x32_v0p01_t0p0001_1001
\trug.dat
\ty_32x32_v0p01_t0p0001_1002
\trug.dat
\ty_32x32_v0p01_t0p0001_1003
\trug.dat

Another way to uset is to place an 'options.txt' file in the same directory
where the 'directories.txt' file resides, and then only list the directories
and the optinos separately.
In this form, the called script must implement the method,

\tdef parse_options_and_apply(directori, options).

[Ok. Work in progress. Sembla que xuta :P]
"""

import sys,os
from directory_apply_exceptions import AbortDirectoryApply

__all__ = []

class LineNumberFile(file):
    def line(self, n):
        for i in range(n):
            super(LineNumberFile, self).next()
        return super(LineNumberFile, self).next()

def directories_line(i, directories, fitxer):
    for dir in directories:
        nom = os.path.join(dir, fitxer)
        f = LineNumberFile(nom)
        yield f.line(i)
        f.close()

__all__.append("directories_list")
def directories_list(directori):
    directories = os.path.join(directori, "directories.txt")
    directories = open(directories).readlines()
    directories = [directori.rstrip() for directori in directories]
    return directories

def error_handler(missatge):
    fitxer = os.path.join("/home/pau/recerca/def_rug/codi/data_analysis/py/","logs_error_directory_apply.txt")
    open(fitxer,"a").write(missatge + "\n")

def import_script_named(script_name):
    script_dir, script_name = os.path.split(script_name)
    sys.path.append(script_dir)
    script_name = script_name[:-3] ## remove trailing .py
    return __import__(script_name)

def load_options(opt_dir):
    opt_file = os.path.join(opt_dir, "options.txt")
    try:
        opt_file = open(opt_file).readlines()
    except:
        return None
    return opt_file

class Abort(Exception): pass

def without_options(directory, script_module):
    directories = os.path.join(directory, "directories.txt")
    directories = open(directories,'r').readlines()
    directories = [directori.rstrip() for directori in directories]
    for directori in directories:
        try:
            script_module.parse_and_apply(directori)
        except Exception, e:
            if isinstance(e, AbortDirectoryApply):
                sys.stderr.write("Aborto per excepció: %s\n" % str(e))
                break
            print >> sys.stderr, "[Error] amb el %s" % directori
            error_handler("[Error] amb el %s" % directori)

def with_options(directory, script_module, options):
    directories = os.path.join(directory, "directories.txt")
    directories = open(directories,'r').readlines()
    directories = [directori.rstrip() for directori in directories]
    for directori in directories:
        try:
            script_module.parse_options_and_apply(directori, options)
        except Exception, e:
            if isinstance(e, AbortDirectoryApply):
                sys.stderr.write("Aborto per excepció: %s\n" % str(e))
                break
            print >> sys.stderr, "[Error] amb el %s" % directori
            error_handler("[Error] amb el %s" % directori)

def main(directory, script_module, options):
    if options is None:
        without_options(directory, script_module)
    else:
        with_options(directory, script_module, options)

if __name__ == "__main__":
    default_directory = "./"
    default_script = None
    default_options = None

    # searching for the place where directories.txt resides
    try:
        directory = sys.argv[1]
    except:
        directory = default_directory

    # serching for the module to execute
    try:
        script = sys.argv[2]
    except:
        script = default_script

    # serching for options file
    try:
        opt_dir = sys.argv[3]
        options = load_options(opt_dir)
    except:
        options = default_options

    script_module = None
    if script is not None:
        script_module = import_script_named(script)
    else:
        print __doc__; sys.exit(-1)

    main(directory, script_module, options)
