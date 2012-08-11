#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import os,sys,subprocess

def parse_and_apply(algo):
    try:
        directory = algo.split()[0]
        main(directory)
    except Exception, e: raise e

PATH_TO_DATA_ANALYSIS="/home/pau/recerca/def_rug/codi/data_analysis/data_analysis"

def assert_file_is_empty(path):
    try:
        os.rename(path,path + "ara_et_borro")
        os.remove(path + "ara_et_borro")
    except:
        pass
    f = open(path,"w")
    f.close()

def data_analysis(directori):
    print
    cmd = PATH_TO_DATA_ANALYSIS + " %s" % directori + " 2> /dev/null"
    try:
        file_name = os.path.join(directori, "data")
        assert_file_is_empty (file_name)
        file_name = os.path.join(directori, "historia_defectes")
        assert_file_is_empty (file_name)
    except:
        pass
    try:
        p = subprocess.Popen(cmd, shell=True)
        print "analitzant %s amb el subprocess %d" % (directori, p.pid)
    except OSError, e:
        print "Error a l'executar data_analysis", e
    pid, ret = os.waitpid(p.pid, 0)
    if ret == 0:
        print directori, "subprocés %d acabat correctament" % pid
    else:
        print directori, "mal analitzat"
    return None

def extract(filename, extract_cmd):
    assert_file_is_empty (filename)
    try:
        p = subprocess.Popen(extract_cmd, shell=True)
        print "Extraient %s" % filename
    except OSError, e:
        print "Error a l'executar %s" % cmd
    pid,ret = os.waitpid(p.pid, 0)
    if ret == 0: pass
    else: print "Error"
    return None

def extract_stress(directori):
    file_name = os.path.join(directori, "stressm.dat")
    cmd = "egrep mol %s | cut -f2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17 -d' ' | sed -e 1d > %s" % (os.path.join(directori,"data"), file_name)
    extract(file_name, cmd)

def extract_rug(directori):
    file_name = os.path.join(directori, "rug.dat")
    cmd = "egrep rug %s | cut -f2,3,4,5,6,7,8,9 -d' ' | sed -e 1d > %s" \
          % (os.path.join(directori,"data"), file_name)
    extract(file_name, cmd)

def extract_ene(directori):
    file_name = os.path.join(directori, "eneNowBe.dat")
    cmd = "egrep ene %s | cut -f2,3,4,5,6,7,8,9,10 -d' ' | sed -e 1d > %s" \
          % (os.path.join(directori, "data"), file_name)
    extract (file_name, cmd)

def main(directori):
    data_analysis (directori)
    extract_stress (directori)
    extract_rug (directori)
    extract_ene (directori)
    print
    return None

if __name__ == "__main__":
    try:
        directory = sys.argv[1]
        main(directory)
    except Exception, e: raise e

    sys.exit(0)
