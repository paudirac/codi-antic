#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import os,sys

def parse_and_apply(algo):
    try:
        infile,outfile = algo.split()
        print algo
        print infile
        print outfile
        main(infile, outfile)
    except Exception, e: raise e

def parse_options_and_apply(directori, options):
    infile, outfile = options[0].split()
    infile = os.path.join(directori, infile)
    outfile = os.path.join("./", outfile)
    main(infile, outfile)

def main(infile,outfile):
    print "joining", infile, "with", outfile
    open(outfile, "a").writelines(open(infile).readlines())
    #file_out = open(outfile, "a")
    #file_out.writelines(open(infile).readlines())
    #file_out.close()
